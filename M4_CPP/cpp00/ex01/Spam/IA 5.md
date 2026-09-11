# Rapport de reprise — C++ 42

## Contexte

- **Étudiant** : 
- **Cadre** : École 42, cursus C++
- **Objectif global** : terminer tous les modules **cpp00 à cpp04** et leurs exercices dans environ un mois et demi, tout en comprenant et en pouvant expliquer le code en peer evaluation.
- **Exercice actuel** : `cpp00/ex01` — **My Awesome PhoneBook**.

## Contraintes de cpp00/ex01

- Compiler en **C++98** avec :

```bash
c++ -Wall -Wextra -Werror -std=c++98
```

- Créer deux classes : `Contact` et `PhoneBook`.
- `PhoneBook` contient un tableau fixe de 8 contacts :

```cpp
Contact _contacts[8];
```

- Aucune allocation dynamique (`new` / `delete`) dans cet exercice.
- Commandes autorisées : `ADD`, `SEARCH`, `EXIT`.
- Un contact possède cinq champs : prénom, nom, surnom, numéro de téléphone et secret le plus sombre.
- Aucun champ enregistré ne doit être vide.
- `SEARCH` doit afficher index, prénom, nom et surnom dans quatre colonnes de largeur 10, alignées à droite ; les textes trop longs sont tronqués et leur dernier caractère affichable devient `.`.
- Organisation demandée : `Contact.hpp`, `Contact.cpp`, `PhoneBook.hpp`, `PhoneBook.cpp`, `main.cpp`, `Makefile`.

---

## Méthode d’apprentissage

L’étudiant préfère :

- Des chapitres courts.
- Une seule notion à la fois.
- Des explications simples.
- De petits exemples directement liés au projet.
- Des notes faciles à recopier dans un cahier.
- Une progression affichée régulièrement.

L’étudiant ne veut pas :

- Une solution complète donnée d’un coup.
- De longues explications sur plusieurs notions à la fois.
- Des exercices annexes non indispensables, car le temps est limité.

### Règle essentielle pour la future IA

Avancer **micro-étape par micro-étape** : proposer une seule modification ou une seule notion, attendre les questions et la compilation avant de continuer.

---

## État actuel

Le code compile après la dernière correction du constructeur avec paramètres dans `Contact.cpp`.

### `Contact.hpp`

```cpp
#ifndef CONTACT_HPP
#define CONTACT_HPP

#include <iostream>
#include <string>

class Contact {
private:
    std::string _firstname;
    std::string _lastname;
    std::string _nickname;
    std::string _phonenumber;
    std::string _darkestsecret;

public:
    // Constructeur par défaut
    Contact();

    // Constructeur avec 5 paramètres
    Contact(
        const std::string& firstname,
        const std::string& lastname,
        const std::string& nickname,
        const std::string& phonenumber,
        const std::string& darkestsecret
    );

    // Setters
    void setFirstName(const std::string& firstname);
    void setLastName(const std::string& lastname);
    void setNickName(const std::string& nickname);
    void setPhoneNumber(const std::string& phonenumber);
    void setDarkestSecret(const std::string& darkestsecret);

    // Getters
    std::string getFirstName() const;
    std::string getLastName() const;
    std::string getNickName() const;
    std::string getPhoneNumber() const;
    std::string getDarkestSecret() const;
};

#endif
```

### `Contact.cpp`

```cpp
#include "Contact.hpp"

// Constructeur par défaut
Contact::Contact()
{
    _firstname = "";
    _lastname = "";
    _nickname = "";
    _phonenumber = "";
    _darkestsecret = "";
}

// Constructeur avec 5 paramètres
Contact::Contact(
    const std::string& firstname,
    const std::string& lastname,
    const std::string& nickname,
    const std::string& phonenumber,
    const std::string& darkestsecret
)
: _firstname(firstname),
  _lastname(lastname),
  _nickname(nickname),
  _phonenumber(phonenumber),
  _darkestsecret(darkestsecret)
{
}

// Setters
void Contact::setFirstName(const std::string& firstname)
{
    _firstname = firstname;
}

void Contact::setLastName(const std::string& lastname)
{
    _lastname = lastname;
}

void Contact::setNickName(const std::string& nickname)
{
    _nickname = nickname;
}

void Contact::setPhoneNumber(const std::string& phonenumber)
{
    _phonenumber = phonenumber;
}

void Contact::setDarkestSecret(const std::string& darkestsecret)
{
    _darkestsecret = darkestsecret;
}

// Getters
std::string Contact::getFirstName() const
{
    return _firstname;
}

std::string Contact::getLastName() const
{
    return _lastname;
}

std::string Contact::getNickName() const
{
    return _nickname;
}

std::string Contact::getPhoneNumber() const
{
    return _phonenumber;
}

std::string Contact::getDarkestSecret() const
{
    return _darkestsecret;
}
```

### `PhoneBook.hpp` (squelette)

```cpp
#ifndef PHONEBOOK_HPP
#define PHONEBOOK_HPP

#include "Contact.hpp"

class PhoneBook
{
private:
    Contact _contacts[8];
    int _count;

public:
    PhoneBook();
    void addContact();
    void searchContact();
};

#endif
```

### `PhoneBook.cpp` (squelette)

```cpp
#include "PhoneBook.hpp"

PhoneBook::PhoneBook()
{
    _count = 0;
}
```

### `main.cpp` actuel

Le `main` teste encore un contact créé avec le constructeur à cinq paramètres :

