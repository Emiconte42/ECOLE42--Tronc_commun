# 🔑 LEXIQUE — Chaque mot-clé C++98 à portée de clic

Une page par mot-clé (ou groupe étroitement lié) : ce qu'il fait, comment il est implémenté, le comportement matériel, des astuces, des diagrammes ASCII. **Les 63 mots-clés de C++98 sont couverts** — trouvez le vôtre dans la table A→Z, cliquez, comprenez, repartez.

> Vous cherchez un *concept* plutôt qu'un mot-clé (RAII, lvalue, deep copy, vtable…) ? C'est dans le [`GLOSSAIRE.md`](GLOSSAIRE.md).
> Les fichiers thématiques plus généraux se trouvent dans [`../notions/`](../notions/INDEX.md) ; les guides de projets dans [`../projets/`](../projets/INDEX.md).

---

## 🔤 A→Z — les 63 mots-clés

| Mot-clé | Page | | Mot-clé | Page |
|---|---|---|---|---|
| `asm` | [`LEGACY_KEYWORDS.md`](LEGACY_KEYWORDS.md) | | `namespace` | [`NAMESPACE.md`](NAMESPACE.md) |
| `auto` ⚠️ | [`LEGACY_KEYWORDS.md`](LEGACY_KEYWORDS.md) | | `new` | [`NEW.md`](NEW.md) |
| `bool` | [`BOOL.md`](BOOL.md) | | `operator` | [`OPERATOR.md`](OPERATOR.md) |
| `break` | [`LOOPS.md`](LOOPS.md) | | `private` | [`PUBLIC_PRIVATE_PROTECTED.md`](PUBLIC_PRIVATE_PROTECTED.md) |
| `case` | [`SWITCH.md`](SWITCH.md) | | `protected` | [`PUBLIC_PRIVATE_PROTECTED.md`](PUBLIC_PRIVATE_PROTECTED.md) |
| `catch` | [`TRY_CATCH_THROW.md`](TRY_CATCH_THROW.md) | | `public` | [`PUBLIC_PRIVATE_PROTECTED.md`](PUBLIC_PRIVATE_PROTECTED.md) |
| `char` | [`TYPES.md`](TYPES.md) | | `register` | [`LEGACY_KEYWORDS.md`](LEGACY_KEYWORDS.md) |
| `class` | [`CLASS.md`](CLASS.md) | | `reinterpret_cast` | [`CASTS.md`](CASTS.md) |
| `const` | [`CONST.md`](CONST.md) | | `return` | [`RETURN_GOTO.md`](RETURN_GOTO.md) |
| `const_cast` | [`CASTS.md`](CASTS.md) | | `short` | [`TYPES.md`](TYPES.md) |
| `continue` | [`LOOPS.md`](LOOPS.md) | | `signed` | [`TYPES.md`](TYPES.md) |
| `default` | [`SWITCH.md`](SWITCH.md) | | `sizeof` | [`SIZEOF.md`](SIZEOF.md) |
| `delete` | [`DELETE.md`](DELETE.md) | | `static` | [`STATIC.md`](STATIC.md) |
| `do` | [`LOOPS.md`](LOOPS.md) | | `static_cast` | [`CASTS.md`](CASTS.md) |
| `double` | [`TYPES.md`](TYPES.md) | | `struct` | [`STRUCT.md`](STRUCT.md) |
| `dynamic_cast` | [`CASTS.md`](CASTS.md) | | `switch` | [`SWITCH.md`](SWITCH.md) |
| `else` | [`IF_ELSE.md`](IF_ELSE.md) | | `template` | [`TEMPLATE.md`](TEMPLATE.md) |
| `enum` | [`ENUM.md`](ENUM.md) | | `this` | [`THIS.md`](THIS.md) |
| `explicit` | [`EXPLICIT.md`](EXPLICIT.md) | | `throw` | [`TRY_CATCH_THROW.md`](TRY_CATCH_THROW.md) |
| `export` | [`LEGACY_KEYWORDS.md`](LEGACY_KEYWORDS.md) | | `true` / `false` | [`BOOL.md`](BOOL.md) |
| `extern` | [`EXTERN.md`](EXTERN.md) | | `try` | [`TRY_CATCH_THROW.md`](TRY_CATCH_THROW.md) |
| `float` | [`TYPES.md`](TYPES.md) | | `typedef` | [`TYPEDEF.md`](TYPEDEF.md) |
| `for` | [`LOOPS.md`](LOOPS.md) | | `typeid` | [`TYPEID.md`](TYPEID.md) |
| `friend` | [`FRIEND.md`](FRIEND.md) | | `typename` | [`TYPENAME.md`](TYPENAME.md) |
| `goto` | [`RETURN_GOTO.md`](RETURN_GOTO.md) | | `union` | [`UNION.md`](UNION.md) |
| `if` | [`IF_ELSE.md`](IF_ELSE.md) | | `unsigned` | [`TYPES.md`](TYPES.md) |
| `inline` | [`INLINE.md`](INLINE.md) | | `using` | [`USING.md`](USING.md) |
| `int` | [`TYPES.md`](TYPES.md) | | `virtual` | [`VIRTUAL.md`](VIRTUAL.md) |
| `long` | [`TYPES.md`](TYPES.md) | | `void` | [`VOID.md`](VOID.md) |
| `mutable` | [`MUTABLE.md`](MUTABLE.md) | | `volatile` | [`VOLATILE.md`](VOLATILE.md) |
| | | | `wchar_t` | [`TYPES.md`](TYPES.md) · `while` → [`LOOPS.md`](LOOPS.md) |

