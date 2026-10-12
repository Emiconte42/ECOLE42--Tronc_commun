# `sizeof` — Une question à la compilation sur le layout des types

> **TL;DR.** `sizeof(expr)` renvoie le nombre d'octets qu'un objet du type de `expr` occupe en mémoire. C'est un opérateur de **compile-time** (l'expression n'est pas évaluée, seul son type l'est), et le résultat est un `std::size_t` (un type entier non signé suffisamment large pour adresser le plus grand objet sur la plateforme).

En lien : [`CLASS.md`](CLASS.md) · [`STRUCT.md`](STRUCT.md) · [`MEMORY.md`](../notions/fundamentals/MEMORY.md)

---

## 1. Les deux formes

```cpp
sizeof(int);             // l'opérande est un TYPE — parenthèses obligatoires
sizeof(myVar);           // l'opérande est une EXPRESSION — parenthèses optionnelles mais conventionnelles
sizeof  myVar;           // légal ; rare
sizeof(arr) / sizeof(arr[0]);   // idiome courant : nombre d'éléments du tableau
```

Le type du résultat est `std::size_t` (un entier non signé dans `<cstddef>` ou tout autre header standard).

---

## 2. L'évaluation au compile-time

```cpp
sizeof(crash())          // crash() n'est JAMAIS appelée !
                         // sizeof demande au compiler "quel type crash() renvoie-t-elle ?"
                         // et indique sa taille. L'expression n'est pas exécutée.
```

C'est fondamental : `sizeof` opère sur le **type statique**, calculé au compile-time. Il n'y a aucun appel au runtime, aucun overhead — juste une constante insérée dans le code.

```
   source :   int n = sizeof(int);

   le compiler voit : int (4 octets sur cette plateforme)

   asm émis :        mov dword [n], 4         ; simplement une constante immédiate
```

C'est pourquoi `sizeof` fonctionne même sur des fonctions qui planteraient si elles étaient appelées, sur des types incomplets déclarés mais non définis, etc. — *tant que le type est connu au point d'utilisation*.

---

## 3. Ce que `sizeof` mesure

Pour les types intégrés (built-in) : la taille en octets du stockage.

```
   sizeof(char)     == 1   (toujours — par définition)
   sizeof(bool)     == 1   (typique, mais défini par l'implémentation ; pourrait être plus grand)
   sizeof(short)    >= 2
   sizeof(int)      >= 2   (4 sur la plupart des systèmes modernes)
   sizeof(long)     >= 4   (8 sur Linux/macOS x86_64, 4 sur Windows)
   sizeof(long long)>= 8
   sizeof(float)    == 4   (typiquement ; simple précision IEEE 754)
   sizeof(double)   == 8   (typiquement ; double précision IEEE 754)
   sizeof(void*)    == 8   sur x86_64 ; 4 sur x86 ; varie sur l'embarqué
```

Le standard garantit : `sizeof(char) == 1` et `sizeof(short) <= sizeof(int) <= sizeof(long)`. Au-delà, c'est au choix de la plateforme.

Pour les tableaux : le nombre total d'octets (nombre d'éléments × taille d'un élément) :

```cpp
int arr[10];
sizeof(arr);                      // 40   (10 * sizeof(int) = 10 * 4)
sizeof(arr) / sizeof(arr[0]);     // 10   (idiome pour le nombre d'éléments)
```

Pour les structs/classes : les octets du layout (membres + padding) :

```cpp
struct Mixed { char c; int i; };
sizeof(Mixed);                    // 8   (1 + 3 de padding + 4)
```

Pour les pointers : la taille du pointer, **pas** la taille de ce vers quoi il pointe :

```cpp
int  arr[10];
int *p = arr;
sizeof(p);                        // 8 sur x86_64 — taille du pointer
sizeof(*p);                       // 4 — taille de l'int
```

### sizeof(empty class) >= 1

```cpp
class Empty {};
sizeof(Empty);                    // 1 — jamais zéro
```

Pour que des objets vides distincts aient des adresses distinctes. L'empty base optimization peut éliminer cela dans le cadre de l'héritage.

---

## 4. Padding et alignement — `sizeof` les prend en compte

```cpp
struct A {
    char  c;        // 1
    int   i;        // 4
};                   // sizeof == 8 (1 + 3 de padding + 4)

struct B {
    int   i;        // 4
    char  c;        // 1
};                   // sizeof == 8 (4 + 1 + 3 de tail padding)
```

