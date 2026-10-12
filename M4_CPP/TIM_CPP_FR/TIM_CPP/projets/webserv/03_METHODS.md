# 03 — Méthodes HTTP

> *"La méthode est un verbe. Le chemin est un nom. Ensemble, ils forment la phrase sur laquelle le client demande au serveur d'agir."*

Le premier élément (token) de chaque ligne de requête. HTTP définit une poignée de méthodes ; webserv exige que vous preniez en charge **au moins `GET`, `POST`, `DELETE`** — mais vous devez savoir ce que signifient les autres pour la couche d'analyse syntaxique (parsing) (vous les rejetterez proprement).

---

## Les méthodes en un coup d'œil

| Méthode   | Signification                                      | Safe | Idempotent | Cacheable | Webserv          |
| --------- | -------------------------------------------------- | ---- | ---------- | --------- | ---------------- |
| `GET`     | Récupérer une représentation d'une ressource       | ✅    | ✅          | ✅         | **obligatoire**  |
| `HEAD`    | Comme `GET` mais sans corps dans la réponse        | ✅    | ✅          | ✅         | optionnel, facile |
| `POST`    | Soumettre des données — le serveur décide quoi faire | ❌    | ❌          | ⚠️ rare   | **obligatoire**  |
| `PUT`     | Remplacer la ressource à une URL donnée            | ❌    | ✅          | ❌         | non requis       |
| `DELETE`  | Supprimer la ressource à une URL donnée            | ❌    | ✅          | ❌         | **obligatoire**  |
| `PATCH`   | Modification partielle                             | ❌    | ❌          | ❌         | non requis       |
| `OPTIONS` | Demander quelles méthodes sont autorisées          | ✅    | ✅          | ❌         | non requis       |
| `CONNECT` | Établir un tunnel TCP via un proxy                 | ❌    | ❌          | ❌         | ignorer          |
| `TRACE`   | Renvoyer la requête en écho                        | ✅    | ✅          | ❌         | ignorer          |

Trois termes de vocabulaire indispensables pour comprendre le tableau :

---

## Safe

Une méthode est dite **safe** si elle est *destinée* à être en lecture seule — c'est-à-dire que son invocation ne doit pas modifier l'état du serveur. Les robots d'indexation (web crawlers) peuvent appeler librement les méthodes safe.

- `GET`, `HEAD`, `OPTIONS`, `TRACE` sont safe.
- `POST`, `PUT`, `DELETE`, `PATCH` ne sont pas safe (elles *font* des choses).

> ⚠️ "Safe" est une **promesse d'intention**, pas une garantie. Une URL comme `GET /users/42/delete` est *techniquement* un GET mais n'est **pas safe** — et est largement considérée comme un antipattern précisément pour cette raison. La spécification HTTP suppose que les méthodes safe n'ont aucun effet de bord ; si vous enfreignez cette règle, les robots d'indexation supprimeront vos données à votre place.

---

## Idempotent

Une méthode est **idempotente** si effectuer la même requête **N fois** a exactement le même effet que de la faire **une seule fois**.

- `GET`, `HEAD`, `PUT`, `DELETE`, `OPTIONS`, `TRACE` sont idempotents.
- `POST`, `PATCH` ne le sont pas.

**Pourquoi cela compte pour les clients :** si une requête échoue (pépin réseau, timeout), le client peut **retenter** une requête idempotente en toute sécurité. Il ne peut pas retenter un `POST` sans risque — le premier a pu aboutir, et la nouvelle tentative créerait deux enregistrements.

**Exemples :**
- `DELETE /users/42` est idempotent : après le premier appel, l'utilisateur a disparu. Un second appel renvoie 404 (ou 204) — l'*état* reste le même.
- `PUT /users/42` avec un corps est idempotent : vous écrasez avec le même contenu à chaque fois.
- `POST /users` avec un corps n'est **pas** idempotent : chaque appel crée un nouvel utilisateur.

---

## Cacheable

Une méthode est **cacheable** si la réponse est autorisée à être stockée par des intermédiaires (navigateurs, proxys, CDN) et réutilisée pour des requêtes ultérieures.

- `GET`, `HEAD` sont toujours cacheables (sous réserve des en-têtes de réponse — voir le [fichier 11](11_CACHING.md)).
- `POST` est cacheable en théorie, mais presque jamais en pratique.
- `PUT`, `DELETE`, `PATCH` ne sont pas cacheables.

---

## Les méthodes en détail

### `GET`

**"Donne-moi la représentation de la ressource à cette URL."**

- Pas de corps de requête (les serveurs doivent l'ignorer s'il est présent).
- Le corps de la réponse contient la représentation (HTML, JSON, image, ...).
- Ne doit pas modifier l'état du serveur.

Pour webserv : chercher le fichier sur le disque (après avoir appliqué la règle `root` de la configuration), définir le `Content-Type` à partir de l'extension ([fichier 09](09_CONTENT_NEGOTIATION.md)), définir le `Content-Length`, envoyer.

### `HEAD`

**"Donne-moi la réponse que tu donnerais à un `GET` sur cette URL, mais sans le corps."**

