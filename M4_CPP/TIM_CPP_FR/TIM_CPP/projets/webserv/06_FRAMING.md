# 06 — Découpage / Framing (Content-Length vs Chunked)

> *"Si vous ne pouvez pas savoir où se termine un message, vous ne pouvez pas lire le suivant."*

**Le fichier le plus important de cette bibliothèque.** Ratez le découpage (framing) et toutes les autres couches de webserv s'effondrent : les requêtes débordent les unes sur les autres, les CGI se bloquent, le keep-alive plante, les réponses sont tronquées. Le sujet le mentionne explicitement à deux reprises (requêtes chunked, chunked-vs-content-length sur le flux de retour CGI).

---

## Le problème

TCP est un flux d'octets. Il n'a **aucune frontière de message**. Lorsque vous appelez `recv()`, vous pouvez obtenir :

- Une fraction d'une requête
- Une requête complète
- Une requête complète plus une partie de la suivante
- Plusieurs requêtes à la fois

Le protocole HTTP ne place pas de « longueur de message » dans la ligne de départ. Alors comment le serveur sait-il **quand s'arrêter de lire le corps (body)** d'une requête ? Et comment le client sait-il **quand s'arrêter de lire le corps** d'une réponse ?

Trois moyens, par ordre de priorité :

1. **`Content-Length: N`** — lire exactement N octets de plus après la section des headers.
2. **`Transfer-Encoding: chunked`** — le corps est une séquence de tronçons (chunks) préfixés par leur taille, terminée par un tronçon spécial de « longueur zéro ».
3. **Pas de header de longueur, pas de chunked** — lire jusqu'à ce que la connexion se ferme (sémantique `Connection: close`).

Si `Content-Length` et `Transfer-Encoding` sont tous les deux présents, **`Transfer-Encoding` prévaut** et `Content-Length` doit être ignoré (la spécification indique que la requête est suspecte — les anciens proxys utilisent cela pour des attaques de type HTTP request smuggling ; rejetez-la avec une 400 par sécurité).

---

## Méthode 1 — `Content-Length`

Le cas simple. Le header vous indique le nombre d'octets du corps.

### Sur une requête

```
POST /upload HTTP/1.1\r\n
Host: example.com\r\n
Content-Type: text/plain\r\n
Content-Length: 11\r\n
\r\n
Hello world
```

Votre parser :
1. Lit les headers jusqu'à la ligne vide.
2. Cherche `Content-Length` → `11`.
3. Lit **exactement** 11 octets supplémentaires. Le corps est `"Hello world"`.
4. Tout ce qui suit ces 11 octets appartient à la requête **suivante** sur cette connexion.

### Sur une réponse

```
HTTP/1.1 200 OK\r\n
Content-Type: text/html\r\n
Content-Length: 53\r\n
\r\n
<!DOCTYPE html><html><body>Hello world</body></html>
```

Le client fait de même : lit les headers, cherche `Content-Length`, lit ce nombre d'octets, s'arrête.

### Pièges

- **Compter les octets, pas les caractères.** Les chaînes UTF-8 ont des caractères de largeur variable. `"hé"` fait 3 octets (`0x68 0xC3 0xA9`), pas 2.
- **Décalage d'un octet (off-by-one) à cause de `\r\n`.** Le corps commence **après** la ligne vide qui termine les headers. La ligne vide elle-même ne fait pas partie du corps.
- **`\r\n` final dans le corps.** Certains émetteurs en incluent un accidentellement. Le `Content-Length` fait foi — s'il indique 11 et que quelqu'un a ajouté un `\r\n` après "Hello world", vous lisez 11 octets (`"Hello world"`) et le `\r\n` fait partie de la requête suivante (qui devient alors malformée).
- **`Content-Length: 0`** est valide et signifie « pas de corps ». Fréquent sur les requêtes `DELETE`.

---

## Méthode 2 — `Transfer-Encoding: chunked`

Lorsque l'émetteur ne connaît pas la longueur du corps à l'avance (streaming, génération dynamique), il utilise l'encodage par tronçons (chunked encoding). Le corps devient une séquence de tronçons préfixés par leur taille.

### Format sur le réseau (wire format)

