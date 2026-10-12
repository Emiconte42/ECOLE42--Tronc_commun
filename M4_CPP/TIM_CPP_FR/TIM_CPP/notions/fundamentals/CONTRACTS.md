# 🤝 Les contrats — La colonne vertébrale cachée du C++

> **Zoom sur les mots-clés :** [`CONST`](../../lexique/CONST.md) · [`EXPLICIT`](../../lexique/EXPLICIT.md) · [`PUBLIC/PRIVATE/PROTECTED`](../../lexique/PUBLIC_PRIVATE_PROTECTED.md) · [`VIRTUAL`](../../lexique/VIRTUAL.md) · [`STATIC`](../../lexique/STATIC.md)
>
> **Carrefours thématiques :** [`BASICS`](BASICS.md) · [`MEMORY`](MEMORY.md) · [`oop/ORTHODOX_CANONICAL_FORM`](../oop/ORTHODOX_CANONICAL_FORM.md) · [`oop/POLYMORPHISM`](../oop/POLYMORPHISM.md) · [`io-errors/ERROR_MANAGEMENT`](../io-errors/ERROR_MANAGEMENT.md)

---

## TL;DR

Un **contrat** est une promesse entre deux parties — le caller et le callee, la class et ses utilisateurs, vous et le compiler. Le C++ est le langage où le plus grand nombre de contrats sont **appliqués au compile time**, avant même que votre programme ne s'exécute. Dès lors que vous voyez le C++ comme un empilement de contrats, le langage cesse de paraître arbitraire et révèle toute sa logique.

```
   ┌─────────── CALLER ───────────┐         ┌─────────── CALLEE ───────────┐
   │  I PROMISE to honor your     │ ──────► │  I PROMISE the documented    │
   │  preconditions               │         │  behavior + postconditions   │
   │  I EXPECT  the result        │ ◄────── │  I EXPECT  valid arguments   │
   └──────────────────────────────┘         └──────────────────────────────┘
                       break it  ⇒  UB  /  compile error  /  b≥ug
```

---

## 1. Qu'est-ce qu'un contrat ?

En développement logiciel, un contrat est l'**accord complet** entre deux morceaux de code sur la manière dont ils coopèrent. Il comporte trois volets :

| Volet | Signification | Exemple |
|---|---|---|
| **Préconditions** | Ce qui doit être vrai *avant* d'appeler la fonction | « le pointer n'est pas null » |
| **Postconditions** | Ce qui sera vrai *après* le retour de la fonction | « le vector contient le nouvel élément » |
| **Invariants** | Ce qui reste vrai en permanence | « la taille ne dépasse jamais la capacité » |

Cette idée est plus ancienne que le C++ — Bertrand Meyer l'a formalisée dans les années 80 sous le nom de **Design by Contract**. Le C++ l'a adoptée à travers sa *syntaxe* : au lieu d'écrire des commentaires en langage naturel, vous encodez le contrat directement dans le code, et le compiler se charge de faire respecter les règles.

---

## 2. Pourquoi le C++ s'appuie autant sur les contrats

Le C et le C++ vous permettent tous deux d'exprimer des choses dangereuses. La différence réside ici :

```
   C:   "Here's a footgun. Try not to shoot yourself."
   C++: "Here's a footgun, plus 50 ways to ask the compiler
                 to refuse to let you fire it in the wrong direction."
```

La majeure partie de la syntaxe C++ qui ressemble à de la décoration (`const`, `explicit`, `private`, `virtual`, `noexcept`...) est en réalité un **contrat que vous signez avec le compiler**. En retour, le compiler :

1. Refuse de compiler le code qui enfreint le contrat.
2. Génère un code plus rapide car il peut assumer que le contrat est respecté.
3. Documente l'intention pour le prochain lecteur humain.

Java dispose de vérifications au runtime (`NullPointerException`). Le C++ dispose de refus au compile time et de l'undefined behavior — rapide quand vous avez raison, brutal quand vous avez tort. **Les contrats sont le moyen de rester du bon côté de cette frontière.**

---

## 3. Les contrats qui se cachent derrière la syntaxe

Une vue d'ensemble. Chaque ligne représente un contrat réel que vous signez ou acceptez des dizaines de fois par jour.

