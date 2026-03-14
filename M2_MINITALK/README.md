*This project has been created as part of the 42 curriculum by emiconte.*

# MINITALK

## Description

Minitalk est un projet de communication entre 2 programmes, un client et un serveur.

L'objectif est de creer un protocole de communication simple en utilisant uniquement des signaux.

Seul 2 signaux sont autorises:
- SIGUSR1
- SIGUSR2

le server doit :
- Etre lance en premier
- Afficher son PID
- Recevoir des signaux a tout moment
- Reconstituer et afficher le message envoye

le client doit:
- Prendre 2 arguments : le PID du serveur et une chaine de caractere
- Envoyer le code et encoder le message
- Envoyer chaque bit sous forme de signal

---

## Instructions

### Compilation

make

### Exécution
Executer le serveur : ./serveur

Executer le client : ./client PID "message a envoyer"

### Nettoyage

make clean
make fclean
make re

---

## Ressources

Comment utiliser les signaux : 
https://creatorsarea.fr/tutoriels/comment-utiliser-les-signaux-unix/creer-votre-propre-protocole-de-communication-client-serveur

Minitalk: un système de communication client-serveur simple:
https://medium.com/@leogaudin/minitalk-a-simple-client-server-communication-system-57ccc2ae5178

---

## Utilisation de l’IA

L’IA a été utilisée comme "professeur" uniquement pour clarifier les principes sur les signaux.

Aucun code source de production n’a été généré automatiquement.