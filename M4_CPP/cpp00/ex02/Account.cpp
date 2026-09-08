
/*
Account.hpp, tests.cpp et le fichier journal sont disponibles au téléchargement sur la page intranet du module.

C'est votre premier jour chez GlobalBanksters United.
Après avoir réussi les tests de recrutement (grâce à quelques astuces Microsoft Office qu'un ami vous a montrées),
vous avez rejoint l'équipe de développement. Vous savez aussi que le recruteur a été impressionné par la rapidité
avec laquelle vous avez installé Adobe Reader.
Ce petit « plus » a fait toute la différence et vous a permis de battre tous vos concurrents
(alias les autres candidats) : vous avez décroché le poste !
Quoi qu'il en soit, votre responsable vient de vous confier une mission.
Votre première tâche consiste à recréer un fichier perdu. Un incident s'est produit et un fichier
source a été supprimé par erreur. Malheureusement, vos collègues ignorent ce qu'est Git
et utilisent des clés USB pour partager le code.
À ce stade, il serait logique de quitter les lieux sur-le-champ. Pourtant, vous décidez de rester. Défi accepté !
Vos collègues développeurs vous remettent un ensemble de fichiers.
La compilation de `tests.cpp` révèle que le fichier manquant est `Account.cpp`.
Par chance, le fichier d'en-tête `Account.hpp` a été préservé. 
Il y a également un fichier journal ; peut-être pourrez-vous l'utiliser pour comprendre comment la classe `Account` a été implémentée.

Vous commencez à recréer le fichier Account.cpp.
En quelques minutes seulement, vous écrivez quelques lignes d'un C++ absolument génial.
Après quelques échecs de compilation, votre programme réussit les tests.
Sa sortie correspond parfaitement à celle enregistrée dans le fichier journal (à l'exception des horodatages,
qui différeront forcément puisque les tests consignés dans le fichier ont été exécutés avant votre embauche).
Bon sang, vous êtes impressionnant !

L'ordre d'appel des destructeurs peut varier selon le compilateur ou le système d'exploitation.
Ainsi, vos destructeurs pourraient être appelés dans l'ordre inverse.
La réalisation de l'exercice 02 n'est pas obligatoire pour valider ce module.
*/