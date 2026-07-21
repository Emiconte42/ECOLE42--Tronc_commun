Chapitre I — Introduction

Le C++ est un langage de programmation généraliste créé par Bjarne Stroustrup comme extension du langage C, ou « C avec des classes » (source : Wikipédia).

Le but de ces modules est de t'introduire à la Programmation Orientée Objet. 
Ce sera le point de départ de ton voyage en C++. Beaucoup de langages sont recommandés pour apprendre la POO. 
Le C++ a été choisi car il dérive du bon vieux C. 
Comme le C++ est un langage complexe, et pour garder les choses simples, ton code suivra la norme C++98.

Nous savons que le C++ moderne est très différent sur de nombreux aspects. 
Donc si tu veux devenir un développeur C++ compétent, c'est à toi d'aller plus loin après le Tronc Commun de 42 !

Tu découvriras de nouveaux concepts étape par étape. 
Les exercices augmenteront progressivement en complexité.

Chapitre II — Règles générales

Compilation

Compile ton code avec c++ et les flags -Wall -Wextra -Werror
Ton code doit toujours compiler si tu ajoutes le flag -std=c++98

Formatage et conventions de nommage

Les dossiers d'exercices seront nommés : ex00, ex01, ..., exn
Nomme tes fichiers, classes, fonctions, fonctions membres et attributs comme demandé dans les consignes.
Écris les noms de classes en UpperCamelCase. 
Les fichiers contenant le code d'une classe portent toujours le nom de la classe (ex : ClassName.hpp, ClassName.cpp).
Sauf indication contraire, chaque message affiché doit se terminer par un retour à la ligne et être affiché sur la sortie standard.
Adieu Norminette ! Aucun style de code n'est imposé en C++. 
Choisis ton style préféré, mais garde en tête qu'un code que tes correcteurs ne comprennent pas est un code qu'ils ne peuvent pas noter.

Autorisé / Interdit

Tu ne codes plus en C, il est temps de passer au C++ ! Donc :

Tu peux utiliser presque tout de la bibliothèque standard. 
Privilégie les versions « C++ » des fonctions C que tu connais.
Tu ne peux utiliser aucune autre bibliothèque externe : le C++11 (et versions dérivées) et Boost sont interdits. 
Les fonctions *printf(), *alloc() et free() sont aussi interdites — leur usage donne une note de 0.
Sauf mention contraire, using namespace <ns_name> et friend sont interdits — sinon note de -42.
Tu ne peux utiliser la STL que dans les Modules 08 et 09 : pas de conteneurs (vector/list/map...) ni d'algorithmes (rien qui nécessite <algorithm>) avant — sinon note de -42.

Quelques exigences de conception

Les fuites mémoire existent aussi en C++. 
Si tu utilises new, évite les memory leaks.
Du Module 02 au Module 09, tes classes doivent respecter la Forme Canonique Orthodoxe, sauf mention contraire.
Toute implémentation de fonction dans un header (sauf les templates) = 0 à l'exercice.
Chaque header doit être utilisable indépendamment (inclure ses propres dépendances) et protégé par des include guards contre la double inclusion, sinon note de 0.

À lire

Tu peux ajouter des fichiers supplémentaires si besoin.
Les consignes peuvent sembler courtes, mais les exemples révèlent parfois des exigences non écrites explicitement.
Lis chaque module en entier avant de commencer !
Par Odin, par Thor ! Utilise ton cerveau !!!

⚠️ Concernant le Makefile, les mêmes règles qu'en C s'appliquent (voir le chapitre Norme sur le Makefile).
💡 Tu vas devoir implémenter beaucoup de classes — ça peut sembler fastidieux, sauf si tu sais scripter ton éditeur de texte préféré.
ℹ️ Une certaine liberté t'est laissée pour ces exercices, mais respecte les règles obligatoires et ne sois pas fainéant : tu manquerais plein d'infos utiles !

Chapitre III — Instructions IA

Contexte : ce projet vise à te faire découvrir les bases fondamentales de ta formation à 42. 
Pour bien ancrer les connaissances et compétences, il est essentiel d'adopter une approche réfléchie de l'utilisation des outils d'IA. 
Le véritable apprentissage fondamental exige un effort intellectuel réel — via le défi, la répétition et les échanges entre pairs.

Message principal

Construis des bases solides sans raccourcis.
Développe réellement tes compétences techniques et humaines.
Vis une véritable expérience de peer-learning, apprends à apprendre et à résoudre de nouveaux problèmes.
Le chemin d'apprentissage compte plus que le résultat.
Comprends les risques liés à l'IA et développe de bonnes pratiques pour éviter les pièges courants.