| Syntaxe | Le contrat | Qui est protégé |
|---|---|---|
| `const T x` | « Je ne réassignerai pas x. » | Vous dans le futur, le reviewer |
| `const T& param` | « Je ne modifierai pas ce que vous m'avez prêté. » | Le caller |
| `T method() const` | « Appeler ceci ne modifiera pas l'objet. » | Le caller, l'optimiseur |
| `private:` | « Le code extérieur ne doit pas toucher à ceci. » | Les invariants de la class |
| `protected:` | « Les sous-classes le peuvent, les inconnus non. » | La hiérarchie |
| `public:` | « Ceci fait partie de mon interface publique. » | Les utilisateurs |
| `virtual` | « Les sous-classes peuvent override ceci. » | Les sites d'appel polymorphiques |
| `= 0` (pure virtual) | « Les sous-classes **doivent** implémenter ceci. » | L'abstraction |
| `explicit` ctor | « Ne m'appelle pas par conversion implicite. » | Le caller, vous dans le futur |
| `noexcept` | « Je ne lancerai jamais d'exception. » | Le mécanisme de stack-unwind |
| `static_cast<T>` | « J'ai vérifié, cette conversion est sûre. » | Le lecteur |
| `dynamic_cast<T*>` | « Vérifie au runtime ; null en cas d'échec. » | Les utilisateurs du polymorphisme |
| `T*` retourné par `new` | « Le caller en est maintenant propriétaire et doit faire `delete`. » | La heap |
| Reference `T&` | « Liée, non-null, jamais réassignée. » | Quiconque l'utilise |
| Le système de types lui-même | « Si les structures correspondent, les types correspondent. » | Tout le monde |
| **Undefined behavior** | « Le caller promet : pas de déréférencement de null, pas de hors-limites (OOB), pas d'overflow. » | L'optimiseur |

---

## 4. Quatre échelles de contrats

```
                 ┌──────────────────────────┐
                 │   1. FUNCTION contract   │  smallest, per-call
                 │     parameters/return    │
                 └────────────┬─────────────┘
                              │
                 ┌────────────▼─────────────┐
                 │    2. CLASS contract     │  per-object lifetime
                 │   public API + invariants│
                 └────────────┬─────────────┘
                              │
                 ┌────────────▼─────────────┐
                 │  3. HIERARCHY contract   │  base promises kept by all
                 │   virtual + Liskov       │
                 └────────────┬─────────────┘
                              │
                 ┌────────────▼─────────────┐
                 │  4. LANGUAGE contract    │  you ↔ compiler
                 │  type system + UB rules  │
                 └──────────────────────────┘
```

### 4.1 Au niveau de la fonction

```cpp
// Pré :  v est non vide
// Post : retourne le plus petit élément
// Inv :  v est inchangé
int min(const std::vector<int>& v);
```

La simple lecture de la signature vous dit presque tout : `const std::vector<int>&` signifie « Je ne le modifierai pas et je ne le copierai pas. » Seule la précondition « non vide » nécessite un commentaire.

### 4.2 Au niveau de la class

Le contrat d'une class est son **interface publique plus ses invariants**. La class promet que les invariants restent vrais ; les utilisateurs promettent de ne la manipuler qu'à travers l'API publique.

```cpp
class Stack {
    int *data_;
    size_t size_, cap_;
public:
    void  push(int x);     // contrat public
    int   pop();
    size_t size() const;
    // Invariants (contrat privé avec elle-même) :
    //   1. 0 <= size_ <= cap_
    //   2. data_ possède cap_ int (ou est null quand cap_ == 0)
};
```

Si un utilisateur *pouvait* modifier `data_` directement, il pourrait briser l'invariant n°2 et votre destructeur ferait un `delete` sur des données corrompues. `private:` est le verrou ; l'invariant est ce qu'il protège.

### 4.3 Au niveau de la hiérarchie (principe de substitution de Liskov)

Toute fonction qui opère sur un `Base*` doit continuer à fonctionner lorsqu'on lui passe un `Derived*`. Les sous-classes peuvent **renforcer les postconditions** et **affaiblir les préconditions**, mais jamais l'inverse.

```cpp
class Bird           { public: virtual void fly(); };
class Penguin : Bird { public: void fly() override { throw "no!"; } };
                                                   // 💥 contrat rompu
```

Une fonction qui appelle `bird->fly()` avait la promesse que cela fonctionnerait. Penguin rompt l'accord. La solution : reconcevoir la hiérarchie (`Bird` ne devrait pas promettre `fly`).

### 4.4 Vous ↔ le compiler

