# C++ - Module 00

**Namespaces, classes, member functions, stdio streams, initialization lists, static, const, et d'autres bases**

*Résumé : Ce document contient les exercices du Module 00 des modules C++.*

*Version : 11.1*

---

## Sommaire

- I. Introduction ................................................. 2
- II. Règles générales ............................................ 3
- III. Instructions IA ............................................ 6
- IV. Exercice 00 : Megaphone .................................... 8
- V. Exercice 01 : My Awesome PhoneBook .......................... 9
- VI. Exercice 02 : The Job Of Your Dreams ....................... 11
- VII. Rendu et évaluation par les pairs .......................... 13

---

## Chapitre I — Introduction

*C++ est un langage de programmation généraliste créé par Bjarne Stroustrup comme une extension du langage de programmation C, ou « C avec des classes » (source : Wikipedia).*

L'objectif de ces modules est de vous initier à la **Programmation Orientée Objet** (Object-Oriented Programming). Ce sera le point de départ de votre parcours en C++. De nombreux langages sont recommandés pour apprendre l'OOP. Nous avons choisi le C++ car il dérive de votre vieil ami le C. Le C++ étant un langage complexe, et afin de garder les choses simples, votre code devra suivre le standard **C++98**.

Nous avons conscience que le C++ moderne est très différent sur de nombreux aspects. Donc si vous voulez devenir un développeur C++ compétent, c'est à vous d'aller plus loin après le Common Core 42 !

Vous découvrirez de nouveaux concepts étape par étape. Les exercices augmenteront progressivement en complexité.

---

## Chapitre II — Règles générales

### Compilation

- Compilez votre code avec `c++` et les flags `-Wall -Wextra -Werror`
- Votre code doit toujours compiler si vous ajoutez le flag `-std=c++98`

### Conventions de formatage et de nommage

- Les répertoires des exercices seront nommés ainsi : `ex00`, `ex01`, ... , `exn`
- Nommez vos fichiers, classes, fonctions, member functions et attributs comme requis dans les consignes.
- Écrivez les noms de classes au format **UpperCamelCase**. Les fichiers contenant le code d'une classe seront toujours nommés selon le nom de la classe. Par exemple : `ClassName.hpp`/`ClassName.h`, `ClassName.cpp`, ou `ClassName.tpp`. Ainsi, si vous avez un fichier d'en-tête contenant la définition d'une classe « BrickWall » représentant un mur de briques, son nom sera `BrickWall.hpp`.
- Sauf indication contraire, tout message affiché doit se terminer par un caractère de retour à la ligne et être affiché sur la sortie standard.
- *Adieu la Norminette !* Aucun style de code n'est imposé dans les modules C++. Vous pouvez suivre votre style préféré. Mais gardez à l'esprit qu'un code que vos correcteurs (peer evaluators) ne comprennent pas est un code qu'ils ne peuvent pas noter. Faites de votre mieux pour écrire un code propre et lisible.

### Autorisé / Interdit

Vous ne codez plus en C. Il est temps de passer au C++ ! Par conséquent :

- Vous êtes autorisé à utiliser presque tout ce que propose la bibliothèque standard. Ainsi, plutôt que de vous en tenir à ce que vous connaissez déjà, il serait judicieux d'utiliser autant que possible les versions « C++-isées » des fonctions C que vous avez l'habitude d'utiliser.
- Cependant, vous ne pouvez utiliser aucune autre bibliothèque externe. Cela signifie que le **C++11** (et ses formes dérivées) et les bibliothèques **Boost** sont interdits. Les fonctions suivantes sont également interdites : `*printf()`, `*alloc()` et `free()`. Si vous les utilisez, votre note sera de 0, un point c'est tout.
- Notez que, sauf mention contraire explicite, les mots-clés `using namespace <ns_name>` et `friend` sont interdits. Sinon, votre note sera de -42.
- **Vous n'êtes autorisé à utiliser la STL que dans les Modules 08 et 09.** C'est-à-dire : pas de **Containers** (vector/list/map, etc.) et pas d'**Algorithms** (tout ce qui nécessite d'inclure le header `<algorithm>`) avant cela. Sinon, votre note sera de -42.

### Quelques exigences de conception