```cpp
#include <iostream>
#include "Contact.hpp"

int main()
{
    Contact c1("aaA", "AAA", "AAA", "AAA", "AAA");
    std::cout << c1.getFirstName() << std::endl;
    std::cout << c1.getLastName() << std::endl;
    std::cout << c1.getNickName() << std::endl;
    std::cout << c1.getPhoneNumber() << std::endl;
    std::cout << c1.getDarkestSecret() << std::endl;
    return 0;
}
```

---

## Notes de cours déjà vues

### Classe, objet et état

- Une **classe** est un modèle : `Contact`, `PhoneBook`.
- Un **objet** est une instance de la classe : `Contact c1;`.
- Un objet possède son propre état : ses propres valeurs d’attributs.
- Deux objets `Contact` ont la même structure, mais peuvent contenir des valeurs différentes.

```text
Classe
│
├── attributs
└── méthodes
↓
Objet
│
└── état propre
```

### Relation entre `Contact` et `PhoneBook`

```text
PhoneBook
│
├── possède _contacts[8]
├── possède _count
├── ajoute des contacts
└── recherche des contacts

Contact
│
├── représente une personne
├── possède cinq informations
└── fournit getters et setters
```

`PhoneBook` **possède** des `Contact`. Ce n’est pas de l’héritage.

### `.hpp` et `.cpp`

- Le fichier `.hpp` déclare la classe : attributs et prototypes de méthodes.
- Le fichier `.cpp` implémente les méthodes : leur comportement réel.
- Le header dit **quoi existe** ; le fichier source dit **comment cela fonctionne**.

### Constructeur

Un constructeur :

- a le même nom que la classe ;
- n’a pas de type de retour ;
- est appelé automatiquement quand un objet est créé ;
- initialise l’état de l’objet.

### Les deux constructeurs de `Contact`

```cpp
Contact();
```

- Constructeur par défaut, sans paramètre.
- Utilisé par :

```cpp
Contact c;
Contact tab[8];
```

- Il est nécessaire à cause de :

```cpp
Contact _contacts[8];
```

Le compilateur doit créer les huit objets sans recevoir leurs informations.

```cpp
Contact(
    const std::string& firstname,
    const std::string& lastname,
    const std::string& nickname,
    const std::string& phonenumber,
    const std::string& darkestsecret
);
```

- Constructeur avec cinq paramètres.
- Utilisé par :

```cpp
Contact c1("Alice", "Dupont", "Ali", "0601020304", "secret");
```

- Il crée un contact directement rempli.

### Liste d’initialisation

Dans le constructeur avec paramètres :

```cpp
: _firstname(firstname),
  _lastname(lastname)
```

Le `:` commence une liste d’initialisation. Elle initialise les attributs avant d’entrer dans le corps `{ }` du constructeur.

Erreur rencontrée et corrigée : écrire `_firstname(firstname)` dans le corps `{ }` est interprété comme un appel de fonction sur une chaîne, ce qui provoque une erreur de compilation. La liste doit être placée après `)` et avant `{`.

---

## Éléments compris et points à revoir

### Déjà compris globalement

- Classe vs objet.
- Attributs privés et méthodes publiques.
- Getters et setters.
- Séparation `.hpp` / `.cpp`.
- Constructeur par défaut nécessaire pour le tableau de huit contacts.
- Syntaxe de base de la liste d’initialisation.

### Point encore légèrement flou

La différence pratique entre le constructeur par défaut et le constructeur avec cinq paramètres n’est pas encore totalement intuitive. Ne pas bloquer dessus : elle deviendra plus concrète lorsque `PhoneBook` créera automatiquement son tableau, puis que `ADD` remplira un contact via les setters.

---

## Prochaine étape immédiate

Ne faire qu’une seule étape : modifier `main.cpp` afin de tester la construction de `PhoneBook` et donc la création automatique de huit `Contact`.

```cpp
#include "PhoneBook.hpp"

int main()
{
    PhoneBook phoneBook;
    return 0;
}
```

Puis lancer :

```bash
make
```

Si une erreur apparaît, la recopier intégralement avant toute nouvelle modification.

Après cette compilation, seulement ensuite : expliquer le constructeur `PhoneBook::PhoneBook()` et l’initialisation de `_count = 0`.

---

## Roadmap cpp00/ex01

1. Vérifier que `PhoneBook phoneBook;` compile.
2. Comprendre `PhoneBook::PhoneBook()` et `_count = 0`.
3. Écrire la boucle principale du programme : lecture des commandes `ADD`, `SEARCH`, `EXIT`.
4. Implémenter `ADD` progressivement : `std::getline`, cinq champs, validation des champs vides, setters.
5. Gérer le compteur et le remplacement circulaire après huit contacts.
6. Implémenter `SEARCH` : liste en quatre colonnes, `std::setw`, troncature, demande d’index, affichage détaillé.
7. Finaliser le Makefile, les tests et la préparation peer evaluation.

---

## Règles techniques à conserver

- C++98 uniquement.
- Pas de `new`, `delete`, `std::vector`, ni fonctionnalités C++11+ dans cpp00/ex01.
- Chaque méthode déclarée dans le header doit être implémentée avec le préfixe de classe dans le `.cpp` :

```cpp
std::string Contact::getFirstName() const
```

- Les déclarations dans le `.hpp` finissent par `;`.
- Le Makefile doit compiler à terme : `main.cpp`, `Contact.cpp`, `PhoneBook.cpp`.
- Ne pas fournir une solution complète : guider étape par étape.