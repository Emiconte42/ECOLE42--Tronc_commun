# `switch` / `case` / `default` / `break` — Branchement multidirectionnel

> **TL;DR.** `switch (expr)` saute vers le label `case` dont la constante correspond à `expr`. L'exécution traverse ensuite *chaque* instruction suivante jusqu'à ce qu'elle rencontre un `break` ou la fin du bloc — c'est le **fall-through**, et c'est le piège numéro un. `default:` est la solution de repli quand rien ne correspond.

Articles liés : [`ENUM.md`](ENUM.md) · [`BOOL.md`](BOOL.md) · [`MEMBER_FUNCTION_POINTERS.md`](../notions/advanced/MEMBER_FUNCTION_POINTERS.md) (le pattern alternatif)

---

## 1. La structure

```cpp
switch (level) {
    case 0:
        std::cout << "DEBUG"   << std::endl;
        break;
    case 1:
        std::cout << "INFO"    << std::endl;
        break;
    case 2:
        std::cout << "WARNING" << std::endl;
        break;
    case 3:
        std::cout << "ERROR"   << std::endl;
        break;
    default:
        std::cout << "unknown" << std::endl;
}
```

À lire comme : *"évaluer `level` une seule fois, puis sauter vers le `case` correspondant."* Sans `break`, l'exécution se poursuit dans le case suivant — il n'y a aucune barrière implicite entre les cases.

---

## 2. Ce que `expr` peut être

L'expression dans `switch (expr)` doit être d'un **type intégral** (ou implicitement convertible vers un type intégral) :

- `int`, `short`, `long`, `char`, `bool`
- Les valeurs d'`enum` (elles se convertissent en `int`)
- Tout ce qui possède une conversion définie par l'utilisateur vers un type intégral

**Interdit en C++98 :**

- `std::string` — aucune conversion intégrale. C'est pourquoi l'exercice Harl utilise des tableaux parallèles de chaînes + une boucle `for`, et non un `switch`.
- `float` / `double` — tester l'égalité sur des flottants n'a pas de sens.
- Les types class sans conversion intégrale.

```cpp
switch (std::string("hi")) { ... }     // ❌ won't compile
```

Les labels `case` eux-mêmes doivent être des **expressions constantes intégrales** — des valeurs que le compiler peut calculer à la compilation. Les variables ne fonctionnent pas :

```cpp
int x = 3;
switch (n) {
    case x: ...        // ❌ x is not a constant
    case 3: ...        // ✅
    case 1 + 2: ...    // ✅ constant arithmetic
}
```

---

## 3. Le fall-through — la caractéristique déterminante

`switch` est un `goto` déguisé. Dès que l'exécution atterrit sur un `case` correspondant, elle continue en exécutant *chaque instruction* jusqu'à rencontrer un `break`, un `return`, un `throw`, ou l'accolade fermante `}`.

```cpp
switch (n) {
    case 1:
        std::cout << "one ";        // n==1 prints: "one two three "
    case 2:
        std::cout << "two ";        // n==2 prints: "two three "
    case 3:
        std::cout << "three ";      // n==3 prints: "three "
}
```

C'est **intentionnel** en C++ — parfois, c'est exactement ce que l'on souhaite :

```cpp
switch (level) {
    case 0:  debug();      // fall through
    case 1:  info();       // fall through
    case 2:  warning();    // fall through
    case 3:  error();      break;
    default: complainUnknown();
}
```

C'est le pattern du **filtre Harl** (CPP01 ex06) : "se plaindre à ce niveau **et à tous les niveaux supérieurs**." Le fall-through offre gratuitement ce comportement en cascade.

Mais cette même fonctionnalité est à l'origine du bug le plus courant en C — oublier un `break` pour que deux cases fusionnent silencieusement. Les compilers modernes émettent un warning (`-Wimplicit-fallthrough` en C++17+), mais C++98 n'offre rien. **La discipline est votre seul filet de sécurité.**

---

## 4. `default` — la solution de repli

`default:` est le label atteint quand **aucun** `case` ne correspond. Il peut apparaître n'importe où dans le switch (le compiler s'en moque), mais par convention, on le place en dernier.

```cpp
switch (c) {
    case 'y': case 'Y': accept(); break;
    case 'n': case 'N': reject(); break;
    default:            askAgain();
}
```

