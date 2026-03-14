*Ce projet a été réalisé dans le cadre du cursus 42 par emiconte.*

# PUSH SWAP

## Description
Push Swap est un projet algorithmique dont le but est de trier une liste de nombres en utilisant uniquement deux piles : A et B, ainsi qu’un ensemble limite d’operations.

Au depart, tous les nombres sont places dans la pile A. La pile B est vide. Le programme doit ensuite generer une suite d’instructions (comme sa, pb, ra, etc.) permettant de trier la pile A dans l’ordre croissant.

La difficulte du projet ne consiste pas simplement a trier les nombres, mais a le faire avec le moins d’operations possible. Il faut donc reflechir a une strategie de tri adaptee aux contraintes du sujet.

Dans mon cas, j’ai choisi d’utiliser une approche basee sur l’algorithme radix. L’idee est de trier les nombres en utilisant la representation binaire de leurs index. Cet algorithme est particulierement adapte pour ce projet car il permet de trier efficacement de grandes listes tout en gardant un nombre d’operations raisonnable.

### Operations
Le projet impose un ensemble limite d’operations permettant de manipuler les deux piles.

Operations sur une seule pile :

- `sa` : swap les deux premiers elements de la pile A  
- `sb` : swap les deux premiers elements de la pile B  

Operations sur les deux piles simultanement :

- `ss` : effectue `sa` et `sb` en meme temps  

Operations de push :

- `pa` : prend le premier element de B et le met sur A  
- `pb` : prend le premier element de A et le met sur B  

Operations de rotation :

- `ra` : decale tous les elements de A vers le haut  
- `rb` : decale tous les elements de B vers le haut  
- `rr` : effectue `ra` et `rb` en meme temps (simultanement)

Operations de rotation inverse :

- `rra` : decale tous les elements de A vers le bas  
- `rrb` : decale tous les elements de B vers le bas  
- `rrr` : effectue `rra` et `rrb` en meme temps (simultanement)

Toutes les instructions affichees par le programme doivent appartenir a cette liste.

### Algorithme utilise
Pour trier les nombres, j’ai utilise une approche basee sur l’algorithme **Radix Sort**.

Avant de lancer l’algorithme, chaque nombre est remplace par son **index dans la liste triee**. Cela permet de travailler sur des valeurs plus simples et facilite l’utilisation du binaire.

Le tri se fait ensuite bit par bit :

1. On regarde un bit donne pour chaque element de la pile A
2. Si le bit est `0`, on envoie l’element dans la pile B (`pb`)
3. Si le bit est `1`, on fait une rotation dans A (`ra`)
4. Une fois tous les elements traites, on remet tout dans A (`pa`)

On repete cette operation pour chaque bit jusqu’a ce que la pile soit triee.

Cette methode est efficace pour les grandes listes car le nombre d’operations reste relativement stable meme quand la taille de l’entree augmente.

## Instructions


### Compilation
Pour compiler le programme :
```
make
```


### Exécution
Lancer le programme avec une liste de nombres a trier :
```
./push_swap [nombres à trier]
```
exemple : ./push_swap 3 2 1 6 5 8

Le programme va afficher une suite d’instructions correspondant aux operations a effectuer pour trier la pile.

Ces instructions peuvent ensuite etre verifiees avec le programme `checker`.

### Nettoyage
Pour supprimer les fichiers objets :
```
make clean
```

Pour supprimer les fichiers objets et l’executable :
```
make fclean
```

Pour tout recompiler proprement :
```
make re
```
---

## Ressources
Quelques ressources qui peuvent aider a comprendre le projet ou les algorithmes utilises :

* Site du gitbook pour comprendre le sujet : 
https://42-cursus.gitbook.io/guide/2-rank-02/push_swap

* Documentation sur l’algorithme Radix Sort :
https://en.wikipedia.org/wiki/Radix_sort

* Visualisateur push_swap (tres utile pour tester les algorithmes)  
https://github.com/o-reo/push_swap_visualizer

---


## Utilisation de l’IA
L’IA a ete utilisee uniquement comme outil d’aide pour clarifier certains principes theoriques et pour aider a la redaction du README.

Le code du projet, la logique de tri et l’implementation des algorithmes ont ete ecrits et compris manuellement.