# Mario Sokoban – SDL 3

Cette branche contient une réécriture native pour SDL 3. Elle utilise directement
les API SDL 3 et SDL_ttf 3, sans couche d’adaptation des fonctions SDL 1.2.
Les graphismes sont les assets originaux générés par `tools/generate_assets.py`
et dédiés au domaine public (CC0 1.0).

## Build avec CMake et CLion

Sous Windows avec le compilateur MinGW de CLion, configurez normalement avec :

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel
```

Si SDL 3 ou SDL_ttf 3 ne sont pas installés, CMake télécharge leurs SDK MinGW
officiels dans le dossier de build au premier configure. Une connexion Internet
est alors nécessaire; les DLL sont ensuite copiées avec l’exécutable.
Sur les autres systèmes, installez les paquets de développement SDL 3 et SDL_ttf 3.

L’exécutable et les ressources sont copiés dans `build/package`. Pour une
compilation sur Ubuntu via GitHub Actions, la CI installe les dernières versions
SDL 3 et SDL_ttf 3 avant de compiler et d’archiver l’application.
Pour lancer le jeu depuis un build local, exécutez `build/package/mario-sokoban`
avec `build/package` comme répertoire de travail.

## Commandes

- Flèches ou ZQSD pour déplacer le gardien
- `R` pour recommencer le niveau
- `1` à `9` pour sélectionner un niveau
- `N` pour charger le niveau suivant
- `E` pour ouvrir l’éditeur, touches `1` à `5` pour choisir mur/sol/caisse/but/gardien, clic pour placer
- `S` pour enregistrer le niveau 10, `O` pour le recharger
- `Échap` pour quitter

Le jeu reprend les neuf niveaux de 10 × 8 cases et inclut un éditeur natif SDL 3
pour enregistrer un niveau personnalisé dans `niveaux perso/niveau10.txt`.
Consultez [LICENSES.md](LICENSES.md) pour les licences des dépendances et des
ressources.
