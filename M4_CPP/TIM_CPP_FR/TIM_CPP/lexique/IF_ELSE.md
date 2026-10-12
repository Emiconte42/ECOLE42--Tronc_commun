# `if` / `else` — Branchement conditionnel

> **TL;DR.** `if (condition)` exécute l'instruction ou le bloc suivant lorsque la condition est convertie en `true`. `else` intercepte l'autre chemin. Enchaînez avec `else if`. La condition est **toute expression convertible en `bool`** — c'est là que se cachent les bugs.

Associé : [`SWITCH.md`](SWITCH.md) · [`BOOL.md`](BOOL.md) · [`LOOPS.md`](LOOPS.md)

---

## 1. La structure

```cpp
if (hp <= 0) {
    std::cout << "dead" << std::endl;
} else if (hp < 20) {
    std::cout << "critical" << std::endl;
} else {
    std::cout << "fine" << std::endl;
}
```

```
        condition ──true──►  branch A
            │
          false
            ▼
        else if ──true──►  branch B
            │
          false
            ▼
          else  ─────────►  branch C
```

Exactement **une** branche est exécutée. Les conditions sont testées de haut en bas ; le premier `true` l'emporte.

## 2. Ce qui compte comme true

Toute valeur arithmétique non nulle, tout pointer non nul. `0`, `0.0`, et les null pointers sont `false`.

```cpp
int *p = ft_find(x);
if (p)          // idiomatique : "p is not null"
    use(*p);
```

## 3. Les pièges classiques

**`=` au lieu de `==`** — une assignation à l'intérieur d'une condition compile et est presque toujours un bug :

```cpp
if (x = 5)      // 💥 assigne 5, toujours true
if (x == 5)     // ✅ compare
```
`-Wall` l'intercepte (`-Wparentheses`). Werror le rend fatal — l'une des raisons pour lesquelles 42 impose ces flags.

**Dangling `else`** — le `else` se rattache au `if` le plus **proche**, et non à celui suggéré par votre indentation :

```cpp
if (a)
    if (b) f();
else g();       // 💥 appartient à `if (b)` malgré l'indentation
```
Remède : toujours mettre des accolades lors de l'imbrication.

**Accolades manquantes + ligne ajoutée** :

```cpp
if (err)
    log(err);
    return 1;   // 💥 s'exécute inconditionnellement — l'indentation ment
```

## 4. Le ternaire `?:`

Un `if/else` qui est une **expression** — il produit une valeur. Parfaitement accepté en C++ (seulement banni par la norminette C).

```cpp
int max = (a > b) ? a : b;
std::cout << (ok ? "yes" : "no");
```

Utilisez-le pour un *choix de valeur*, jamais pour un *choix d'action* — les effets de bord dans un ternaire sont affreux à lire.

## 5. Conseils

- Testez d'abord le cas d'erreur / cas limite et faites un return anticipé — cela permet de garder le happy path non indenté.
- Comparer une variable à une constante ? `if (x == 42)`. Plusieurs constantes pour une seule variable ? Pensez à [`switch`](SWITCH.md).
- Une longue chaîne de `else if` qui dispatche selon le *type* est un smell — c'est précisément le rôle de [`virtual`](VIRTUAL.md).