Si vous omettez `default` et que rien ne correspond, l'intégralité du switch est une opération sans effet (no-op).

Remarque : plusieurs labels `case` peuvent être empilés pour une même instruction (`case 'y': case 'Y':` ci-dessus). Chaque label n'est qu'une cible de saut ; l'instruction n'est exécutée qu'une seule fois lorsqu'elle est atteinte.

---

## 5. Vue implémentation / matérielle — jump tables

Un `switch` sur des valeurs entières **denses et contiguës** compile généralement en une **jump table** : un tableau d'adresses de code indexé par la valeur du switch. C'est plus rapide qu'une série de `if` car c'est en `O(1)` et non en `O(n)` :

```
  switch (level)         ──┐
                            │   table lookup
                            ▼
              level →   [ &case_0, &case_1, &case_2, &case_3 ]
                            │
                            └──▶ jmp *table[level]
```

Pour des valeurs **éparses** (`case 1: case 7: case 100:`) ou des plages non contiguës, le compiler peut se rabattre sur un arbre de recherche binaire de comparaisons, ou même sur une chaîne de `if/else` s'il n'y a que peu de cases. Vous ne pouvez pas forcer une stratégie plutôt qu'une autre — c'est le choix de l'optimiseur.

Dans tous les cas, `switch` est au moins aussi rapide que la chaîne `if/else` équivalente, et souvent plus rapide. Optimisation *prématurée*, certes — mais c'est un gain réel sur les chemins critiques (hot paths).

---

## 6. Portée des variables (variable scope) à l'intérieur d'un switch

Tout le corps du switch constitue **un seul scope**. Les cases n'introduisent pas de nouveaux scopes par eux-mêmes. Cela signifie que vous ne pouvez pas déclarer une variable dans un case et vous attendre à ce qu'elle soit détruite avant le suivant :

```cpp
switch (n) {
    case 1:
        int x = 10;            // ⚠ this declaration is visible in case 2!
        std::cout << x;
        break;
    case 2:
        std::cout << x;        // x exists but was never initialized for this path
        break;
}
```

C'est un véritable piège. Le compiler peut même **rejeter** le code si vous sautez par-dessus une initialisation non triviale. La solution : **entourer un case d'accolades** pour lui donner son propre scope :

```cpp
switch (n) {
    case 1: {
        int x = 10;
        std::cout << x;
        break;
    }
    case 2:
        std::cout << "no x here";
        break;
}
```

Prenez l'habitude de mettre des `{ }` autour d'un case contenant plusieurs instructions — cela ne coûte rien et évite ce piège.

---

## 7. `switch` vs `if/else` vs tables de dispatch

Trois outils, trois cas d'usage idéaux :