```
   adresse →    0    1    2    3    4    5    6    7
   A:          ┌────┬────┬────┬────┬────┬────┬────┬────┐
               │ c  │ pd │ pd │ pd │ i.0│ i.1│ i.2│ i.3│
               └────┴────┴────┴────┴────┴────┴────┴────┘
   B:          ┌────┬────┬────┬────┬────┬────┬────┬────┐
               │ i.0│ i.1│ i.2│ i.3│ c  │ pd │ pd │ pd │
               └────┴────┴────┴────┴────┴────┴────┴────┘
```

Le padding final (tail padding) garantit que les tableaux de cette struct fonctionnent — chaque élément commence au bon alignement.

---

## 5. `sizeof` et les pointers — le piège de l'array-decay

```cpp
int  arr[10];

void f(int parr[10]) {           // parr ressemble à un tableau mais c'est un pointer !
    sizeof(parr);                 // 8 — taille du pointer, PAS du tableau
}

sizeof(arr);                      // 40 dans la fonction qui possède le tableau
```

Lorsque vous passez un tableau à une fonction, il **decay en un pointer**. Le `sizeof` que vous observez à l'intérieur de la fonction est la taille du pointer, pas les octets du tableau. C'est l'un des pièges les plus anciens du C/C++.

Pour connaître la taille à l'intérieur d'une fonction, passez-la explicitement :

```cpp
void f(int *p, std::size_t n) { ... }
```

En C++, passez un `std::vector<int>&` et appelez `.size()`.

---

## 6. `sizeof` ne peut pas être utilisé sur des types incomplets

```cpp
class Forward;            // forward declaration
sizeof(Forward);          // ERREUR — taille de la classe inconnue
```

Les forward declarations sont utiles pour les pointers/references, mais pas lorsque vous avez besoin de la taille.

```cpp
extern int huge[];        // taille inconnue ici
sizeof(huge);             // ERREUR
```

Si la taille du tableau est fixée ailleurs, vous devez la connaître localement :

```cpp
extern int huge[100];
sizeof(huge);             // 400 — la taille est connue
```

---

## 7. `sizeof` d'une expression — le type uniquement, pas d'évaluation

```cpp
int *p = 0;
sizeof(*p);               // 4 — même si *p déréférencerait null !
                          //     aucun déréférencement réel n'a lieu, seul le type est examiné.

int a;
sizeof(a++);              // 4 — a n'est PAS incrémenté.
```

`sizeof` examine le type de l'expression et indique sa taille. L'expression elle-même est **ignorée** — aucun effet de bord, aucun appel de fonction, aucun accès réel.

Cela rend `sizeof` sûr à utiliser sur n'importe quelle expression bien typée, même celles qui ne s'exécuteraient pas correctement.

---

## 8. `sizeof...` (variadic templates) — C++11+, pas à 42

En C++11 :

```cpp
template <typename... Ts>
void f(Ts... args) {
    std::cout << sizeof...(args);     // nombre d'arguments
}
```

Opérateur différent (remarquez les points). N'existe pas en C++98.

---

## 9. Vision matériel/codegen

```
   sizeof est résolu par le compiler sous forme de constante.
   aucun coût au runtime. aucune référence de symbole.

   source :   char buf[sizeof(int) * 10];
   asm :      sub rsp, 40                 ; simple réservation sur la stack
                                          ; le compiler a calculé 40 au compile-time
```

La taille dépend de l'**architecture cible**. Compiler pour ARM, l'embarqué ou le 32-bit donne des valeurs de `sizeof` différentes. Le code qui encode en dur `sizeof(int) == 4` peut casser sur les plateformes où ce n'est pas le cas.

---

## 10. Trucs et astuces

### 10.1 Nombre d'éléments d'un tableau

```cpp
#define ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))

int arr[10];
ARRAY_SIZE(arr);          // 10
```

Attention : cela fonctionne uniquement sur les **tableaux**, pas sur les pointers. À l'intérieur d'une fonction avec un paramètre pointer, cela renvoie une mauvaise réponse (1 ou 2 selon la taille des éléments).

En C++ moderne, utilisez `std::size(arr)` (C++17) ou `std::extent` pour les tableaux.

### 10.2 Calculer le layout d'une struct pour la sérialisation

```cpp
struct PacketHeader {
    uint8_t  version;
    uint32_t length;
};

std::cout << "header bytes: " << sizeof(PacketHeader);   // taille incluant le padding
```