Règles pour l'apprenant

Tu dois raisonner sur tes tâches avant de te tourner vers l'IA.
Tu ne dois pas demander directement les réponses à l'IA.
Tu dois comprendre l'approche globale de 42 vis-à-vis de l'IA.

Résultats de la phase : bases techniques solides + compréhension des risques de l'IA.

Commentaires

Oui, l'IA existe et peut résoudre tes projets. Mais tu es là pour apprendre, pas pour prouver que l'IA sait faire.
Apprendre à 42, ce n'est pas connaître la réponse, c'est développer la capacité à la trouver.
Pendant les examens, pas d'IA (ni internet, ni smartphone) — tu réaliseras vite si tu en as trop dépendu.
Le peer-learning est plus précieux que discuter avec un bot.
L'IA fera partie du cursus, comme outil et comme sujet d'étude (approche « crescendo », détaillée sur l'intranet).

✅ Bonne pratique : « Je bloque sur un nouveau concept. Je demande à quelqu'un à côté comment il/elle a procédé. On discute 10 minutes — et ça devient clair. »

❌ Mauvaise pratique : « J'utilise l'IA en cachette, je copie du code qui a l'air correct. En évaluation, je ne sais rien expliquer. J'échoue. À l'examen, sans IA, je suis bloquée. J'échoue. »

Chapitre IV — Exercice 00 : Megaphone
	
Dossier	ex00/
Fichiers à rendre	Makefile, megaphone.cpp
Interdit	Rien

Juste pour vérifier que tout le monde est réveillé, écris un programme qui produit la sortie suivante :

$>./megaphone "shhhhh... I think the students are asleep..."
SHHHHH... I THINK THE STUDENTS ARE ASLEEP...
$>./megaphone Damnit " ! " "Sorry students, I thought this thing was off."
DAMNIT ! SORRY STUDENTS, I THOUGHT THIS THING WAS OFF.
$>./megaphone
* LOUD AND UNBEARABLE FEEDBACK NOISE *
$>

💡 Résous les exercices « à la manière C++ ».

Chapitre V — Exercice 01 : My Awesome PhoneBook
	
Dossier	ex01/
Fichiers à rendre	Makefile, *.cpp, *.{h, hpp}
Interdit	Rien

Bienvenue dans les années 80 et leur technologie incroyable ! 
Écris un programme qui se comporte comme un (mauvais mais génial) logiciel de répertoire téléphonique.

Tu dois implémenter deux classes :

PhoneBook : possède un tableau de contacts, peut en stocker 8 maximum (le 9ᵉ remplace le plus ancien), et l'allocation dynamique est interdite.
Contact : représente un contact du répertoire.

Le répertoire doit être instancié comme une instance de PhoneBook, et chaque contact comme une instance de Contact. Conçois tes classes librement, mais garde en tête : ce qui est toujours utilisé en interne à la classe doit être private, ce qui peut être utilisé de l'extérieur doit être public.

ℹ️ N'oublie pas de regarder les vidéos de l'intranet.

Au démarrage, le répertoire est vide et l'utilisateur est invité à saisir une des trois commandes : ADD, SEARCH, EXIT.

ADD : ajoute un contact. L'utilisateur saisit chaque champ un par un : prénom, nom, surnom, numéro de téléphone, secret le plus sombre. Un contact enregistré ne peut pas avoir de champ vide.
SEARCH : affiche la liste des contacts sous forme de tableau à 4 colonnes (index, prénom, nom, surnom). Chaque colonne fait 10 caractères de large, séparée par un |, texte aligné à droite. Si le texte est trop long, il est tronqué et le dernier caractère affichable est remplacé par un point (.). Ensuite, on demande l'index du contact à afficher : si l'index est invalide, définis un comportement pertinent ; sinon affiche le contact champ par champ.
EXIT : quitte le programme (les contacts sont perdus pour toujours !)
Toute autre entrée est ignorée.

Une fois une commande exécutée, le programme attend la suivante, jusqu'à EXIT.

Donne un nom pertinent à ton exécutable.

💡 Liens utiles : http://www.cplusplus.com/reference/string/string/ et http://www.cplusplus.com/reference/iomanip/

Chapitre VI — Exercice 02 : The Job Of Your Dreams
	
Dossier	ex02/
Fichiers à rendre	Makefile, Account.cpp, Account.hpp, tests.cpp
Interdit	Rien

ℹ️ Account.hpp, tests.cpp et le fichier de log sont disponibles au téléchargement sur la page intranet du module.

C'est ton premier jour chez GlobalBanksters United. Après avoir brillamment passé les tests de recrutement (grâce à quelques astuces Microsoft Office montrées par un ami), tu rejoins l'équipe de dev. Le recruteur a aussi été impressionné par la rapidité avec laquelle tu as installé Adobe Reader. Ce petit plus a fait toute la différence face aux autres candidats : tu as réussi !

Ton manager te confie une tâche : recréer un fichier perdu. Un fichier source a été supprimé par erreur. Malheureusement tes collègues ne connaissent pas Git et partagent le code par clé USB. Tu pourrais partir sur-le-champ, mais tu décides de rester. Défi accepté !

Tes collègues te donnent un tas de fichiers. En compilant tests.cpp, tu découvres que le fichier manquant est Account.cpp. Heureusement, le header Account.hpp a été sauvegardé. Il y a aussi un fichier de log qui peut t'aider à comprendre comment la classe Account a été implémentée.

Tu recrées Account.cpp. En quelques minutes, tu codes quelques lignes de C++ pur et génial. Après quelques échecs de compilation, ton programme passe les tests. Sa sortie correspond parfaitement à celle sauvegardée dans le log (sauf les timestamps, qui différeront forcément puisque les tests du log ont été exécutés avant ton embauche).

Waouh, impressionnant !

ℹ️ L'ordre d'appel des destructeurs peut varier selon ton compilateur/système d'exploitation — ils peuvent être appelés dans l'ordre inverse.
ℹ️ Compléter l'exercice 02 n'est pas obligatoire pour valider ce module.

Chapitre VII — Rendu et évaluation par les pairs

Rends ton travail sur ton dépôt Git comme d'habitude. Seul le contenu de ton dépôt sera évalué en soutenance. Vérifie bien les noms de tes fichiers.

Pendant l'évaluation, une petite modification du projet pourra parfois t'être demandée : un changement mineur de comportement, quelques lignes de code à écrire/réécrire, ou une fonctionnalité facile à ajouter.

Cette étape ne s'applique pas forcément à tous les projets, mais tu dois t'y préparer si elle est mentionnée dans le barème. Elle sert à vérifier ta compréhension réelle d'une partie précise du projet. Tu peux utiliser l'environnement de développement de ton choix, et cela doit être faisable en quelques minutes, sauf si un délai précis est indiqué.

Exemples : mettre à jour une fonction, modifier un affichage, ajuster une structure de données pour stocker une nouvelle info, etc.

Les détails (portée, cible, etc.) seront précisés dans le barème d'évaluation et peuvent varier d'une évaluation à l'autre pour un même projet.

Comment aborder ce sujet quand on ne connaît rien au C++

Quelques conseils pour démarrer sereinement :

Ne saute pas les prérequis conceptuels. Avant même d'ouvrir ex00, assure-toi de comprendre : la différence entre C et C++, ce qu'est une classe (attributs + méthodes), les notions public/private, les namespaces, et std::string/std::cout par opposition à printf.
Vas-y exercice par exercice, dans l'ordre. ex00 (Megaphone) sert juste à vérifier que tu sais compiler, lire argv/argc, manipuler des std::string — pas besoin de classe. C'est un bon échauffement.
ex01 est le vrai premier exercice de POO. Avant de coder, dessine sur papier : quels attributs pour Contact ? Quelles méthodes publiques pour PhoneBook (ajouter, chercher, afficher) ? Réfléchis à la contrainte « 8 contacts max, pas d'allocation dynamique » — ça oriente vers un tableau fixe de Contact (pas de pointeurs).
Utilise les ressources autorisées : cplusplus.com/reference (string, iomanip) est explicitement recommandé dans le sujet — prends l'habitude de t'y référer plutôt que de deviner la syntaxe.
Compile souvent, avec les bons flags (-Wall -Wextra -Werror -std=c++98) dès la première ligne de code, pas seulement à la fin — les erreurs de compilation en C++ sont plus verbeuses qu'en C, mieux vaut les affronter petit à petit.
ex02 (Account) est plus avancé — garde-le pour la fin, une fois ex01 maîtrisé, et lis bien la log fournie pour reverse-engineer le comportement attendu.
Respecte l'esprit « pas d'IA pour les réponses directes » rappelé dans le sujet lui-même : utilise-moi plutôt pour comprendre un concept bloquant, relire ta logique, ou débugger une erreur de compilation — pas pour écrire le code à ta place. C'est aussi dans ton intérêt pour la soutenance orale, où on peut te demander de modifier ton code en direct.