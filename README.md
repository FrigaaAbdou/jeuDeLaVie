# Jeu de la vie (C++17 + SFML 3)

Implémentation du Jeu de la vie de Conway avec deux modes d’exécution (console et interface SFML). La logique est séparée du rendu : `Grid` / `GameOfLife` / `Rule` portent l’automate, `GraphicRunner` gère l’affichage et les interactions.

## Présentation du programme

- Automate cellulaire classique (Conway) : une grille de cellules vivantes/mortes, évolution synchrone à chaque itération selon la règle B3/S23.
- Deux modes d’utilisation complémentaires :
  - **Console** pour la génération et l’export de séries d’états (fichiers texte).
  - **Graphique (SFML)** pour la visualisation interactive, avec pause et pas à pas.
- Arrêt automatique sur stabilité (grille inchangée) ou après un nombre d’itérations maximum.
- Itérateur d’itération affiché dans le titre de la fenêtre et en overlay (si une police est trouvée).
- Grille par défaut non torique (bords absorbants) ; le champ `toroidal` de la configuration permettrait un mode torique si activé dans le code client.

### Fonctionnement interne (résumé académique)

- **Lecture** : `InitialStateLoader` charge un fichier texte (dimensions + matrice). Validation stricte sur la cohérence lignes/colonnes.
- **État** : chaque cellule porte un `CellState` (vivante/morte) clonable, stocké dans `Grid`. Copie profonde assurée pour préserver l’intégrité lors des duplications de grilles.
- **Règle** : `Rule` (stratégie abstraite) et `ConwayRule` implémentent B3/S23 (naissance sur 3 voisins, survie sur 2 ou 3).
- **Simulation** : `GameOfLife` calcule la génération suivante, mémorise la précédente pour détecter la stabilité, et incrémente un compteur d’itérations.
- **Persistance / I/O** : `GridExporter` écrit chaque grille en texte (console), `GraphicRunner` affiche via SFML (rendu en rectangles, 20 px).

## Compilation

Prérequis : compilateur C++17, SFML 3.x, `pkg-config`. Sous macOS/Homebrew, SFML est généralement installé sous `/opt/homebrew/Cellar/sfml/<version>/`.

```bash
make          # construit l’exécutable ./jeu (via pkg-config sfml-graphics)
make clean    # supprime objets/dépendances/binaire
```

## Exécution

- Mode graphique : `./jeu graphic <fichier_entree> [maxIter]`
- Mode console : `./jeu console <fichier_entree> <prefixe_sortie> [maxIter]`

Remarques :
- `maxIter` prend la valeur par défaut de `SimulationConfig::maxIterations` (100 actuellement) si l’argument est omis.
- Le fichier d’entrée est lu au lancement uniquement ; pour prendre en compte une modification, relancez le programme.

### Commandes (mode graphique)

- Barre espace : pause / reprise
- N : avance d’un pas (en pause)
- Le titre de la fenêtre et l’overlay vert affichent l’itération courante.

### Sorties (mode console)

Chaque itération est écrite dans `<prefixe_sortie>_<n>.txt` jusqu’à stabilisation ou `maxIter`.

## Format du fichier d’entrée

Fichier texte : première ligne `lignes colonnes`, puis la matrice de `0`/`1` (morte/vivante) :

```
5 5
0 0 0 0 0
0 1 1 1 0
0 0 0 0 0
0 0 0 0 0
0 0 0 0 0
```

Un exemple `state.txt` (40×40 aléatoire) est présent à la racine.

## Organisation du projet

- `CellState` (abstraite), `AliveState`, `DeadState` : états clonables des cellules.
- `Rule`, `ConwayRule` : règle d’évolution (stratégie).
- `Cell`, `Grid` : stockage de la grille et comptage des voisins (option torique disponible, désactivée par défaut).
- `GameOfLife` : exécute les pas, détecte la stabilité et compte les itérations.
- `ConsoleRunner`, `GraphicRunner` : points d’entrée mode console / SFML.
- `GridExporter`, `InitialStateLoader` : E/S des grilles texte.
- `SimulationConfig` : configuration d’exécution (fichier d’entrée, préfixe de sortie, itérations max, torique, mode).

## Dépannage

- « Fichier invalide (etat cellules) » : l’entête (lignes/colonnes) ne correspond pas au nombre de lignes/colonnes fournies.
- Pas d’overlay texte dans la fenêtre : aucune police système trouvée dans les emplacements testés ; ajouter un chemin de police dans `GraphicRunner.cpp` si besoin.
- Arrêt à 50 itérations malgré la modification : recompiler après avoir changé `SimulationConfig.h` (`make clean && make`), ou passer explicitement l’argument `maxIter` en ligne de commande.

## Crédits

- Réalisation et code : Abderrahmane Frigaa, Mehdi Benahmed.
- Encadrement pédagogique : projet POO (Jeu de la vie) — implémentation C++17 avec séparation logique / interface.