Le contrat le plus fondamental. Le standard C++ est une liste de choses que vous promettez de ne pas faire (UB) en échange de performances.

```cpp
int *p = nullptr;
*p = 42;          // VOUS avez rompu le contrat
                  // le compiler a le droit de faire n'importe quoi — y compris rien
```

L'UB ne signifie pas « le programme plante ». Cela signifie « le programme a l'autorisation de faire n'importe quoi », parce que le compiler a optimisé en partant du principe que vous ne feriez jamais cela.

---

## 5. L'exemple le plus limpide, ligne par ligne

Quatre mots, quatre contrats :

```cpp
int sum(const std::vector<int>& v);
//   ↑       ↑           ↑     ↑
//   │       │           │     └── caller : "tu me prêtes ce vector"
//   │       │           └──────── callee : "je ne vais pas le COPIER (& = peu coûteux)"
//   │       └──────────────────── callee : "je ne vais pas le MODIFIER (const)"
//   └──────────────────────────── callee : "je vais retourner un int"
```

Chaque promesse est appliquée par le compiler :
- Essayer de modifier `v` dans `sum` → compile error.
- Passer une `std::list<int>` → compile error.
- Oublier de retourner un `int` → compile error.

C'est pourquoi les programmeurs C++ sont obsédés par les signatures. La signature *est* le contrat.

---

## 6. La Forme Canonique Orthodoxe : un contrat de cycle de vie

📚 [`oop/ORTHODOX_CANONICAL_FORM.md`](../oop/ORTHODOX_CANONICAL_FORM.md)

Lorsque vous écrivez les quatre fonctions canoniques, vous signez le contrat de cycle de vie :

```
   ┌──────────────────────────────────────────────────────────┐
   │  1. Default constructor   "I can be born from nothing."  │
   │  2. Copy constructor      "I can be cloned safely."      │
   │  3. Copy assignment       "I can be replaced safely."    │
   │  4. Destructor            "I clean up after myself."     │
   └──────────────────────────────────────────────────────────┘
```

