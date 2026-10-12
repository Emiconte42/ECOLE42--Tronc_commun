# 09 — Types de contenu & Négociation

> *"Le corps n'est que des octets. Le Content-Type est la façon dont le client sait quoi en faire."*

Un fichier `.png` ressemble exactement à un fichier `.txt` au niveau des octets si l'on plisse les yeux — les deux sont des séquences de `unsigned char`. Le header `Content-Type` est ce qui indique au navigateur « affiche ceci comme une image » ou « affiche ceci comme du texte ». Si vous vous trompez, votre CSS arrivera sous forme de texte brut, vos images comme rien du tout, et votre JavaScript comme une page HTML.

---

## Ce que sont les types MIME

**MIME** signifie *Multipurpose Internet Mail Extensions* — ils ont été inventés pour les e-mails et réutilisés par HTTP. Format :

```
type/subtype
type/subtype; parameter=value
type/subtype; parameter=value; parameter=value
```

Exemples :

```
text/html
text/html; charset=utf-8
application/json
multipart/form-data; boundary=----WebKitFormBoundaryX
image/png
```

La liste standard est maintenue par l'IANA et en compte des milliers. Vous n'en avez besoin que d'environ 15.

---

## Les types MIME que webserv doit servir

Une table minimale pour le service de fichiers statiques :

| Extension | Type MIME | Notes |
|---|---|---|
| `.html`, `.htm` | `text/html; charset=utf-8` | Incluez toujours le charset pour les types de texte. |
| `.css` | `text/css` | |
| `.js`, `.mjs` | `application/javascript` | Certains serveurs indiquent `text/javascript` ; les deux fonctionnent. |
| `.json` | `application/json` | |
| `.xml` | `application/xml` | |
| `.txt`, `.md` | `text/plain; charset=utf-8` | |
| `.png` | `image/png` | |
| `.jpg`, `.jpeg` | `image/jpeg` | |
| `.gif` | `image/gif` | |
| `.svg` | `image/svg+xml` | |
| `.webp` | `image/webp` | |
| `.ico` | `image/x-icon` | Les navigateurs demandent `/favicon.ico` par défaut. |
| `.pdf` | `application/pdf` | |
| `.zip` | `application/zip` | |
| `.mp4` | `video/mp4` | |
| `.mp3` | `audio/mpeg` | |
| tout le reste | `application/octet-stream` | "Blob binaire inconnu" — le navigateur proposera de le télécharger. |

### Pourquoi `application/octet-stream` est la valeur par défaut sûre

Si vous ne reconnaissez pas l'extension, envoyez `application/octet-stream`. Le navigateur va soit l'enregistrer sous forme de téléchargement, soit refuser de l'afficher en ligne — deux issues sûres. La mauvaise approche consiste à deviner `text/html` et à faire afficher un fichier binaire sous forme de texte corrompu.

---

## Charset — quand l'inclure

Pour les types **texte** (`text/html`, `text/css`, `text/plain`, `text/javascript`), incluez `; charset=utf-8`. Sans cela, les navigateurs risquent de se tromper en le devinant et d'afficher `é` sous la forme `Ã©`.

Pour les types binaires (`image/*`, `application/*`), le charset n'a aucun sens — omettez-le.

Pour `application/json`, la spécification impose implicitement l'UTF-8 ; vous n'avez pas strictement besoin de `charset=utf-8`, mais cela ne fait pas de mal.

---

## Implémenter une table de correspondance MIME

Une `std::map<std::string, std::string>` faisant correspondre l'extension au type :

```cpp
// pseudo-code
std::map<std::string, std::string> build_mime_table() {
    std::map<std::string, std::string> m;
    m[".html"] = "text/html; charset=utf-8";
    m[".css"]  = "text/css";
    m[".js"]   = "application/javascript";
    m[".json"] = "application/json";
    m[".png"]  = "image/png";
    m[".jpg"]  = "image/jpeg";
    m[".jpeg"] = "image/jpeg";
    m[".gif"]  = "image/gif";
    m[".txt"]  = "text/plain; charset=utf-8";
    m[".pdf"]  = "application/pdf";
    return m;
}

std::string mime_for(const std::string& path) {
    size_t dot = path.find_last_of('.');
    if (dot == std::string::npos) return "application/octet-stream";

    std::string ext = path.substr(dot);
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

    static const std::map<std::string, std::string> table = build_mime_table();
    std::map<std::string, std::string>::const_iterator it = table.find(ext);
    return it == table.end() ? "application/octet-stream" : it->second;
}
```