> ⚠️ `auto` est le piège classique des évaluations — en C++98, il ne signifie **pas** déduction de type. Voir [`LEGACY_KEYWORDS.md`](LEGACY_KEYWORDS.md).

---

## 🗂️ Par thématique

| Thématique | Pages |
|---|---|
| **Types & valeurs** | [`TYPES`](TYPES.md) · [`BOOL`](BOOL.md) · [`VOID`](VOID.md) · [`ENUM`](ENUM.md) · [`UNION`](UNION.md) · [`SIZEOF`](SIZEOF.md) · [`TYPEDEF`](TYPEDEF.md) |
| **Flux de contrôle** | [`IF_ELSE`](IF_ELSE.md) · [`LOOPS`](LOOPS.md) · [`SWITCH`](SWITCH.md) · [`RETURN_GOTO`](RETURN_GOTO.md) |
| **Stockage & qualificateurs** | [`CONST`](CONST.md) · [`STATIC`](STATIC.md) · [`VOLATILE`](VOLATILE.md) · [`MUTABLE`](MUTABLE.md) · [`EXPLICIT`](EXPLICIT.md) · [`INLINE`](INLINE.md) · [`EXTERN`](EXTERN.md) |
| **POO** | [`CLASS`](CLASS.md) · [`STRUCT`](STRUCT.md) · [`PUBLIC_PRIVATE_PROTECTED`](PUBLIC_PRIVATE_PROTECTED.md) · [`THIS`](THIS.md) · [`FRIEND`](FRIEND.md) · [`VIRTUAL`](VIRTUAL.md) · [`OPERATOR`](OPERATOR.md) |
| **Mémoire** | [`NEW`](NEW.md) · [`DELETE`](DELETE.md) |
| **Templates & portée** | [`TEMPLATE`](TEMPLATE.md) · [`TYPENAME`](TYPENAME.md) · [`NAMESPACE`](NAMESPACE.md) · [`USING`](USING.md) |
| **Casts & RTTI** | [`CASTS`](CASTS.md) · [`TYPEID`](TYPEID.md) |
| **Exceptions** | [`TRY_CATCH_THROW`](TRY_CATCH_THROW.md) |
| **Fossiles** | [`LEGACY_KEYWORDS`](LEGACY_KEYWORDS.md) — `auto`, `register`, `asm`, `export` |

---

## 🗺️ Carrefour mot-clé ↔ notion

| En lisant la notion… | Zoomez sur ces mots-clés |
|---|---|
| [`fundamentals/BASICS.md`](../notions/fundamentals/BASICS.md) | [`CLASS`](CLASS.md) · [`PUBLIC_PRIVATE_PROTECTED`](PUBLIC_PRIVATE_PROTECTED.md) · [`CONST`](CONST.md) · [`NAMESPACE`](NAMESPACE.md) |
| [`fundamentals/MEMORY.md`](../notions/fundamentals/MEMORY.md) | [`NEW`](NEW.md) · [`DELETE`](DELETE.md) · [`STATIC`](STATIC.md) |
| [`oop/ORTHODOX_CANONICAL_FORM.md`](../notions/oop/ORTHODOX_CANONICAL_FORM.md) | [`OPERATOR`](OPERATOR.md) · [`THIS`](THIS.md) · [`EXPLICIT`](EXPLICIT.md) |
| [`oop/INHERITANCE.md`](../notions/oop/INHERITANCE.md) | [`PUBLIC_PRIVATE_PROTECTED`](PUBLIC_PRIVATE_PROTECTED.md) · [`USING`](USING.md) · [`VIRTUAL`](VIRTUAL.md) |
| [`oop/POLYMORPHISM.md`](../notions/oop/POLYMORPHISM.md) | [`VIRTUAL`](VIRTUAL.md) · [`TYPEID`](TYPEID.md) · [`CASTS`](CASTS.md) |
| [`advanced/TEMPLATES.md`](../notions/advanced/TEMPLATES.md) | [`TEMPLATE`](TEMPLATE.md) · [`TYPENAME`](TYPENAME.md) |
| [`io-errors/ERROR_MANAGEMENT.md`](../notions/io-errors/ERROR_MANAGEMENT.md) | [`TRY_CATCH_THROW`](TRY_CATCH_THROW.md) · [`RETURN_GOTO`](RETURN_GOTO.md) |
| [`tooling/LIBRARIES.md`](../notions/tooling/LIBRARIES.md) | [`EXTERN`](EXTERN.md) · [`STATIC`](STATIC.md) · [`INLINE`](INLINE.md) |

---

## 📐 Ce que chaque page couvre

Chaque fichier de mot-clé suit le même squelette pour que vous sachiez où chercher :

1. **TL;DR** — une phrase pour poser les bases du reste.
2. **Ce qu'il fait** — la signification au niveau du langage.
3. **Implémentation / vue matérielle** — ce que le compiler émet, où résident les données.
4. **Exemples pratiques** — extraits C++98 prêts à être copiés-collés.
5. **Astuces, erreurs courantes, résumé visuel.**

Les pages groupées plus récentes (`TYPES`, `LOOPS`, `IF_ELSE`, `RETURN_GOTO`, `UNION`, `LEGACY_KEYWORDS`) sont délibérément plus courtes — TL;DR, structure, pièges, terminé.