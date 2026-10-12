# 05 — Headers

> *"Les headers sont la manière dont HTTP attache des métadonnées au texte. La majeure partie de l'expressivité du protocole réside ici, et non dans la start line."*

Un catalogue des headers que vous allez **analyser (parse) depuis les requêtes** et **émettre sur les réponses** pour webserv. Regroupés par famille. Chaque entrée : ce que c'est, où cela apparaît, pourquoi c'est important, quel piège y est associé.

La RFC complète liste des centaines de headers. Vous en avez besoin d'environ 20.

---

## Familles de headers (vocabulaire RFC)

Les headers sont conventionnellement regroupés en :

- **General** — applicables à la fois aux requêtes et aux réponses (`Date`, `Connection`)
- **Request** — apparaissent uniquement sur les requêtes (`Host`, `User-Agent`, `Accept`, `Cookie`)
- **Response** — apparaissent uniquement sur les réponses (`Server`, `Location`, `Set-Cookie`, `Allow`)
- **Entity / Representation** — décrivent le corps (`Content-Length`, `Content-Type`, `Content-Encoding`)

En termes modernes (RFC 7230+), la famille "entity" est renommée en "payload" ou "representation", mais le modèle original à quatre catégories reste un modèle mental utile.

---

## Règles de parsing (récapitulatif, à assimiler impérativement)

- **Les noms sont insensibles à la casse.** Convertir en minuscules au stockage, convertir en minuscules à la recherche.
- **Les valeurs peuvent comporter des espaces au début/à la fin.** Il faut les découper (trim).
- **Les valeurs multiples** peuvent être séparées par des virgules sur une seule ligne, ou apparaître sous la forme de plusieurs lignes portant le même nom.
- **Aucun header n'est obligatoire pour HTTP/1.0.** `Host` est obligatoire pour HTTP/1.1.

---

## Headers généraux (General headers)

### `Host`
**Obligatoire sur les requêtes HTTP/1.1.** Indique au serveur pour quel "virtual host" la requête est destinée, lorsque plusieurs sites partagent une adresse IP et un port.

```
Host: example.com
Host: api.example.com:8443
```