- Les fuites mémoire (memory leaks) existent aussi en C++. Lorsque vous allouez de la mémoire (en utilisant le mot-clé `new`), vous devez éviter les **memory leaks**.
- Du Module 02 au Module 09, vos classes doivent être conçues selon la **Forme Canonique Orthodoxe** (Orthodox Canonical Form), sauf mention contraire explicite.
- Toute implémentation de fonction placée dans un fichier d'en-tête (header), à l'exception des templates de fonctions, entraîne un 0 à l'exercice.
- Vous devez pouvoir utiliser chacun de vos headers indépendamment des autres. Ainsi, ils doivent inclure toutes les dépendances dont ils ont besoin. Cependant, vous devez éviter le problème de la double inclusion en ajoutant des **include guards**. Sinon, votre note sera de 0.

### Lisez-moi

- Vous pouvez ajouter des fichiers supplémentaires si nécessaire (par exemple pour découper votre code). Ces rendus n'étant pas vérifiés par un programme, sentez-vous libre de le faire, tant que vous rendez les fichiers obligatoires.
- Parfois, les consignes d'un exercice peuvent sembler courtes, mais les exemples peuvent révéler des exigences qui ne sont pas explicitement écrites dans les instructions.
- Lisez chaque module en entier avant de commencer ! Vraiment, faites-le.
- Par Odin, par Thor ! Utilisez votre cerveau !!!

> ⚠️ Concernant le Makefile pour les projets C++, les mêmes règles qu'en C s'appliquent (voir le chapitre de la Norme sur le Makefile).

> 💡 Vous allez devoir implémenter beaucoup de classes. Cela peut sembler fastidieux, à moins que vous ne sachiez scripter votre éditeur de texte préféré.

> ℹ️ Vous disposez d'une certaine liberté pour réaliser les exercices. Cependant, respectez les règles obligatoires et ne soyez pas paresseux. Vous manqueriez beaucoup d'informations utiles ! N'hésitez pas à lire sur les concepts théoriques.

---

## Chapitre III — Instructions IA

### ● Contexte

Ce projet est conçu pour vous aider à découvrir les briques fondamentales de votre formation 42.

Pour ancrer correctement les connaissances et compétences clés, il est essentiel d'adopter une approche réfléchie quant à l'utilisation des outils et du support d'IA.

Un véritable apprentissage des fondamentaux nécessite un effort intellectuel réel — à travers le défi, la répétition et les échanges d'apprentissage entre pairs (peer-learning).

Pour une vue d'ensemble plus complète de notre position sur l'IA — en tant qu'outil d'apprentissage, dans le cadre de la formation 42, et en tant qu'attente sur le marché du travail — merci de vous référer à la FAQ dédiée sur l'intranet.

### ● Message principal

- ☛ Construisez des bases solides sans raccourcis.
- ☛ Développez réellement vos compétences techniques et humaines (tech & power skills).
- ☛ Vivez un véritable apprentissage entre pairs, apprenez à apprendre et à résoudre de nouveaux problèmes.
- ☛ Le chemin d'apprentissage compte plus que le résultat.
- ☛ Apprenez les risques liés à l'IA, et développez des pratiques de contrôle et des contre-mesures efficaces pour éviter les pièges courants.

### ● Règles pour l'apprenant :

- Vous devez appliquer votre raisonnement à vos tâches assignées, en particulier avant de vous tourner vers l'IA.
- Vous ne devez pas demander de réponses directes à l'IA.
- Vous devez apprendre l'approche globale de 42 concernant l'IA.

### ● Résultats de la phase :

Au cours de cette phase fondationnelle, vous obtiendrez les résultats suivants :

- Acquérir de solides bases techniques et de codage.
- Comprendre pourquoi et comment l'IA peut être dangereuse durant cette phase.

### ● Commentaires et exemple :

- Oui, nous savons que l'IA existe — et oui, elle peut résoudre vos projets. Mais vous êtes ici pour apprendre, pas pour prouver que l'IA a appris. Ne perdez pas votre temps (ni le nôtre) juste pour démontrer que l'IA peut résoudre le problème donné.
- Apprendre à 42, ce n'est pas connaître la réponse — c'est développer la capacité d'en trouver une. L'IA vous donne la réponse directement, mais cela vous empêche de construire votre propre raisonnement. Et le raisonnement demande du temps, des efforts, et implique l'échec. Le chemin vers la réussite n'est pas censé être facile.
- Gardez à l'esprit que pendant les examens, l'IA n'est pas disponible — pas d'internet, pas de smartphones, etc. Vous réaliserez vite si vous vous êtes trop reposé sur l'IA dans votre processus d'apprentissage.
- L'apprentissage entre pairs vous expose à des idées et des approches différentes, améliorant vos compétences interpersonnelles et votre capacité à penser de façon divergente. C'est bien plus précieux que de simplement discuter avec un bot. Alors ne soyez pas timide — parlez, posez des questions, et apprenez ensemble !
- Oui, l'IA fera partie du cursus — à la fois comme outil d'apprentissage et comme sujet en tant que tel. Vous aurez même l'occasion de construire votre propre logiciel d'IA. Pour en savoir plus sur notre approche en crescendo, vous la retrouverez dans la documentation disponible sur l'intranet.