```
<chunk-size-in-hex>\r\n
<chunk-data>\r\n
<chunk-size-in-hex>\r\n
<chunk-data>\r\n
...
0\r\n
\r\n
```

Chaque tronçon (chunk) :
1. **Taille** en hexadécimal ASCII, suivie de `\r\n`.
2. **Données** — exactement ce nombre d'octets.
3. `\r\n` de fin (non compté dans la taille).

Un tronçon de taille `0` marque la fin. Après cela, optionnellement, des headers de fin ("trailers"), puis `\r\n`.

### Exemple détaillé

Le corps `"Hello world"` (11 octets) envoyé en deux tronçons de 5 et 6 octets :

```
5\r\n
Hello\r\n
6\r\n
 world\r\n
0\r\n
\r\n
```

(Remarquez l'espace au début de `" world"`.)

Total des octets sur le réseau : 24, bien que le contenu réel soit de 11. Ce surcoût est le prix à payer pour « nous ne savons pas à l'avance quelle sera la taille ».

### Taille en hexadécimal — piège

La taille est en **hexadécimal**, pas en décimal. Un tronçon de 255 octets s'écrit `ff\r\n`, pas `255\r\n`. Un tronçon de 10 s'écrit `a\r\n`, pas `10\r\n`. (Le `10` décimal en hexadécimal signifie 16 octets — bug facile à commettre.)

### Schéma de décodeur

```cpp
// pseudo-code
std::string decode_chunked(SocketBuffer& sock) {
    std::string body;
    while (true) {
        std::string size_line = sock.read_line();   // lit jusqu'a \r\n et le consomme
        size_t size = parse_hex(size_line);          // strtol(s.c_str(), NULL, 16)
        if (size == 0) {
            sock.read_line();                        // consomme la ligne vide finale
            break;
        }
        std::string chunk = sock.read_n_bytes(size);
        sock.read_line();                            // consomme le \r\n apres le chunk
        body += chunk;
    }
    return body;
}
```

**Notes d'implémentation :**
- `strtol(s.c_str(), NULL, 16)` en C++98 parse l'hexadécimal. Il y a aussi `std::stringstream s; s >> std::hex >> n;`.
- Le décodeur est direct **mais doit être compatible avec le mode non-bloquant** dans webserv. Vous ne pouvez pas faire un `recv()` et attendre. Vous lisez ce qui est disponible, sauvegardez votre état (« je suis au milieu du tronçon 3 avec 47 octets restants à lire »), et retournez dans `poll()`. Quand d'autres octets arrivent, reprenez là où vous vous étiez arrêté.
- Cela signifie que votre décodeur chunked doit pouvoir être **repris** (resumable) — cela ne peut pas être un simple appel de fonction. C'est une petite machine à états.

### Piège : ne transférez pas de corps chunked au CGI

Le sujet indique explicitement :

> *"for chunked requests, your server needs to un-chunk them, the CGI will expect EOF as the end of the body."*

Les scripts CGI (PHP, Python, etc.) ne gèrent pas le chunked. Ils lisent sur stdin jusqu'à EOF. Donc : **dé-chunker d'abord**, puis transmettre par pipe le corps décodé vers le stdin du CGI, puis fermer ce stdin pour que le CGI voie l'EOF.

---

## Méthode 3 — Lire jusqu'à la fermeture

Si ni `Content-Length` ni `Transfer-Encoding: chunked` n'est présent dans une **réponse**, le client lit jusqu'à ce que le serveur ferme la connexion. C'est le comportement par défaut de HTTP/1.0.

Pour les requêtes, cette méthode n'est **pas valide** — une requête sans `Content-Length` sur une méthode qui comporte un corps est malformée ; vous devez répondre avec `411 Length Required`.

Pour les réponses, c'est ce qui arrive si vous oubliez `Content-Length`. Cela « fonctionne », mais casse le keep-alive : le client doit attendre le `FIN` pour savoir que le corps est terminé, ce qui signifie que vous ne pouvez pas réutiliser la connexion TCP. **Définissez toujours `Content-Length` sur les réponses avec un corps.**

---

## Découpage en sortie — vos réponses

Pour chaque réponse que vous générez, choisissez-en une :

| Taille du corps connue d'avance ? | Utiliser |
|---|---|
| Oui (fichier statique, page d'erreur) | `Content-Length` |
| Non (sortie CGI en streaming sans `Content-Length` fourni par le CGI) | `Transfer-Encoding: chunked` (si HTTP/1.1) ou simplement fermer après écriture (si HTTP/1.0) |

Dans webserv, **privilégiez par défaut `Content-Length`** — la taille de vos fichiers statiques est obtenue via `stat()`, vos pages d'erreur sont des chaînes préconstruites, la sortie de votre CGI (une fois entièrement mise en mémoire tampon) a une taille connue.

> **Compromis streaming vs mise en mémoire tampon (buffer) pour le CGI.** Lire l'intégralité de la sortie du CGI avant d'envoyer vous permet de calculer le `Content-Length` et d'utiliser le framing simple. Le coût est la mémoire — un CGI renvoyant un fichier de 100 Mo représentera 100 Mo dans la heap de votre serveur. L'alternative est de transférer les octets du CGI par tronçons au fur et à mesure de leur arrivée — plus de code, plus de risques d'erreurs. **Pour webserv : utilisez un buffer.** Vos tests de charge ne généreront pas de réponses CGI de 100 Mo.

---

## Quand envoyer `Connection: close` vs réutiliser

Si votre réponse utilise `Content-Length`, le client peut savoir quand le corps se termine et réutiliser la connexion (keep-alive).

Si votre réponse **n'a pas** de `Content-Length` et n'est pas en chunked, vous **devez** inclure `Connection: close` afin que le client sache qu'il doit attendre EOF comme délimiteur de fin de corps. Sinon, il restera bloqué à attendre le corps indéfiniment.

Voir le [fichier 07](07_CONNECTION.md) pour la vue d'ensemble du keep-alive.

---

## Le problème de framing côté retour CGI

D'après le sujet :

> *"If no content_length is returned from the CGI, EOF will mark the end of the returned data."*

Lorsque vous lancez un CGI et lisez son stdout, le CGI peut :

1. **Renvoyer `Content-Length:` lui-même** — facile. Vous le transférez en tant que `Content-Length` de votre réponse.
2. **Ne pas renvoyer `Content-Length:`** — vous lisez jusqu'à ce que le pipe stdout du CGI se ferme (EOF). Ensuite, soit :
   - Vous redécoupez avec votre propre `Content-Length` (recommandé ; vous savez combien d'octets vous avez collectés)
   - Vous transférez avec `Transfer-Encoding: chunked` (plus complexe)
   - Vous envoyez avec `Connection: close` (solution de facilité, mais valide)

---

## Cas limites à tester

- Corps de longueur 0 avec `Content-Length: 0` → corps vide, valide.
- Corps de longueur 0 sans `Content-Length` → pas de corps, valide.
- `Content-Length` en désaccord avec les octets réels → indéfini ; rejeter avec 400.
- `Content-Length` plus grand que votre configuration `client_max_body_size` → 413 Payload Too Large, ne pas lire le corps.
- Corps chunked où le premier tronçon est `0\r\n\r\n` → corps vide, valide.
- Corps chunked qui n'envoie jamais le `0\r\n` final → timeout au bout d'un moment (408 Request Timeout).
- Une requête avec à la fois `Content-Length` et `Transfer-Encoding: chunked` → 400 Bad Request (vecteur de request smuggling).

---

## Modèle mental TL;DR

> La fin du corps ne se trouve pas dans la ligne de départ — elle se trouve dans le `Content-Length` (compter les octets) ou dans le `Transfer-Encoding: chunked` (décoder le format des tronçons), et si aucun n'est présent, lire jusqu'à la fermeture. **Pour webserv :** décodez les requêtes chunked avant de les traiter ; dé-chunkez avant d'envoyer dans le pipe vers le CGI ; définissez toujours `Content-Length` sur vos réponses sortantes ; rejetez tout découpage conflictuel avec une 400.
**Continuer vers [`07_CONNECTION.md`](07_CONNECTION.md)** — une fois le body terminé, qu'advient-il de la connexion TCP ?