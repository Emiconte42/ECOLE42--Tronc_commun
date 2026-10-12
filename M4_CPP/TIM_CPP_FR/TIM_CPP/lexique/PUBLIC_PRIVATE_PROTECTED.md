# `public` / `private` / `protected` — Contrôle d'accès

> **TL;DR.** Trois mots-clés qui étiquettent des sections d'une class. Ils contrôlent **qui peut voir les noms des membres** au moment de la compilation. Il n'y a aucun contrôle à l'exécution, aucun stockage supplémentaire, aucun ralentissement — juste une promesse côté compiler concernant qui a le droit de manipuler quoi.

Articles connexes : [`CLASS.md`](CLASS.md) · [`STRUCT.md`](STRUCT.md) · [`FRIEND.md`](FRIEND.md) · [`INHERITANCE.md`](../notions/oop/INHERITANCE.md) · [`GETTERS_SETTERS.md`](../notions/oop/GETTERS_SETTERS.md)

---

## 1. Les trois étiquettes

```cpp
class C {
public:                          // visible par tout le monde
    int  publicData;
    void publicFn();

protected:                       // visible par C et par les classes dérivées
    int  protectedData;
    void protectedFn();

private:                         // visible uniquement à l'intérieur de C lui-même
    int  privateData;
    void privateFn();
};
```

Chaque étiquette modifie l'accès pour **tout ce qui suit** jusqu'à l'étiquette suivante. Vous pouvez avoir autant d'étiquettes que vous le souhaitez, dans n'importe quel ordre.

---

## 2. La matrice d'accès

```
                 │   accédé par     │ accédé par       │ accédé par
                 │   la classe      │ une classe dérivée│ du code extérieur
                 │   elle-même      │ (sa sous-classe) │ (un étranger)
─────────────────┼──────────────────┼──────────────────┼─────────────────
  public         │   oui            │   oui            │   oui
─────────────────┼──────────────────┼──────────────────┼─────────────────
  protected      │   oui            │   oui            │   non
─────────────────┼──────────────────┼──────────────────┼─────────────────
  private        │   oui            │   non            │   non
```

Mémorisez ce tableau. Chaque erreur d'accès déroutante que vous rencontrerez correspond à une ligne de celui-ci.

---

## 3. Exemple pratique

```cpp
class Animal {
public:
    void speak() const { _doSound(); }

protected:
    virtual void _doSound() const = 0;          // pure virtual

private:
    int _hunger;                                 // état propre à la base
};

class Dog : public Animal {
protected:
    void _doSound() const { /* bark */ }         // OK : protected → accessible dans la sous-classe
    // peut aussi toucher à... eh bien, pas _hunger (privé à Animal).
};

void someoneOutside(const Animal &a) {
    a.speak();          // OK   — public
    // a._doSound();    // ERREUR — protected
    // a._hunger;       // ERREUR — private
}
```

---

## 4. Uniquement à la compilation — il n'y a pas de barrière de sécurité

Le contrôle d'accès est appliqué **purement par le compiler**. Les octets d'un membre privé se trouvent à un offset connu et sur la même page mémoire que tout le reste.

```cpp
class Box {
private:
    int _secret;
};

Box b;
int *p = (int*)&b;
*p = 42;            // parfaitement légal ; b._secret vaut maintenant 42 (strictement parlant UB, mais
                    //                                                    portable sur les vrais compilers)
```

Ainsi, `private` n'est **pas** une fonctionnalité de sécurité. C'est une fonctionnalité de **conception** — elle vous indique : « le mainteneur ne voulait pas que vous touchiez à ceci ; si vous y accédez de force, vous assumez les conséquences. »

```
   à la compilation          à l'exécution
   ────────────────          ─────────────
   class Box {               mémoire d'un objet Box :
     int _secret;            ┌─────────────┐
   };                        │  _secret    │   ← int ordinaire, mémoire ordinaire,
                             └─────────────┘     adressable normalement.
   compiler :                                    aucune vérification à l'exécution.
     "_secret" est private.
     rejeter tout code qui
     tente de le nommer depuis
     l'extérieur de Box.
```

---

## 5. L'agencement d'une classe selon la norme 42

Dans la norme 42, l'ordre est conventionnellement le suivant :

