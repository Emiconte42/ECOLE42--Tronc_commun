# 🌐 HTTP & Webserv — Support Library

> *"Un serveur n'est rien d'autre qu'un parser poli qui sait quand arrêter de lire et quand commencer à écrire."*

Tout ce dont vous avez besoin pour **comprendre HTTP au niveau d'un praticien**, afin que lorsque le sujet `webserv` de 42 arrive sur votre bureau, il se lise comme une spécification que vous connaissez déjà — et non comme une langue étrangère.

Cette bibliothèque est axée **d'abord sur le protocole, puis sur le projet** :
- Les fichiers `01` → `15` vous enseignent HTTP lui-même. Ils sont indépendants du langage.
- Les fichiers `16` → `17` appliquent HTTP au véritable projet C++98 `webserv`.
- Le [GLOSSARY](GLOSSARY.md) est là pour une recherche rapide quand vous oubliez ce que signifie `Transfer-Encoding: chunked` à 2 h du matin.

---

## 🗺️ Ordre de lecture

Si vous n'avez jamais écrit de serveur auparavant, lisez-les dans l'ordre. Chacun s'appuie sur le précédent.

| # | Fichier | Ce que vous apprenez |
|---|---|---|
| 1 | [`01_FUNDAMENTALS.md`](01_FUNDAMENTALS.md) | Ce qu'**est** HTTP : un protocole texte sans état au-dessus de TCP. Sa place dans la stack OSI / TCP-IP. Modèle requête-réponse. |
| 2 | [`02_MESSAGE_ANATOMY.md`](02_MESSAGE_ANATOMY.md) | Les quatre parties de chaque message HTTP : start-line, headers, ligne vide, body. Lu par votre parser, écrit par votre répondeur. |
| 3 | [`03_METHODS.md`](03_METHODS.md) | GET, POST, PUT, DELETE, HEAD, OPTIONS — ce que chacun *signifie*, ainsi que la sûreté, l'idempotence, la mise en cache. |
| 4 | [`04_STATUS_CODES.md`](04_STATUS_CODES.md) | Les cinq familles (`1xx`–`5xx`), les ~15 codes que vous utiliserez réellement, et comment choisir le bon. |
| 5 | [`05_HEADERS.md`](05_HEADERS.md) | Les headers qui comptent, regroupés par famille. Un catalogue que vous pouvez grepper lors de l'implémentation. |
| 6 | [`06_FRAMING.md`](06_FRAMING.md) | **Le fichier le plus important pour webserv.** Comment un serveur sait où se termine un body : `Content-Length` vs `Transfer-Encoding: chunked`. |
| 7 | [`07_CONNECTION.md`](07_CONNECTION.md) | Cycle de vie d'une connexion : `Connection: close`, keep-alive, pipelining. Pourquoi HTTP/1.1 est persistant par défaut. |
| 8 | [`08_URLS.md`](08_URLS.md) | Anatomie d'une URI (`scheme://host:port/path?query#fragment`), percent-encoding, parsing de query string. |
| 9 | [`09_CONTENT_NEGOTIATION.md`](09_CONTENT_NEGOTIATION.md) | Types MIME, `Content-Type` vs `Accept`, charsets, comment un client et un serveur s'accordent sur une représentation. |
| 10 | [`10_REDIRECTS.md`](10_REDIRECTS.md) | La famille `3xx` : 301, 302, 303, 307, 308 — quand chacun est approprié. |
| 11 | [`11_CACHING.md`](11_CACHING.md) | `Cache-Control`, `ETag`, `If-None-Match`, `304 Not Modified`. Comment le même octet est envoyé moins souvent. |
| 12 | [`12_COOKIES_SESSIONS.md`](12_COOKIES_SESSIONS.md) | Comment un protocole sans état acquiert un état. `Set-Cookie`, attributs, sessions vs tokens. |
| 13 | [`13_HTTPS_TLS.md`](13_HTTPS_TLS.md) | Ce que HTTPS ajoute par-dessus HTTP. Juste assez pour se débrouiller — webserv ne nécessite pas d'implémenter TLS. |
| 14 | [`14_CGI.md`](14_CGI.md) | Comment un serveur délègue le traitement d'une requête à un programme externe. **Requis par le sujet webserv.** |
| 15 | [`15_TOOLS.md`](15_TOOLS.md) | Votre boîte à outils quotidienne : `curl -v`, `nc`, browser devtools, `wireshark`, `httpbin`. |
| 16 | [`16_TINY_SERVER_LAB.md`](16_TINY_SERVER_LAB.md) | **Construisez le serveur HTTP le plus bête possible en ~50 lignes de C++**. Connexion unique, une seule requête, réponse fixe. Démystifie tout le protocole. |
| 17 | [`17_WEBSERV_SUBJECT.md`](17_WEBSERV_SUBJECT.md) | Ce que le sujet 42 webserv **exige réellement** : config façon nginx, multiplexage d'E/S via `poll`/`select`/`kqueue`, CGI, pages d'erreur, la liste complète des fonctionnalités. |
| 18 | [`18_SOCKETS_AND_FDS.md`](18_SOCKETS_AND_FDS.md) | La couche OS sous-jacente à HTTP : file descriptors, cycle de vie des sockets, TCP en tant que stream, isolation des fd par client, et la boucle d'événements `poll()`. |
| 19 | [`19_CONFIGURATION.md`](19_CONFIGURATION.md) | Une config de style NGINX prête à copier : la minimale qui démarre, une réaliste et annotée, comment une requête est résolue par rapport à elle, et des notes de parsing C++98. |
| 20 | [`20_HTTP.md`](20_HTTP.md) | **Le hub HTTP.** Vue d'ensemble sur un écran + correspondance avec `01`–`12`, les notes de conception transversales (sans état vs connexion ouverte ; parser→dispatcher→répondre), et gestion des versions (1.0 vs 1.1 : l'enum + trois divergences de comportement). |

