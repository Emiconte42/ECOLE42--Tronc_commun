# `mutable` — Le membre qui peut changer à travers `const`

> **TL;DR.** `mutable` permet à un membre d'être modifié même lorsqu'il est accédé via une member function `const` (ou une instance `const`). Utilisez-le pour les éléments qui changent sans modifier l'**état observable** — caches, mutexes, valeurs évaluées paresseusement (*lazy values*).

Connexe : [`CONST.md`](CONST.md) · [`STATIC.md`](STATIC.md)

---

## 1. La motivation

Parfois, une member function `const` a besoin d'écrire dans *quelque chose* :

```cpp
class Date {
    int _year, _month, _day;
public:
    std::string toString() const {
        // nous voulons mettre en cache la chaine formatee pour ne pas la recalculer
        // ... mais c'est une member function const. nous ne pouvons pas ecrire dans _cache.
    }
};
```

Le modèle mental de l'utilisateur est : *"appeler `toString()` ne modifie pas la date."* C'est vrai — cela ne modifie pas `_year`, `_month`, `_day`. Mais en interne, nous voulons **mémoïser**. C'est là que `mutable` intervient.

```cpp
class Date {
    int _year, _month, _day;
    mutable std::string _cached;       // peut etre ecrit meme depuis des member fns const
    mutable bool        _isCached;
public:
    Date() : _year(0), _month(0), _day(0), _isCached(false) {}

    const std::string& toString() const {
        if (!_isCached) {
            _cached  = format(_year, _month, _day);
            _isCached = true;
        }
        return _cached;
    }
};
```

`toString()` reste **logiquement** const — deux appels consécutifs retournent la même chaîne et personne à l'extérieur ne peut savoir que le cache est impliqué.

---

## 2. La règle

À l'intérieur d'une member function `const` :

```
   membres non-mutable   →   lecture seule (this est const T*)
   membres mutable       →   lecture/écriture
```

Un membre `mutable` est traité comme si `this` était non-const, *uniquement pour ce membre*.

```cpp
class X {
    int           _a;      // objet const → ecriture impossible
    mutable int   _b;      // objet const → ecriture POSSIBLE
public:
    void f() const {
        _a = 1;   // ERREUR — _a n'est pas mutable
        _b = 1;   // OK     — _b est mutable
    }
};
```

---

## 3. Disposition en mémoire — `mutable` ne change rien

`mutable` est une **annotation du système de types** pour le compiler. Au niveau matériel, il n'y a aucune différence :

```
class Date          disposition en mémoire (sans padding représenté) :
┌───────────────────────────────────────────────────────────┐
│ _year (int)  │ _month (int) │ _day (int) │ _cached (str)  │
│              │              │            │ _isCached (b)  │
└───────────────────────────────────────────────────────────┘
       tous ces elements sont a des offsets fixes,
       tous sont en vraie memoire lecture/ecriture.
       'mutable' ne les deplace pas ; c'est un indice uniquement pour le compiler.
```

Même `const Date d;` réside dans une mémoire accessible en écriture s'il est sur la stack — la page est en lecture/écriture au niveau de l'OS. Le compiler refuse simplement de compiler les écritures à travers l'interface `const`. `mutable` crée une exception.

Si vous placez un `const Date` dans `.rodata` (par exemple, une variable globale avec initialisation littérale) et que vous écrivez ensuite dans un membre `mutable`, vous obtenez un SIGSEGV à l'exécution — les tables de pages ne se soucient pas de `mutable`. Ne faites pas cela.

---

## 4. Les cas d'utilisation légitimes

### 4.1 Initialisation paresseuse (*lazy initialization*) / mémoïsation

```cpp
class Polynomial {
    std::vector<double> _coeffs;
    mutable bool        _hasRoots;
    mutable std::vector<double> _roots;
public:
    const std::vector<double>& roots() const {
        if (!_hasRoots) {
            _roots    = computeRoots(_coeffs);
            _hasRoots = true;
        }
        return _roots;
    }
};
```

Le calcul des racines est coûteux. La mise en cache est interne. L'appelant ne voit jamais le cache.

### 4.2 Protection par mutex dans les opérations const

```cpp
class ThreadSafeCounter {
    int _count;
    mutable pthread_mutex_t _mu;
public:
    int get() const {
        pthread_mutex_lock(&_mu);            // mu est mutable → ok
        int c = _count;
        pthread_mutex_unlock(&_mu);
        return c;
    }
};
```

Verrouiller un mutex est une écriture, même lors d'une opération de "lecture". `mutable` est la réponse standard.

### 4.3 Décompte de références / compteurs de debug

