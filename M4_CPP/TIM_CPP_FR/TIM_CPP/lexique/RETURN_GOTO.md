# `return` / `goto` — Quitter prématurément

> **TL;DR.** `return` termine la fonction courante, renvoie une valeur à l'appelant, et — point crucial en C++ — **les destructors de toutes les variables locales s'exécutent** à la sortie. `goto` saute vers un label dans la même fonction ; il existe toujours en C++98, et vous n'en écrirez pratiquement jamais.

Connexe : [`LOOPS.md`](LOOPS.md) · [`TRY_CATCH_THROW.md`](TRY_CATCH_THROW.md) · [`VOID.md`](VOID.md)

---

## 1. `return`

```cpp
int abs(int x) {
    if (x < 0)
        return -x;      // early return — terminé, on sort maintenant
    return x;
}

void log(const std::string& s) {
    if (s.empty())
        return;         // fonctions void : return sans valeur
    std::cout << s << std::endl;
}
```

Ce qui se passe réellement lors d'un `return` :

```
   return expr;
      │
      ├─ 1. expr est évaluée (éventuellement copiée dans le return slot)
      ├─ 2. les destructors des variables locales s'exécutent, dans l'ordre inverse de création
      └─ 3. le contrôle revient à l'appelant
```

L'étape 2 est la spécificité du C++ : le RAII garantit qu'un `return` prématuré **ne peut pas faire fuiter** de ressources gérées sur la stack. C'est pourquoi le dogme du « point de sortie unique » venu du C a beaucoup moins d'importance ici.

## 2. Le style early-return (guard clauses)

```cpp
int process(File& f) {
    if (!f.isOpen())   return ERR_CLOSED;   // les guards d'abord,
    if (f.empty())     return 0;            // plat et lisible
    // happy path, non indenté
    return doWork(f);
}
```

## 3. Pièges

**Retourner une reference/pointer vers une variable locale** — la variable locale meurt à `}`, l'appelant récupère un cadavre :

```cpp
std::string& bad() {
    std::string s = "hi";
    return s;            // 💥 dangling reference — UB
}
```

**`return` manquant sur un chemin d'exécution** — UB, pas une erreur par défaut. `-Werror` + `-Wall` la rend fatale. Le [`switch`](SWITCH.md) sur un enum avec tous les cas et sans `default` aide `-Wswitch` à prouver la couverture.

**`return` dans `main`** : `return 0;` = succès. Atteindre le `}` du `main` sans return est valide (un `return 0` implicite) — mais le style 42 l'écrit explicitement.

## 4. `goto` — la pièce de musée

```cpp
    if (err1) goto cleanup;
    ...
cleanup:
    close(fd);
```

C'est l'idiome du **C** pour le nettoyage centralisé. En C++, le destructor effectue ce travail automatiquement — le RAII a remplacé `goto cleanup`. Dernier cas d'usage valide mais rare : sortir de boucles profondément imbriquées, et même dans ce cas, une fonction + `return` est plus propre.

Les règles si jamais vous en croisez un : au sein de la même fonction uniquement, impossible de sauter par-dessus l'initialisation d'un objet ayant un constructor (erreur du compiler), et il est interdit par la plupart des guides de style, y compris tout ce qui touche à 42.

> **Modèle mental :** `return` = sortir par la porte d'entrée, les destructors vous serrent la main sur le pas de la porte. `goto` = passer par la fenêtre ; le C++ les a pour la plupart condamnées.