**✓ Bonne pratique :**
> Je bloque sur un nouveau concept. Je demande à quelqu'un à côté comment il a abordé le problème. On discute pendant 10 minutes — et soudain, ça clique. Je comprends.

**✗ Mauvaise pratique :**
> J'utilise l'IA en cachette, je copie du code qui semble correct. Pendant l'évaluation par les pairs, je ne peux rien expliquer. J'échoue. Pendant l'examen — pas d'IA — je suis de nouveau bloqué. J'échoue.

---

## Chapitre IV — Exercice 00 : Megaphone

| | |
|---|---|
| **Exercice** | 00 |
| | **Megaphone** |
| Répertoire | `ex00/` |
| Fichiers à rendre | `Makefile`, `megaphone.cpp` |
| Interdit | Aucun |

Histoire de s'assurer que tout le monde est bien réveillé, écrivez un programme qui produit la sortie suivante :

```
$>./megaphone "shhhhh... I think the students are asleep..."
SHHHHH... I THINK THE STUDENTS ARE ASLEEP...
$>./megaphone Damnit " ! " "Sorry students, I thought this thing was off."
DAMNIT ! SORRY STUDENTS, I THOUGHT THIS THING WAS OFF.
$>./megaphone
* LOUD AND UNBEARABLE FEEDBACK NOISE *
$>
```

> 💡 Résolvez les exercices à la manière C++ (*in a C++ manner*).

---

## Chapitre V — Exercice 01 : My Awesome PhoneBook

| | |
|---|---|
| **Exercice** | 01 |
| | **My Awesome PhoneBook** |
| Répertoire | `ex01/` |
| Fichiers à rendre | `Makefile`, `*.cpp`, `*.{h, hpp}` |
| Interdit | Aucun |

Bienvenue dans les années 80 et leur technologie incroyable ! Écrivez un programme qui se comporte comme un logiciel d'annuaire téléphonique (phonebook) minable mais génial.

Vous devez implémenter deux classes :

- **PhoneBook**
  - Elle possède un tableau (array) de contacts.
  - Elle peut stocker un maximum de **8 contacts**. Si l'utilisateur tente d'ajouter un 9ᵉ contact, remplacez le plus ancien par le nouveau.
  - Notez que l'allocation dynamique est interdite.
- **Contact**
  - Représente un contact de l'annuaire.

Dans votre code, l'annuaire (phonebook) doit être instancié comme une instance de la classe **PhoneBook**. Idem pour les contacts. Chacun d'eux doit être instancié comme une instance de la classe **Contact**. Vous êtes libre de concevoir les classes comme bon vous semble, mais gardez à l'esprit que tout ce qui sera toujours utilisé à l'intérieur d'une classe doit être `private`, et que tout ce qui peut être utilisé à l'extérieur d'une classe doit être `public`.

> ℹ️ N'oubliez pas de regarder les vidéos de l'intranet.

À l'initialisation du programme, l'annuaire est vide et l'utilisateur est invité à saisir l'une des trois commandes. Le programme n'accepte que `ADD`, `SEARCH` et `EXIT`.

- **ADD** : enregistrer un nouveau contact
  - Si l'utilisateur saisit cette commande, il est invité à saisir les informations du nouveau contact champ par champ. Une fois tous les champs complétés, ajoutez le contact à l'annuaire.
  - Les champs du contact sont : prénom (first name), nom (last name), surnom (nickname), numéro de téléphone (phone number), et secret le plus profond (darkest secret). Un contact enregistré ne peut pas avoir de champs vides.
- **SEARCH** : afficher un contact spécifique
  - Affichez les contacts enregistrés sous forme d'une liste à **4 colonnes** : index, prénom, nom et surnom.
  - Chaque colonne doit faire **10 caractères** de large. Un caractère pipe (`|`) les sépare. Le texte doit être aligné à droite. Si le texte est plus long que la colonne, il doit être tronqué et le dernier caractère affichable doit être remplacé par un point (`.`).
  - Ensuite, redemandez à l'utilisateur l'index de l'entrée à afficher. Si l'index est hors limites ou incorrect, définissez un comportement pertinent. Sinon, affichez les informations du contact, un champ par ligne.