```cpp
class Tracked {
    mutable int _accessCount;
public:
    int value() const { ++_accessCount; return _v; }
    int accessCount() const { return _accessCount; }
private:
    int _v;
};
```

Déterminer si cela compte comme un "état observable" relève du jugement au cas par cas. La norme 42 ne l'interdit pas.

---

## 5. Les cas d'utilisation illégitimes

`mutable` n'est **pas** fait pour :

- **Contourner la `const`-correctness par paresse.** Si vous vous retrouvez à rendre la moitié de vos membres mutable, votre conception est mauvaise.
- **Stocker un état visible par l'utilisateur.** Si deux appels const retournent des valeurs différentes pour des raisons qui importent à l'appelant, ce n'est pas const.
- **Contourner une API que vous ne voulez pas modifier.** Ajoutez plutôt une overload non-const.

Un test :
> *Un appelant raisonnable pourrait-il écrire un test basé sur les propriétés qui affirme : "deux appels consécutifs retournent la même chose" ?*

Si oui — l'état de mise en cache est interne — `mutable` convient.
Si non — l'état observable change — votre fonction ne devrait pas être `const`.

---

## 6. Trucs & astuces

### 6.1 `mutable` ne supprime PAS `const` partout

```cpp
class X {
    mutable int _n;
public:
    void f() const {
        _n = 1;            // OK
        modify(*this);     // ERREUR si modify prend X& — *this est toujours const X&
    }
    void modify(X& x);
};
```

`mutable` n'assouplit l'accès qu'à ce **membre spécifique**. Le pointer `this` reste `const X*` partout ailleurs.

### 6.2 `mutable` et les references

```cpp
class X {
    mutable int& _ref;     // legal — la ref est liee a la construction, on peut ecrire a travers elle
};
```

Remarque : une reference `mutable` ne vous permet pas de la relier (*rebind*, les refs ne se relient jamais), seulement d'écrire à travers elle.

### 6.3 `mutable` ne peut pas être appliqué à

- **Des membres `const`** — `mutable const int x;` est une contradiction ; le compiler le rejette.
- **Des membres `static`** — ils ne font pas partie de la disposition par instance ; `mutable` n'a aucun sens sur eux.
- **Des references vers des données const** — `mutable const T&` est également rejeté.

### 6.4 Ne pas combiner avec `volatile` sans raison valable

`mutable volatile T` est valide mais rare — typiquement un registre matériel accédé depuis un wrapper "logiquement const". Si vous n'écrivez pas de pilote (*driver*), vous n'en avez pas besoin.

### 6.5 Documenter la raison d'être de chaque `mutable`

En revue de code, `mutable` est un signal d'alerte. Un commentaire d'une ligne `// met en cache le résultat d'un computeRoots() coûteux` le rend acceptable.

---

## 7. Erreurs courantes

| Erreur | Cause | Solution |
|---|---|---|
| `assignment of member 'X::_n' in read-only object` | Écriture dans un membre non-mutable depuis une member fn const | Ajouter `mutable` s'il s'agit d'un cache ; sinon retirer `const` de la fonction |
| `'mutable' is incompatible with 'const'` | `mutable const int x;` | Choisir l'un des deux — `mutable` et `const` ensemble n'ont aucun sens |
| Crash / SIGSEGV lors de l'écriture d'un membre mutable d'une variable globale `const` | L'objet est dans `.rodata` ; l'OS rejette l'écriture | Rendre la globale non-`const`, ou externaliser le cache |

---

## 8. Résumé visuel

```
                          class X {
                              int       _a;        ← respecte const, observable
                              mutable int _b;      ← exception const, interne
                          };

                                 │
            void f() const  ─────┘
                 │
                 │   vue de *this par le compiler :
                 │   ┌────────────────────────────────────┐
                 ▼   │   _a:   const int   (ecriture imp.)│
                     │   _b:         int   (ecriture poss.)│
                     └────────────────────────────────────┘

                   "logiquement const" mais avec
                  un état interne autorisé à muter
```

---

## 9. Pratique

1. Pourquoi `mutable static int n;` est-il rejeté ? *(Les membres static ne sont pas liés à une instance, l'exception liée au const-this ne s'applique donc pas.)*
2. Pourriez-vous implémenter un getter `const` thread-safe sans `mutable` ? *(Oui — rendez le mutex externe, ou passez-le en paramètre. Mais `mutable` est l'idiome standard.)*
3. Écrire dans un membre `mutable` d'une variable globale `const X g;` fonctionnera-t-il à l'exécution ? *(Parfois — cela dépend si le linker a placé `g` dans `.rodata`. Si c'est le cas, vous aurez un SIGSEGV. Ne combinez pas globale `const` + membre `mutable`.)*