# `auto` / `register` / `asm` / `export` — Les fossiles du C++98

> **TL;DR.** Quatre keywords qui existent en C++98 mais que vous n'écrirez jamais utilement. Ils importent pour une seule raison : **`auto` signifie quelque chose de totalement différent en C++98 par rapport au C++ moderne**, et les évaluateurs adorent cette question.

Voir aussi : [`STATIC.md`](STATIC.md) · [`EXTERN.md`](EXTERN.md) · [`TYPEDEF.md`](TYPEDEF.md)

---

## 1. `auto` — la question piège

En **C++98**, `auto` est un *storage-class specifier* signifiant « durée de stockage automatique » (*automatic storage duration*) — c'est-à-dire une simple variable locale sur la stack. Ce qui est déjà le comportement par défaut, le rendant 100 % redondant :

```cpp
auto int x = 5;   // C++98 : identique a `int x = 5;` — legal, inutile
```

En **C++11+**, le keyword a été recyclé pour la déduction de type :

```cpp
auto x = 5;       // C++11 : le compiler deduit int — INTERDIT a 42
```

> **La réponse en soutenance :** « `auto` existe en C++98 mais uniquement en tant que storage specifier ; l'`auto` de déduction de type date du C++11, il est donc interdit par le sujet. » Même histoire de recyclage C++98/C++11 que pour les alias dans [`USING.md`](USING.md).

## 2. `register` — une indication que personne n'écoute

```cpp
register int i;   // "s'il te plait, garde i dans un registre CPU"
```

Les compilers modernes ignorent totalement ce hint — leurs allocateurs de registres sont meilleurs que le vôtre. Le seul effet visible en C++98 : vous ne pouvez pas prendre l'adresse d'une variable `register`... sauf que les compilers ne l'appliquent presque jamais non plus. Deprecated en C++11, supprimé en C++17.

## 3. `asm` — assembly inline

```cpp
asm("nop");       // injecte de l'assembly brut
```

Standard de nom seulement — la syntaxe et le comportement sont implementation-defined (`asm volatile (...)` style GCC, etc.). Territoire réservé aux kernels et aux drivers. À 42 : jamais.

## 4. `export` — le keyword qui n'a jamais fonctionné

Destiné à permettre aux définitions de template de résider dans des fichiers `.cpp` au lieu des headers. Exactement **un** frontend de compiler l'a implémenté un jour (EDG) ; tous les autres ont refusé. Supprimé en C++11. Les templates restent dans les headers — voir [`TEMPLATE.md`](TEMPLATE.md) pour comprendre pourquoi (la monomorphisation a besoin du code source).

---

## Tableau récapitulatif

| Keyword | Signification C++98 | Destin | À écrire un jour ? |
|---|---|---|---|
| `auto` | automatic storage (par défaut de toute façon) | recyclé en C++11 pour la déduction de type | ❌ — mais connaissez le piège |
| `register` | hint pour registre | supprimé en C++17 | ❌ |
| `asm` | assembly inline | toujours présent, implementation-defined | ❌ |
| `export` | templates hors-header | jamais implémenté, supprimé en C++11 | ❌ |