- **EXIT**
  - Le programme se termine et les contacts sont perdus à jamais !
- **Toute autre saisie est ignorée.**

Une fois qu'une commande a été correctement exécutée, le programme attend une nouvelle commande. Il s'arrête lorsque l'utilisateur saisit `EXIT`.

Donnez un nom pertinent à votre exécutable.

> 💡 http://www.cplusplus.com/reference/string/string/ et bien sûr http://www.cplusplus.com/reference/iomanip/

---

## Chapitre VI — Exercice 02 : The Job Of Your Dreams

| | |
|---|---|
| **Exercice** | 02 |
| | **The Job Of Your Dreams** |
| Répertoire | `ex02/` |
| Fichiers à rendre | `Makefile`, `Account.cpp`, `Account.hpp`, `tests.cpp` |
| Interdit | Aucun |

> ℹ️ `Account.hpp`, `tests.cpp`, et le fichier de log sont disponibles au téléchargement sur la page intranet du module.

Aujourd'hui c'est votre premier jour chez *GlobalBanksters United*. Après avoir réussi les tests de recrutement (grâce à quelques astuces *Microsoft Office* qu'un ami vous a montrées), vous avez rejoint l'équipe de développement. Vous savez aussi que le recruteur a été impressionné par la rapidité avec laquelle vous avez installé *Adobe Reader*. Ce petit plus a fait toute la différence et vous a aidé à battre tous vos concurrents (autrement dit les autres candidats) : vous avez réussi !

Quoi qu'il en soit, votre manager vient de vous donner du travail. Votre première tâche est de recréer un fichier perdu. Quelque chose a mal tourné et un fichier source a été supprimé par erreur. Malheureusement, vos collègues ne savent pas ce qu'est **Git** et utilisent des clés USB pour partager le code. À ce stade, il serait sensé de quitter cet endroit tout de suite. Cependant, vous décidez de rester. Challenge accepté !

Vos collègues développeurs vous donnent un tas de fichiers. En compilant `tests.cpp`, vous découvrez que le fichier manquant est `Account.cpp`. Par chance, le fichier d'en-tête `Account.hpp` a été sauvegardé. Il y a aussi un fichier de log. Peut-être pourriez-vous l'utiliser pour comprendre comment la classe **Account** a été implémentée.

Vous commencez à recréer le fichier `Account.cpp`. En seulement quelques minutes, vous codez quelques lignes de pur C++ génial. Après quelques échecs de compilation, votre programme passe les tests. Sa sortie correspond parfaitement à celle enregistrée dans le fichier de log (**à l'exception des timestamps**, qui différeront évidemment puisque les tests enregistrés dans le fichier de log ont été exécutés avant votre embauche).

Vous êtes impressionnant, dites donc !

> ℹ️ L'ordre dans lequel les destructeurs sont appelés peut différer selon votre compilateur/système d'exploitation. Vos destructeurs peuvent donc être appelés dans l'ordre inverse.

> ℹ️ Terminer l'exercice 02 n'est pas obligatoire pour valider ce module.

---

## Chapitre VII — Rendu et évaluation par les pairs

Rendez votre travail sur votre dépôt **Git** comme d'habitude. Seul le travail présent dans votre dépôt sera évalué lors de la soutenance. N'hésitez pas à vérifier les noms de vos fichiers pour vous assurer qu'ils sont corrects.

Pendant l'évaluation, une brève **modification du projet** peut occasionnellement être demandée. Cela peut impliquer un léger changement de comportement, quelques lignes de code à écrire ou réécrire, ou une fonctionnalité facile à ajouter.

Bien que cette étape ne soit **pas applicable à tous les projets**, vous devez y être préparé si cela est mentionné dans les consignes d'évaluation.

Cette étape vise à vérifier votre compréhension réelle d'une partie spécifique du projet. La modification peut être effectuée dans l'environnement de développement de votre choix (par exemple votre configuration habituelle), et devrait être réalisable en quelques minutes — sauf si un délai spécifique est défini dans le cadre de l'évaluation.

On peut par exemple vous demander de faire une petite mise à jour d'une fonction ou d'un script, de modifier un affichage, ou d'ajuster une structure de données pour stocker de nouvelles informations, etc.

Les détails (portée, cible, etc.) seront précisés dans les **consignes d'évaluation** et peuvent varier d'une évaluation à l'autre pour un même projet.