**Piège C++98 :** une `static const std::map<…>` initialisée par un appel de fonction est valide en C++98 (l'initialisation `static` n'a lieu qu'une seule fois). Mais vous ne pouvez pas utiliser les listes d'initialisation entre accolades ou `std::initializer_list` pour la remplir — cela relève de C++11. D'où les appels explicites `m[...] = ...;`.

---

## Négociation de contenu — le header `Accept`

Les clients peuvent exprimer leurs préférences via le header `Accept` :

```
Accept: text/html, application/xhtml+xml, application/xml;q=0.9, */*;q=0.8
```

À lire comme : « Je préfère le HTML, puis le XHTML, puis le XML (poids 0.9), puis n'importe quoi (poids 0.8). »

Un serveur qui négocie choisirait le type ayant le poids le plus élevé qu'il est capable de produire. **Pour webserv, vous ne négociez pas** — l'extension du fichier détermine le type. Contentez-vous d'**ignorer `Accept`** pour le service statique et transmettez-le au CGI via `HTTP_ACCEPT`.

La même règle s'applique à `Accept-Language`, `Accept-Encoding`, `Accept-Charset` — ignorez-les pour le statique, transmettez-les pour le CGI.

---

## Types de contenu du corps de requête — ce qu'apporte `POST`

Lorsqu'un client effectue un `POST` de données, le `Content-Type` du corps vous indique le format :

### `application/x-www-form-urlencoded`

La valeur par défaut pour les soumissions de `<form>` HTML :

```
POST /login HTTP/1.1\r\n
Content-Type: application/x-www-form-urlencoded\r\n
Content-Length: 31\r\n
\r\n
username=alice&password=hunter2
```

Même format qu'une query string d'URL. Décodez-le avec le parser de query-string du [fichier 08](08_URLS.md).

### `multipart/form-data`

Utilisé pour les téléversements de fichiers. Le corps est segmenté par une **chaîne frontière (boundary)** :

```
POST /upload HTTP/1.1\r\n
Content-Type: multipart/form-data; boundary=----WebKitFormBoundaryABCDE\r\n
Content-Length: 384\r\n
\r\n
------WebKitFormBoundaryABCDE\r\n
Content-Disposition: form-data; name="username"\r\n
\r\n
alice\r\n
------WebKitFormBoundaryABCDE\r\n
Content-Disposition: form-data; name="file"; filename="photo.jpg"\r\n
Content-Type: image/jpeg\r\n
\r\n
<binary JPEG bytes>\r\n
------WebKitFormBoundaryABCDE--\r\n
```

Structure :
- Le paramètre `boundary` dans le `Content-Type` est la chaîne séparatrice (avec un préfixe `--` à chaque occurrence).
- Chaque partie a ses propres headers (en particulier `Content-Disposition` avec `name="..."` et optionnellement `filename="..."`), suivis de ses données.
- La frontière finale comporte un `--` final (`------WebKitFormBoundaryABCDE--`).

**Pour webserv :** vous aurez besoin d'un parser multipart si l'exigence du sujet « le client doit pouvoir téléverser des fichiers » requiert `multipart/form-data` (le mécanisme standard des formulaires de navigateur). Plan : découper le corps sur la frontière, parser les headers de chaque partie, extraire le nom de fichier et le contenu, écrire sur le disque.

C'est la tâche de parsing la plus délicate dans webserv. Testez-la avec de véritables téléversements de navigateur.

### `application/json`

```
POST /api HTTP/1.1\r\n
Content-Type: application/json\r\n
Content-Length: 24\r\n
\r\n
{"key":"value","n":42}
```

Pour webserv : **ne parsez pas le JSON vous-même**. Transmettez le corps au CGI ; le script CGI (Python, PHP) le parsera. C++98 ne dispose d'aucun parser JSON dans la bibliothèque standard, et en écrire un à partir de zéro est largement hors du périmètre de webserv.

### `text/plain`

```
POST /note HTTP/1.1\r\n
Content-Type: text/plain; charset=utf-8\r\n
Content-Length: 27\r\n
\r\n
Just a plain text body here
```

Facile. Le corps est du texte brut. Utile pour tester le CGI.

### `application/octet-stream` ou non reconnu

Traitez-le comme des octets bruts. Transmettez-le tel quel au CGI.

---

## Le header `Vary` (avancé — à ignorer pour webserv)

Si un serveur renvoie des réponses différentes pour la même URL en fonction d'un header de requête (par exemple, un contenu différent selon l'`Accept-Language`), il doit envoyer `Vary: Accept-Language` afin que les caches ne servent pas la mauvaise version en cache.

Webserv ne négocie pas, vous n'avez donc pas besoin de `Vary`. Ignorez.

---

## Bugs courants

| Bug | Symptôme | Correction |
|---|---|---|
| Envoyer `text/plain` pour des fichiers HTML | Le navigateur affiche le code source HTML sous forme de texte au lieu de le rendre | Recherchez le bon MIME par extension. |
| Oublier `charset=utf-8` | Les caractères français/japonais s'affichent sous forme de mojibake | Ajoutez `; charset=utf-8` aux types text/*. |
| Envoyer `text/html` pour des extensions inconnues | Le navigateur tente d'afficher des données binaires corrompues | Utilisez par défaut `application/octet-stream`. |
| Le parser multipart découpe littéralement sur `boundary` | Manque les véritables frontières car la spécification ajoute un préfixe `--` | Cherchez `--` + boundary, et pas seulement boundary. |
| Oublier totalement de définir `Content-Type` | Le navigateur devine (sniffe) le type ; parfois incorrectement, parfois dangereusement | Incluez toujours `Content-Type` sur les réponses comportant un corps. |

---

## Modèle mental TL;DR

> Le `Content-Type` est une étiquette qui indique au destinataire comment interpréter les octets du corps. Pour webserv : une petite table extension-vers-MIME couvre le service statique ; le parsing multipart est nécessaire pour les téléversements de fichiers ; tout le reste peut être transmis textuellement au CGI. **Ignorez `Accept` pour les fichiers statiques ; transmettez-le au CGI.**

**Passez à [`10_REDIRECTS.md`](10_REDIRECTS.md)** pour le détail de la famille des 3xx.
