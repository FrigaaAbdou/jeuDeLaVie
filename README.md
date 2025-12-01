# Projet POO — Jeu de la Vie (Conway)

Auteurs: FRIGAA Abderrahmane, BENAHMED Mehdi

Ce dépôt contient une implémentation orientée objet du « Jeu de la vie » de John Conway en C++ (C++17) avec une interface graphique SFML. Le projet respecte les contraintes pédagogiques décrites dans `Projet.html` (polymorphisme, séparation logique/GUI, modes console et graphique, lecture/écriture de fichiers, tests unitaires simples).

## Fonctionnalités

- Deux modes d'exécution:
  - Mode graphique (SFML): visualisation, contrôle du pas de temps, rechargement de la grille.
  - Mode console: lit un état initial et écrit les `n` premières itérations dans un dossier `<nom_fichier>_out/`.
- Lecture d'un fichier d'entrée: première ligne `lignes colonnes`, puis une matrice de 0/1 (état des cellules).
- Arrêt sur stabilité (console: fin automatique; graphique: la simulation se met en pause quand l'automate n'évolue plus).
- Raccourcis utiles: pause, pas à pas, vitesse, aléatoire, « refresh » depuis le fichier.
- Tests unitaires de base (block, blinker).

## Dépendances

- Compilateur C++17 (g++/clang++).
- SFML 3.x recommandé (l'application utilise l'API d'événements de SFML 3). Avec SFML 2.x, il faudrait adapter la boucle d'événements.
- `pkg-config` (optionnel). Sous macOS/Homebrew, SFML est généralement installé en `/opt/homebrew`.

## Construction (Makefile)

Commandes principales:

- Compilation: `make`
- Lancer en mode graphique: `make run`
- Lancer le GUI avec des arguments: `make gui GUI_ARGS="--step 0.1 state.txt"`
- Mode console (écrit les itérations): `make console ITER=10 FILE=state.txt`
- Tests unitaires: `make test`
- Nettoyage: `make clean`

Le Makefile essaie d'abord `pkg-config`; sinon, il bascule sur les chemins Homebrew (`/opt/homebrew`).

Compilation manuelle (exemple):

```bash
g++ -std=c++17 -O2 main.cpp -o main \
  -I/opt/homebrew/include \
  -L/opt/homebrew/lib \
  -lsfml-graphics -lsfml-window -lsfml-system
```

## Format du fichier d'entrée

Première ligne: `nb_lignes nb_colonnes`, puis la matrice de 0/1. Exemple (`state.txt`):

```
5 10
0 0 1 0 0 0 0 0 0 0
0 0 0 1 0 0 0 0 0 0
0 1 1 1 0 0 0 0 0 0
0 0 0 0 0 0 0 0 0 0
0 0 0 0 0 0 0 0 0 0
```

Un exemple `state.txt` est fourni à la racine du dépôt.

## Utilisation

### Mode graphique (par défaut)

```bash
./main [--step <secondes>] [<fichier_entree>]
```

- `--step <secondes>`: durée entre deux générations (ex: `--step 0.1`).
- `<fichier_entree>`: chemin d'un fichier au format ci‑dessus. Sans fichier, une grille aléatoire est générée.

Contrôles clavier/souris:

- Espace: Pause/lecture.
- N: Avance d'un pas (quand en pause).
- Flèche Haut / Bas: Diminue / augmente la durée entre générations.
- R: Nouvelle configuration aléatoire (mêmes dimensions).
- F ou clic sur le bouton « Refresh » (barre supérieure): recharge la grille depuis le fichier fourni et relance immédiatement la simulation.

Remarque: quand l'automate devient stable (aucun changement entre deux itérations), la simulation se met en pause automatiquement pour le signaler.

### Mode console

```bash
./main --console <iterations> <fichier_entree>
```

- Lit l'état initial, calcule jusqu'à `<iterations>` ou jusqu'à stabilité.
- Écrit chaque itération dans `<dossier_du_fichier>/<nom_sans_extension>_out/1.txt`, `2.txt`, … (même format que l'entrée).

### Tests unitaires

```bash
./main --test
```

Exécute des tests simples (still life « block », oscillateur « blinker »).

## Architecture (vue d'ensemble)

- `CellState` (abstraite), `AliveState`, `DeadState`: modélisent l'état d'une cellule (vivante/morte) via polymorphisme.
- `Rule` (stratégie) + `ConwayRule`: hiérarchie indépendante des états pour exprimer les règles d'évolution.
- `Cell`: encapsule un pointeur vers un `CellState`.
- `Grid`: contient la matrice de cellules, le comptage des voisins et l'application d'une `Rule`.
- `Game`: orchestre une `Grid` avec une `Rule`.
- GUI SFML: rendu et gestion des entrées; la logique du jeu reste dans `Grid`/`Game`/`Rule`.

Pour un aperçu UML du cœur (états/cellule), voir `diagramme.md`.

## Conformité au cahier des charges (résumé)

- POO & SOLID: hiérarchie d'états, stratégie de règles, séparation logique/GUI.
- Lecture d'un fichier d'entrée et génération des objets (grille, cellules): oui.
- Deux modes: console (avec fichiers de sortie) et graphique: oui.
- Arrêt sur stabilité ou après un nombre d'itérations: oui (console: fin; graphique: pause auto).
- Tests unitaires: oui (fonction `--test`).

## Prochaines extensions possibles

- Grille torique, cellules « obstacle », motifs pré‑programmés au clavier, parallélisation de la génération.

---

© FRIGAA Abderrahmane & BENAHMED Mehdi — Projet POO « Jeu de la Vie »

