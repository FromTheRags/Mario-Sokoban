# Mario Sokoban – Règles du jeu

## 1. Le but du jeu

Le but du Sokoban est de ranger **toutes les caisses** sur les **objectifs**
(les cibles dorées dessinées au sol). Quand une caisse est sur un objectif,
elle devient verte avec un cadenas doré : elle est rangée et **ne bouge plus**.

Le niveau est gagné dès que tous les objectifs sont couverts par une caisse.

## 2. Déplacer le personnage

- Les **flèches du clavier** déplacent le personnage d’une case (haut, bas,
  gauche, droite, jamais en diagonale).
- Il ne peut que **pousser** une caisse, jamais la tirer, et une seule à la
  fois : une caisse ne peut pas en pousser une autre.
- Il ne peut traverser ni les **murs**, ni les **objectifs**, ni les caisses
  déjà rangées. Il peut en revanche passer sur les cases des boutons du haut
  de l’écran.
- Le chronomètre en haut de l’écran mesure le temps de la partie.

Pendant une partie :

- la **flèche rouge** (en haut à gauche) ramène au choix des niveaux ;
- le bouton **recommencer** (en haut à droite) remet le niveau à zéro.

À la victoire, le temps s’affiche avec une médaille :

- niveaux du jeu : **REJOUER**, **NIVEAU** (choix des niveaux) ou **MENU** ;
- niveaux personnels : **REJOUER** ou **MENU**.

## 3. Les menus

- **1 – JOUER** : les six niveaux du jeu. Une médaille marque les niveaux
  réussis et le **top 3 des meilleurs temps** de chaque niveau est affiché.
  Le bouton **NIVEAUX PERSO** donne accès aux niveaux personnels.
- **2 – CREER** : l’éditeur de niveaux.
- Au clavier : `1` ou `2` dans le menu principal, `1` à `6` dans le choix des
  niveaux.

Les niveaux personnels sont rangés sur **3 pages de 9 niveaux** (27 niveaux).
On change de page avec les boutons `< PREC` / `SUIV >` ou les flèches
gauche/droite du clavier. Chaque niveau affiche un aperçu, sa médaille et ses
3 meilleurs temps.

## 4. L’éditeur de niveaux

Le menu de l’éditeur propose deux choix :

- **MODIFIER** : choisissez un emplacement dans la liste. Un emplacement déjà
  utilisé s’ouvre avec son niveau ; un emplacement vide permet de créer un
  nouveau niveau.
- **REINITIALISATION** : choisissez un emplacement puis confirmez avec **OUI**
  (ou **NON** pour revenir à la liste). Le niveau est vidé, et sa médaille et
  ses temps sont effacés.

Dans l’éditeur, la barre d’outils en haut de l’écran contient, de gauche à
droite :

| Bouton | Rôle |
|---|---|
| Flèche rouge | Revenir à la liste des niveaux à éditer |
| Disquette | Sauvegarder le niveau |
| Caisse | Poser une caisse (sur un objectif : caisse déjà rangée) |
| Personnage | Placer le personnage (un seul par niveau) |
| Mur | Poser un mur |
| Objectif | Poser un objectif |
| Gomme | Effacer une case |
| Sol | Remettre du sol sur une case |
| GO | Tester le niveau (sans enregistrer de temps ni de médaille) |
| Recommencer | Recharger la dernière sauvegarde du niveau |

Cliquez d’abord sur un outil (il est souligné en rouge), puis sur les cases
du plateau. Les cases jamais peintes sont du sol.

Un niveau doit contenir **un personnage, des caisses et des objectifs** pour
être jouable ; sinon le message « NIVEAU INCOMPLET OU VIDE » s’affiche.

## 5. Plein écran

- `F11` ou `Alt+Entrée` : passer en plein écran ou revenir en fenêtre.
- `Échap` : quitter le plein écran.
- Option `--fullscreen` (ou `-f`) : démarrer en plein écran.

La fenêtre peut aussi être redimensionnée ; le jeu reste centré.

## 6. Pour aller plus loin

À côté de l’exécutable :

- `niveaux perso/niveau1.txt` … `niveau27.txt` contiennent les niveaux
  personnels. Copiez ce dossier pour sauvegarder ou partager vos niveaux.
- `data/` contient les meilleurs temps (`best_times.txt`) et les médailles.
  Supprimez ce dossier pour repartir de zéro.
