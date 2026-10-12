# References C++ — Guide complet

> **Focus sur les mots-clés :** [`CONST`](../../lexique/CONST.md) · [`THIS`](../../lexique/THIS.md)

## 1. Qu'est-ce qu'une reference ?

Une **reference** est un *alias* pour une variable existante. Une fois liée, la reference et l'original font reference au même emplacement mémoire — ce sont littéralement le même objet sous deux noms différents.

```cpp
int x = 42;
int &r = x;   // r est maintenant un autre nom pour x

r = 100;
std::cout << x;  // affiche 100
std::cout << &x << " == " << &r;  // même adresse
```

Syntaxe : le `&` se place **après le type** dans une déclaration. Cela signifie "reference to".

---

## 2. References vs Pointers (en venant du C)

En C, indirection = pointers. Le C++ ajoute les references comme une alternative plus sûre et plus propre pour la plupart des cas d'utilisation.

| Fonctionnalité | Pointer (`int *p`) | Reference (`int &r`) |
|---|---|---|
| Peut être null | Oui (`NULL` / `nullptr`) | **Non** — doit être liée à la création |
| Doit être initialisé | Non | **Oui** |
| Peut être réassigné à un autre objet | Oui (`p = &y;`) | **Non** — liée à vie |
| Syntaxe pour accéder à la valeur | `*p` | `r` (direct) |
| Syntaxe pour obtenir l'adresse | `p` | `&r` |
| Peut en faire un array | Oui | **Non** (`int &arr[10];` illégal) |
| Peut être `const` | Oui (`int *const p`) | Implicitement — les references sont toujours liées de manière `const` |
| Coût mémoire | Prend de la place (8 octets en 64 bits) | Généralement éliminé à l'optimisation par le compiler |

### Modèle mental
- Pointer = "une variable qui contient une adresse"
- Reference = "un surnom pour une variable existante"

---

## 3. Les trois règles absolues

1. **Doit être initialisée à la déclaration.**
   ```cpp
   int &r;       // ERREUR : les references doivent être liées
   int &r = x;   // OK
   ```

2. **Ne peut pas être réassignée (rebound).** Une fois que `r` fait reference à `x`, elle fait toujours reference à `x`.
   ```cpp
   int x = 1, y = 2;
   int &r = x;
   r = y;        // Cela ne réassigne PAS r à y.
                 // Cela assigne la valeur de y à x (r est toujours x).
   // Après cela : x == 2, y == 2, r fait reference à x
   ```

3. **Ne peut pas être null.**
   ```cpp
   int &r = *(int*)NULL;  // Comportement indéfini (UB) — ne faites pas cela
   ```
   Une reference valide fait toujours reference à un objet valide. Aucune vérification de null n'est nécessaire.

---

## 4. Pourquoi les references existent — cas d'utilisation

### 4.1 Passage par reference (éviter les copies)

En C, pour modifier une variable à l'intérieur d'une fonction, on passe son adresse :
```c
void increment(int *n) { (*n)++; }
increment(&x);
```

En C++, avec les references, c'est plus propre :
```cpp
void increment(int &n) { n++; }
increment(x);  // pas de &, pas de *, ça fonctionne tout simplement
```

La fonction **modifie directement la variable de l'appelant**, sans copie.

### 4.2 `const` reference — lecture seule, sans copie

La manière idiomatique de passer de "gros" objets (strings, classes) sans les copier *et* sans autoriser de mutation :

```cpp
void print(const std::string &s) {
    std::cout << s;
    // s = "oops"; // ERREUR : s est const
}
```

C'est la forme standard pour :
- Les copy constructors : `ClassName(const ClassName &src);`
- L'assignation par copie : `ClassName &operator=(const ClassName &rhs);`
- Tout paramètre de fonction où vous n'avez pas besoin de modifier l'argument.

### 4.3 Retour par reference — permettre le chaînage / l'accès lvalue

```cpp
ClassName &operator=(const ClassName &rhs) {
    // ...
    return *this;   // renvoie une reference vers l'objet courant
}

a = b = c;   // fonctionne car operator= renvoie une reference
```

Permet également à une fonction de renvoyer quelque chose qui peut apparaître sur le **côté gauche** d'une assignation :
```cpp
int &getElement(int idx) { return arr[idx]; }
getElement(3) = 99;   // modifie arr[3]
```

---

## 5. References `const` — la forme la plus courante

```cpp
const int &r = x;   // r est un alias en lecture seule pour x
```

Propriété spéciale : une `const` reference peut se lier à un **temporaire (rvalue)** :
```cpp
const int &r = 42;           // OK — la durée de vie de 42 est prolongée
const std::string &s = "hi"; // OK — la string temporaire est maintenue en vie
```

Une reference non-const **ne le peut pas** :
```cpp
int &r = 42;   // ERREUR
```

C'est pourquoi les paramètres de fonction sont presque toujours `const T &` — ils acceptent à la fois des variables et des littéraux/temporaires.

---

## 6. References vers des pointers, pointers vers des references

