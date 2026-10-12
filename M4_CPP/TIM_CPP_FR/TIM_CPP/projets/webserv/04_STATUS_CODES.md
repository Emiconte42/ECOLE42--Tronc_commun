# 04 — Codes de statut

> *"Le code de statut est le résumé en une phrase par le serveur de ce qui vient de se passer."*

Trois chiffres dans la status line de la réponse. Choisis correctement, le client sait comment réagir sans même lire le body. Choisis n'importe comment, votre serveur fonctionne techniquement mais est sémantiquement cassé — et le sujet webserv dit explicitement **"Vos codes de statut de réponse HTTP doivent être précis."**

---

## Les cinq familles

| Plage | Famille | Signification |
|---|---|---|
| `1xx` | Informationnel | "Requête reçue, le traitement continue." Rare dans webserv. |
| `2xx` | Succès | "Ça a fonctionné." |
| `3xx` | Redirection | "Regardez ailleurs." |
| `4xx` | Erreur client | "Votre requête est invalide — ne réessayez pas telle quelle." |
| `5xx` | Erreur serveur | "J'ai planté. Réessayer pourrait fonctionner." |

Le premier chiffre vous indique vers qui pointer du doigt. Assimilez bien cela et vous pourrez deviner la plupart des codes de statut à partir du contexte.

---

## Les codes que vous utiliserez réellement

La spécification HTTP liste environ 60 codes. Webserv en a besoin d'une quinzaine. Voici la liste restreinte :

### 2xx — succès

| Code                 | Raison                                                                                  | Quand                                             |
| -------------------- | --------------------------------------------------------------------------------------- | ------------------------------------------------- |
| **`200 OK`**         | Le succès par défaut. Le `GET` a fonctionné, le body contient la ressource.             | La plupart des réponses réussies.                 |
| **`201 Created`**    | La ressource a été créée. Devrait inclure un header `Location:` pointant vers la nouvelle ressource. | Après un `POST` réussi qui crée quelque chose.   |
| **`204 No Content`** | Succès mais pas de body. Les headers se terminent toujours par `\r\n\r\n`, mais aucun octet après. | `DELETE` qui a réussi ; `PUT` qui a mis à jour.   |

### 3xx — redirection

Voir le [fichier 10](10_REDIRECTS.md) pour le tableau complet.

| Code                        | Raison                                                                                                                  | Quand                                                                           |
| --------------------------- | ----------------------------------------------------------------------------------------------------------------------- | ------------------------------------------------------------------------------- |
| **`301 Moved Permanently`** | La ressource est maintenant à l'URL indiquée dans `Location:`. Les navigateurs mettront à jour les favoris, les moteurs de recherche remplaceront l'entrée d'index. | Règle `redirect` du fichier de configuration, lorsqu'elle est permanente.       |
| **`302 Found`**             | Redirection temporaire. Le client doit refaire la requête à la nouvelle URL mais ne pas mémoriser le déplacement.      | La plupart des règles de config `redirect`. (`303` et `307` sont plus précises — voir fichier 10.) |
| **`304 Not Modified`**      | "Votre copie en cache est toujours valide." Pas de body.                                                                | Lors de l'implémentation de la mise en cache ([fichier 11](11_CACHING.md)). Non requis par webserv. |

### 4xx — erreur client

| Code | Raison | Quand |
|---|---|---|
| **`400 Bad Request`** | Requête malformée — votre parser n'a pas pu lui donner de sens. | Header `Host` manquant en HTTP/1.1, chunked encoding corrompu, ligne non terminée par `\r\n`, etc. |
| **`403 Forbidden`** | Vous avez compris la requête, vous avez trouvé la ressource, mais vous refusez de la servir. | Le fichier existe mais les permissions refusent la lecture ; listing de répertoire désactivé et pas de fichier index. |
| **`404 Not Found`** | La ressource n'existe pas. | La cible après application du `root` ne correspond pas à un fichier réel. |
| **`405 Method Not Allowed`** | Méthode connue mais non autorisée sur cette route. **Doit inclure un header `Allow:` listant ce qui est autorisé.** | La config de route indique `methods: GET POST`, le client a envoyé `DELETE`. |
| **`408 Request Timeout`** | Le client a ouvert la connexion mais a mis trop de temps à envoyer. | Nettoyage des connexions inactives. Pas strictement requis par le sujet mais bon pour la résilience. |
| **`411 Length Required`** | Body présent mais pas de `Content-Length`. | Optionnel, puisque vous pouvez aussi accepter le chunked. Certains serveurs l'imposent strictement. |
| **`413 Payload Too Large`** | Le body de la requête dépasse votre `client_max_body_size`. **Configurable via le fichier de configuration du sujet.** | La lecture du body dépasse la limite. |
| **`414 URI Too Long`** | Le chemin/query est plus grand que ce que vous acceptez de parser. | Protection optionnelle. |
| **`415 Unsupported Media Type`** | Le `Content-Type` du body est un type que vous ne pouvez pas gérer. | Rare dans webserv. |
| **`431 Request Header Fields Too Large`** | Section des headers plus grande que votre buffer. | Protection optionnelle. |

### 5xx — erreur serveur

| Code | Raison | Quand |
|---|---|---|
| **`500 Internal Server Error`** | Fourre-tout générique quand quelque chose s'est mal passé de votre côté. | Crash du CGI, erreur du système de fichiers non anticipée. |
| **`501 Not Implemented`** | La méthode est inconnue de votre serveur. | Le client a envoyé `PROPFIND` ou un truc absurde — vous ne le reconnaissez pas. |
| **`502 Bad Gateway`** | Un CGI que vous avez lancé a crashé ou renvoyé n'importe quoi. | Le CGI a renvoyé une sortie non-HTTP, ou aucune sortie du tout. |
| **`503 Service Unavailable`** | Vous êtes surchargé ou sur le point de vous éteindre. | Optionnel. |
| **`504 Gateway Timeout`** | Le CGI a mis trop de temps. | Requis si vous implémentez des timeouts CGI. |
| **`505 HTTP Version Not Supported`** | La requête utilisait `HTTP/2.0` ou une version que vous ne parlez pas. | Plus poli à envoyer que de simplement fermer la connexion. |

