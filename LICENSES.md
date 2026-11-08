# Licences et provenance des ressources

## SDL 3 et SDL_ttf 3

La branche `SDL_3` utilise les API natives de SDL 3 et SDL_ttf 3. Les deux
bibliothèques sont distribuées sous licence zlib. La CI GitHub utilise
`libsdl-org/setup-sdl` pour récupérer leurs versions stables les plus récentes
et inclut les bibliothèques dynamiques requises dans l’archive Linux.
Les textes de ces licences sont fournis dans `SDL3-zlib.txt` et
`SDL3-ttf-zlib.txt`.

## Noto Sans

La police fournie est Noto Sans du projet Google Fonts, sous licence SIL Open
Font License 1.1. La licence est incluse dans `NotoSans-OFL.txt`.

## Illustrations du jeu

Les images dans `sprites_mario_sokoban/`, ainsi que `caisse.bmp` et `caisse.ico`,
ont été créées à partir de formes géométriques par `tools/generate_assets.py`.
Elles sont dédiées au domaine public sous CC0 1.0. Elles ne réutilisent pas
les personnages, logos, captures ou textures du jeu d’origine.

Le code et le nom du projet d’origine n’ont pas été réécrits dans le cadre de
l’audit des ressources.