### Reference vers un pointer — légal
```cpp
int *p = &x;
int *&rp = p;   // rp est une reference vers le pointer p
rp = &y;        // modifie p, maintenant p pointe vers y
```

Utilisé lorsqu'une fonction doit modifier **l'adresse** que contient un pointer :
```cpp
void allocate(int *&ptr) { ptr = new int(5); }
```

### Pointer vers une reference — illégal
```cpp
int &*p;   // ERREUR : cela n'existe pas
```
Parce que les references ne sont pas des objets ayant leur propre adresse — `&r` donne l'adresse de ce à quoi `r` fait reference, et non de la reference elle-même.

---

## 7. Pièges courants

### 7.1 Dangling reference

Une reference qui survit à l'objet auquel elle fait reference → undefined behavior.

```cpp
int &bad() {
    int local = 5;
    return local;   // local meurt ici — la reference retournée est dangling
}

int &r = bad();
std::cout << r;     // UB : accès à de la mémoire morte
```

**Ne retournez jamais une reference vers une variable locale.** Retourner une reference vers un membre, une variable `static` ou `*this` ne pose aucun problème.

### 7.2 Confondre assignation et rebinding

```cpp
int x = 1, y = 2;
int &r = x;
r = y;   // assigne 2 dans x. r est toujours liée à x !
```

Si vous souhaitez un comportement de "rebinding", utilisez un pointer :
```cpp
int *p = &x;
p = &y;  // maintenant p pointe vers y — véritable rebinding
```

### 7.3 Reference dans les membres de class

Si une class a un membre de type reference, il **doit** être initialisé dans la liste d'initialisation du constructeur (pas dans le corps) :

```cpp
class HumanA {
private:
    Weapon &_weapon;
public:
    HumanA(std::string name, Weapon &w) : _weapon(w) {}  // DOIT utiliser la liste d'initialisation
};
```

Conséquence : les classes avec des membres de type reference ne peuvent généralement pas être réassignées (l'`operator=` généré par le compiler essaierait de faire un rebinding, ce qui est impossible).

---

## 8. Quand utiliser une reference vs un pointer

| Situation | Utilisation |
|---|---|
| Doit exister, aucun cas null | Reference |
| Peut être null / optionnel | Pointer |
| Besoin de rebind vers différents objets | Pointer |
| Stockage dans un conteneur (`std::vector<T&>` est illégal) | Pointer |
| Paramètre de fonction (gros objet en lecture seule) | `const T&` |
| Paramètre de fonction (modifier la variable de l'appelant) | `T&` |
| Retour de `operator=`, `operator<<`, etc. | Reference |
| Résultat d'une allocation dynamique (`new`) | Pointer |
| Interopérabilité avec les API C | Pointer |

**Règle empirique de 42 :** préférez les references lorsque l'objet *doit* exister et que vous n'avez pas besoin de rebind. Utilisez des pointers lorsque vous avez besoin de null, de réassignation ou d'arrays.

---

## 9. Exemples rapides

### Passage par reference
```cpp
void swap(int &a, int &b) {
    int tmp = a;
    a = b;
    b = tmp;
}

int x = 1, y = 2;
swap(x, y);   // x == 2, y == 1
```

### Paramètre const reference (idiomatique)
```cpp
void greet(const std::string &name) {
    std::cout << "Hello, " << name << "\n";
}
```

### Retourner *this par reference (OCF)
```cpp
Fixed &Fixed::operator=(const Fixed &rhs) {
    if (this != &rhs)
        _raw = rhs._raw;
    return *this;
}
```

### Membre reference (pattern ex03)
```cpp
class HumanA {
    std::string _name;
    Weapon &_weapon;   // fait toujours reference à une vraie Weapon
public:
    HumanA(std::string name, Weapon &w) : _name(name), _weapon(w) {}
    void attack() { std::cout << _name << " attacks with " << _weapon.getType(); }
};
```

vs `HumanB` qui peut ne pas toujours avoir une arme — utilise un pointer :
```cpp
class HumanB {
    std::string _name;
    Weapon *_weapon;   // peut être null
public:
    HumanB(std::string name) : _name(name), _weapon(NULL) {}
    void setWeapon(Weapon &w) { _weapon = &w; }
};
```

C'est exactement la raison pour laquelle `HumanA` utilise une reference et `HumanB` utilise un pointer dans l'exercice ex03.

---

## 10. Aide-mémoire récapitulatif

```cpp
int x = 10;

int  &r  = x;       // reference (alias)
const int &cr = x;  // const reference (alias en lecture seule, accepte les temporaires)

int *p = &x;        // pointer
int *&rp = p;       // reference vers un pointer

void f1(int n);           // par valeur (copie)
void f2(int *n);          // par pointer (peut être null)
void f3(int &n);          // par reference (doit exister, modifiable)
void f4(const int &n);    // par const reference (doit exister, lecture seule, pas de copie)

ClassName &operator=(const ClassName &rhs);  // signature canonique
```

**Règle d'or :** une reference n'est qu'un autre nom pour un objet existant. Si cette idée reste claire dans votre esprit, vous avez tout compris.