Vous êtes déjà en train de coder ? Rendez-vous à la section [**Par partie d'implémentation**](#%F0%9F%A7%A9%20By%20implementation%20part) ci-dessous — les mêmes fichiers regroupés selon le module sur lequel vous travaillez.

Référence rapide : [`GLOSSARY.md`](GLOSSARY.md).
**Conseils pratiques & outils de débogage :** [`TIPS.md`](TIPS.md) — outils, habitudes de journalisation, ordre de développement et pièges courants.
**Tests de charge & stress :** [`TESTS.md`](TESTS.md) — `ab` & `siege` en profondeur, et les vérifications de disponibilité/leaks/fd que l'évaluation exécute sous charge.
**Plan d'exécution sur une page :** [`OVERVIEW.md`](OVERVIEW.md) — épinglez ceci à votre mur.
**Référence des syscalls :** [`functions/INDEX.md`](functions/INDEX.md) — chaque fonction autorisée expliquée au niveau du kernel, avec des exemples spécifiques à webserv.

---

## 🧩 By implementation part

Le tableau ci-dessus correspond à l'ordre d'**apprentissage** (le protocole d'abord). Ceci est l'ordre de **construction** — les mêmes fichiers regroupés par composant de `webserv` sur lequel vous travaillez. Lorsque vous êtes plongé au cœur d'un module, commencez par ici.

### Server handling — sockets, the event loop, connections
La couche OS et le cœur non-bloquant. Tout ce qui déplace des octets avant que HTTP n'intervienne.

- [`18_SOCKETS_AND_FDS.md`](18_SOCKETS_AND_FDS.md) — fd, cycle de vie des sockets, TCP en tant que stream, la boucle poll *(le modèle mental)*
- [`16_TINY_SERVER_LAB.md`](16_TINY_SERVER_LAB.md) — le plus petit serveur qui fonctionne ; construisez celui-ci en premier
- [`07_CONNECTION.md`](07_CONNECTION.md) — keep-alive, `poll()` unique, la règle interdisant errno
- [`functions/01_SOCKET_LIFECYCLE.md`](functions/01_SOCKET_LIFECYCLE.md) — `socket`/`bind`/`listen`/`accept`/`setsockopt`
- [`functions/02_IO_MULTIPLEXING.md`](functions/02_IO_MULTIPLEXING.md) — `poll`/`select`/`kqueue`/`fcntl`
- [`POLL_DEEP_DIVE.md`](../../../../LEVEL5/WebServTest/notes/POLL_DEEP_DIVE.md) — **analyse approfondie :** la boucle `poll()` ligne par ligne — mécanique côté kernel, le tableau pollfd dynamique + backlog, nettoyage *(note sandbox, complémentaire à `functions/02`)*
- [`functions/03_DATA_TRANSFER.md`](functions/03_DATA_TRANSFER.md) — `read`/`write`/`send`/`recv`, lectures et écritures partielles (short reads & writes)

### HTTP parsing — raw bytes → structured request, structured response → bytes
- [`02_MESSAGE_ANATOMY.md`](02_MESSAGE_ANATOMY.md) — les quatre parties de chaque message
- [`05_HEADERS.md`](05_HEADERS.md) — les headers qui comptent, par famille
- [`06_FRAMING.md`](06_FRAMING.md) — **où se termine un body :** `Content-Length` vs chunked *(la partie la plus difficile)*
- [`20_HTTP.md`](20_HTTP.md) §4 — gestion de 1.0 vs 1.1 : l'enum de version et ses trois divergences de comportement
- [`03_METHODS.md`](03_METHODS.md) — sémantique de GET/POST/DELETE, sûreté, idempotence
- [`04_STATUS_CODES.md`](04_STATUS_CODES.md) — choisir le bon code

### Routing & static serving — match a request, resolve a path, send a file
- [`08_URLS.md`](08_URLS.md) — anatomie d'une URI, percent-encoding, query strings
- [`09_CONTENT_NEGOTIATION.md`](09_CONTENT_NEGOTIATION.md) — types MIME, `Content-Type`
- [`10_REDIRECTS.md`](10_REDIRECTS.md) — la famille `3xx` pour le `return` par route
- [`11_CACHING.md`](11_CACHING.md) — `ETag`, `304` *(optionnel, mais des gains faciles)*
- [`functions/06_FILESYSTEM.md`](functions/06_FILESYSTEM.md) — `stat`/`open`/`opendir`/`readdir` pour les fichiers & autoindex

### CGI — delegate a request to an external program
- [`14_CGI.md`](14_CGI.md) — le contrat env/stdin/stdout, de bout en bout
- [`functions/05_PROCESS_AND_CGI.md`](functions/05_PROCESS_AND_CGI.md) — `fork`/`execve`/`pipe`/`dup2`/`waitpid`
- [`functions/04_ADDRESS_CONVERSION.md`](functions/04_ADDRESS_CONVERSION.md) — `getsockname`/`ntohs` pour `SERVER_NAME`/`SERVER_PORT`

### Config — read & validate the `.conf`
- [`19_CONFIGURATION.md`](19_CONFIGURATION.md) — **une configuration fonctionnelle annotée prête à copier**, plus le cheminement de résolution de requête et des notes de parsing C++98
- [`17_WEBSERV_SUBJECT.md`](17_WEBSERV_SUBJECT.md) §Configuration — les directives de style nginx que vous devez supporter
- [`TIPS.md`](TIPS.md) §Config file — ce qu'il faut bien cerner avant d'écrire le parser

### Cross-cutting references — reach for these from any part
- [`libraries/INDEX.md`](libraries/INDEX.md) — **référence des headers :** ce que chaque `#include` vous apporte (le *quoi*)
- [`functions/INDEX.md`](functions/INDEX.md) — **référence des fonctions :** comment chaque appel se comporte au niveau du kernel (le *comment*)
- [`functions/07_ERRORS.md`](functions/07_ERRORS.md) — rigueur sur `strerror`/`gai_strerror`/`errno`
- [`15_TOOLS.md`](15_TOOLS.md) + [`TIPS.md`](TIPS.md) — boîte à outils de débogage et bonnes habitudes
- [`GLOSSARY.md`](GLOSSARY.md) — recherche de A à Z

> **Les trois niveaux de référence :** `libraries/` répond à *"qu'est-ce que ce header expose ?"*, `functions/` répond à *"comment cet appel se comporte-t-il ?"*, et les fichiers conceptuels (`18`, `16`, la documentation du protocole) répondent à *"pourquoi cela s'articule-t-il ainsi ?"*. Lorsque deux d'entre eux mentionnent la même chose, le plus approfondi détient l'explication complète et les autres pointent vers lui.

---

## ✅ Validation checklist — "Practitioner" level

Vous êtes prêt à démarrer `webserv` lorsque vous pouvez :
- [ ] Écrire une requête HTTP/1.1 brute à la main dans `nc` et obtenir une vraie réponse
- [ ] Lire une réponse HTTP brute dans la sortie de `curl -v` et expliquer chaque ligne
- [ ] Expliquer la différence entre `Content-Length` et `Transfer-Encoding: chunked` et **parser les deux**
- [ ] Nommer les méthodes qui sont sûres (safe), idempotentes et mettables en cache (cacheable)
- [ ] Choisir le bon code de statut pour 8 scénarios inventés sur 10
- [ ] Expliquer à quoi sert le keep-alive et pourquoi HTTP/1.1 l'active par défaut
- [ ] Écrire un serveur C++ de 50 lignes qui traite une requête correctement ([Lab 16](16_TINY_SERVER_LAB.md))
- [ ] Expliquer le fonctionnement de CGI de bout en bout — variables d'environnement, stdin, stdout

Si l'un de ces points est fragile, relis le document correspondant.

---

## 🎯 Le modèle mental

Cinq vérités qui sous-tendent tout le reste. Assimile-les et le reste n'est que du détail.

1. **HTTP est du texte sur TCP.** Chaque octet que tu lis et écris est, en principe, de l'ASCII lisible par un humain (jusqu'au body). Ouvre `telnet www.example.com 80`, tape une requête, observe la réponse. Il n'y a aucune magie.
2. **Sans état (stateless) par conception.** Chaque requête est indépendante. Le serveur ne se souvient de rien concernant les requêtes précédentes *sauf* si tu ajoutes un état par-dessus (cookies, sessions, tokens).
3. **Les messages ont une structure unique.** La requête et la réponse ne diffèrent que par la première ligne. Tout le reste en dessous — headers, ligne vide, body optionnel — est identique.
4. **Le framing est la partie la plus difficile.** Le protocole ne t'indique pas la longueur d'un body dans la ligne de requête. Tu apprends cette longueur via `Content-Length`, via `Transfer-Encoding: chunked`, ou en lisant jusqu'à ce que la connexion se ferme. Fais une erreur ici et la requête suivante ressemblera à du charabia. ([Fichier 06](06_FRAMING.md).)
5. **Le serveur est un parser + un dispatcher + un responder.** Parse une requête sous forme structurée, décide de l'action à mener (fichier statique ? CGI ? proxy ?), sérialise une réponse. Chaque serveur web au monde est une déclinaison de cette boucle.

---

## 📚 Programme d'étude suggéré

Pour un étudiant de 42 qui commence webserv dans ~1 mois :

| Semaine | Objectif | Fichiers |
|---|---|---|
| **1** | Comprendre le protocole de bout en bout | `01` → `06` + Lab `16` |
| **2** | Maîtriser les headers, le caching, la négociation | `07` → `12` |
| **3** | CGI + outils + expérimentations de micro-serveurs | `14`, `15`, refaire le Lab `16` de zéro |
| **4** | Lire le véritable sujet de webserv ; faire le lien avec cette bibliothèque | `17` puis **commencer à coder** |

`HTTPS_TLS` (fichier `13`) est une lecture optionnelle — utile pour l'ensemble de ta carrière, mais non requise pour le sujet webserv.

---

## 🔗 Liens connexes

- [`netpractice/INDEX.md`](../../../netpractice/INDEX.md) — la couche **en dessous** de HTTP (IP, routage). HTTP fonctionne sur TCP/IP ; netpractice enseigne la partie IP.
- [`cpp/io-errors/FSTREAM_GUIDE.md`](../../notions/io-errors/FSTREAM_GUIDE.md) — tu vas faire beaucoup d'I/O en flux dans webserv.
- [`cpp/tooling/MAKEFILE_CPP.md`](../../notions/tooling/MAKEFILE_CPP.md) — webserv comporte de nombreux fichiers ; un Makefile propre n'est pas négociable.
- [`meta/FLAGS.md`](../../../meta/FLAGS.md) — compile webserv avec le jeu de flags le plus strict recommandé par ce repo.

---

## 📌 Remarques sur cette bibliothèque

- **Fidèle à la spec mais pas verbeux.** Quand j'écris « la spec dit X », je fais référence aux RFC 7230/7231/7235 (la famille HTTP/1.1). Je ne cite pas les RFC en longueur — l'objectif est d'acquérir des modèles mentaux, pas une précision juridique.
- **Mises en garde C++98.** Lorsqu'un sujet présente un piège C++ spécifique à webserv (par ex. `std::stringstream` pour le parsing, pas de `std::stoi`), le fichier le signale directement.
- **Schémas tout en ASCII.** Ils s'affichent dans n'importe quel terminal, n'importe quel éditeur, n'importe quel visualiseur Markdown compatible Obsidian.
- **Renvois fréquents.** Quand le fichier `06` mentionne le chunked encoding, il renvoie au fichier `02` pour le header dans lequel il se trouve. Suis les liens au fil de ta lecture.