Si vous envoyez des octets sur un réseau, `sizeof` inclut un padding dépendant de la plateforme — vous voudrez généralement définir explicitement un layout packé (voir [`STRUCT.md`](STRUCT.md)).

### 10.3 Allocation de mémoire générique

```cpp
T *p = new T[n];                          // façon C++ — utilise sizeof(T) en interne
T *q = (T*)std::malloc(n * sizeof(T));    // style C ; rare en C++
```

### 10.4 Ne jamais utiliser `sizeof` pour compter les caractères d'une chaîne

```cpp
const char *s = "hello";
sizeof(s);                // 8 — taille du pointer
sizeof("hello");          // 6 — taille du tableau : 'h','e','l','l','o','\0'
strlen(s);                // 5 — caractères jusqu'au NUL
```

Pour la longueur d'une chaîne, utilisez `std::strlen` (C-string) ou `.length()` (`std::string`).

### 10.5 L'astuce "char[1] == ancre de sizeof"

```cpp
char arr[sizeof(MyClass)];        // un buffer assez grand pour un MyClass
new (arr) MyClass();              // placement new
```

Pattern utilisé dans les types optionnels, les small-buffer optimizations, etc. Particulier mais puissant.

### 10.6 sizeof des bitfields — indéfini pour les bitfields directement

```cpp
struct Flags {
    unsigned int a : 1;
    unsigned int b : 1;
};

sizeof(Flags);            // 4 — largeur de l'int sous-jacent
sizeof(Flags::a);         // ERREUR — impossible d'appliquer sizeof sur un bitfield
```

---

## 11. Erreurs courantes

| Erreur | Cause | Solution |
|---|---|---|
| `sizeof` d'un tableau passé en paramètre de fonction donne 8 (taille de pointer) | Le tableau decay en pointer lors du passage | Passer la taille séparément, ou utiliser un vector / std::array |
| `sizeof(incomplete type)` | Utilisé sur une class forward-declared | Inclure la définition complète |
| Nombre d'octets incorrect selon les plateformes | `sizeof(int)` diffère sur les cibles 16/32/64 bits | Utiliser les types de `<cstdint>` : `int32_t`, `int64_t` etc. |
| `sizeof(void)` | `void` n'a pas de taille | Impossible — `sizeof(void)` est illégal en C++ standard |

---

## 12. Résumé visuel

```
              ┌────────────────────────────────────────────┐
              │   sizeof(type)   ou   sizeof(expression)   │
              │                                              │
              │   compile-time. résultat : std::size_t.     │
              │   pas d'évaluation ; le TYPE est lu.         │
              └────────────────────┬───────────────────────┘
                                   │
              ┌────────────────────┼───────────────────────┐
              ▼                    ▼                       ▼
         types scalaires        tableaux             types utilisateur
        ─────────────────       ──────────────          ──────────
        sizeof(int)   ≈ 4       sizeof(int[10])         déterminé par le layout :
        sizeof(double)≈ 8       = 40                    membres + padding
        sizeof(void*) ≈ 8       sizeof(arr)/            classes polymorphiques :
                                sizeof(arr[0])          ajoutent 8 (vptr)

        pièges :
        - un tableau decay en pointer au passage → sizeof(arg) == 8
        - sizeof d'une empty class >= 1
        - impossible d'appliquer sizeof sur un type incomplet
        - impossible d'appliquer sizeof sur un bitfield
```
---

## 13. Pratique

1. Pourquoi `sizeof(crash())` est-il légal alors que l'appel de `crash()` terminerait le programme ? *(`sizeof` n'évalue pas l'opérande ; il regarde uniquement le type.)*
2. Pourquoi `sizeof(arr)` à l'intérieur d'une fonction (avec un paramètre `int arr[]`) renvoie-t-il la taille d'un pointer plutôt que la taille du tableau ? *(Array-to-pointer decay lors du passage de paramètres.)*
3. Pourquoi `sizeof(class C{})` donne-t-il 1 plutôt que 0 ? *(Pour que des objets vides distincts aient des adresses distinctes.)*
4. Pourquoi le layout de `struct {char c; int i;}` donne-t-il 8 octets, et non 5 ? *(Padding pour l'alignement : 1 octet pour `c`, 3 de padding, 4 pour `i`. Du tail padding peut également s'appliquer.)*