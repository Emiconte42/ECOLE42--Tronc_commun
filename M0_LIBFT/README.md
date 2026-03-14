
# libft

libft est une librairie de fonctions C de base, a realiser soi-meme. Elle permet de disposer d'un ensemble d'outils pour manipuler les chaines, la memoire, les listes chainees, etc. Le but est de comprendre le fonctionnement interne de ces fonctions et d'apprendre a gerer la memoire proprement.

## Compilation

Pour compiler la librairie :

```
make
```

Cela genere un fichier `libft.a` utilisable dans vos projets C.

## Utilisation

Inclure le header dans votre projet :

```c
#include "libft.h"
```

Compiler avec la librairie :

```
gcc monfichier.c -L. -lft -I.
```

## Description

La libft regroupe des fonctions classiques de manipulation de chaines, de memoire, de conversion, d'affichage, et de gestion de listes chainees. Chaque fonction a ete reimplementee pour mieux comprendre leur mecanisme et apprendre a coder de maniere robuste.

Vous trouverez ci-dessous la liste des fonctions disponibles, leur prototype et une courte description.

## Tests de caracteres et conversions

| Fonction     | Prototype                                         | Description                                                                          |
|--------------|---------------------------------------------------|--------------------------------------------------------------------------------------|
| ft_atoi      | int ft_atoi(const char *nptr)                     | Convertit une chaîne en entier (comme `atoi`).                                       |
| ft_isalnum   | int ft_isalnum(int c)                             | Renvoie non-zero si `c` est alphanumerique.                                          |
| ft_isalpha   | int ft_isalpha(int c)                             | Renvoie non-zero si `c` est une lettre alphabetique.                                 |
| ft_isascii   | int ft_isascii(int c)                             | Renvoie non-zero si `c` est un caractère ASCII (0–127).                              |
| ft_isdigit   | int ft_isdigit(int c)                             | Renvoie non-zero si `c` est un chiffre decimal.                                      |
| ft_isprint   | int ft_isprint(int c)                             | Renvoie non-zero si `c` est imprimable (espace inclus).                              |
| ft_tolower   | int ft_tolower(int c)                             | Convertit `c` en minuscule si possible.                                              |
| ft_toupper   | int ft_toupper(int c)                             | Convertit `c` en majuscule si possible.                                              |
|--------------|---------------------------------------------------|--------------------------------------------------------------------------------------|

## Fonctions memoire
                         
| Fonction     | Prototype                                               | Description                                                                    |
|--------------|---------------------------------------------------------|--------------------------------------------------------------------------------|
| ft_memset    | void *ft_memset(void *s, int c, size_t n)               | Remplit `n` octets de `s` avec la valeur `c`.                                  |
| ft_bzero     | void ft_bzero(void *s, size_t n)                        | Met à zero `n` octets de la zone memoire `s`.                                  |
| ft_memcpy    | void *ft_memcpy(void *dest, const void *src, size_t n)  | Copie `n` octets de `src` vers `dest` (non chevauchants).                      |
| ft_memmove   | void *ft_memmove(void *dest, const void *src, size_t n) | Copie `n` octets en gerant le chevauchement.                                   |
| ft_memchr    | void *ft_memchr(const void *s, int c, size_t n)         | Cherche le premier octet `c` dans les `n` premiers octets.                     |
| ft_memcmp    | int ft_memcmp(const void *s1, const void *s2, size_t n) | Compare les `n` premiers octets de `s1` et `s2`.                               |
|--------------|---------------------------------------------------------|--------------------------------------------------------------------------------|

## Chaines de caracteres

| Fonction     | Prototype                                                         | Description                                                          |
|--------------|-------------------------------------------------------------------|----------------------------------------------------------------------|
| ft_strchr    | char *ft_strchr(const char *s, int c)                             | Retourne un pointeur vers la première occurrence de `c` dans `s`.    |
| ft_strrchr   | char *ft_strrchr(const char *s, int c)                            | Retourne un pointeur vers la dernière occurrence de `c` dans `s`.    |
| ft_strlcat   | size_t ft_strlcat(char *dst, const char *src, size_t siz)         | Concatène `src` sur `dst` en respectant la taille `siz`.             |
| ft_strlcpy   | size_t ft_strlcpy(char *dst, char *src, size_t siz)               | Copie `src` vers `dst` en respectant `siz` et termine par `\0`.      |
| ft_strlen    | size_t ft_strlen(const char *str)                                 | Renvoie la longueur de la chaîne `str`.                              |
| ft_strncmp   | int ft_strncmp(const char *s1, const char *s2, size_t n)          | Compare jusqu'à `n` caractères de `s1` et `s2`.                      |
| ft_strnstr   | char *ft_strnstr(const char *big, const char *little, size_t len) | Cherche `little` dans `big` sur au plus `len` caractères.            | 
|--------------|-------------------------------------------------------------------|----------------------------------------------------------------------|

## Allocation / duplication

| Fonction     | Prototype                                         | Description                                                                          |
|--------------|---------------------------------------------------|--------------------------------------------------------------------------------------|
| ft_calloc    | void *ft_calloc(size_t nmemb, size_t size)        | Alloue et initialise à zero un bloc pour `nmemb` elements.                           |
| ft_strdup    | char *ft_strdup(const char *s1)                   | Duplique la chaîne `s1` (allocation avec `malloc`).                                  |
|--------------|---------------------------------------------------|--------------------------------------------------------------------------------------|

## 2eme Partie (fonctions supplementaires)
                         
| Fonction     | Prototype                                                      | Description                                                                          |
|--------------|----------------------------------------------------------------|--------------------------------------------------------------------------------------|
| ft_substr    | char *ft_substr(char const *s, unsigned int start, size_t len) | Alloue et retourne une sous-chaîne de `s`.                                           |
| ft_strjoin   | char *ft_strjoin(char const *s1, char const *s2)               | Alloue et retourne la concatenation de `s1` et `s2`.                                 |
| ft_strtrim   | char *ft_strtrim(char const *s1, char const *set)              | Supprime les caractères de `set` en bordure de `s1`.                                 |
| ft_split     | char **ft_split(char const *s, char c)                         | Coupe `s` en tableau de chaînes separees par `c`.                                    |
| ft_itoa      | char *ft_itoa(int n)                                           | Convertit l'entier `n` en chaîne allouee.                                            |
| ft_strmapi   | char *ft_strmapi(char const *s, char (*f)(unsigned int, char)) | Applique `f` à chaque caractère et retourne une nouvelle chaîne.                     |
| ft_striteri  | void ft_striteri(char *s, void (*f)(unsigned int, char*))      | Applique `f` en place sur chaque caractère de `s`.                                   |
| ft_putchar_fd| void ft_putchar_fd(char c, int fd)                             | ecrit le caractere `c` sur le descripteur `fd`.                                      |
| ft_putstr_fd | void ft_putstr_fd(char *s, int fd)                             | ecrit la chaine `s` sur le descripteur `fd`.                                         |
| ft_putendl_fd| void ft_putendl_fd(char *s, int fd)                            | ecrit la chaine `s` suivie d'un saut de ligne sur `fd`.                              |
| ft_putnbr_fd | void ft_putnbr_fd(int n, int fd)                               | ecrit l'entier `n` sous forme decimale sur `fd`.                                     |
|--------------|----------------------------------------------------------------|--------------------------------------------------------------------------------------|