| Pattern | Quand l'utiliser |
|---|---|
| `if/else` | Deux ou trois cas ; conditions complexes (pas seulement l'égalité) ; plages, prédicats, types mixtes. |
| `switch` | Nombreux cas sur une seule valeur intégrale/enum ; le fall-through est souhaité ou sans danger. |
| dispatch table | Associer une valeur (de n'importe quel type) à une *fonction*. Voir [`MEMBER_FUNCTION_POINTERS.md`](../notions/advanced/MEMBER_FUNCTION_POINTERS.md). |

L'exercice Harl est intéressant car les **trois** s'appliquent à différents endroits :

- **ex05** utilise une dispatch table (chaîne → méthode).
- **ex06** utilise une dispatch table pour *trouver* l'indice du niveau, puis un `switch` pour piloter la cascade de fall-through.

---

## 8. Trucs et astuces

### Associer `switch` et `enum`

L'utilisation la plus propre du `switch` se fait sur un `enum`. Le compiler peut vérifier que vous avez traité chaque énumérateur — c'est le rôle du flag `-Wswitch` de votre Makefile :

```cpp
enum Level { DEBUG, INFO, WARNING, ERROR };

switch (lv) {
    case DEBUG:   ...; break;
    case INFO:    ...; break;
    case WARNING: ...; break;
    // case ERROR missing → -Wswitch warns
}
```

N'ajoutez `default:` que si vous voulez réellement attraper tous les autres cas ; l'omettre rend `-Wswitch` plus bavard et utile.

### Commentaires `// fallthrough` explicites

Lorsque vous avez l'intention d'utiliser le fall-through, **indiquez-le dans un commentaire**. Votre futur vous-même (ou votre évaluateur) supposera sinon qu'un `break` manquant est un bug :

```cpp
case 0: debug();   // fallthrough
case 1: info();    break;
```

### `break` ne quitte que le `switch` (ou la boucle) le plus proche

Un `break` dans un `switch` situé à l'intérieur d'un `for` quitte le `switch`, **pas** la boucle. Si vous devez quitter la boucle depuis un case, utilisez un drapeau (flag) ou un `return`/`goto` :

```cpp
for (int i = 0; i < n; ++i) {
    switch (a[i]) {
        case 0: break;           // exits the switch, NOT the for
        case 1: ...; break;
    }
    // execution continues here after the switch's break
}
```

---

## 9. Erreurs courantes

| Erreur | Résultat | Solution |
|---|---|---|
| `break` oublié | Deux cases fusionnent silencieusement | Ajouter `break;` ou un commentaire `// fallthrough` |
| Variable déclarée dans un case sans `{}` | Erreur de compilation ou fuite de portée (scope leak) | Entourer le case de `{ }` |
| `switch (std::string)` | "switch quantity not an integer" | Utiliser des tableaux parallèles + boucle ou `if/else` |
| Label `case` non constant | "expression is not an integral constant" | Utiliser un littéral ou une constante intégrale `const` |
| Valeurs de `case` en double | "duplicate case value" | Chaque entier ne peut étiqueter qu'un seul case au maximum |
| `default` manquant avec `-Wswitch` sur un enum | Warning concernant un énumérateur non géré | Ajouter le case manquant ou ajouter `default:` |

---

## 10. Résumé visuel

```
    switch (expr)
        │
        ▼  one evaluation
   ┌────────────────┐
   │ jump table /   │
   │ comparisons    │
   └────────────────┘
        │
        ├──▶ case A:  stmt; stmt;  ──▶ break? ─yes─▶ ┐
        │                              │             │
        │                              no            │
        │                              ▼             │
        ├──▶ case B:  stmt; stmt;  ──▶ break? ─yes─▶ │  (fall-through)
        │                                            │
        ├──▶ ...                                     │
        │                                            │
        └──▶ default: stmt;                          │
                                                     ▼
                                              exit switch
```
Les flèches allant vers le *bas* dans le case suivant (lorsque vous oubliez `break`) représentent les chemins de fall-through. La flèche allant vers la *droite* en sortant de `break` est la seule sortie sûre.

---

## 11. Practice questions

1. Qu'affiche `switch (1) { case 1: cout << "a"; case 2: cout << "b"; default: cout << "c"; }` ?
2. Pourquoi `switch (s)` ne compile-t-il pas lorsque `s` est une `std::string` ?
3. Vous écrivez un `switch (lv)` sur un `enum Level { DEBUG, INFO, WARNING, ERROR };` et n'en gérez que trois. Avec `-Wswitch`, que se passe-t-il ?
4. À l'intérieur de `for (...) switch (...) { case 0: break; }`, le `break` quitte-t-il la boucle `for` ?
5. Pourquoi le C++ exige-t-il des accolades autour d'un case qui déclare une variable locale ?

<details>
<summary>Answers</summary>

1. `"abc"` — aucun `break`, donc une fois `case 1` atteint, l'exécution fait un fall-through à travers chaque instruction.
2. `std::string` n'a pas de conversion implicite vers un type integral ; `switch` n'accepte que des expressions de type integral.
3. Le compiler émet un avertissement concernant l'enumerator non géré. Avec `-Werror` (défini par votre Makefile), cela devient une erreur — le programme ne compilera pas tant que vous ne l'aurez pas traité ou que vous n'aurez pas ajouté `default:`.
4. Non — `break` quitte le `switch` ou la boucle englobant(e) le plus proche. Le `switch` étant plus proche, le `for` continue.
5. Tout le corps du `switch` partage un seul et même scope. Une déclaration sans bloc peut être « sautée » si le flux d'exécution entre par un case ultérieur, laissant la variable dans un état indéfini. Les accolades donnent au case son propre scope, de sorte que la variable ne peut pas fuiter vers d'autres cases.

</details>