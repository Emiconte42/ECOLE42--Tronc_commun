# 01 — Fondamentaux

> *"HTTP n'est que du texte brut sur un socket TCP. Tout le reste n'est que convention."*

Avant les headers et les codes d'état, vous devez avoir une idée claire de **ce qu'est réellement HTTP**, **où il se situe**, et **ce qu'il n'est pas**. Maîtrisez cela, et le reste de cette documentation se lira comme un commentaire sur un modèle que vous possédez déjà.

---

## Ce qu'est HTTP

**HTTP (HyperText Transfer Protocol)** est un protocole applicatif *orienté texte*, *sans état (stateless)*, *requête-réponse* qui s'exécute **au-dessus de TCP**.

Trois notions à analyser :

- **Orienté texte** — les octets du protocole que vous lisez sur le câble sont (principalement) de l'ASCII imprimable. Méthode, chemin, headers, code d'état — tous lisibles. Le body peut être binaire, mais le cadrage est textuel. C'est pourquoi vous pouvez faire `telnet www.example.com 80` et saisir une requête à la main.
- **Sans état (stateless)** — le protocole n'impose aucune mémoire des requêtes précédentes. Le serveur ne se "souvient" pas de vous d'une requête à l'autre. Les sessions, cookies, tokens sont de l'état superposé **au-dessus** de HTTP, pas gérés par lui.
- **Requête-réponse** — chaque interaction est constituée d'une requête d'un client suivie d'une réponse d'un serveur. Le serveur n'initie jamais. (Les WebSockets ne sont pas HTTP — c'est un protocole différent qui n'utilise HTTP que pour le handshake initial.)

---

## Sa place dans le stack

```
┌─────────────────────────────────────────┐
│  Application:  HTTP, FTP, SMTP, DNS…    │   ← vous êtes ici
├─────────────────────────────────────────┤
│  Transport:    TCP, UDP                 │   ← HTTP repose sur TCP
├─────────────────────────────────────────┤
│  Network:      IP (v4, v6)              │   ← voir la doc netpractice
├─────────────────────────────────────────┤
│  Link:         Ethernet, Wi-Fi…         │
└─────────────────────────────────────────┘
```

HTTP ne se soucie pas des adresses IP ou des paquets — il transmet un flux d'octets à TCP et TCP fait en sorte que cela fonctionne. Inversement, TCP ne se soucie pas que les octets soient du HTTP — il transporterait tout aussi volontiers une capture d'écran ou un coup d'échecs.

**Pour webserv, cela signifie :** vous passerez environ 30 % de votre code sur la plomberie des sockets (`socket`, `bind`, `listen`, `accept`, `poll`) et environ 70 % sur le parsing des octets qui arrivent sur ces sockets. La première partie est mécanique ; la seconde est la partie intéressante.

Référence croisée : [`netpractice/INDEX.md`](../../../netpractice/INDEX.md) pour ce qui se passe *sous* HTTP.

---

## La boucle requête-réponse en détail

```
   CLIENT                                   SERVER
   ──────                                   ──────
   socket()                                  socket()
                                             bind()
                                             listen()
   connect() ────────── TCP handshake ────── accept()
       │                                         │
       │  ┌──────────────────────────────┐       │
       └──┤ Requête HTTP (octets texte)  ├──────►│
          │ GET /index.html HTTP/1.1\r\n │       │ ← parser la requête
          │ Host: example.com\r\n        │       │ ← décider de l'action
          │ \r\n                         │       │ ← construire la réponse
          └──────────────────────────────┘       │
          ┌──────────────────────────────┐       │
       ◄──┤ Réponse HTTP (octets texte)  ├───────┘
          │ HTTP/1.1 200 OK\r\n          │
          │ Content-Length: 137\r\n      │
          │ Content-Type: text/html\r\n  │
          │ \r\n                         │
          │ <html>…</html>               │
          └──────────────────────────────┘
   close() ──── ou keep-alive, voir fichier 07
```

**Cinq éléments à noter :**

1. Le client initie toujours. Le serveur est un parseur-répondeur passif.
2. Les octets sur le réseau sont du texte jusqu'à (et y compris) la ligne vide. Après cela peuvent venir les octets du body (qui peuvent être n'importe quoi — du JSON, un JPEG, des octets bruts).
3. `\r\n` (CRLF, deux octets — `0x0D 0x0A`) termine chaque ligne des headers. **Pas `\n` seul**. Trompez-vous là-dessus et les clients vous ignoreront ou se bloqueront.
4. La ligne vide entre les headers et le body est *aussi* un `\r\n`. La frontière entre les headers et le body est donc `\r\n\r\n` — quatre octets.
5. Le serveur ne connaît pas la longueur du body à partir de la ligne de départ — il doit lire `Content-Length` ou `Transfer-Encoding: chunked` pour savoir quand s'arrêter. Voir [fichier 06](06_FRAMING.md).

