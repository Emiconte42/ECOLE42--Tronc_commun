# `const` — La promesse faite au compiler

> **TL;DR.** `const` signifie *"Je promets que cela ne changera pas après l'initialisation."* Le compiler fait respecter cette promesse ; le linker l'utilise pour placer les données dans la mémoire en lecture seule ; l'optimiseur l'utilise pour ignorer les rechargements redondants.

Voir aussi : [`BASICS.md`](../notions/fundamentals/BASICS.md#10.%20const%20%E2%80%94%20much%20more%20powerful%20than%20in%20C) · [`REFERENCE.md`](../notions/fundamentals/REFERENCE.md) · [`STATIC.md`](STATIC.md) · [`MUTABLE.md`](MUTABLE.md)

---

## 1. Le modèle mental

`const` est **un contrat**, pas une classe de stockage. Il indique : *"à travers ce nom, la valeur ne peut pas être modifiée."*

Il s'applique à ce qui se trouve **immédiatement à sa gauche** — et s'il n'y a rien à sa gauche, à ce qui se trouve immédiatement à sa droite.

```cpp
const int   a = 1;   // const applies to 'int'         → "constant int"
int   const b = 1;   // identical: 'const' applies to 'int' on its left
const int  *p;       // pointer to const int           → can't change *p
int  *const q = &x;  // const pointer to int           → can't change q
const int *const r = &x; // const pointer to const int → neither can change
```

### Lisez-le de droite à gauche

```
const int *const r;
       │     │
       │     └─ r itself is const
       └─────── what r points to is const
```

```
       int  ← what r points to  ← const ← * ← const ← r
       │                                          │
       └─ "r is a const pointer to a const int"
```

---

## 2. Les quatre emplacements où vit `const`

```
   ┌───────────────────────────────────┐
   │ 1. const variable (file/local)    │   const int N = 10;
   ├───────────────────────────────────┤
   │ 2. const reference / pointee      │   void f(const std::string& s);
   ├───────────────────────────────────┤
   │ 3. const member function          │   int getX() const;
   ├───────────────────────────────────┤
   │ 4. const member variable          │   const int _id;
   └───────────────────────────────────┘
```

Chacun indique une chose différente au compiler. Assimilez-les.

---

## 3. Les variables `const` — où vivent-elles ?

```cpp
const int N = 10;     // file scope
int main() {
    const int M = 20; // local scope
}
```

| Où elle vit | Ce que fait habituellement le compiler |
|---|---|
| **`const` de portée fichier avec initialiseur littéral** | Intégrée inline dans une section en lecture seule du binaire (`.rodata`/`.rdata`). Peut même ne pas occuper d'adresse à l'exécution. |
| **`const` locale avec initialiseur littéral** | Souvent éliminée par optimisation — la valeur est propagée directement dans les instructions (constant folding). |
| **`const` avec un initialiseur à l'exécution** (`const int n = rand();`) | Vit sur la stack comme n'importe quelle variable locale. Le `const` est purement une vérification à la compilation. |

```
Process memory layout (typical Linux/macOS)
┌──────────────────┐ high addr
│      stack       │  ← local 'const' lives here unless optimized away
├──────────────────┤
│       heap       │  ← 'new'-allocated objects
├──────────────────┤
│       BSS        │  ← uninitialized globals
├──────────────────┤
│      data        │  ← initialized globals (writable)
├──────────────────┤
│     rodata       │  ← string literals + 'const' globals
├──────────────────┤
│       text       │  ← machine code
└──────────────────┘ low  addr
```

Vérifiez-le sous Linux :
```bash
$ objdump -t a.out | grep " N$"
0000000000002004 g     O .rodata    0000000000000004  N
                            ^^^^^^^^   N is in .rodata — read-only
```

Écrire à travers un pointer dépouillé de son caractère constant via `const_cast` vers une telle mémoire est un **undefined behavior** et produira un SIGSEGV sur la plupart des systèmes — l'OS marque les pages `.rodata` en lecture seule au niveau de la table des pages.

```
        page table entry              MMU
        ┌────────────────────────┐    
        │ phys addr | R | W | X  │  → if W=0 and the CPU writes,
        │   …       │ 1 │ 0 │ 0  │    you get SIGSEGV (signal 11)
        └────────────────────────┘
```

---

## 4. Paramètres & references `const` — la bête de somme

Il s'agit de l'utilisation la plus courante de `const` en C++ en pratique :

```cpp
void print(const std::string& s);   // pass-by-const-reference
```

Ce que cela vous apporte :

- **Aucune copie.** Vous passez un alias — typiquement un pointer que le compiler masque — au lieu de dupliquer l'objet.
- **La fonction ne peut pas modifier vos données.** Le compiler s'en assure.
- **Elle se lie aux littéraux et aux temporaires.** Une reference non-const ne peut pas se lier à `"hello"` ; une `const std::string&` le peut (après une unique conversion).

```cpp
void f(std::string& s);          // can't accept "hello"
void g(const std::string& s);    // can accept "hello", "x", x, std::string("y"), …
```

### Vue matérielle

```
caller                    callee
┌───────────────┐         ┌────────────────────┐
│ s = "long..." │ ◄─────  │ const std::string& │
│ (heap buffer) │  addr   │  s in callee       │
└───────────────┘         └────────────────────┘
   the heap-allocated buffer is shared. Only the
   address (8 bytes on x86_64) is passed in a register.
```

Comparez avec le passage par valeur (pass-by-value) :

```
caller                    callee
┌───────────────┐         ┌────────────────────┐
│ s = "long..." │ ───┐    │ std::string s      │
│               │    └──► │  (full copy made)  │
└───────────────┘         └────────────────────┘
   a malloc + memcpy happens on every call.
```

**Règle empirique :** si le type est plus grand qu'un `void*` (8 octets) et que vous n'avez pas besoin de le copier, passez par `const T&`.

---

## 5. Fonctions membres `const` — le const à la fin

```cpp
class Circle {
    double _r;
public:
    double area() const { return 3.14159 * _r * _r; }   // const member fn
    void   setRadius(double r) { _r = r; }              // non-const
};
```

Ce que fait `const` après la liste de paramètres :

- Promet que la fonction ne modifie pas l'état observable de l'objet.
- Fait en sorte que `this` soit de type `const Circle*` au lieu de `Circle*` à l'intérieur de la fonction.
- Est **requis** pour appeler la fonction sur un `const Circle` (ou `const Circle&`, ou `const Circle*`).

```cpp
const Circle c(5.0);
c.area();         // OK
c.setRadius(3);   // ERROR — non-const member fn on const object
```

### Pourquoi la règle existe — la vtable n'est d'aucune aide ici

Il s'agit purement d'une **vérification du système de types**. Rien ne change au niveau du code machine : les mêmes instructions sont exécutées pour les fonctions membres `const` et non-const ; c'est uniquement le compiler qui refuse de compiler l'appel.

### Surcharge avec `const` (`const` overloading)

Vous pouvez avoir deux fonctions membres avec le même nom et les mêmes paramètres qui ne diffèrent que par `const` :

```cpp
class Buffer {
    char _data[1024];
public:
    char&       operator[](std::size_t i) { return _data[i]; }   //mutable access
    
    const char& operator[](std::size_t i) const { return _data[i]; } //read-only};


Buffer       b;
const Buffer cb;

b[0] = 'x';        // calls non-const overload — OK
char c = cb[0];    // calls const overload — OK
cb[0] = 'x';       // ERROR — const overload returns const char&
```

### Réaction en chaîne de la `const`-correctness

Si `getName()` est `const`, chaque fonction qu'elle appelle sur `*this` doit également être `const`. Un seul `const` manquant se répercute sur toute votre class. Ajoutez `const` à un getter dès le départ, pas à la fin.

---

## 6. Variables membres `const`

```cpp
class Contact {
    const int _id;          // every Contact has its own _id, set once
public:
    Contact(int id) : _id(id) {}   // MUST use init list — assignment in body fails
};
```

Contraintes :
- **Doit être initialisée dans l'initializer list.** Une affectation dans le corps du constructeur constituerait une écriture sur un élément `const`.
- **Aucune affectation par copie par défaut.** L'`operator=` généré par le compiler devrait écrire dans `_id`. Si vous avez besoin de l'affectation, écrivez-la vous-même et ne mettez pas à jour `_id`, ou reconsidérez la pertinence de le déclarer `const`.

```cpp
Contact a(1), b(2);
a = b;  // ERROR: copy assignment implicitly deleted because _id is const
```

C'est l'une des raisons les plus fréquentes pour lesquelles "l'OCF compile pour tout le monde sauf pour moi" — une variable membre `const` oubliée désactive silencieusement l'`operator=`.

---

## 7. La table pointer / reference

Mémorisez ceci. C'est la source de 80 % de la confusion autour de `const`.

| Déclaration | Peut réassigner la cible (rebind) ? | Peut modifier la cible ? |
|---|:-:|:-:|
| `int *p`              | oui | oui |
| `const int *p`        | oui | **non** |
| `int const *p`        | oui | **non** *(identique à ci-dessus)* |
| `int *const p`        | **non** | oui |
| `const int *const p`  | **non** | **non** |
| `int &r`              | n/a *(les refs ne rebindent jamais)* | oui |
| `const int &r`        | n/a | **non** |

Une reference est *implicitement* liée de manière constante ("const-bound") — il n'existe pas de syntaxe pour `int &const r` car cela serait redondant. Seule la *cible* d'une reference peut être `const`.

---

## 8. Constantes vs `#define`

```cpp
#define MAX 100              // C-style — preprocessor textual replacement
const int MAX = 100;         // C++ — typed, scoped, debuggable
```

Pourquoi `const` l'emporte :
- **Possède un type.** `MAX` est un `int`, pas "ce à quoi 100 ressemble sur le site d'appel".
- **Possède une portée (scope).** Vivre à l'intérieur d'un namespace ou d'une class limite sa portée.
- **Possède une adresse.** Vous pouvez prendre `&MAX` et le passer à une fonction (le compiler matérialisera son adresse si nécessaire).
- **Survit dans le debugger.** Les noms créés par `#define` ont disparu au moment où gdb examine le binaire.
Utilisez `#define` uniquement pour les include guards et la compilation conditionnelle, pas pour les constantes.

---

## 9. Astuces et conseils

### 9.1 Mettez `const` sur tout ce que vous ne modifiez pas

Visez le "const par défaut". Le coût mental est faible ; la protection est réelle. Si vous vous surprenez à vouloir retirer un `const`, demandez-vous d'abord si vous ne devriez pas plutôt modifier le flux de données.

### 9.2 L'échappatoire `const_cast` — presque jamais la solution

```cpp
void legacy_api(char *s);           // prend un char*, n'écrit pas réellement

const char *msg = "hello";
legacy_api(const_cast<char *>(msg)); // compile, mais : écrire dans msg est un UB
```

N'utilisez `const_cast` que lors de l'interfaçage avec une ancienne API qui *promet* de ne pas écrire mais prend un pointer non-const. Jamais pour "faire disparaître l'erreur".

### 9.3 `mutable` — l'échappatoire `const` légitime

Un membre déclaré `mutable` peut être modifié depuis une member function `const`. Utilisé pour les caches, les mutexes, l'initialisation paresseuse. Voir [`MUTABLE.md`](MUTABLE.md).

### 9.4 Retourner par valeur `const` ? Ne le faites pas.

```cpp
const std::string getName() const;   // le 'const' externe est inutile en C++98
```

En C++98, cela bloque simplement des appels comme `getName().clear()` — mais cela bloque aussi la sémantique de déplacement en C++11+. Évitez-le. Rendez la *member function* `const`, pas la valeur de retour.

### 9.5 `const` ne signifie PAS "constante à la compilation"

```cpp
const int n = readFromFile();   // légal — la valeur de n est calculée au runtime
int arr[n];                     // ILLÉGAL en C++98 — nécessite une expression constante
```

Pour les constantes à la compilation en C++98, utilisez `enum { N = 10 };` ou un entier `static const` initialisé dans le header. Pour les constantes à la compilation en C++11+, utilisez `constexpr`.

### 9.6 Le `const` de premier niveau sur les paramètres est ignoré pour la correspondance de signature

```cpp
void f(int x);
void f(const int x);    // MÊME fonction. Le linker ne voit qu'un seul symbole.
```

Mais :

```cpp
void f(int *p);
void f(const int *p);   // Fonctions DIFFÉRENTES. Le const du pointé compte.
```

Le niveau supérieur sur un paramètre par valeur n'est qu'une indication pour l'implémenteur. Le const du pointé/référé fait partie du type.

---

## 10. Erreurs de compilation courantes

| Erreur | Cause | Solution |
|---|---|---|
| `passing 'const X' as 'this' argument discards qualifiers` | Appel d'une member fn non-const sur un objet const | Ajouter `const` à cette member fn |
| `assignment of read-only location` | Écriture dans `*p` où `p` est `const T*` | Retirer le `const` ou arrêter d'écrire |
| `uninitialized const member '_x'` | Membre `const` sans entrée dans l'initializer-list | Initialiser dans `: _x(value)` |
| `binding reference of type 'X&' to 'const X' discards qualifiers` | Passage d'un objet `const` à un paramètre `T&` | Passer le paramètre en `const T&` |

---

## 11. Résumé visuel

```
                ┌────────────────────────────────────────┐
                │             const X                     │
                │   "cet objet ne peut pas changer"       │
                └─────────────────┬──────────────────────┘
                                  │
        ┌─────────────────────────┼─────────────────────────┐
        ▼                         ▼                         ▼
   const variable          const reference          const member fn
   (données dans .rodata   (alias qui ne            (this devient
    ou stack, impossible    modifiera pas la cible)   const T*; peut être
    d'écrire via                                      appelée sur des objets const)
    ce nom)
                              │
                              ▼
                       const T&  →  pass-by-reference
                                    + no-copy
                                    + lie les temporaires
```

---

## 12. Questions d'entraînement pour vous tester

1. Pourquoi `int& r = 42;` échoue-t-il alors que `const int& r = 42;` fonctionne ? *(réponse : une référence non-const nécessite une vraie lvalue ; une const ref crée un temporaire caché avec une durée de vie prolongée.)*
2. Deux member functions peuvent-elles être overload uniquement sur `const` ? *(oui — c'est le pattern standard pour les getters qui retournent des vues modifiables/immuables.)*
3. Où réside un `const int N = 10;` de portée fichier dans le binaire ? *(généralement dans `.rodata` si son adresse est prise ; sinon inliné et jamais matérialisé.)*
4. Pourquoi est-il légal de déclarer `const int *p` sans l'initialiser, alors que `const int& r` ne l'est pas ? *(les pointers peuvent être nuls ; les references doivent être liées à un objet réel.)*