```cpp
class MyClass {
private:
    // variables membres (état)
    std::string _name;
    int         _age;

public:
    // constructeurs / destructeur d'abord
    MyClass();
    MyClass(const std::string& name, int age);
    MyClass(const MyClass& other);
    MyClass& operator=(const MyClass& other);
    ~MyClass();

    // accesseurs ensuite
    const std::string& getName() const;
    int                getAge() const;
    void               setName(const std::string& name);
    void               setAge(int age);

    // autres comportements publics à la fin
    void greet() const;
};
```

Certaines bases de code l'inversent (public d'abord, private à la fin). Les deux approches conviennent. Choisissez-en une et restez cohérent.

---

## 6. `protected` — la passerelle de l'héritage

`protected` est le *moins courant* des trois. Utilisez-le lorsqu'une classe dérivée a besoin de lire ou de modifier directement une partie de l'état de la classe de base :

```cpp
class Animal {
protected:
    std::string _type;          // l'identifiant de type — défini par la sous-classe

public:
    Animal() : _type("Animal") {}
    virtual ~Animal() {}
    const std::string& getType() const { return _type; }
};

class Dog : public Animal {
public:
    Dog() { _type = "Dog"; }    // accès protected — OK
};
```

Si `_type` était privé, `Dog` aurait besoin d'un setter sur `Animal` — c'est parfois la bonne solution, parfois une corvée inutile. Décidez au cas par cas.

### Règle générale

- `private` par défaut. La plupart des membres.
- `protected` uniquement lorsqu'une sous-classe en a clairement besoin.
- `public` pour le contrat de la classe.

Une classe où tout est `protected` est suspecte ; vous avez rendu l'interface d'héritage aussi large que l'interface publique.

---

## 7. Spécificateur d'accès à l'héritage — `class D : public B` vs `class D : private B`

Il existe un **second** axe où ces mots-clés apparaissent : le spécificateur d'héritage.

```cpp
class B { public: void f(); protected: void g(); private: void h(); };

class Pub  : public    B { /* … */ };       // héritage standard de type "est-un"
class Prot : protected B { /* … */ };       // rare
class Priv : private   B { /* … */ };       // "implémenté en termes de"
```

Ce qui change pour **la classe dérivée elle-même** : rien — les membres `private` de `B` restent inaccessibles depuis `Pub`, `Prot` ou `Priv`.

Ce qui change pour **le reste du monde observant la classe dérivée** :

| Héritage | `B::f` (public) devient… | `B::g` (protected) devient… | `B::h` (private) devient… |
|---|---|---|---|
| `: public B`    | public dans la dérivée    | protected dans la dérivée    | inaccessible |
| `: protected B` | protected dans la dérivée | protected dans la dérivée    | inaccessible |
| `: private B`   | private dans la dérivée   | private dans la dérivée      | inaccessible |

L'héritage `public` est le **seul** qui modélise la relation « est-un ». Utilisez-le pour le polymorphisme ; les autres sont avancés/inhabituels.

Les modules de 42 utilisent presque exclusivement `: public Base`. Faites de même.

---

## 8. `friend` outrepasse l'accès (brièvement)

Une déclaration `friend` dans une classe A indique : « cette fonction ou cette classe peut accéder à mes membres private/protected ». Voir [`FRIEND.md`](FRIEND.md).

```cpp
class Account {
    int _balance;
    friend std::ostream& operator<<(std::ostream&, const Account&);
};
```

`friend` est le seul moyen légal de donner à un identifiant extérieur l'accès à un état privé.

---

## 9. Vision matériel/compiler — ce que ces mots-clés génèrent réellement

Rien.

```
   class A { public:    int x; };
   class B { private:   int x; };

   sizeof(A) == sizeof(B) == 4
   le layout, l'alignement et le code généré pour A et B sont identiques.
   le compiler REFUSE simplement de compiler certaines expressions d'accès
   pour B qu'il ACCEPTE pour A.
```

Il n'y a pas de métadonnées par objet, pas de champs supplémentaires, pas de vérifications à l'exécution. C'est une application purement issue du système de types.

---

## 10. Trucs & astuces

### 10.1 `private` par défaut

Rendez les membres privés ; exposez-les via des fonctions membres `public` si nécessaire. C'est l'encapsulation résumée en une phrase. Ouvrir un accès plus tard ne coûte rien ; en retirer un est un changement cassant.

### 10.2 Gardez votre surface publique restreinte