---

## Choisir le bon — arbre de décision

```
La requête du client a-t-elle du sens en tant qu'octets ?
├── Non → 400 Bad Request
└── Oui
    │
    La méthode était-elle reconnue ?
    ├── Non → 501 Not Implemented
    └── Oui
        │
        La méthode est-elle autorisée sur cette route ?
        ├── Non → 405 Method Not Allowed (avec le header Allow:)
        └── Oui
            │
            La ressource existe-t-elle ?
            ├── Non → 404 Not Found
            └── Oui
                │
                Êtes-vous autorisé à y accéder ?
                ├── Non → 403 Forbidden
                └── Oui
                    │
                    Tout ce qui suit a-t-il réussi ?
                    ├── Non → 500 / 502 / 504
                    └── Oui → 200 / 201 / 204 / 3xx
```

Cet arbre prend en charge 95 % de vos décisions relatives aux codes de statut.

---

## Reason phrases — quoi mettre après le numéro

Les phrases standard sont :

| Code | Phrase |
|---|---|
| 200 | `OK` |
| 201 | `Created` |
| 204 | `No Content` |
| 301 | `Moved Permanently` |
| 302 | `Found` |
| 400 | `Bad Request` |
| 403 | `Forbidden` |
| 404 | `Not Found` |
| 405 | `Method Not Allowed` |
| 413 | `Payload Too Large` |
| 500 | `Internal Server Error` |
| 501 | `Not Implemented` |
| 502 | `Bad Gateway` |
| 505 | `HTTP Version Not Supported` |

> En toute rigueur, les clients devraient ignorer la formulation et se fier uniquement au numéro. En pratique : tenez-vous-en aux phrases standard. Certains outils basés sur grep effectuent des correspondances dessus.

Une fonction de correspondance en C++98 :

```cpp
const char* status_reason(int code) {
    switch (code) {
        case 200: return "OK";
        case 201: return "Created";
        case 204: return "No Content";
        case 301: return "Moved Permanently";
        case 302: return "Found";
        case 400: return "Bad Request";
        case 403: return "Forbidden";
        case 404: return "Not Found";
        case 405: return "Method Not Allowed";
        case 413: return "Payload Too Large";
        case 500: return "Internal Server Error";
        case 501: return "Not Implemented";
        case 502: return "Bad Gateway";
        case 505: return "HTTP Version Not Supported";
        default:  return "Unknown";
    }
}
```

Ce sera l'une des premières fonctions que vous écrirez. Déplacez-la dans un header afin que le reste de la base de code puisse y accéder.

---

## Pages d'erreur — ce que demande le sujet

Le sujet indique : *"Votre serveur doit avoir des pages d'erreur par défaut si aucune n'est fournie."*

Implémentation :

1. Le fichier de config permet à l'utilisateur de spécifier des pages d'erreur par code de statut :
   ```
   error_page 404 /errors/404.html;
   error_page 500 502 503 504 /errors/5xx.html;
   ```
2. Lorsque vous devez envoyer une erreur, vérifiez la config : une page personnalisée est-elle configurée ? Si oui, lisez-la et envoyez-la (avec le code de statut de **l'erreur**, pas 200).
3. Si aucune page personnalisée n'est configurée, envoyez une page par défaut intégrée — un minuscule body HTML comme :
   ```html
   <html><head><title>404 Not Found</title></head>
   <body><h1>404 Not Found</h1><p>The requested resource was not found.</p></body></html>
   ```

Créez un helper qui les génère à la volée afin de ne pas avoir besoin de 10 fichiers de pages par défaut dans votre dépôt.

---

## Bugs courants concernant les codes de statut

| Bug | Correction |
|---|---|
| Envoyer `200 OK` avec un body HTML qui *dit* "404 Not Found" | C'est la status line qui compte. Définissez le code de statut réel. |
| Oublier le header `Allow:` sur un 405 | Incluez toujours `Allow: GET, POST, DELETE` (ou ce que la route autorise). |
| Renvoyer 500 pour les erreurs du client | Si la requête était malformée, c'est 400. S'ils ont demandé quelque chose qui n'existe pas, c'est 404. 500 signifie que *votre code* a échoué. |
| Renvoyer 200 avec un body vide lorsque vous voulez dire "supprimé" | Utilisez 204 No Content — et assurez-vous de n'envoyer aucun octet de body. |
| Renvoyer 404 lorsque la *route* n'existe pas | Même code, c'est très bien. 404 couvre à la fois "pas de route" et "la route existe mais le fichier est manquant." |
| Renvoyer 405 lorsque la *route* n'existe pas | Incorrect — 405 implique que la ressource existe, simplement pas via cette méthode. Utilisez 404. |
---

## Modèle mental TL;DR

> Premier chiffre = attribution des responsabilités. Deuxième et troisième chiffres = spécificité. **Choisissez le code le plus spécifique qui soit vrai.** 200 est le succès par défaut ; 400/404/405/500 couvrent la plupart des cas d'erreur. Envoyez toujours un body avec les erreurs (page par défaut ou configurée) — les clients l'affichent à l'utilisateur.

**Continuez vers [`05_HEADERS.md`](05_HEADERS.md)** pour le catalogue des headers qui portent la sémantique réelle.