# Getters et Setters en C++ — Guide complet

> **Zoom sur les mots-clés :** [`PUBLIC/PRIVATE/PROTECTED`](../../lexique/PUBLIC_PRIVATE_PROTECTED.md) · [`CONST`](../../lexique/CONST.md) · [`THIS`](../../lexique/THIS.md) · [`MUTABLE`](../../lexique/MUTABLE.md) · [`EXPLICIT`](../../lexique/EXPLICIT.md)

## Table des matières
1. [Que sont les getters et les setters ?](#What%20are%20getters%20and%20setters%3F)
2. [Le modèle mental : encapsulation et invariants de classe](#The%20mental%20model%3A%20encapsulation%20and%20class%20invariants)
3. [Exemple minimal](#Minimal%20example)
4. [Anatomie d'un bon getter](#Anatomy%20of%20a%20good%20getter)
   - [Retour par valeur, reference ou const reference ?](#Return%20by%20value%2C%20reference%2C%20or%20const%20reference%3F)
   - [Le `const` final](#The%20trailing%20const)
   - [Getters calculés (dérivés)](#Computed%20%28derived%29%20getters)
5. [Anatomie d'un bon setter](#Anatomy%20of%20a%20good%20setter)
   - [Passer les paramètres par const reference](#Pass%20parameters%20by%20const%20reference)
   - [Stratégies de validation](#Validation%20strategies)
6. [L'histoire de `const` (analyse approfondie)](#The%20const%20story%20%28deep%20dive%29)
7. [Quand NE PAS écrire de getter/setter](#When%20NOT%20to%20write%20a%20getter%2Fsetter)
8. [Pièges courants](#Common%20pitfalls)
9. [Conventions de nommage](#Naming%20conventions)
10. [Exemple concret — `Contact` de PhoneBook](#Worked%20example%20%E2%80%94%20PhoneBook%20Contact)
11. [Arbre de décision / antisèche](#Decision%20tree%20%2F%20cheat%20sheet)

---

## Que sont les getters et les setters ?

Un **getter** et un **setter** sont des member functions publiques dont le seul rôle est
de lire ou d'écrire un data member private. Ce sont les *portes d'accès contrôlées* entre
le monde extérieur et le fonctionnement interne d'une class.

```
Outside world  ──► setter()  ──► private member
Outside world  ◄── getter()  ◄── private member
```

En C, vous lisez et écrivez directement dans les champs d'une struct. En C++ idiomatique, les champs
sont `private` et accessibles uniquement par le biais de méthodes. Les getters et setters sont le cas
le plus simple de ce pattern.

> **En résumé :** la class est propriétaire de ses données ; les getters et setters sont la manière dont
> elle choisit d'exposer ces données aux appelants.

---

## Le modèle mental : encapsulation et invariants de classe

L'encapsulation ne consiste pas simplement à « rendre le champ private ». Sa véritable
raison d'être réside dans les **invariants de classe** — des propriétés de l'objet qui doivent être
vérifiées à tout moment pour que l'objet soit valide.

Exemples :
- `Date` — le mois doit être entre `1..12`, le jour doit être valide pour ce mois.
- `Account` — le solde doit être égal à la somme des dépôts passés moins les retraits.
- `Contact` (PhoneBook) — le numéro de téléphone ne doit contenir que des chiffres.

Si le champ est `public`, *n'importe quel* code, n'importe où, peut briser l'invariant. La
class ne peut plus rien garantir quant à son propre état, car elle n'a aucun
contrôle sur ce qui est écrit, ni par qui.

Si le champ est `private` et modifié uniquement via un setter, le setter
**applique** l'invariant à chaque écriture. La class devient alors *auto-défendue*.

> **Pourquoi un « getter/setter » et non un « public field » ?** Parce qu'un `public field`
> signifie « aucun invariant possible ». Le coût d'une paire getter/setter est un seul
> niveau d'indirection ; le bénéfice est que la class contrôle sa propre
> validité. Pour des champs non triviaux, le compromis en vaut presque toujours la peine.

---

## Exemple minimal

```cpp
class Contact {
private:
    std::string m_name;          // private — le code extérieur ne peut pas toucher à ceci

public:
    void               setName(const std::string& name) { m_name = name; }
    const std::string& getName() const { return m_name; }
};
```

```cpp
Contact c;
c.setName("Alice");                     // passe par le setter
std::cout << c.getName() << std::endl;  // passe par le getter
// c.m_name = "Bob";                    // ERREUR — private, ne compilera pas
```

Trois détails minuscules dans cet exemple effectuent un travail essentiel — tous expliqués dans
les deux sections suivantes :

1. Le setter prend son paramètre en tant que `const std::string&` (const reference).
2. Le getter retourne `const std::string&` (const reference).
3. Le getter est marqué `const` après la liste de paramètres.

---

## Anatomie d'un bon getter

### Retour par valeur, reference ou const reference ?

C'est la décision la plus importante lors de l'écriture d'un getter. La
« bonne » réponse dépend du type du member.

```cpp
//  A) Retour PAR VALEUR — l'appelant reçoit une copie
std::string getName() const { return m_name; }

//  B) Retour PAR CONST REFERENCE — l'appelant reçoit un handle en lecture seule, pas de copie
const std::string& getName() const { return m_name; }

//  C) Retour PAR NON-CONST REFERENCE — l'appelant peut modifier le member directement
std::string& getName() { return m_name; }   // (presque toujours une mauvaise idée)
```

| Forme | Quand l'utiliser | Coût |
|------|-------------|------|
| `T` (par valeur) | Types primitifs (`int`, `bool`, `char`, ...), ou lorsque la valeur est **calculée** et ne survit pas à l'appel. | Peu coûteux pour les types primitifs, une copie pour les types lourds. |
| `const T&` | Types lourds (`std::string`, conteneurs, classes personnalisées) où le member existe déjà dans l'objet. | Zéro copie. La reference est valide tant que l'objet l'est. |
| `T&` | **Presque jamais.** Donne à l'appelant une prise pour muter votre member private sans aucune validation. Détruit l'encapsulation. Acceptable uniquement si la class expose délibérément des éléments internes modifiables (par ex., `std::vector::operator[]`). |

> **Règle empirique :** pour `int`/`bool`/`char` → retour par valeur.
> Pour `std::string`/conteneurs → retour par `const T&`.
> Ne retournez par non-const reference que si vous souhaitez réellement signifier « ce champ est
> public ; je l'écris simplement `obj.foo()` au lieu de `obj.foo` ».

### Le `const` final

```cpp
const std::string& getName() const { return m_name; }
//                            ^^^^^
//   "cette méthode ne modifie pas *this"
```

Ce `const` sur la méthode produit deux effets :

1. Le compiler **impose** que la méthode ne modifie aucun
   champ non-`mutable`. Si vous écrivez par accident `m_name = "x";` dans le corps, la compilation
   échoue. C'est un contrat vérifié à la compilation.
2. Il rend la méthode **appelable sur des objets `const`** :

   ```cpp
   void print(const Contact& c) {
       std::cout << c.getName();   // fonctionne UNIQUEMENT si getName() est const
   }
   ```

   Si `getName()` n'était pas marquée `const`, l'appel échouerait car
   `c` est `const Contact&` — un objet `const` ne peut appeler que des méthodes `const`.

**Marquez toujours les getters avec `const`.** Un getter qui ne l'est pas soit ment sur le fait
de ne pas modifier l'état, soit s'exclut silencieusement d'une utilisation sur des
objets `const` (ce qui signifie que les const references vers votre class deviennent inutilisables).

### Getters calculés (dérivés)

Un getter n'a pas nécessairement besoin de retourner un champ stocké — il peut calculer la valeur :

```cpp
class Person {
private:
    std::string m_firstName;
    std::string m_lastName;

public:
    std::string getFullName() const {           // ← PAR VALEUR, pas par reference
        return m_firstName + " " + m_lastName;
    }
};
```

> **Piège :** ne retournez *jamais* une `const T&` vers une valeur calculée à l'intérieur de la
> fonction. La variable temporaire qui stocke la concaténation est détruite à la
> fin de la fonction, et l'appelant recevrait une reference vers de la mémoire corrompue :
>
> ```cpp
> const std::string& getFullName() const {                 // ⚠ DANGLING
>     return m_firstName + " " + m_lastName;               //   l'objet temporaire est détruit ici
> }
> ```
>
> Les valeurs calculées doivent être retournées **par valeur**. Le compiler élimine
> généralement la copie (return value optimisation), donc le coût est souvent nul.

---

## Anatomie d'un bon setter

### Passer les paramètres par const reference

```cpp
void setName(const std::string& name) { m_name = name; }
//           ^^^^^^^^^^^^^^^^^^^
```

Pour tout type non trivial, prenez le paramètre par `const T&` :
- `const` — le setter promet de ne pas modifier l'argument de l'appelant.
- `&` — l'argument n'est pas copié à l'appel ; la fonction opère directement sur
  l'objet de l'appelant.

Pour les types primitifs (`int`, `bool`, `char`, pointers), passez par valeur — des references
vers une valeur de la taille d'un registre ne sont pas plus rapides que la valeur elle-même.

### Stratégies de validation

C'est ici que l'approche getter/setter justifie pleinement son intérêt. Un setter est le
point de passage obligatoire par lequel transitent toutes les écritures — c'est l'endroit naturel pour
faire respecter l'invariant de classe.

Il existe quatre manières courantes de gérer une entrée incorrecte :

```cpp
// 1. REJET SILENCIEUX — l'appelant n'a aucune idée que l'appel a été sans effet.
//    Facile mais source d'erreurs ; utile pour les mises à jour en mode « best-effort ».
void setAge(int age) {
    if (age < 0 || age > 150) return;
    m_age = age;
}

// 2. RETOURNER UN BOOL — l'appelant choisit comment réagir.
//    Courant en C++98 lorsque l'on souhaite éviter les exceptions.
bool setAge(int age) {
    if (age < 0 || age > 150) return false;
    m_age = age;
    return true;
}

// 3. LEVER UNE EXCEPTION EN CAS D'ENTRÉE INCORRECTE — l'appelant doit la gérer ou la propager.
//    Préférable quand l'erreur est réellement exceptionnelle ; conforme au style de la std-lib.
void setAge(int age) {
    if (age < 0 || age > 150)
        throw std::invalid_argument("Contact: age out of range");
    m_age = age;
}

// 4. CONTRAT DE PRÉCONDITION — aucun contrôle à l'exécution, la doc spécifie « must be in [0,150] ».
//    Le plus économique ; adapté lorsque l'appelant est exclusivement du code de confiance.
void setAge(int age) {
    m_age = age;   // UB si la précondition est violée
}
```
Il n'y a pas de réponse unique parfaite — choisissez-en une et **soyez cohérent à travers toute la class**. Pour les projets de piscine 42, les options (1) et (3) sont toutes deux courantes : rejet silencieux pour des cas comme une boucle de saisie UI, throw pour les erreurs du programmeur.

> **Pattern utile :** lorsque plusieurs setters partagent une logique de validation, factorisez la vérification dans un helper privé (`bool isValidName(const std::string&) const`) et appelez-le à la fois depuis le setter et depuis tout constructor acceptant le même champ.

---

## Le cas de `const` (analyse approfondie)

`const` est omniprésent dans le code C++ de getters/setters. Cela mérite qu'on s'y attarde.

```cpp
class Foo {
public:
    int                f1() const;            // la méthode est const
    void               f2(const int& x);      // le paramètre est une const reference
    const std::string& f3() const;            // le type de retour est une const ref, la méthode est const
private:
    int m_x;
};
```

Trois utilisations indépendantes de `const`, chacune sur un « emplacement » différent :

| Emplacement | Signification |
|-------------|---------------|
| Après la liste de paramètres (`f1() const`) | La méthode ne modifie pas `*this`. |
| Avant le type d'un paramètre (`const int& x`) | La méthode ne modifie pas l'argument de l'appelant. |
| Avant le type de retour (`const std::string&`) | L'appelant ne peut pas modifier ce vers quoi pointe la reference de retour. |

### `const` overloading

Vous pouvez avoir **deux getters avec le même nom**, l'un `const` et l'autre non :

```cpp
class Buffer {
public:
          int& operator[](size_t i)       { return m_data[i]; }
    const int& operator[](size_t i) const { return m_data[i]; }
private:
    int m_data[100];
};
```

Le compiler choisit le bon overload selon que l'objet est `const` ou non. Sur un `Buffer` non-const, vous obtenez la reference mutable ; sur un `Buffer` `const`, vous obtenez celle en lecture seule. C'est exactement ainsi que fonctionne `std::vector::operator[]`.

### `const` logique vs binaire (bitwise)

Le `const` sur une méthode est « logique » — il est *vérifié* par le compiler au niveau binaire (aucun champ ne peut être assigné), mais vous pouvez vous en affranchir avec [`mutable`](../../lexique/MUTABLE.md) pour les champs tels que les caches ou les mutexes qui ne représentent pas l'état observable de l'objet.

```cpp
class Cache {
public:
    int compute() const {
        if (!m_done) { m_value = expensive(); m_done = true; }
        return m_value;
    }
private:
    mutable int  m_value;   // peut être modifié dans les méthodes const
    mutable bool m_done;
};
```

> **Mise en garde sur les membres pointer :** un `const` sur une méthode protège `m_ptr` lui-même d'une réassignation, mais **pas** ce vers quoi `m_ptr` pointe. Un getter `const` peut toujours exécuter `*m_ptr = 42;`. Pour empêcher cela, le pointer doit être un `const T*` ou sa cible doit être const.

---

## Quand NE PAS écrire de getter/setter

C'est la partie que la plupart des guides pour débutants omettent, et c'est la plus importante.

Si chaque membre d'une class possède une paire `getX`/`setX`, la class est **anémique** : c'est un simple sac de données sans comportement, et la logique qui *devrait* vivre dans la class a fuité chez ses appelants. Chaque fois que vous voyez ceci :

```cpp
account.setBalance(account.getBalance() - 50);    // smell
```

…la class ne fait pas son travail. Le comportement a sa place **à l'intérieur** de la class :

```cpp
account.withdraw(50);                              // la méthode fait le travail
```

Le principe est **« tell, don't ask »** (ordonnez, ne demandez pas) — au lieu de demander à l'objet ses données, de calculer quelque chose et de le réécrire, *dites* à l'objet ce que vous voulez qu'il fasse.

Une class saine possède généralement :
- Une poignée de getters pour les champs que le monde extérieur a réellement besoin de lire.
- **Peu ou pas** de setters bruts ; à la place, des méthodes nommées d'après des opérations (`deposit`, `rename`, `validate`, `addContact`) qui font muter l'état de manière significative pour le domaine métier.
- Des setters uniquement pour les champs qui sont véritablement de « simples données » sans invariants au niveau de la class les liant à d'autres.

> **Règle empirique avant d'écrire `setX` :** demandez-vous « existe-t-il une opération de plus haut niveau ici ? ». Si oui, écrivez plutôt cette opération. Si ce n'est vraiment pas le cas, le setter convient.

---

## Pièges courants

| # | Piège | Symptôme | Solution |
|---|-------|----------|----------|
| 1 | Retourner une `const T&` vers une variable temporaire calculée dans la fonction | Utilisation de mémoire corrompue ; fonctionne parfois, plante d'autres fois | Retourner par valeur pour les résultats calculés |
| 2 | Retourner une `T&` (non-const) vers un membre privé | L'appelant contourne la validation ; l'encapsulation est perdue | Retourner une `const T&` à la place |
| 3 | Oublier `const` sur la méthode | L'appel échoue sur les appelants `const Contact&` ; avertissement sur « this is const » | Ajouter `const` après la liste de paramètres |
| 4 | Passer `std::string` par valeur dans un setter | Une copie inutile par appel | Utiliser `const std::string&` |
| 5 | Valider dans les setters mais pas dans le constructor | La class peut être construite dans un état invalide | Appliquer la même vérification dans les deux, la factoriser dans un helper privé |
| 6 | Le setter rejette silencieusement une mauvaise entrée, l'appelant suppose que cela a fonctionné | Données périmées, bugs déroutants | Retourner un `bool`, throw, ou assert |
| 7 | Champ public « pour les performances » | L'invariant est brisé dès que quelqu'un d'autre écrit dedans | Profiler d'abord ; en pratique le coût d'un getter `const T&` est nul |
| 8 | Écrire `setX/getX` pour *chaque* membre | La class devient un sac de données sans comportement | « Tell, don't ask » — modélisez des opérations, pas des champs |

---

## Conventions de nommage

| Style       | Getter        | Setter        | Notes |
|-------------|---------------|---------------|-------|
| Classique   | `getName()`   | `setName()`   | Le plus universel ; tradition C++/Java |
| Style Qt    | `name()`      | `setName()`   | Préféré en C++ moderne où le nom *est* la lecture |
| Style 42    | `F_GetName()` | `F_SetName()` | Reliquat des sujets ; vous le verrez dans la piscine |

Pour les projets 42, l'un ou l'autre convient. Au sein d'un même projet, choisissez-en un et **tenez-vous-y** — mélanger les styles est la pire option.

Nommage des membres (champs privés) :

```cpp
std::string m_name;     // préfixe m_ — courant à 42 / C++ plus ancien
std::string _name;      // underscore au début — également courant (mais réservé au scope global/fichier)
std::string name_;      // underscore à la fin — style Google
std::string name;       // aucun préfixe — repose sur `this->name` pour lever l'ambiguïté
```

L'intérêt du préfixe est de distinguer le champ d'un paramètre ou d'une variable locale du même nom (`setName(const std::string& name) { m_name = name; }`).

---

## Exemple concret — `Contact` de PhoneBook

Un `Contact` pour le PhoneBook du module 00, illustrant tous les patterns ci-dessus en contexte.

### `Contact.hpp`

```cpp
#ifndef CONTACT_HPP
# define CONTACT_HPP

# include <string>

class Contact {
private:
    std::string m_firstName;
    std::string m_lastName;
    std::string m_nickname;
    std::string m_phone;
    std::string m_secret;

    bool isPrintable(const std::string& s) const;

public:
    Contact();
    Contact(const std::string& first, const std::string& last,
            const std::string& nick,  const std::string& phone,
            const std::string& secret);
    Contact(const Contact& other);
    Contact& operator=(const Contact& other);
    ~Contact();

    // --- getters: const refs pour std::string, toutes les méthodes sont const ---
    const std::string& getFirstName() const;
    const std::string& getLastName()  const;
    const std::string& getNickname()  const;
    const std::string& getPhone()     const;
    const std::string& getSecret()    const;

    // --- setters: paramètres const-ref, validation qui throw sur entrée invalide ---
    void setFirstName(const std::string& name);
    void setLastName (const std::string& name);
    void setNickname (const std::string& nick);
    void setPhone    (const std::string& phone);
    void setSecret   (const std::string& secret);

    // --- une vraie opération, pas un setter ---
    bool isEmpty() const;
};

#endif
```

### `Contact.cpp` (extraits)

```cpp
#include "Contact.hpp"
#include <stdexcept>
#include <cctype>

bool Contact::isPrintable(const std::string& s) const {
    for (size_t i = 0; i < s.size(); ++i)
        if (!std::isprint(static_cast<unsigned char>(s[i]))) return false;
    return !s.empty();
}

const std::string& Contact::getFirstName() const { return m_firstName; }
const std::string& Contact::getPhone()     const { return m_phone; }
// ... autres getters

void Contact::setFirstName(const std::string& name) {
    if (!isPrintable(name))
        throw std::invalid_argument("Contact: first name not printable");
    m_firstName = name;
}

void Contact::setPhone(const std::string& phone) {
    for (size_t i = 0; i < phone.size(); ++i)
        if (!std::isdigit(static_cast<unsigned char>(phone[i])))
            throw std::invalid_argument("Contact: phone has non-digit");
    m_phone = phone;
}

bool Contact::isEmpty() const {
    return m_firstName.empty() && m_lastName.empty()
        && m_nickname.empty()  && m_phone.empty()
        && m_secret.empty();
}
```

Points à remarquer :
- Chaque getter est `const`, retourne une `const std::string&`. Zéro copie.
- Chaque setter prend une `const std::string&`, valide, et throw sur une entrée invalide.
- `isPrintable` est `private` et `const` — un helper utilisé par les setters.
- `isEmpty()` n'est **pas** un getter — c'est une question au niveau de la class, calculée à partir de multiples champs. C'est le genre de méthode auquel mène le principe « tell, don't ask ».
---

## Arbre de décision / aide-mémoire

```
Le code extérieur a-t-il besoin de LIRE ce champ ?
├── Non → pas de getter
└── Oui → ajouter un getter
         ├── Le champ est primitif (int, bool, char, ...) ?
         │   └── retour par valeur :     T    getX() const;
         ├── Le champ est lourd (string, container, class) ?
         │   └── retour par const ref :  const T& getX() const;
         └── La valeur est CALCULÉE (non stockée) ?
             └── retour par valeur :     T    getX() const;       (JAMAIS const T&)

Le code extérieur a-t-il besoin d'ÉCRIRE ce champ ?
├── Non → pas de setter (le champ est initialisé dans le constructor / pas du tout)
└── Oui → existe-t-il une opération de plus haut niveau qui devrait prendre en charge cette écriture ?
         ├── Oui → écrire CETTE opération, pas un setter brut
         └── Non → ajouter un setter
                  ├── Param : const T& si lourd, T si primitif
                  ├── Valider la nouvelle valeur (ou documenter la précondition)
                  └── Choisir une stratégie d'erreur (silent / bool / throw) et
                      utiliser la même dans toute la class
```

> **Résumé en une ligne :** chaque getter est `const`. Les types lourds sont retournés
> par `const T&`. Les valeurs calculées sont retournées par valeur. Les setters valident.
> Si vous vous retrouvez à écrire `setX/getX` pour chaque champ, demandez-vous si
> la class n'a pas plutôt besoin d'*opérations*.