---

## Versions HTTP — ce que chacune nous a apporté

| Version | Année | Ce qu'elle a apporté | Pertinence pour vous |
|---|---|---|---|
| **HTTP/0.9** | 1991 | Protocole sur une ligne : `GET /path` + body de réponse. Pas de headers, pas de code d'état. | Historique. Jamais vu en production. |
| **HTTP/1.0** | 1996 | Codes d'état, headers, méthodes. La connexion se ferme après chaque requête. | La **référence suggérée** par le sujet de 42. De nombreuses fonctionnalités manquent — pas de header `Host`, pas de chunked, pas de keep-alive par défaut. |
| **HTTP/1.1** | 1997 | Header `Host` (permet les serveurs virtuels), connexions persistantes par défaut, transfert chunked, pipelining, directives de mise en cache. | **Ce que parlent réellement les navigateurs.** Même si webserv "suggère" 1.0, vos réponses devront satisfaire les clients 1.1. La chaîne de version dans vos réponses doit être `HTTP/1.1`. |
| HTTP/2 | 2015 | Cadrage binaire, multiplexage, compression des headers, server push. | **Hors périmètre.** |
| HTTP/3 | 2022 | Fonctionne sur QUIC (UDP) au lieu de TCP. | **Hors périmètre.** |

> ⚠️ **Le piège du sujet webserv qui "suggère le 1.0".** Si vous implémentez littéralement et uniquement la sémantique 1.0, les navigateurs modernes se comporteront mal : ils enverront des headers `Host:` que vous ignorerez, ils enverront des bodies de requête `Transfer-Encoding: chunked` que vous ne pourrez pas parser, ils s'attendront à des connexions persistantes. **Implémentez suffisamment de 1.1 pour être compatible avec les navigateurs.** Plus précisément : gérez `Host`, gérez `Connection: close`, gérez `Content-Length` et les bodies de requête chunked.

---

## Ce que HTTP **n'est pas**

| Ce n'est pas… | Parce que… |
|---|---|
| Chiffré | Ça, c'est TLS. HTTP-sur-TLS est HTTPS. Voir [fichier 13](13_HTTPS_TLS.md). |
| Stateful | Les cookies/sessions sont superposés au-dessus. Le protocole lui-même ne retient rien. Voir [fichier 12](12_COOKIES_SESSIONS.md). |
| Authentifié par défaut | L'authentification est un problème à part entière (Basic, Bearer, OAuth…). Hors périmètre pour webserv. |
| Bidirectionnel | Le serveur ne peut pas envoyer de données au client sans sollicitation en HTTP/1.x. (Server-Sent Events et WebSockets sont des solutions de contournement.) |
| Streamé en temps réel | Chaque réponse est une "chose" unique. Vous pouvez la découper en chunks, mais la cadence requête/réponse fonctionne strictement au tour par tour. |

---

## Une première requête en octets bruts

La plus petite requête HTTP/1.1 valide :

```
GET / HTTP/1.1\r\n
Host: example.com\r\n
\r\n
```

Trois lignes : une ligne de requête, un seul header obligatoire (`Host`, depuis HTTP/1.1), une ligne vide. C'est tout. Essayez par vous-même :

```sh
printf 'GET / HTTP/1.1\r\nHost: example.com\r\n\r\n' | nc example.com 80
```

Vous verrez une vraie réponse HTTP revenir. **C'est ça, le protocole.** Tout le reste dans cette documentation revient à "quels champs, dans quel ordre, avec quelles règles."

---

## TL;DR modèle mental

> Un serveur HTTP est une fonction : `bytes_in → bytes_out`. L'entrée est un bloc de texte structuré ; la sortie est également un bloc de texte structuré. Les difficultés sont : (a) savoir quand l'entrée se termine, (b) décider de l'action à mener, (c) cadrer la sortie afin que le client sache quand *elle* se termine.

**Passez à [`02_MESSAGE_ANATOMY.md`](02_MESSAGE_ANATOMY.md)** pour décortiquer la structure de ces blocs de texte.