Une classe est plus facile à maintenir lorsque ses méthodes publiques sont peu nombreuses et bien nommées. Si vous vous retrouvez à écrire `getX/setX` pour chaque membre, demandez-vous si les données ne devraient pas être encapsulées différemment — en passant une struct, en renvoyant une copie, etc. (Voir [`GETTERS_SETTERS.md`](../notions/oop/GETTERS_SETTERS.md).)

### 10.3 Les sections peuvent se répéter

```cpp
class C {
public:    void f();
private:   int  _a;
public:    void g();           // légal — mais les lecteurs préfèrent une seule section de chaque
private:   int  _b;
};
```

Évitez cela dans la norme 42. Une seule section `private:`, une seule section `public:`, dans un ordre cohérent.

### 10.4 `public:` après `class` est implicite pour `struct`

```cpp
struct S {
    int x;            // public — aucune étiquette nécessaire
};
```

Vous pouvez toujours ajouter `private:` et `protected:` à une struct si vous souhaitez masquer une partie de son état.

### 10.5 Les classes déclarées de manière anticipée n'ont aucune information d'accès

```cpp
class C;            // forward declaration
C *p;               // OK
p->member;          // ERREUR — le type est incomplet ; impossible de vérifier l'accès
```

Vous avez besoin de la définition complète avant que les vérifications d'accès ne s'appliquent.

### 10.6 La « loi de Déméter » — n'enchaînez pas les getters

```cpp
// Mauvaise pratique en C++ :
phonebook.getContact(0).getAddress().getZip();   // exposition de chaînes de détails internes

// meilleur :
phonebook.zipOfContact(0);                       // une seule méthode révélant l'intention
```

Lorsqu'une méthode n'existe que pour en alimenter une autre, demandez-vous si la chaîne ne peut pas être condensée en une seule opération.
### 10.7 Les bugs de contrôle d'accès sont généralement des bugs de conception

Si vous êtes tenté d'écrire `friend` pour une fonction dans votre *propre* code (pas pour `operator<<`, etc.), prenez du recul : peut-être que la fonction devrait être un member, ou peut-être que les données devraient être exposées via une abstraction différente.

---

## 11. Erreurs courantes

| Erreur | Cause | Solution |
|---|---|---|
| `'x' is private within this context` | Accès à un élément private depuis l'extérieur | Ajouter un getter/setter ou un `friend` |
| `'x' is protected within this context` | Accès à un élément protected depuis l'extérieur (un appelant non dérivé) | Si l'accès est nécessaire, réfléchissez au pourquoi ; sinon, ajoutez un getter |
| Surprise de slicing : les champs spécifiques à derived disparaissent après la copie vers base | (Pas lié aux accès, mais adjacent) | Passer par reference, ou utiliser le clonage virtuel |
| `cannot access private member declared in base class` après `class D : private B` | Les members de B ne sont pas exposés via l'interface public de D | Utiliser `: public B`, ou écrire des member fns public dans D qui transfèrent l'appel |

---

## 12. Résumé visuel

```
                ┌──────────────────────────────────────────┐
                │  class C {                                │
                │     private:    /* internal only */       │
                │                                            │
                │     protected:  /* + subclasses */        │
                │                                            │
                │     public:     /* + everyone */          │
                │  };                                        │
                └─────────────────────┬──────────────────────┘
                                      │
                ┌─────────────────────┼─────────────────────┐
                ▼                     ▼                     ▼
            visibilité            mode                aucun coût
            des members           d'inheritance       au runtime
            ──────────            ──────────          ──────────
            qui peut nommer       aspect des          appliqué au
            X::y dans le code     members depuis      compile time
                                  l'extérieur de      uniquement.
                                  la derived class    le layout mémoire
                                                      reste inchangé.
```

---

## 13. Pratique

1. Pourquoi `private` n'est-il pas une frontière de sécurité ? *(Il est appliqué au compile-time ; les octets sont toujours dans la mémoire ordinaire et accessibles via des casts.)*
2. Quel mode d'inheritance correspond à « est-un » (« is-a ») ? *(Public.)*
3. Pourquoi passer de `class C` à `struct C` change-t-il le sens du code qui utilise les members de `C` ? *(L'accès par défaut change — ce qui était `private` dans `class` devient `public` dans `struct`.)*
4. Peut-on avoir `private:`, puis `public:`, puis à nouveau `private:` ? *(Légal, mais considéré comme un mauvais style.)*