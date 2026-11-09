# Licences et provenance des ressources

## SDL 3

La branche `SDL_3` utilise les API natives de SDL 3, distribuée sous licence
zlib (texte dans `SDL3-zlib.txt`). Sous Windows, CMake télécharge le SDK
officiel ; la CI GitHub utilise `libsdl-org/setup-sdl` et inclut la
bibliothèque dynamique dans l’archive Linux.

## Police

Les textes sont dessinés avec une police bitmap 5 × 7 originale définie dans
`bitmap_font.hpp` (la même que sur la branche `master`). Aucun fichier de
police n’est fourni.

## Illustrations du jeu

Les images dans `sprites_mario_sokoban/`, ainsi que `caisse.bmp` et `caisse.ico`,
ont été créées à partir de formes géométriques par `tools/generate_assets.py`.
Elles sont dédiées au domaine public sous CC0 1.0. Elles ne réutilisent pas
les personnages, logos, captures ou textures du jeu d’origine.

Le code et le nom du projet d’origine n’ont pas été réécrits dans le cadre de
l’audit des ressources.