Si vous en omettez une et que le compiler synthétise un comportement par défaut inadapté (shallow copy d'un pointer possédé, par exemple), vous avez fait une promesse que vous ne pouvez pas tenir — et le bug fait surface à des kilomètres de son origine.

---

## 7. Contrats et ownership

📚 [`fundamentals/MEMORY.md`](MEMORY.md)
`new` et `delete` sont un **contrat d'ownership** entre l'appelant et l'appelé :

```cpp
Zombie* spawn();        // returns a heap pointer
                        // contract: caller must delete the result
```

Rien dans le système de types ne force cela en C++98 — c'est exprimé dans le **nom et la documentation**. C'est exactement pourquoi les bugs d'ownership sont fréquents en C et ont été un facteur de motivation majeur pour les smart pointers en C++ moderne. En C++98, on compense par la discipline :

- Documentez qui possède chaque pointer qui traverse une frontière de fonction.
- Préférez les **valeurs et les references** aux pointers lorsque l'ownership n'est pas transféré.
- Associez chaque `new` à un destructeur clair qui s'exécute (RAII).

---

## 8. Contrats et exceptions

📚 [`io-errors/ERROR_MANAGEMENT.md`](../io-errors/ERROR_MANAGEMENT.md)

Les exceptions existent pour les **contrats rompus que le code local ne peut pas réparer**.

```cpp
int Stack::pop() {
    if (size_ == 0)
        throw std::out_of_range("pop on empty stack");
    return data_[--size_];
}
```

La précondition de la fonction était "la stack n'est pas vide". L'appelant l'a rompue. Lancer une exception est la façon bruyante de dire "ta part du contrat a échoué ; je refuse de produire silencieusement des données corrompues".

Les quatre **garanties de sécurité d'exception** sont elles-mêmes des contrats offerts par la fonction :

| Niveau | Contrat |
|---|---|
| **No-throw** | "Je ne lancerai jamais d'exception." |
| **Strong** | "Soit je réussis, soit je laisse le monde intact." |
| **Basic** | "Je ne provoquerai ni leak ni corruption — mais l'état peut différer." |
| **None** | "Je ne fais aucune promesse. Évitez-moi." |

---

## 9. Comment *lire* le C++ sous forme de contrats

Lorsqu'une ligne semble confuse, analysez-la comme un contrat. Trois victoires rapides :

```cpp
void log(const std::string& msg) const noexcept;
//        ──────────────────────  ─────  ────────
//        won't modify msg        won't  won't throw
//                                modify
//                                *this
```

Analysez de l'extérieur vers l'intérieur : chaque qualificateur est une promesse. La signature seule vous indique ce qu'il est sûr de faire, ce qu'il est sûr de supposer et sur quelles garanties vous pouvez compter sur le site d'appel.

---

## 10. Comment *écrire* le C++ sous forme de contrats

Trois habitudes qui portent leurs fruits :

1. **`const` par défaut.** Ne le retirez que lorsque vous avez réellement besoin de muter. La plupart des paramètres, la plupart des méthodes, la plupart des variables → `const`.
2. **Rendez l'API publique minimale et les invariants privés explicites.** Commentez l'invariant une seule fois au début de la class. Chaque méthode le préserve ou le restaure.
3. **Faites confiance aux signatures, validez les frontières.** À l'intérieur de votre base de code, ayez confiance dans le fait que les appelants respectent le contrat. Aux extrémités (entrées utilisateur, E/S de fichiers, réseau) — validez, puis faites confiance.

```
   ┌────────────────────── BOUNDARY ──────────────────────┐
   │ validate here  │ trust everywhere   │ validate here  │
   │ (user, file)   │ inside             │ (network out)  │
   └──────────────────────────────────────────────────────┘
```

À l'intérieur de la zone de confiance, les contrats remplacent le code défensif. C'est ainsi que le code C++ reste à la fois rapide et correct.

---

## 11. Bugs de contrat courants (et comment les repérer)

| Code smell | Le contrat rompu | Le correctif |
|---|---|---|
| `delete` appelé deux fois | Deux owners → chacun pense en être responsable | Un seul owner ; le reste détient des non-owning pointers |
| Use-after-free | Le lecteur a supposé lifetime ≥ usage | Liez la lifetime à un scope (RAII) |
| `const` supprimé via un cast | Promesse faite puis révoquée | Ne mentez pas — rendez-le non-const s'il doit muter |
| Slicing | Le contrat de base a été respecté, mais les parties dérivées ont disparu | Passez par reference/pointer, pas par valeur |
| `override` manquant | La sous-classe *pensait* surcharger mais ne l'a pas fait | Utilisez `override` (C++11) ou vérifiez soigneusement les signatures en C++98 |
| Exception non gérée à travers l'ABI | Contrat `noexcept` violé | Enveloppez la frontière dans un `try/catch` |

---

## 12. Résumé visuel

```
                  ┌────────────────────────────────┐
                  │      Every C++ symbol is       │
                  │       a contract somewhere     │
                  └────────────────┬───────────────┘
                                   │
        ┌──────────────────────────┼──────────────────────────┐
        │                          │                          │
   ┌────▼─────┐              ┌─────▼─────┐              ┌─────▼─────┐
   │  const   │              │  private  │              │  virtual  │
   │ "I won't │              │  "Hands   │              │ "Override │
   │ change"  │              │   off."   │              │  if you   │
   └──────────┘              └───────────┘              │  wish."   │
                                                        └───────────┘
        │                          │                          │
   ┌────▼─────┐              ┌─────▼─────┐              ┌─────▼─────┐
   │ explicit │              │  ~Class() │              │   = 0     │
   │ "Don't   │              │ "I'll     │              │ "Override │
   │ convert  │              │  clean up │              │  you must"│
   │ secretly"│              │  myself." │              └───────────┘
   └──────────┘              └───────────┘

         All of them: enforced at compile time → no runtime cost.
```

---

## 13. Questions d'entraînement

1. Quel contrat établit `int& at(size_t i) const;` ? À quoi le `const` s'applique-t-il — la reference, le paramètre, ou la méthode ?
2. Une fonction renvoie `std::string&`. Quel invariant l'appelant doit-il supposer ? Où cela pourrait-il mal tourner ?
3. Vous voyez `void f(T* p);` sans aucun commentaire. Quels trois différents contrats d'ownership pourraient être en jeu, et comment décideriez-vous lequel s'applique ?
4. Pourquoi le comportement indéfini (undefined behavior) existe-t-il au lieu que le standard n'impose "ceci devrait crasher" ?
5. Une class a 12 setters et 12 getters et aucun invariant — quel contrat manque-t-il, et qu'est-ce que cela signifie pour la class ?

> Répondez-y dans votre tête, pas sur papier. Si trois réponses sur cinq vous semblent floues, relisez les sections 3 à 5 — c'est là que réside l'essentiel.