# 02 — Anatomie des messages

> *"Une requête et une réponse ont la même forme avec des premières lignes différentes."*

Chaque message HTTP — requête ou réponse — possède la **même structure en quatre parties**. Mémorisez-la une fois et vous aurez mémorisé la moitié du protocole.

---

## La forme universelle

```
┌────────────────────────────────────────────┐
│  start-line          (une ligne, CRLF)     │   ← diffère entre requête et réponse
├────────────────────────────────────────────┤
│  header-1: value-1   (une ligne, CRLF)     │
│  header-2: value-2   (une ligne, CRLF)     │   ← zéro ou plusieurs headers
│  ...                                       │
├────────────────────────────────────────────┤
│  (ligne vide)        (juste CRLF)          │   ← séparateur OBLIGATOIRE
├────────────────────────────────────────────┤
│  body (optionnel, peut être binaire)       │
│  ...                                       │
└────────────────────────────────────────────┘
```

Quatre parties. **`\r\n`** termine chaque ligne — y compris la ligne vide qui termine la section des headers.

---

## Requête

### Start-line — la **request line**

```
METHOD  SP  request-target  SP  HTTP-version  CRLF
```

Exemple :
```
GET /api/users/42?fields=name HTTP/1.1\r\n
```

| Champ | Notes |
|---|---|
| `METHOD` | En majuscules. `GET`, `POST`, `PUT`, `DELETE`, `HEAD`, `OPTIONS`, etc. Voir [fichier 03](03_METHODS.md). |
| `request-target` | Généralement la partie path-et-query d'une URL (`/api/users/42?fields=name`). D'autres formes existent (URI absolue pour les proxys, `*` pour `OPTIONS`) mais vous pouvez les ignorer pour webserv. |
| `HTTP-version` | `HTTP/1.0` ou `HTTP/1.1` — lettres majuscules, slash, deux chiffres. |
| `SP` | Un espace simple (`0x20`). |
| `CRLF` | `\r\n` (`0x0D 0x0A`). |

### Headers

Zéro ou plusieurs lignes de la forme :

```
Header-Name: header-value\r\n
```

**Règles importantes pour le parsing :**

- Les noms de header **sont insensibles à la casse** (`Content-Length`, `content-length`, `CONTENT-LENGTH` signifient tous la même chose). Votre parser doit normaliser les clés en minuscules.
- Les valeurs peuvent avoir des espaces initiaux après le deux-points qui doivent être supprimés (trim).
- Plusieurs valeurs pour le même header peuvent apparaître soit sous forme de **plusieurs lignes**, soit sous forme d'une **liste séparée par des virgules** sur une seule ligne. Les deux sont valides.
- Une valeur de header peut être repliée (folded) sur plusieurs lignes (une ligne de continuation commence par un espace). **Obsolète dans le HTTP/1.1 moderne** — la RFC 7230 indique que les expéditeurs NE DOIVENT PAS (MUST NOT) en générer. Pour webserv, vous pouvez rejeter tout header replié avec une 400.
- Certains headers sont obligatoires dans certaines circonstances (par exemple, `Host` pour les requêtes HTTP/1.1).

### Ligne vide

Un simple `\r\n`. **Obligatoire.** Marque la fin des headers.

### Body

Présent sur certaines méthodes (`POST`, `PUT`), absent sur la plupart des autres. **Le protocole n'inclut pas la "taille du body" dans la request line — vous l'apprenez via les headers** (`Content-Length` ou `Transfer-Encoding: chunked`). Voir [fichier 06](06_FRAMING.md).

### Exemple détaillé — requête

```
POST /api/login HTTP/1.1\r\n
Host: example.com\r\n
Content-Type: application/json\r\n
Content-Length: 38\r\n
\r\n
{"user":"alice","password":"hunter2"}
```

Le travail du parser :
1. Lire la ligne 1 : découper sur les espaces → method `POST`, target `/api/login`, version `HTTP/1.1`.
2. Lire les lignes 2–4 : découper sur le premier `:` → stocker sous forme de paires clé/valeur.
3. Lire la ligne 5 : vide → section des headers terminée.
4. Consulter `Content-Length: 38` → lire exactement 38 octets de plus dans le body.

---

## Réponse

### Start-line — la **status line**

```
HTTP-version  SP  status-code  SP  reason-phrase  CRLF
```

Exemple :
```
HTTP/1.1 404 Not Found\r\n
```

| Champ           | Notes                                                                                                                                                                                          |
| --------------- | ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| `HTTP-version`  | Identique à la requête — `HTTP/1.1` est ce que vous devriez envoyer.                                                                                                                           |
| `status-code`   | Nombre à 3 chiffres. Voir [fichier 04](04_STATUS_CODES.md).                                                                                                                                     |
| `reason-phrase` | Texte lisible par un humain. Les clients devraient ignorer la formulation exacte, mais incluez-en une qui soit raisonnable. Les raisons standard (`OK`, `Not Found`, `Internal Server Error`, …) sont conventionnelles, pas imposées. |