- Mêmes en-têtes que le `GET` équivalent, mais sans corps.
- Utile pour les clients vérifiant des métadonnées : cette URL existe-t-elle ? Quelle est sa taille ? Quand a-t-elle été modifiée pour la dernière fois ?

Pour webserv : à implémenter en exécutant votre gestionnaire de GET et en omettant simplement l'envoi du corps. Fonctionnalité gratuite — cinq lignes de code supplémentaires.

### `POST`

**"Voici des données. Fais-en quelque chose. Le 'quelque chose' dépend de toi."**

- Comporte un corps de requête (en général).
- Le serveur peut créer une ressource, exécuter un CGI, ajouter une ligne à un log, envoyer un e-mail — n'importe quoi.
- La réponse doit indiquer ce qui s'est passé (`201 Created` avec un en-tête `Location:` pointant vers la nouvelle ressource, ou `200 OK` avec le résultat, ou `204 No Content`).

Pour webserv : route la requête vers un module d'upload de fichiers, un CGI, ou (rarement) un point d'accès de contenu statique. Le `Content-Type` du corps a son importance :
- `application/x-www-form-urlencoded` — champs de formulaire encodés dans l'URL
- `multipart/form-data` — téléversement de fichiers
- `application/json`, `text/plain`, etc. — transmis tels quels au CGI

### `DELETE`

**"Supprime la ressource à cette URL."**

- En règle générale, pas de corps de requête.
- Réponse : `200 OK` avec confirmation, `204 No Content` s'il n'y a rien à dire, ou `404` si elle n'existait pas.

Pour webserv : appliquer `unlink(2)` sur le fichier sous la directive `root` de la route (ou renvoyer 405 / 403 selon la configuration et les permissions). Le sujet stipule "vous devez au minimum... méthodes DELETE" — implémentez-la pour les fichiers, cela suffit.

### `PUT`

**"Remplace la ressource à cette URL par ce corps."**

Non requis par le sujet. Mais conceptuellement : prendre le corps, l'écrire sur le chemin du disque. Diffère de `POST` dans le sens où le client choisit l'URL (`PUT /files/foo.txt`) plutôt que le serveur (`POST /files` → le serveur lui donne un nom).

### `OPTIONS`

**"Quelles sont les méthodes autorisées sur cette URL ?"**

La réponse inclut un en-tête `Allow:` listant les méthodes. Très utilisé pour les requêtes de contrôle préalable (preflight) CORS dans les applications web. Non requis par webserv, mais si vous l'implémentez, renvoyez simplement `Allow: GET, POST, DELETE` (ou ce que permet la configuration de la route).

---

## Répartition des méthodes dans webserv

Le fichier de configuration définit les méthodes autorisées par route. Pseudo-code :

```cpp
// pseudo-code, not literal
const RouteConfig& route = config.match(request.target);

if (std::find(route.allowed_methods.begin(),
              route.allowed_methods.end(),
              request.method) == route.allowed_methods.end()) {
    return make_response(405, "Method Not Allowed",
                         /* Allow: */ join(route.allowed_methods, ", "));
}

if (request.method == "GET" || request.method == "HEAD") {
    handle_get(request, route);
} else if (request.method == "POST") {
    handle_post(request, route);
} else if (request.method == "DELETE") {
    handle_delete(request, route);
} else {
    return make_response(501, "Not Implemented");
}
```

**Deux codes de statut que vous DEVEZ utiliser ici :**
- `405 Method Not Allowed` lorsque la méthode est connue mais non autorisée sur cette route. Doit inclure l'en-tête `Allow:`.
- `501 Not Implemented` lorsque la méthode est totalement inconnue de votre serveur (`PROPFIND`, termes inventés, etc.).

---

## CGI et sémantique des méthodes

Lorsque la route correspond à un CGI (ex. `.php`), la sémantique de la méthode ne disparaît pas — elle est transmise au script CGI :

| Propriété de la requête | Variable d'environnement CGI |
|---|---|
| Méthode | `REQUEST_METHOD` |
| Chemin | `PATH_INFO`, `SCRIPT_NAME` |
| Query string | `QUERY_STRING` |
| Corps | redirigé (pipe) vers le stdin du CGI |
| Longueur du corps | `CONTENT_LENGTH` |
| Type de corps | `CONTENT_TYPE` |

Ainsi, un `POST /upload.php` implique : lancer un processus `php-cgi`, définir `REQUEST_METHOD=POST`, injecter le corps de la requête dans son stdin, lire son stdout, et le renvoyer en réponse.

Consultez le [fichier 14](14_CGI.md) pour tout le protocole lié au CGI.

---

## Modèle mental TL;DR

> La méthode indique au serveur **quel type d'action** est demandé. Safe signifie "aucun effet de bord attendu". Idempotent signifie "les réessais sont sans danger". Cacheable signifie "les intermédiaires peuvent stocker la réponse". Pour webserv : implémentez `GET`, `HEAD` (bonus facile), `POST` (pour les uploads + CGI), `DELETE` (pour les fichiers). Rejetez tout le reste avec `405` si la méthode est connue, `501` si elle est inconnue.

**Passez à [`04_STATUS_CODES.md`](04_STATUS_CODES.md)** pour apprendre à *répondre* à une requête.