Pour webserv, **les virtual hosts sont hors de portée selon le sujet** — vous effectuez le dispatch par paire `interface:port`, pas par `Host`. Mais vous devez tout de même accepter le header en entrée (en renvoyant 400 s'il est manquant sur une requête HTTP/1.1).

### `Connection`
Contrôle le cycle de vie de la connexion. Voir [fichier 07](07_CONNECTION.md).

```
Connection: close          ← fermer la connexion TCP après cette réponse
Connection: keep-alive     ← maintenir la connexion TCP ouverte (par défaut en HTTP/1.1)
```

Pour webserv : respectez `Connection: close` provenant des clients (fermez après la réponse). La valeur par défaut en HTTP/1.1 est keep-alive. Si vous n'implémentez pas encore keep-alive, **envoyez toujours `Connection: close`** dans vos réponses et fermez la socket.

### `Date`
Date formatée selon la RFC 1123.

```
Date: Sun, 30 May 2026 14:23:11 GMT
```

Vous devez envoyer `Date` sur chaque réponse. Utilisez `time(NULL)` et `strftime` avec le format `%a, %d %b %Y %H:%M:%S GMT`. N'oubliez pas GMT — la spécification l'impose.

### `Transfer-Encoding`
**Crucial pour webserv.** Décrit la manière dont le body est découpé (framing). Voir [fichier 06](06_FRAMING.md).

```
Transfer-Encoding: chunked
```

Si une requête possède ce header, le body est encodé par morceaux (chunked) — vous devez le décoder avant de le consommer. Si une réponse l'a, le client s'attend à une sortie chunked.

---

## Headers de requête (Request headers)

### `User-Agent`
Identifie le client.

```
User-Agent: Mozilla/5.0 (Macintosh; Intel Mac OS X 10_15_7) ...
User-Agent: curl/8.4.0
```

Pour webserv : **lisez-le pour les logs**, ignorez-le pour le comportement. Certains serveurs font de la négociation de contenu basée sur l'UA — démesuré ici.

### `Accept`
Quels types MIME le client accepte de recevoir. Voir [fichier 09](09_CONTENT_NEGOTIATION.md).

```
Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8
```

Pour webserv : **ignorez-le** pour les fichiers statiques (l'extension du fichier détermine le `Content-Type`). Pour les CGI, transmettez-le via la variable d'environnement `HTTP_ACCEPT`.

### `Accept-Encoding`
Quels algorithmes de compression le client prend en charge.

```
Accept-Encoding: gzip, deflate, br
```

Pour webserv : ignorez sauf si vous implémentez la compression de sortie en gzip (vous n'y êtes pas obligé ; hors sujet).

### `Accept-Language`
Langues préférées.

```
Accept-Language: en-US,en;q=0.9,fr;q=0.8
```

Pour webserv : ignorez.

### `Authorization`
Identifiants de connexion.

```
Authorization: Basic dXNlcjpwYXNz
Authorization: Bearer eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9...
```

Pour webserv : non requis. Si vous implémentez l'authentification basique, décodez et vérifiez ; sinon, transmettez au CGI.

### `Cookie`
État stocké côté client renvoyé au serveur. Voir [fichier 12](12_COOKIES_SESSIONS.md).

```
Cookie: sessionid=abc123; theme=dark
```

Pour webserv : **bonus uniquement** selon le sujet. Si vous implémentez les sessions, parsez ce header. Sinon, transmettez au CGI (qui gérera son propre suivi de session).

### `Referer` (sic — faute d'orthographe historique)
URL depuis laquelle le client est arrivé.

```
Referer: https://example.com/page1
```

Pour webserv : lisez-le pour les logs, ignorez pour le comportement.

### `Content-Length` (sur les requêtes avec body)
Nombre d'octets dans le body. Voir [fichier 06](06_FRAMING.md).

```
Content-Length: 137
```

Pour webserv : **lisez ceci sur les requêtes POST/PUT** pour savoir quelle quantité de body consommer.

### `Content-Type` (sur les requêtes avec body)
Type MIME du body.

```
Content-Type: application/json
Content-Type: multipart/form-data; boundary=----WebKitFormBoundaryABCDE
Content-Type: application/x-www-form-urlencoded
```

Pour webserv : crucial pour le parsing de téléversement de fichiers (file upload). `multipart/form-data` vous oblige à trouver le paramètre `boundary=` et à découper le body à partir de celui-ci.

---

## Headers de réponse (Response headers)

### `Server`
Identifie votre serveur.

```
Server: webserv/1.0
```

À toujours inclure. De la publicité gratuite.

### `Location`
URL pour les redirections (3xx) ou emplacement d'une ressource nouvellement créée (201).

```
Location: https://example.com/new-path
Location: /resources/42
```

Les URL absolues et relatives sont toutes deux valides. Obligatoire sur les réponses 3xx.

### `Allow`
Méthodes autorisées sur cette ressource — **obligatoire sur les réponses 405**.

```
Allow: GET, POST, DELETE
```

### `Set-Cookie`
Définit un cookie sur le client. Voir [fichier 12](12_COOKIES_SESSIONS.md).

```
Set-Cookie: sessionid=abc123; Path=/; HttpOnly; Max-Age=3600
```

Pour webserv : bonus uniquement.

### `WWW-Authenticate`
Envoyé avec une 401 pour indiquer au client quel schéma d'authentification utiliser.

```
WWW-Authenticate: Basic realm="Access to staging"
```

Non requis par webserv.

### `Content-Length` (sur les réponses avec body)
**À toujours inclure** sauf si vous utilisez `Transfer-Encoding: chunked`.

```
Content-Length: 8421
```

Sans cela, le client ne sait pas quand le body se termine — ce qui convient si vous fermez la connexion, mais pose problème pour le keep-alive.

### `Content-Type` (sur les réponses avec body)
**À toujours inclure** lorsqu'il y a un body.

```
Content-Type: text/html; charset=utf-8
Content-Type: application/octet-stream
```

Pour les fichiers statiques : déduire à partir de l'extension. Maintenez une petite table de correspondance :

```cpp
// pseudo-table, build a std::map<std::string,std::string>
".html"  → "text/html; charset=utf-8"
".css"   → "text/css"
".js"    → "application/javascript"
".png"   → "image/png"
".jpg"   → "image/jpeg"
".gif"   → "image/gif"
".json"  → "application/json"
".txt"   → "text/plain; charset=utf-8"
default  → "application/octet-stream"
```

Voir [fichier 09](09_CONTENT_NEGOTIATION.md) pour la liste complète des types MIME courants.

### `Content-Encoding`
Compression du body. **Ce n'est pas la même chose que Transfer-Encoding.**

```
Content-Encoding: gzip
```

Pour webserv : ignorez. Hors sujet.

### `Last-Modified`
Date de dernière modification de la ressource. Utile avec le `GET` conditionnel (voir [fichier 11](11_CACHING.md)).

```
Last-Modified: Tue, 15 Apr 2025 10:13:22 GMT
```

Pour les fichiers statiques de webserv : déduire du `st_mtime` issu de `stat()`.

### `ETag`
Jeton opaque identifiant la version actuelle d'une ressource. Voir [fichier 11](11_CACHING.md).

```
ETag: "abc123"
```

Non requis par webserv.

---

## Headers d'entité / payload (apparaissent du côté qui contient le body)

Couvert ci-dessus avec leur côté associé : `Content-Length`, `Content-Type`, `Content-Encoding`, `Transfer-Encoding`.

Headers supplémentaires rarement nécessaires :
- `Content-Disposition` — suggère un nom de fichier pour les téléchargements (`attachment; filename="x.pdf"`). Utile pour les endpoints de serveurs de fichiers.
- `Content-Language`, `Content-Location` — cas d'usage rares.

---

## Headers personnalisés / d'extension (Custom / extension headers)

Tout ce qui commence par un nom non standard est recevable. La convention historique est le préfixe `X-` (`X-Forwarded-For`, `X-Request-ID`), bien que `X-` ait été déprécié par la RFC 6648 au profit de noms simples.

Traitez tout header inconnu ainsi : **stockez-le**, ignorez-le pour le comportement, **transmettez-le au CGI** sous forme de variable d'environnement `HTTP_<NAME>` (en majuscules, les tirets remplacés par des underscores). Voir [fichier 14](14_CGI.md).

---

## Le minimum de response headers que webserv devrait toujours émettre

```
HTTP/1.1 <code> <reason>\r\n
Server: webserv/1.0\r\n
Date: <rfc1123 date>\r\n
Content-Type: <derived or text/html>\r\n
Content-Length: <byte count>\r\n
Connection: close\r\n          ← or keep-alive once you implement it
\r\n
<body>
```

Six headers, à chaque fois. Construction d'une fonction d'aide :

```cpp
// pseudo-code, illustrates structure
std::string build_response(int code,
                           const std::string& body,
                           const std::string& content_type) {
    std::stringstream s;
    s << "HTTP/1.1 " << code << " " << status_reason(code) << "\r\n";
    s << "Server: webserv/1.0\r\n";
    s << "Date: " << current_http_date() << "\r\n";
    s << "Content-Type: " << content_type << "\r\n";
    s << "Content-Length: " << body.size() << "\r\n";
    s << "Connection: close\r\n";
    s << "\r\n";
    s << body;
    return s.str();
}
```
Cette unique fonction gérera 80 % de vos réponses. Les cas particuliers (304, 204, 3xx avec `Location`, 405 avec `Allow`) ont droit à de petits wrappers.

---

## Modèle mental TL;DR

> Les headers sont la façon dont HTTP transporte les métadonnées. **Les noms sont insensibles à la casse, les valeurs sont trimmed, les répétitions peuvent être fusionnées avec des virgules (comma-folded).** Les quatre familles (général / requête / réponse / entité) forment une taxonomie utile, mais non contraignante. Pour webserv : un petit ensemble fixe par réponse (`Server`, `Date`, `Content-Type`, `Content-Length`, `Connection`) couvre presque tout ; n'en ajoutez d'autres que lors de l'implémentation d'une fonctionnalité spécifique.

**Passez à [`06_FRAMING.md`](06_FRAMING.md)** — le fichier le plus important de cette bibliothèque pour le projet webserv.