### Headers, ligne vide, body

Identiques à la requête — même structure en quatre parties. Le body est tout ce que vous renvoyez : HTML, JSON, une image, une page d'erreur.

### Exemple détaillé — réponse

```
HTTP/1.1 200 OK\r\n
Content-Type: text/html; charset=utf-8\r\n
Content-Length: 53\r\n
Connection: keep-alive\r\n
\r\n
<!DOCTYPE html><html><body>Hello world</body></html>
```

---

## Ébauche de parser (pour webserv)

Pseudo-code pour le parser de requête que vous écrirez :

```
read_until("\r\n")            → request_line
parse_request_line(request_line) → {method, target, version}

headers = {}
loop:
    line = read_until("\r\n")
    if line is empty: break        # ligne vide → headers terminés
    key, value = split_on_first(line, ':')
    headers[lowercase(key)] = trim(value)

if "content-length" in headers:
    body = read_exactly(int(headers["content-length"]))
elif headers.get("transfer-encoding") == "chunked":
    body = read_chunked()           # voir fichier 06
else:
    body = ""                       # GET, DELETE, etc.

dispatch(method, target, headers, body)
```

Voici votre parser en trois lignes de logique plus un décodeur chunked. **Prenez le temps de bien le concevoir** — chaque autre partie de webserv alimente ce parser ou consomme sa sortie.

---

## Pièges courants

| Piège | Symptôme | Solution |
|---|---|---|
| Traiter `\n` comme terminateur de ligne | Les requêtes des navigateurs se parsent bien ; certaines requêtes `curl` échouent ; la requête suivante semble "fuiter" dans le body de la précédente | Lire exactement `\r\n`. Rejeter les lignes se terminant par un `\n` isolé (ou accepter les deux si vous voulez être tolérant). |
| Stocker les headers de manière sensible à la casse | `Content-Length` fonctionne, `content-length` non, selon le client avec lequel vous avez testé | Mettre la clé en minuscules à l'insertion **et** à la recherche. |
| Lire trop d'octets du body | La requête suivante est mal parsée car vous avez avalé sa première ligne | Respecter `Content-Length` exactement. Ne jamais lire plus. |
| Lire trop peu d'octets du body | Bloqué sur `read` en attendant d'autres octets qui n'arrivent jamais | Boucler jusqu'à avoir lu `Content-Length` octets au total, en prenant en compte les lectures partielles. |
| Oublier la ligne vide dans votre réponse | Les clients restent bloqués en attendant que le body commence | Toujours émettre `\r\n` entre les headers et le body, même si le body est vide. |
| Pas de `Content-Length` dans une réponse avec un body | Le client lit jusqu'à la fermeture de la connexion — fonctionne si vous fermez, casse le keep-alive | Toujours inclure `Content-Length` (ou `Transfer-Encoding: chunked`) sur les réponses avec body. |

---

## Outils C++98 pour le parsing

Vous ferez beaucoup de manipulation de chaînes. La bibliothèque standard C++98 vous fournit :

| Besoin | Outil |
|---|---|
| Découper une chaîne sur un délimiteur | `std::string::find` + `std::string::substr` dans une boucle |
| Nettoyer les espaces (trim) | Personnalisé — trouver le premier/dernier non-espace, `substr` entre les deux |
| Convertir en minuscules | `std::transform(s.begin(), s.end(), s.begin(), ::tolower)` |
| Chaîne → int (pour `Content-Length`) | `std::stringstream` + `>>` (C++98 n'a pas de `std::stoi`) |
| Comparaison insensible à la casse | Mettre les deux en minuscules puis comparer — il n'y a pas de `strcasecmp` dans le standard C++ |
| Lire une ligne depuis le buffer de la socket | C'est à vous de l'implémenter. La socket n'est pas un `std::istream`. |

**Piège :** `std::stringstream` réussit même avec une entrée invalide (renvoie silencieusement 0 si l'extraction du flux échoue). Vérifiez toujours `ss.fail()` ou `ss.eof()` après l'extraction.

---

## Modèle mental TL;DR

> Les deux messages sont : **une start-line + zéro ou plusieurs lignes de header `Key: Value` + une ligne vide + des octets de body optionnels**. La start-line est ce qui distingue une requête d'une réponse. Tout ce qui suit est structurellement identique.

**Continuez vers [`03_METHODS.md`](03_METHODS.md)** pour apprendre ce que ces méthodes sur la request line *signifient* réellement.