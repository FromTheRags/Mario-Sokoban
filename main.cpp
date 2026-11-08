#ifdef __cplusplus
    #include <cstdlib>
#else
    #include <stdlib.h>
#endif
#include <SDL.h>
#include "bitmap_font.hpp"
#include <string.h>
//code source retouché: rechercher "//bug résolu :" pour voir les retouches
void pause();
void enregistreniv(int tableau[],int niveaufichier,int eff);
void lecture(int niveau,int tableauniv[]);
int choixniveau(SDL_Rect posiclic);
void choixniv(SDL_Surface* ecran);
void load(SDL_Surface *ecran , int niveau, int tableau1[]);
int tableau(int niveau , int fois);
int cases(SDL_Rect posimario);
int casesmodif(SDL_Rect posimario);
int verif(int niveau ,SDL_Rect posimario, int tableau[],int direct);
int victoire(SDL_Surface* ecran);
void affvict(SDL_Surface* ecran,SDL_Rect posivict, int fois,SDL_Surface* affiche);
void save(int niveau);
void afftrait(int jaune,SDL_Surface* ecran,SDL_Rect positionlignes,SDL_Surface*colleimagecrea);
int caseposi(int cases,int number);
int niveaucrea(SDL_Surface* ecran,SDL_Rect posiclic);
int tobesur(SDL_Surface* ecran,SDL_Rect posiclic);
int play(int tableau1[],SDL_Surface* caisse,SDL_Rect posicaisse,SDL_Surface* ecran,SDL_Rect posiclic,SDL_Rect posiretour,SDL_Rect posireplay,SDL_Rect posimario,SDL_Surface* mario,SDL_Surface* retour,SDL_Surface* replay);
int affonlyvictyouinon(SDL_Surface* ecran,SDL_Rect posiclic);
void affefface(SDL_Surface* ecran);
int affcomplet(SDL_Surface* ecran,SDL_Rect posiclic);

int main ( int argc, char* argv[] )
{

    TTF_Init();//démarrage de la bibliothèque texte
   // initialize SDL video
    if ( SDL_Init( SDL_INIT_VIDEO ) < 0 )
    {
        printf( "Unable to init SDL: %s\n", SDL_GetError() );
        return 1;
    }

    // make sure SDL cleans up before exit
    atexit(SDL_Quit);

    SDL_WM_SetIcon(SDL_LoadBMP("caisse.bmp"), NULL);
    SDL_WM_SetCaption("Mario Sokoban !", NULL);//sert à renomer la fenêtre,second parametre icone
    // create a new window
    SDL_Surface* ecran = SDL_SetVideoMode(680, 544, 16,
                                           SDL_HWSURFACE|SDL_DOUBLEBUF);
    if ( !ecran )
    {
        printf("Unable to set 680x544 video: %s\n", SDL_GetError());
        return 1;
    }


    SDL_Color couleurorange = {255,123,0};
    SDL_Color couleurbleumarine = {0,0,255};

    SDL_Surface *image_demarrage = NULL,*ecran_partie = NULL,*ecran_dev =NULL,*mario = NULL,*caisse = NULL,*replay=NULL,*retour=NULL,*victoireyou=NULL,*colleimagecrea = NULL;
    SDL_Rect pimage_demarrage,posi_partie,posi_dev,posimario,posicaisse,posireplay,posiretour,posivictoireyou,posicolleimagecrea;
    pimage_demarrage.x = 0;
    pimage_demarrage.y = 0;
    posi_partie.x = 0;
    posi_partie.y = 0;
    posi_dev.x = 0;
    posi_dev.y = 0;
    posimario.x = 0;
    posimario.y = 0;
    posicaisse.x=0;
    posicaisse.y = 0;
    posireplay.x=0;
    posireplay.y=0;
    posiretour.x=0;
    posiretour.y=0;
    posivictoireyou.x=0;
    posivictoireyou.y=0;
    posicolleimagecrea.x=0;
    posicolleimagecrea.y=0;

SDL_Surface *rectangle = NULL;
SDL_Rect position;//création de la variable position qui contient 2 sous variable
    //les coordonnés
    position.x = 0;
    position.y = 0;
     SDL_Rect posiclic;
    posiclic.x = 0;
    posiclic.y = 0;

    TTF_Font *police = NULL;
    SDL_Surface *texte = NULL;
    SDL_Rect positexte;
    SDL_Surface *lignes[18]={NULL};
    SDL_Rect positionlignes;
    positionlignes.x = 0;
    positionlignes.y = 0;

    SDL_Event event;
    FILE*test = NULL;
    int returnplay = 0,vide = 0,accesniv = 0,boucleniv = 0;
    int jouer = 0,debutcrea = 0,quita = 0,cursus = 0,nombfois = 0,valeurtest = 0,clicniv = 0,pass=0,sauteif = 0,tobechoix = 0,boucle = 0;
    int continuer = 1,quit =0,partie = 0,niveau = 0,fois = 0,finpartie = 2,endwhile = 0,level= 0,lignesvar = 0,arrete =0,creationend = 0;
    int tableau1[80]={0};
    int tableaucrea[80]={0};
    int tableauvide[80]={0};
    SDL_EnableKeyRepeat(400, 20);
while(finpartie < 4 && quit == 0)
{

    rectangle = SDL_CreateRGBSurface(SDL_HWSURFACE, 680, 544, 16, 0, 0,0, 0);//dimension et mode de couleur du rectangle
    SDL_FillRect(rectangle, NULL, SDL_MapRGB(ecran->format, 0, 0, 0));//couleur du rectangle
    SDL_BlitSurface(rectangle, NULL, ecran, &position);//colle le rectangle à l'écran(0 parce que l'on ne sert pas de ces paramètres

    image_demarrage = SDL_LoadBMP ("sprites_mario_sokoban/menu.bmp");
    SDL_BlitSurface(image_demarrage , NULL, ecran, &pimage_demarrage );
    police = TTF_OpenFont("NotoSans.ttf", 40);
    positexte.x = 10;
    positexte.y = 500;
    texte = TTF_RenderText_Blended(police, "créé par Rémi Fache....",couleurorange);
    SDL_BlitSurface(texte, NULL, ecran, &positexte); /* Blit du texte par-dessus */
    TTF_CloseFont(police); /* Doit être avant TTF_Quit() */

    police = TTF_OpenFont("NotoSans.ttf", 40);
    SDL_Color couleurtexte = {255, 245, 205};
    SDL_Color couleurfond = {32, 55, 84};
    texte = TTF_RenderText_Shaded(police, "MARIO SOKOBAN", couleurtexte, couleurfond);
    positexte.x = 225;
    positexte.y = 75;
    SDL_BlitSurface(texte, NULL, ecran, &positexte);
    SDL_FreeSurface(texte);
    texte = TTF_RenderText_Shaded(police, "1 - JOUER", couleurtexte, couleurfond);
    positexte.x = 248;
    positexte.y = 218;
    SDL_BlitSurface(texte, NULL, ecran, &positexte);
    SDL_FreeSurface(texte);
    texte = TTF_RenderText_Shaded(police, "2 - CREER", couleurtexte, couleurfond);
    positexte.x = 248;
    positexte.y = 262;
    SDL_BlitSurface(texte, NULL, ecran, &positexte);
    SDL_FreeSurface(texte);
    TTF_CloseFont(police);

    SDL_Flip(ecran);

    endwhile = 0;
    finpartie=2;
    creationend = 0;
    continuer = 1;
   while (continuer)
    {
       SDL_WaitEvent(&event);
     switch(event.type)
    {
        case SDL_QUIT:
        continuer = 0;
        quit = 1;
         break;
      case SDL_KEYDOWN:

     if(event.key.keysym.sym == SDLK_1 || event.key.keysym.sym ==SDLK_KP1)
 {
          continuer = 0;
           partie =1;
 }
              if(event.key.keysym.sym == SDLK_KP2 ||event.key.keysym.sym ==SDLK_2)
 {
         continuer = 0;
          partie =2;
 }

      break;
      case SDL_MOUSEBUTTONUP: /* Clic de la souris */
      posiclic.x = event.button.x; /* change les coordonnées de posiclic*/
      posiclic.y = event.button.y;
      if(posiclic.x >= 186 && posiclic.x <= 330 && posiclic.y >= 222 && posiclic.y <= 252)
      {partie =1;
      continuer=0;
      }
      if(posiclic.x >= 176 && posiclic.x <=480  && posiclic.y >= 257 && posiclic.y <= 287)
    {partie = 2;
    continuer = 0;}
       }

    }



   if(partie == 1)
   {
      while(finpartie < 3 && quit == 0)
 {
     do
     {boucleniv = 0;
     accesniv=0;
     endwhile = 0;
     finpartie=2;
  choixniv(ecran);
    retour = SDL_LoadBMP ("sprites_mario_sokoban/back.bmp");
    SDL_BlitSurface(retour , NULL, ecran, &posiretour );
    SDL_Flip(ecran);

         fois=0;
     test =fopen("data/open1.lvl","r");
     if (test != NULL)
     {
         fscanf(test," %d ",&level);
         fclose(test);
        if(level)
        {
            fois = 1;
            affvict(ecran,posivictoireyou,fois,victoireyou);
        }
     }
     test = NULL;
     test =fopen("data/open2.lvl","r");
     if (test != NULL)
     {
         fscanf(test," %d ",&level);
         fclose(test);
        if(level)
        {
            fois = 2;
            affvict(ecran,posivictoireyou,fois,victoireyou);
        }
     }
          test = NULL;
     test =fopen("data/open3.lvl","r");
     if (test != NULL)
     {
         fscanf(test," %d ",&level);
         fclose(test);
        if(level)
        {
            fois = 3;
            affvict(ecran,posivictoireyou,fois,victoireyou);
        }
     }
          test = NULL;
     test =fopen("data/open4.lvl","r");
     if (test != NULL)
     {
         fscanf(test," %d ",&level);
         fclose(test);
        if(level)
        {
            fois = 4;
            affvict(ecran,posivictoireyou,fois,victoireyou);
        }
     }
          test = NULL;
     test =fopen("data/open5.lvl","r");
     if (test != NULL)
     {
         fscanf(test," %d ",&level);
         fclose(test);
        if(level)
        {
            fois = 5;
            affvict(ecran,posivictoireyou,fois,victoireyou);
        }
     }
          test = NULL;
     test =fopen("data/open6.lvl","r");
     if (test != NULL)
     {
         fscanf(test," %d ",&level);
         fclose(test);
        if(level)
        {
            fois = 6;
            affvict(ecran,posivictoireyou,fois,victoireyou);
        }
     }

       continuer = 1;
            while (continuer)
    {
       SDL_WaitEvent(&event);
     switch(event.type)
    {
        case SDL_QUIT:
        continuer = 0;
        quit = 1;
         break;
      case SDL_KEYDOWN:

     if(event.key.keysym.sym == SDLK_1 || event.key.keysym.sym ==SDLK_KP1)
     {niveau = 1;continuer = 0;}
     if(event.key.keysym.sym == SDLK_KP2 ||event.key.keysym.sym ==SDLK_2)
     {niveau = 2;continuer = 0;}
     if(event.key.keysym.sym == SDLK_3 || event.key.keysym.sym ==SDLK_KP3)
     {niveau = 3;continuer = 0;
     }
     if(event.key.keysym.sym == SDLK_KP4 ||event.key.keysym.sym ==SDLK_4)
     {niveau = 4;
     continuer = 0;
     }
     if(event.key.keysym.sym == SDLK_KP5 ||event.key.keysym.sym ==SDLK_5)
     {niveau = 5;
     continuer = 0;
     }
     if(event.key.keysym.sym == SDLK_KP6 ||event.key.keysym.sym ==SDLK_6)
     {niveau = 6;
     continuer = 0;
     }
      break;
      case SDL_MOUSEBUTTONUP: /* Clic de la souris */
      posiclic.x = event.button.x; /* change les coordonnées de posiclic*/
      posiclic.y = event.button.y;
      if(posiclic.x >= 0 && posiclic.x <= 68 && posiclic.y >= 0 && posiclic.y <= 68)
      {finpartie = 3;
      continuer=0;
      niveau = 1;
      }
      if(posiclic.x >= 120 && posiclic.x <=194  && posiclic.y >= 150 && posiclic.y <= 226)
    {niveau = 1;continuer = 0;}
    if(posiclic.x >= 285 && posiclic.x <= 359 && posiclic.y >= 150 && posiclic.y <= 226)
    {niveau = 2;continuer = 0;}
    if(posiclic.x >= 450 && posiclic.x <= 524 && posiclic.y >= 150 && posiclic.y <= 226)
    {niveau =3;continuer = 0;}
    if(posiclic.x >= 120 && posiclic.x <= 194 && posiclic.y >= 250 && posiclic.y <=326 )
    {niveau =4;continuer = 0;}
    if(posiclic.x >= 285 && posiclic.x <= 359 && posiclic.y >= 250 && posiclic.y <=326 )
    {niveau =5;continuer = 0;}
    if(posiclic.x >= 450 && posiclic.x <= 524 && posiclic.y >= 250 && posiclic.y <=326 )
    {niveau =6;continuer = 0;}
    if(posiclic.x >= 120 && posiclic.x <= 524 && posiclic.y >= 350 && posiclic.y <=420 )
    {niveau =7;continuer = 0;}

      break;
       }
    }
    if(niveau == 7 && quit == 0)
    {
        accesniv = 0;
        while(accesniv<80)
        {
            tableaucrea[accesniv] = 0;
            accesniv++;
        }
        accesniv = 0;
        accesniv=niveaucrea(ecran,posiclic);
        if(accesniv == 0)
        quit = 1;
        if(quit == 0 && accesniv != 10)
        {lecture(accesniv,tableaucrea);
        accesniv = 0;
          accesniv =play(tableaucrea,caisse,posicaisse,ecran,posiclic,posiretour,posireplay,posimario,mario,retour,replay);
          if(accesniv)
          {quit = 1;
          }
          else
          {
              boucleniv = 1;
          }
        }
        if(accesniv == 10)
        {
            boucleniv = 1;
        }

     }
     }while(boucleniv == 1 && quit == 0);


    while(finpartie ==2 && quit == 0)
 {
    fois = 0;
    while(fois < 80 )
    {
    tableau1[fois] = tableau(niveau,fois);
    fois++;
    }
    endwhile = 0;
    load(ecran,niveau,tableau1);
    retour = SDL_LoadBMP ("sprites_mario_sokoban/back.bmp");
    SDL_BlitSurface(retour , NULL, ecran, &posiretour );

    posireplay.x=9*68;
    replay = SDL_LoadBMP ("sprites_mario_sokoban/retry.bmp");
    SDL_BlitSurface(replay , NULL, ecran, &posireplay );
    SDL_Flip(ecran);


   continuer = 1;
   if(niveau == 1)
   {
       posimario.x = 3*68;
       posimario.y=5*68;
   }
   if(niveau == 2)
   {
       posimario.x = 1*68;
       posimario.y=1*68;
   }
   if(niveau == 3)
   {
       posimario.x = 1*68;
       posimario.y=2*68;
   }
   if(niveau == 4)
   {
       posimario.x = 1*68;
       posimario.y=2*68;
   }
   if(niveau == 5)
   {
       posimario.x = 1*68;
       posimario.y=2*68;
   }
   if(niveau == 6)
   {
       posimario.x = 8*68;
       posimario.y=6*68;
   }
fois=0;



   while (continuer == 1 && quit == 0 && endwhile == 0)
       {
         SDL_WaitEvent(&event);
             switch(event.type)
           {
          case SDL_QUIT:
              continuer = 0;
              quit = 1;
             break;
         case SDL_KEYDOWN:
         //bug résolu :
          if(event.key.keysym.sym == SDLK_UP ||event.key.keysym.sym ==SDLK_DOWN
             ||event.key.keysym.sym ==SDLK_RIGHT||event.key.keysym.sym ==SDLK_LEFT)
     {
         mario = SDL_CreateRGBSurface(SDL_HWSURFACE, 68, 68, 16, 0, 0,0, 0);//on charge un carré blanc à l'ancien emplacement de mario
    SDL_FillRect(mario, NULL, SDL_MapRGB(ecran->format, 255,255,255));//cela permet d'éviter de se servir à chaque boucle de la fonction load(ce qui fait moche et ramer)
    SDL_BlitSurface(mario, NULL, ecran, &posimario); /* On
place mario à sa nouvelle position */
SDL_Flip(ecran); /* On met à jour l'affichage */
     }
         if(event.key.keysym.sym == SDLK_UP)
     {

         posimario.y -= 68;

     mario = SDL_LoadBMP("sprites_mario_sokoban/keeper_up.bmp");
      if(verif(niveau,posimario,tableau1,1) == 1 )
           {
               posimario.y += 68;
           }
            if(verif(niveau,posimario,tableau1,1) == 2 )
            {
                tableau1[cases(posimario)-1] = 2;
                posimario.y -= 68;
                tableau1[cases(posimario)-1] = 3;
                posicaisse.y=posimario.y;
                posicaisse.x=posimario.x;
                caisse = SDL_LoadBMP("sprites_mario_sokoban/crate.bmp");
                SDL_BlitSurface(caisse, NULL, ecran, &posicaisse);
                posimario.y += 68;
            }
             if(verif(niveau,posimario,tableau1,1) == 4)
            {
                tableau1[cases(posimario)-1] = 2;
                posimario.y -= 68;
                tableau1[cases(posimario)-1] = 6;
                posicaisse.y=posimario.y;
                posicaisse.x=posimario.x;
                caisse = SDL_LoadBMP("sprites_mario_sokoban/crate_target.bmp");
                SDL_BlitSurface(caisse, NULL, ecran, &posicaisse);
                posimario.y += 68;
            }

     }
     if(event.key.keysym.sym == SDLK_DOWN )
     {

         posimario.y += 68;
     mario = SDL_LoadBMP("sprites_mario_sokoban/keeper_down.bmp");
      if(verif(niveau,posimario,tableau1,4) == 1)
           {
               posimario.y -= 68;
           }
           if(verif(niveau,posimario,tableau1,4) == 2)
            {
                tableau1[cases(posimario)-1] = 2;
                posimario.y += 68;
                tableau1[cases(posimario)-1] = 3;
                posicaisse.y=posimario.y;
                posicaisse.x=posimario.x;
                caisse = SDL_LoadBMP("sprites_mario_sokoban/crate.bmp");
                SDL_BlitSurface(caisse, NULL, ecran, &posicaisse);
                posimario.y -= 68;
            }
            if(verif(niveau,posimario,tableau1,4) == 4)
            {
                tableau1[cases(posimario)-1] = 2;
                posimario.y += 68;
                tableau1[cases(posimario)-1] = 6;
                posicaisse.y=posimario.y;
                posicaisse.x=posimario.x;
                caisse = SDL_LoadBMP("sprites_mario_sokoban/crate_target.bmp");
                SDL_BlitSurface(caisse, NULL, ecran, &posicaisse);
                posimario.y -= 68;
            }
     }
     if(event.key.keysym.sym == SDLK_RIGHT)
     {

         posimario.x += 68;
      if(verif(niveau,posimario,tableau1,2) == 1)
           {
               posimario.x -= 68;
           }
            if(verif(niveau,posimario,tableau1,2) == 2)
            {
                tableau1[cases(posimario)-1] = 2;
                posimario.x += 68;
                tableau1[cases(posimario)-1] = 3;
                posicaisse.y=posimario.y;
                posicaisse.x=posimario.x;
                caisse = SDL_LoadBMP("sprites_mario_sokoban/crate.bmp");
                SDL_BlitSurface(caisse, NULL, ecran, &posicaisse);
                posimario.x -= 68;
            }
            if(verif(niveau,posimario,tableau1,2) == 4)
            {
                tableau1[cases(posimario)-1] = 2;
                posimario.x += 68;
                tableau1[cases(posimario)-1] = 6;
                posicaisse.y=posimario.y;
                posicaisse.x=posimario.x;
                caisse = SDL_LoadBMP("sprites_mario_sokoban/crate_target.bmp");
                SDL_BlitSurface(caisse, NULL, ecran, &posicaisse);
                posimario.x -= 68;
            }
     mario = SDL_LoadBMP("sprites_mario_sokoban/keeper_right.bmp");

     }
     if(event.key.keysym.sym == SDLK_LEFT)
     {

         posimario.x -= 68;
     if(verif(niveau,posimario,tableau1,3) == 1)
           {
               posimario.x += 68;
           }
           if(verif(niveau,posimario,tableau1,3) == 2)
           {
               tableau1[cases(posimario)-1] = 2;
               posimario.x -= 68;
               tableau1[cases(posimario)-1] = 3;
                posicaisse.y=posimario.y;
                posicaisse.x=posimario.x;
                caisse = SDL_LoadBMP("sprites_mario_sokoban/crate.bmp");
                SDL_BlitSurface(caisse, NULL, ecran, &posicaisse);
                posimario.x += 68;
           }
           if(verif(niveau,posimario,tableau1,3) == 4)
           {
               tableau1[cases(posimario)-1] = 2;
               posimario.x -= 68;
               tableau1[cases(posimario)-1] = 6;
                posicaisse.y=posimario.y;
                posicaisse.x=posimario.x;
                caisse = SDL_LoadBMP("sprites_mario_sokoban/crate_target.bmp");
                SDL_BlitSurface(caisse, NULL, ecran, &posicaisse);
                posimario.x += 68;
           }
     mario = SDL_LoadBMP("sprites_mario_sokoban/keeper_left.bmp");
     }
         break;
         case SDL_MOUSEBUTTONUP: /* Clic de la souris */
      posiclic.x = event.button.x; /* change les coordonnées de posiclic*/
      posiclic.y = event.button.y;
      if(posiclic.x >= 0 && posiclic.x <= 68 && posiclic.y >= 0 && posiclic.y <= 68)
      {finpartie = 1;
       endwhile=1;
      }
      if(posiclic.x >=68*9  && posiclic.x <= 68*10 && posiclic.y >= 0 && posiclic.y <= 68)
      {finpartie = 2;
       endwhile=1;
      }
      break;
      }

if(posimario.y == 0)
{
    if(posimario.x == 68*8)
    {
    replay = SDL_LoadBMP ("sprites_mario_sokoban/retry.bmp");
    SDL_BlitSurface(replay , NULL, ecran, &posireplay );
    }
    if(posimario.x == 68)
    {
    retour = SDL_LoadBMP ("sprites_mario_sokoban/back.bmp");
    SDL_BlitSurface(retour , NULL, ecran, &posiretour );
    }
}
if(posimario.y == 68)
{
    if(posimario.x == 0)
    {
    retour = SDL_LoadBMP ("sprites_mario_sokoban/back.bmp");
    SDL_BlitSurface(retour , NULL, ecran, &posiretour );
    }
    if(posimario.x == 68*9)
    {
    replay = SDL_LoadBMP ("sprites_mario_sokoban/retry.bmp");
    SDL_BlitSurface(replay , NULL, ecran, &posireplay );
    }
}


SDL_BlitSurface(mario, NULL, ecran, &posimario); /* On
place mario à sa nouvelle position */
SDL_Flip(ecran); /* On met à jour l'affichage */

 if(niveau == 1)
 {
     if(tableau1[14]==6 && tableau1[21]==6 && tableau1[35]==6)
     {
         finpartie = victoire(ecran);
         if(finpartie == 0)
         {
             quit=1;
         }
         endwhile = 1;
         save(niveau);
     }
 }
 if(niveau == 2)
 {
     if(tableau1[70]==6 && tableau1[47]==6 && tableau1[48]==6 && tableau1[49]==6)
     {
         finpartie = victoire(ecran);
         if(finpartie == 0)
         {
             quit=1;
         }
         endwhile = 1;
         save(niveau);
     }
 }
 if(niveau == 3)
 {
     if(tableau1[40]==6 && tableau1[41]==6 && tableau1[42]==6 && tableau1[43]==6 && tableau1[54]==6 && tableau1[72]==6 && tableau1[79]==6 )
     {
         finpartie = victoire(ecran);
         if(finpartie == 0)
         {
             quit=1;
         }
         endwhile = 1;
         save(niveau);
     }
 }
 if(niveau == 4)
 {
     if(tableau1[6]==6 && tableau1[15]==6 && tableau1[25]==6 && tableau1[51]==6 && tableau1[61]==6 && tableau1[72]==6 && tableau1[73]==6 && tableau1[74]==6 && tableau1[79]==6)
     {
         finpartie = victoire(ecran);
         if(finpartie == 0)
         {
             quit=1;
         }
         endwhile = 1;
         save(niveau);
     }
 }
  if(niveau == 5)
 {
     if(tableau1[7]==6 && tableau1[31]==6 && tableau1[51]==6 && tableau1[70]==6 && tableau1[77]==6 )
     {
         finpartie = victoire(ecran);
         if(finpartie == 0)
         {
             quit=1;
         }
         endwhile = 1;
         save(niveau);
     }
 }
 if(niveau == 6)
 {
     if(tableau1[7]==6 && tableau1[27]==6 && tableau1[36]==6 && tableau1[61]==6 && tableau1[64]==6 )
     {
         finpartie = victoire(ecran);
         if(finpartie == 0)
         {
             quit=1;
         }
         endwhile = 1;
         save(niveau);
     }
 }
SDL_Flip(ecran); /* On met à jour l'affichage */
}
}
}
}

if(partie == 2 && quit == 0)
{

do
{
    do
    {
        clicniv = 0;
        sauteif = 0;
        creationend = 0;

   ecran_partie= SDL_LoadBMP ("sprites_mario_sokoban/editor.bmp");
    SDL_BlitSurface(ecran_partie , NULL, ecran, &posi_partie );
    retour = SDL_LoadBMP ("sprites_mario_sokoban/back.bmp");
    SDL_BlitSurface(retour , NULL, ecran, &posiretour );
    police = TTF_OpenFont("NotoSans.ttf", 60);
    positexte.x =210;
    positexte.y = 150;
    texte = TTF_RenderText_Shaded(police, " Nouveau ",couleurorange,couleurbleumarine);
    SDL_BlitSurface(texte, NULL, ecran, &positexte); /* Blit du texte par-dessus */

    positexte.y = 250;
    texte = TTF_RenderText_Shaded(police, " Modifier ",couleurorange,couleurbleumarine);
    SDL_BlitSurface(texte, NULL, ecran, &positexte); /* Blit du texte par-dessus */

    positexte.y = 350;
    texte = TTF_RenderText_Shaded(police, " Supprimer ",couleurorange,couleurbleumarine);
    SDL_BlitSurface(texte, NULL, ecran, &positexte); /* Blit du texte par-dessus */

    SDL_Flip(ecran);


       continuer = 1;
   while (continuer)
    {
       SDL_WaitEvent(&event);
     switch(event.type)
    {
        case SDL_QUIT:
        continuer = 0;
        quit = 1;
         break;
     case SDL_MOUSEBUTTONUP: /* Clic de la souris */
      posiclic.x = event.button.x; /* change les coordonnées de posiclic*/
      posiclic.y = event.button.y;
      if(posiclic.x >= 0 && posiclic.x <= 68 && posiclic.y >= 0 && posiclic.y <= 68)
      {
       finpartie = 3;
       continuer = 0;
       debutcrea = 0;
       jouer = 4;//pour ne pas rentrer dans une boucle(==)
      }
      if(posiclic.x >= 210 && posiclic.x <= 378 && posiclic.y >= 150 && posiclic.y <= 210)
      {           //nouveau
       jouer = 1;
       creationend = 0;
       continuer = 0;
      }
      if(posiclic.x >= 210 && posiclic.x <= 378 && posiclic.y >= 250 && posiclic.y <= 310)
      {
       jouer = 2;//modifier
       creationend = 0;
       continuer = 0;
      }
      if(posiclic.x >= 210 && posiclic.x <= 210+183 && posiclic.y >= 350 && posiclic.y <= 410)
      {
       jouer = 3;//supprimer
       creationend = 0;
       continuer = 0;
      }


      break;
       }

    }
    valeurtest = 0;
    vide = 0;
     test = NULL;
     test = fopen("niveaux perso/niveau1.txt", "r");
     if (test != NULL)
     {fscanf(test," %d",&valeurtest);
    fclose(test);
    if(valeurtest)
    level =1;
    else
    vide = 1;
     }
    test = NULL;
     test = fopen("niveaux perso/niveau2.txt", "r");
     if (test != NULL)
     {fscanf(test," %d",&valeurtest);
    fclose(test);
    if(valeurtest)
    level =2;
    else
    vide = 2;
     }

    test = NULL;
     test = fopen("niveaux perso/niveau3.txt", "r");
     if (test != NULL)
     {fscanf(test," %d",&valeurtest);
    fclose(test);
    if(valeurtest)
    level =3;
    else
    vide = 3;
     }

         test = NULL;
     test = fopen("niveaux perso/niveau4.txt", "r");
     if (test != NULL)
     {fscanf(test," %d",&valeurtest);
    fclose(test);
    if(valeurtest)
    level =4;
    else
    vide = 4;
     }


         test = NULL;
     test = fopen("niveaux perso/niveau5.txt", "r");
     if (test != NULL)
     {fscanf(test," %d",&valeurtest);
    fclose(test);
    if(valeurtest)
    level =5;
    else
    vide = 5;
     }


         test = NULL;
     test = fopen("niveaux perso/niveau6.txt", "r");
     if (test != NULL)
     {fscanf(test," %d",&valeurtest);
    fclose(test);
    if(valeurtest)
    level =6;
    else
    vide = 6;
     }


         test = NULL;
     test = fopen("niveaux perso/niveau7.txt", "r");
     if (test != NULL)
     {fscanf(test," %d",&valeurtest);
    fclose(test);
    if(valeurtest)
    level =7;
    else
    vide = 7;
     }


         test = NULL;
     test = fopen("niveaux perso/niveau8.txt", "r");
     if (test != NULL)
     {fscanf(test," %d",&valeurtest);
    fclose(test);
    if(valeurtest)
    level =8;
    else
    vide = 8;
     }


         test = NULL;
     test = fopen("niveaux perso/niveau9.txt", "r");
     if (test != NULL)
     {fscanf(test," %d",&valeurtest);
    fclose(test);
    if(valeurtest)
    level =9;
    else
    vide = 9;
     }

     if(jouer == 1 && vide >0)
     /*level++;*/level = vide;
     else
     level = 9;
     if(level >9)
     {level = 9;

     }
do
{

     boucle = 0;
     if((jouer == 2 && quit == 0)||(jouer == 3 && quit == 0))
     {
         clicniv = niveaucrea(ecran,posiclic);
         if(clicniv == 0)
         quit = 1;
//si retour clicniv = 10
         pass = 0;
         if(clicniv<10)
         level = clicniv;

     }
     if(jouer == 3 && clicniv >0 && clicniv <10)
     {
         tobechoix = tobesur(ecran,posiclic);
         if(tobechoix == 1)
         {enregistreniv(tableauvide,clicniv,1);
         sauteif = 1;
         }
         if(tobechoix == 0)
         {
         boucle = 1;
         }
         if(tobechoix == 4)
         quit = 1;
     }
}while(boucle == 1 && quit == 0);


    if(quit == 0 && jouer != 4 && clicniv <10 && sauteif == 0)
    {while(creationend < 2 && quit == 0)
    {
        while(arrete < 80)
        {
            tableaucrea[arrete] = 0;
            arrete++;
        }
        quita = 0;
    rectangle = SDL_CreateRGBSurface(SDL_HWSURFACE, 680, 544, 16, 0, 0,0, 0);//dimension et mode de couleur du rectangle
    SDL_FillRect(rectangle, NULL, SDL_MapRGB(ecran->format, 255, 255, 255));//couleur du rectangle
     SDL_BlitSurface(rectangle, NULL, ecran, &position);//colle le rectangle à l'écran(0 parce que l'on ne sert pas de ces paramètres

    if(jouer == 2)
     {
         lecture(clicniv,tableaucrea);
         load(ecran,clicniv,tableaucrea);
     }

    retour = SDL_LoadBMP ("sprites_mario_sokoban/back.bmp");
    SDL_BlitSurface(retour , NULL, ecran, &posiretour );

    posireplay.x=9*68;
    replay = SDL_LoadBMP ("sprites_mario_sokoban/retry.bmp");
    SDL_BlitSurface(replay , NULL, ecran, &posireplay );

    posicolleimagecrea.x =68;
    colleimagecrea= SDL_LoadBMP ("sprites_mario_sokoban/save.bmp");
    SDL_BlitSurface(colleimagecrea , NULL, ecran, &posicolleimagecrea );

    posicolleimagecrea.x =68*2;
    colleimagecrea= SDL_LoadBMP ("sprites_mario_sokoban/crate_editor.bmp");
    SDL_BlitSurface(colleimagecrea , NULL, ecran, &posicolleimagecrea );

    posicolleimagecrea.x =68*3;
    colleimagecrea= SDL_LoadBMP ("sprites_mario_sokoban/keeper_editor.bmp");
    SDL_BlitSurface(colleimagecrea , NULL, ecran, &posicolleimagecrea );

    posicolleimagecrea.x =68*4;
    colleimagecrea= SDL_LoadBMP ("sprites_mario_sokoban/wall_editor.bmp");
    SDL_BlitSurface(colleimagecrea , NULL, ecran, &posicolleimagecrea );

    posicolleimagecrea.x =68*5;
    colleimagecrea= SDL_LoadBMP ("sprites_mario_sokoban/target_editor.bmp");
    SDL_BlitSurface(colleimagecrea , NULL, ecran, &posicolleimagecrea );

    posicolleimagecrea.x =68*6;
    colleimagecrea= SDL_LoadBMP ("sprites_mario_sokoban/eraser.bmp");
    SDL_BlitSurface(colleimagecrea , NULL, ecran, &posicolleimagecrea );

    //plus loin après les lignes blit du bouton jouer

    arrete = 0;
    positionlignes.y = 0;
    positionlignes.x = 0;
    lignesvar = 0;
     while(arrete < 8)
    {

        lignes[lignesvar] = SDL_CreateRGBSurface(SDL_HWSURFACE, 680, 1, 16, 0, 0,0, 0);//dimension et mode de couleur du rectangle
        SDL_FillRect(lignes[lignesvar], NULL, SDL_MapRGB(ecran->format,0,0, 0));//couleur du rectangle

        SDL_BlitSurface(lignes[lignesvar], NULL, ecran, &positionlignes); // Collage

        positionlignes.y += 68;
        lignesvar++;
        arrete++;

    }
    positionlignes.y = 34;
    positionlignes.x = 68;//544 mais en longeur(première ligne)
    colleimagecrea= SDL_CreateRGBSurface(SDL_HWSURFACE, 544, 1, 16, 0, 0,0, 0);//dimension et mode de couleur du rectangle
    SDL_FillRect(colleimagecrea, NULL, SDL_MapRGB(ecran->format,0,0, 0));//couleur du rectangle
    SDL_BlitSurface(colleimagecrea, NULL, ecran, &positionlignes); // Collage

    arrete = 0;
    positionlignes.y = 0;
    positionlignes.x = 0;
    lignesvar = 0;
     while(arrete < 10)
    {
        positionlignes.x += 68;
        lignes[lignesvar] = SDL_CreateRGBSurface(SDL_HWSURFACE, 1, 544, 16, 0, 0,0, 0);//dimension et mode de couleur du rectangle
        SDL_FillRect(lignes[lignesvar], NULL, SDL_MapRGB(ecran->format,0,0, 0));//couleur du rectangle

        SDL_BlitSurface(lignes[lignesvar], NULL, ecran, &positionlignes); // Collage
        lignesvar++;
        arrete++;

    }

    posicolleimagecrea.x =68*7;
    colleimagecrea= SDL_LoadBMP ("sprites_mario_sokoban/play.bmp");
    SDL_BlitSurface(colleimagecrea , NULL, ecran, &posicolleimagecrea );
     if(level == 9)
     {
         affefface(ecran);
     SDL_Flip(ecran);
     }
    SDL_Flip(ecran);
    do
    {positionlignes.y = 31;
       continuer = 1;
   while (continuer)
    {
       SDL_WaitEvent(&event);
     switch(event.type)
    {
        case SDL_QUIT:
        continuer = 0;
        quit = 1;
         break;
     case SDL_MOUSEBUTTONUP: /* Clic de la souris */
      posiclic.x = event.button.x; /* change les coordonnées de posiclic*/
      posiclic.y = event.button.y;
      if(posiclic.x >= 0 && posiclic.x <= 68 && posiclic.y >= 0 && posiclic.y <= 68)
      {
       debutcrea = 1;
       creationend = 3;
       continuer = 0;
       quita = 1;
      }
      if(posiclic.x >=68*9  && posiclic.x <= 68*10 && posiclic.y >= 0 && posiclic.y <= 68)
      {
          creationend = 1;
          continuer = 0;
          quita = 1;
      }
      if(posiclic.x >= 68 && posiclic.x <= 68*2 && posiclic.y >= 0 && posiclic.y <= 34)
      {
          cursus = 1;
          continuer = 0;
          enregistreniv(tableaucrea,level,0);
       //enregistre le fichier
    positionlignes.x = 68*3;
    afftrait(0,ecran,positionlignes,colleimagecrea);
    positionlignes.x = 68*4;
    afftrait(0,ecran,positionlignes,colleimagecrea);
    positionlignes.x = 68*5;
    afftrait(0,ecran,positionlignes,colleimagecrea);
    positionlignes.x = 68*6;
    afftrait(0,ecran,positionlignes,colleimagecrea);
    positionlignes.x = 68*2;
    afftrait(0,ecran,positionlignes,colleimagecrea);
    positionlignes.x = 68*1;
    afftrait(1,ecran,positionlignes,colleimagecrea);
      }
      if(posiclic.x >= 68*2 && posiclic.x <= 68*3 && posiclic.y >= 0 && posiclic.y <= 34)
      {
          cursus = 2;
          nombfois = 0;
          continuer = 0;
    positionlignes.x = 68*1;
    afftrait(0,ecran,positionlignes,colleimagecrea);
    positionlignes.x = 68*3;
    afftrait(0,ecran,positionlignes,colleimagecrea);
    positionlignes.x = 68*4;
    afftrait(0,ecran,positionlignes,colleimagecrea);
    positionlignes.x = 68*5;
    afftrait(0,ecran,positionlignes,colleimagecrea);
    positionlignes.x = 68*6;
    afftrait(0,ecran,positionlignes,colleimagecrea);
    positionlignes.x = 68*2;
    afftrait(1,ecran,positionlignes,colleimagecrea);
      }
       if(posiclic.x >= 68*3 && posiclic.x <= 68*4 && posiclic.y >= 0 && posiclic.y <= 34)
      {
          cursus = 3;
          nombfois = 0;
          continuer = 0;
    positionlignes.x = 68*1;
    afftrait(0,ecran,positionlignes,colleimagecrea);
    positionlignes.x = 68*2;
    afftrait(0,ecran,positionlignes,colleimagecrea);
    positionlignes.x = 68*4;
    afftrait(0,ecran,positionlignes,colleimagecrea);
    positionlignes.x = 68*5;
    afftrait(0,ecran,positionlignes,colleimagecrea);
    positionlignes.x = 68*6;
    afftrait(0,ecran,positionlignes,colleimagecrea);
    positionlignes.x = 68*3;
    afftrait(1,ecran,positionlignes,colleimagecrea);
      }
      if(posiclic.x >= 68*4 && posiclic.x <= 68*5 && posiclic.y >= 0 && posiclic.y <= 34)
      {
          cursus = 4;
          nombfois = 0;
          continuer = 0;
    positionlignes.x = 68*1;
    afftrait(0,ecran,positionlignes,colleimagecrea);
    positionlignes.x = 68*2;
    afftrait(0,ecran,positionlignes,colleimagecrea);
    positionlignes.x = 68*3;
    afftrait(0,ecran,positionlignes,colleimagecrea);
    positionlignes.x = 68*6;
    afftrait(0,ecran,positionlignes,colleimagecrea);
    positionlignes.x = 68*5;
    afftrait(0,ecran,positionlignes,colleimagecrea);
    positionlignes.x = 68*4;
    afftrait(1,ecran,positionlignes,colleimagecrea);
      }
      if(posiclic.x >= 68*5 && posiclic.x <= 68*6 && posiclic.y >= 0 && posiclic.y <= 34)
      {
          cursus = 5;
          nombfois = 0;
          continuer = 0;
    positionlignes.x = 68*1;
    afftrait(0,ecran,positionlignes,colleimagecrea);
    positionlignes.x = 68*2;
    afftrait(0,ecran,positionlignes,colleimagecrea);
    positionlignes.x = 68*3;
    afftrait(0,ecran,positionlignes,colleimagecrea);
    positionlignes.x = 68*6;
    afftrait(0,ecran,positionlignes,colleimagecrea);
    positionlignes.x = 68*4;
    afftrait(0,ecran,positionlignes,colleimagecrea);
    positionlignes.x = 68*5;
    afftrait(1,ecran,positionlignes,colleimagecrea);
      }
      if(posiclic.x >= 68*6 && posiclic.x <= 68*7 && posiclic.y >= 0 && posiclic.y <= 34)
      {
          cursus = 6;
          nombfois = 0;
          continuer = 0;
    positionlignes.x = 68*1;
    afftrait(0,ecran,positionlignes,colleimagecrea);
    positionlignes.x = 68*2;
    afftrait(0,ecran,positionlignes,colleimagecrea);
    positionlignes.x = 68*3;
    afftrait(0,ecran,positionlignes,colleimagecrea);
    positionlignes.x = 68*5;
    afftrait(0,ecran,positionlignes,colleimagecrea);
    positionlignes.x = 68*4;
    afftrait(0,ecran,positionlignes,colleimagecrea);
    positionlignes.x = 68*6;
    afftrait(1,ecran,positionlignes,colleimagecrea);
      }
      if(posiclic.x >= 68*7 && posiclic.x <= 68*9 && posiclic.y >= 0 && posiclic.y <= 34)
      {
          //appelle fonction jouer
          enregistreniv(tableaucrea,level,0);
          lecture(level,tableaucrea);
          returnplay =play(tableaucrea,caisse,posicaisse,ecran,posiclic,posiretour,posireplay,posimario,mario,retour,replay);
          if(returnplay)
          {quit = 1;
          }
          else
          {

          debutcrea = 1;
       creationend = 3;
       continuer = 0;
       quita = 1;

          }



      }
      continuer = 0;
      break;
       }

    }

    if(nombfois>0)
    {
        posiclic.x = caseposi(casesmodif(posiclic),2);
        posiclic.y =caseposi(casesmodif(posiclic),1);
    if(posiclic.y >0 /*|| (posiclic.y == 0 && posiclic.x>6*68)*/)
    {
    if(cursus == 2)
    {tableaucrea[cases(posiclic)-1] =3;
    colleimagecrea= SDL_LoadBMP ("sprites_mario_sokoban/crate.bmp");
    SDL_BlitSurface(colleimagecrea , NULL, ecran, &posiclic );
    }
    if(cursus == 3)
    {
       pass = 0;
     while(pass < 80)
     {
         if(tableaucrea[pass] == 5)
         {
             pass = 100;
         }
         pass ++;
     }
     if(pass <100)
    {tableaucrea[cases(posiclic)-1] =5;
    colleimagecrea= SDL_LoadBMP ("sprites_mario_sokoban/keeper_down.bmp");
    SDL_BlitSurface(colleimagecrea , NULL, ecran, &posiclic );
    }
    }
    if(cursus == 4)
    {tableaucrea[cases(posiclic)-1] =1;
    colleimagecrea= SDL_LoadBMP ("sprites_mario_sokoban/wall.bmp");
    SDL_BlitSurface(colleimagecrea , NULL, ecran, &posiclic );
    }
    if(cursus == 5)
    {tableaucrea[cases(posiclic)-1] =4;
    colleimagecrea= SDL_LoadBMP ("sprites_mario_sokoban/target.bmp");
    SDL_BlitSurface(colleimagecrea , NULL, ecran, &posiclic );
    }
    if(cursus == 6)
    {tableaucrea[cases(posiclic)-1] =2;
    colleimagecrea= SDL_CreateRGBSurface(SDL_HWSURFACE, 68, 68, 16, 0, 0,0, 0);//on charge un carré blanc à l'ancien emplacement DE L'OBJET
    SDL_FillRect(colleimagecrea, NULL, SDL_MapRGB(ecran->format, 255,255,255));
    SDL_BlitSurface(colleimagecrea , NULL, ecran, &posiclic );
    }

    }
    else
    {

        if(cursus == 2)
    {tableaucrea[cases(posiclic)-1] =3;
    posiclic.y +=34;
    colleimagecrea= SDL_LoadBMP ("sprites_mario_sokoban/crate_editor.bmp");
    SDL_BlitSurface(colleimagecrea , NULL, ecran, &posiclic );
    posiclic.y -=34;
    }
    if(cursus == 3)
    {
     pass = 0;
     while(pass < 80)
     {
         if(tableaucrea[pass] == 5)
         {
             pass = 100;
         }
         pass ++;
     }
     if(pass <100)
    {tableaucrea[cases(posiclic)-1] =5;
    posiclic.y +=34;
    colleimagecrea= SDL_LoadBMP ("sprites_mario_sokoban/keeper_editor.bmp");
    SDL_BlitSurface(colleimagecrea , NULL, ecran, &posiclic );
    posiclic.y -=34;
    }
    }
    if(cursus == 4)
    {tableaucrea[cases(posiclic)-1] =1;
    posiclic.y +=34;
    colleimagecrea= SDL_LoadBMP ("sprites_mario_sokoban/wall_editor.bmp");
    SDL_BlitSurface(colleimagecrea , NULL, ecran, &posiclic );
    posiclic.y -=34;
    }
    if(cursus == 5)
    {tableaucrea[cases(posiclic)-1] =4;
    posiclic.y +=34;
    colleimagecrea= SDL_LoadBMP ("sprites_mario_sokoban/target_editor.bmp");
    SDL_BlitSurface(colleimagecrea , NULL, ecran, &posiclic );
    posiclic.y -=34;
    }
    if(cursus == 6)
    {tableaucrea[cases(posiclic)-1] =2;
    posiclic.y +=34;
    colleimagecrea= SDL_CreateRGBSurface(SDL_HWSURFACE, 68, 34, 16, 0, 0,0, 0);//on charge un carré blanc à l'ancien emplacement DE L'OBJET
    SDL_FillRect(colleimagecrea, NULL, SDL_MapRGB(ecran->format, 255,255,255));
    SDL_BlitSurface(colleimagecrea , NULL, ecran, &posiclic );
    posiclic.y -=34;
    }
    }
arrete = 0;
    positionlignes.y = 0;
    positionlignes.x = 0;
    lignesvar = 0;
     while(arrete < 8)
    {

        lignes[lignesvar] = SDL_CreateRGBSurface(SDL_HWSURFACE, 680, 1, 16, 0, 0,0, 0);//dimension et mode de couleur du rectangle
        SDL_FillRect(lignes[lignesvar], NULL, SDL_MapRGB(ecran->format,0,0, 0));//couleur du rectangle

        SDL_BlitSurface(lignes[lignesvar], NULL, ecran, &positionlignes); // Collage

        positionlignes.y += 68;
        lignesvar++;
        arrete++;

    }
    positionlignes.y = 34;
    positionlignes.x = 68;//544 mais en longeur(première ligne)
    colleimagecrea= SDL_CreateRGBSurface(SDL_HWSURFACE, 544, 1, 16, 0, 0,0, 0);//dimension et mode de couleur du rectangle
    SDL_FillRect(colleimagecrea, NULL, SDL_MapRGB(ecran->format,0,0, 0));//couleur du rectangle
    SDL_BlitSurface(colleimagecrea, NULL, ecran, &positionlignes); // Collage
    arrete = 0;
    positionlignes.y = 0;
    positionlignes.x = 0;
    lignesvar = 0;
     while(arrete < 10)
    {
        positionlignes.x += 68;
        lignes[lignesvar] = SDL_CreateRGBSurface(SDL_HWSURFACE, 1, 544, 16, 0, 0,0, 0);//dimension et mode de couleur du rectangle
        SDL_FillRect(lignes[lignesvar], NULL, SDL_MapRGB(ecran->format,0,0, 0));//couleur du rectangle

        SDL_BlitSurface(lignes[lignesvar], NULL, ecran, &positionlignes); // Collage
        lignesvar++;
        arrete++;

    }
    posicolleimagecrea.x =68*7;
    colleimagecrea= SDL_LoadBMP ("sprites_mario_sokoban/play.bmp");
    SDL_BlitSurface(colleimagecrea , NULL, ecran, &posicolleimagecrea );




    }
    SDL_Flip(ecran);
nombfois++;

    }while(quita == 0 && quit == 0);
     arrete = 0;
     lignesvar = 0;
    }
    }
    }while((clicniv >9 && quit == 0 && jouer == 2)||(clicniv >9 && quit == 0 && jouer == 3) );
}while(debutcrea == 1 && quit == 0 && jouer != 4);
}
}



    TTF_Quit();//quitte la biblio texte
    SDL_FreeSurface(ecran_partie);
    SDL_FreeSurface(ecran_dev);
    SDL_FreeSurface(image_demarrage);
    SDL_FreeSurface(mario);
    SDL_FreeSurface(caisse);
    SDL_FreeSurface(replay);
    SDL_FreeSurface(retour);
    SDL_FreeSurface(rectangle);
    SDL_FreeSurface(victoireyou);
    SDL_FreeSurface(texte);
    SDL_FreeSurface(colleimagecrea);
    int net = 0;
    while(net<18)
    {
        SDL_FreeSurface(lignes[net]);
        net++;
    }
    SDL_Quit();//quitte la SDL

  return EXIT_SUCCESS;


}
void pause()
{
    int continuer = 1;
    SDL_Event event;
    while (continuer)
    {
        SDL_WaitEvent(&event);
        switch(event.type)
        {
        case SDL_QUIT:
            continuer = 0;
        }
    }
}
void choixniv(SDL_Surface* ecran)
{

    TTF_Init();//démarrage de la bibliothèque texte
    SDL_Surface *dragon = NULL;


    SDL_Color couleurindigo = {10, 206, 212};
    SDL_Color couleurverte = {128,255,23};
    SDL_Color couleurrouge = {255,0,0};
    SDL_Color couleurviolette = {165,32,175};
    SDL_Color couleurorange = {255,123,0};
    SDL_Color couleurjaune = {255,233,0};
    SDL_Color couleurvertepomme = {177,255,23};
    SDL_Color couleurbleumarine = {0,0,255};
    SDL_Color couleurblanche = {255,255,255};
    SDL_Color couleurNoire = {0, 0, 0};


    TTF_Font *police = NULL;
    SDL_Surface *texte = NULL;
    SDL_Rect positexte;
    SDL_Rect dposition;
dposition.x = 0;
dposition.y = 0;


    police = TTF_OpenFont("NotoSans.ttf", 62);

    dragon = SDL_LoadBMP ("sprites_mario_sokoban/garden_detail.bmp");
    SDL_BlitSurface(dragon , NULL, ecran, &dposition );
    SDL_Flip(ecran);
    positexte.x = 10;
    positexte.y = 50;
    texte = TTF_RenderText_Blended(police, "CHOISIS TON NIVEAU....",couleurorange);
    SDL_BlitSurface(texte, NULL, ecran, &positexte); /* Blit du texte par-dessus */

    /*rectangle = SDL_CreateRGBSurface(SDL_HWSURFACE, 20, 50, 32, 0, 0,0, 0);
    SDL_FillRect(rectangle, NULL, SDL_MapRGB(ecran->format, 255, 255, 255)); // Remplissage de la surface avec du blanc
    SDL_BlitSurface(rectangle, NULL, ecran, &position);
PERMET DE CRER UN RECTANGLE DE DIMENSION VOULUE*/
    positexte.x =120;
    positexte.y = 150;
    texte = TTF_RenderText_Shaded(police, " 1 ",couleurindigo,couleurbleumarine);
    SDL_BlitSurface(texte, NULL, ecran, &positexte); /* Blit du texte par-dessus */
    //SDL_FillRect(ecran, 0, SDL_MapRGB(personne, 0, 0, 0)); A CHANGER CE SERVIR DU COURS

    positexte.x =285;
    positexte.y = 150;
    texte = TTF_RenderText_Shaded(police, " 2 ",couleurrouge,couleurjaune);
    SDL_BlitSurface(texte, NULL, ecran, &positexte); /* Blit du texte par-dessus */

    positexte.x =450;
    positexte.y = 150;
    texte = TTF_RenderText_Shaded(police, " 3 ",couleurjaune,couleurrouge);
    SDL_BlitSurface(texte, NULL, ecran, &positexte); /* Blit du texte par-dessus */

    positexte.x =120;
    positexte.y = 250;
    texte = TTF_RenderText_Shaded(police, " 4 ",couleurviolette,couleurvertepomme);
    SDL_BlitSurface(texte, NULL, ecran, &positexte); /* Blit du texte par-dessus */

    positexte.x =285;
    positexte.y = 250;
    texte = TTF_RenderText_Shaded(police, " 5 ",couleurverte,couleurviolette);
    SDL_BlitSurface(texte, NULL, ecran, &positexte); /* Blit du texte par-dessus */

    positexte.x =450;
    positexte.y = 250;
    texte = TTF_RenderText_Shaded(police, " 6 ",couleurblanche,couleurNoire);
    SDL_BlitSurface(texte, NULL, ecran, &positexte); /* Blit du texte par-dessus */

    TTF_CloseFont(police);
    police = TTF_OpenFont("NotoSans.ttf", 55);
    positexte.x =120;
    positexte.y = 350;
    texte = TTF_RenderText_Shaded(police, " VOS NIVEAUX ",couleurrouge,couleurjaune);
    SDL_BlitSurface(texte, NULL, ecran, &positexte); /* Blit du texte par-dessus */


     SDL_Flip(ecran);
     SDL_FreeSurface(texte);
     SDL_FreeSurface(dragon);
     TTF_CloseFont(police); /* Doit être avant TTF_Quit() */
    TTF_Quit();//quitte la biblio texte

}
void load(SDL_Surface *ecran, int niveau,int tableau1[])
{
    int a = 1,i = 0;
    SDL_Surface *carre = NULL;
    SDL_Rect posicarre;
    posicarre.x = 0;
    posicarre.y = 0;
    SDL_Surface *rectangle = NULL;//sert à créer un pointeur sur rectangle
    rectangle = SDL_CreateRGBSurface(SDL_HWSURFACE, 680, 544, 16, 0, 0,0, 0);//dimension et mode de couleur du rectangle
    SDL_FillRect(rectangle, NULL, SDL_MapRGB(ecran->format, 255,255,255));//couleur du rectangle

    SDL_Rect position;
    position.x = 0;
    position.y = 0;

    SDL_BlitSurface(rectangle, NULL, ecran, &position);//colle le rectangle à l'écran(0 parce que l'on ne sert pas de ces paramètres

    SDL_Flip(ecran); // Mise à jour de l'écran



        while(a <6)
        {
            while(i < 80)
            {
                if(tableau1[i] ==a)
                {

                        if(i <10)
                        {
                            posicarre.x = i*68;
                            posicarre.y = 0;
                        }
                        else if(i>=10 && i<20)
                        {
                            i -=10;
                            posicarre.x = i*68;
                            posicarre.y = 68;
                            i +=10;
                        }
                        else if(i <30 && i>=20)
                        {
                            i -=20;
                            posicarre.x = i*68;
                            posicarre.y = 68*2;
                            i +=20;
                        }
                        else if(i <40 && i>=30)
                        {
                            i -=30;
                            posicarre.x = i*68;
                            posicarre.y = 68*3;
                            i +=30;
                        }
                        else if(i <50 && i>=40)
                        {
                            i -=40;
                            posicarre.x = i*68;
                            posicarre.y = 68*4;
                            i +=40;
                        }
                        else if(i <60 && i>=50)
                        {
                            i -=50;
                            posicarre.x = i*68;
                            posicarre.y = 68*5;
                            i +=50;
                        }
                        else if(i <70 && i>=60)
                        {
                            i -=60;
                            posicarre.x = i*68;
                            posicarre.y = 68*6;
                            i +=60;
                        }
                        else if(i <80 && i>=70)
                        {
                            i -=70;
                            posicarre.x = i*68;
                            posicarre.y = 68*7;
                            i +=70;
                        }
                    if( a == 1)
                    {

                        carre = SDL_LoadBMP ("sprites_mario_sokoban/wall.bmp");
                         SDL_BlitSurface(carre , NULL, ecran, &posicarre );


                    }

                    if( a == 3)
                    {

                        carre = SDL_LoadBMP ("sprites_mario_sokoban/crate.bmp");
                         SDL_BlitSurface(carre , NULL, ecran, &posicarre );


                    }
                    if( a == 4)
                    {

                        carre = SDL_LoadBMP ("sprites_mario_sokoban/target.bmp");
                         SDL_BlitSurface(carre , NULL, ecran, &posicarre );


                    }
                    if( a == 5)
                    {

                        carre = SDL_LoadBMP ("sprites_mario_sokoban/keeper_up.bmp");
                         SDL_BlitSurface(carre , NULL, ecran, &posicarre );


                    }

                }
                i++;
            }
            a++;
            i = 0;
        }
SDL_FreeSurface(rectangle);
SDL_FreeSurface(carre);
SDL_Flip(ecran);

}
int tableau(int niveau, int fois)
{
     int a = 1;
    if(niveau == 1)
    {int tableau1[80]={1,1,1,1,1,1,1,1,1,1,1,1,1,1,4,2,1,1,1,1,1,4,2,2,3,2,1,1,1,1,1,1,2,2,3,4,1,1,1,1,1,1,2,3,2,2,1,1,1,1,1,1,1,5,2,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};
    return tableau1[fois];}
    else if(niveau == 2)
    {int tableau1[80]={1,1,1,1,1,1,1,1,1,1,1,5,2,2,1,1,1,1,1,1,1,2,3,3,1,1,1,1,1,1,1,2,3,2,1,1,1,1,1,1,1,1,1,2,1,1,1,4,4,4,1,1,1,2,2,2,2,2,2,2,1,2,2,2,2,1,2,2,2,2,4,3,2,2,2,1,1,1,1,1};
    return tableau1[fois];}
    else if(niveau == 3)
    {int tableau1[80]={1,1,2,2,2,2,2,1,1,1,1,1,3,1,1,1,2,2,2,1,2,5,2,2,3,2,2,3,2,1,2,2,2,2,1,2,3,2,1,1,4,4,4,4,1,2,2,2,1,1,1,1,1,1,4,1,1,2,1,1,1,1,2,2,3,1,2,3,2,2,1,1,4,2,2,3,2,2,2,4};
        return tableau1[fois];
    }
    else if(niveau == 4)
    {int tableau1[80]={1,1,1,1,1,1,4,2,2,2,1,1,2,2,1,4,3,2,3,2,1,5,3,2,1,4,3,2,1,2,1,1,3,2,1,2,2,1,2,2,1,1,2,3,2,1,2,2,3,2,1,4,3,2,2,1,1,1,1,2,1,4,2,3,2,2,2,2,2,2,1,1,4,4,4,1,2,2,2,4};
    return tableau1[fois];}
    else if(niveau == 5)
    {int tableau1[80]={1,1,1,2,2,1,2,4,1,1,1,2,2,3,2,1,2,2,1,1,1,5,2,1,2,1,2,2,1,1,1,4,2,2,2,2,2,3,2,1,1,1,1,1,2,1,2,2,1,1,1,4,2,2,3,1,2,2,1,1,1,2,3,1,2,1,2,2,1,1,4,2,2,3,2,2,2,4,1,1};
    return tableau1[fois];}
    else if(niveau == 6)
    {int tableau1[80]={2,2,2,2,2,2,2,4,1,2,1,2,2,1,1,1,1,1,1,1,1,1,3,2,3,2,2,4,1,1,2,2,2,3,2,2,4,1,1,1,2,2,3,2,3,2,1,1,1,1,1,1,2,2,1,2,1,1,1,1,1,4,2,2,4,2,2,2,5,1,1,1,1,1,1,1,1,1,1,1};
    return tableau1[fois];}

    return a;

}
int verif(int niveau, SDL_Rect posimario, int tableau[],int direct)
{
    int i= 0,bidon = 0;

       // int tableau1[80]={1,1,1,1,1,1,1,1,1,1,1,1,1,1,4,2,1,1,1,1,1,4,2,2,3,2,1,1,1,1,1,1,2,2,3,4,1,1,1,1,1,1,2,3,2,2,1,1,1,1,1,1,1,5,2,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};
        i = cases(posimario);
        i--;
//POUR LES AUTRES NIVEAU REMPLIR TABLEAU1 PAR UNE BOUCLE COMME DANS LOAD(EN UTILISANT LA FONCTION TABLEAU)
            if(posimario.x >= 0 && posimario.x < 680 && posimario.y >= 0 && posimario.y < 544 )
            bidon = 1;
            else
            return 1;
            if(posimario.y == 476 && tableau[i]==3 && direct == 4)
            return 1;
            if(posimario.y == 0 && tableau[i]==3 && direct == 1)
            return 1;
            if(posimario.x == 0 && tableau[i]==3 && direct == 3)
            return 1;
            if(posimario.y == 612 && tableau[i]==3 && direct == 2)
            return 1;

            if(tableau[i] == 1 || tableau[i] == 4)//empêche le passage(mur ou objectif)
            return 1;


            if(tableau[i] == 3 && tableau[i - 10] ==1  && direct == 1)//caisse poussée contre un mur
            return 1;
            if(tableau[i] == 3 && tableau[i - 1] ==1 && direct == 3 )//caisse poussée contre un mur
            return 1;
            if(tableau[i] == 3 && tableau[i + 1] ==1 && direct == 2 )//caisse poussée contre un mur
            return 1;
            if(tableau[i] == 3 && tableau[i + 10] ==1 && direct == 4 )//caisse poussée contre un mur
            return 1;

             if(tableau[i] == 3 && tableau[i - 10] ==3  && direct == 1)//caisse poussée contre une autre caisse
            return 1;
            if(tableau[i] == 3 && tableau[i - 1] ==3 && direct == 3 )//caisse poussée contre une autre caisse
            return 1;
            if(tableau[i] == 3 && tableau[i + 1] ==3 && direct == 2 )//caisse poussée contre une autre caisse
            return 1;
            if(tableau[i] == 3 && tableau[i + 10] ==3 && direct == 4 )//caisse poussée contre une autre caisse
            return 1;

            if(tableau[i] == 3 && tableau[i - 10] ==6  && direct == 1)//caisse poussée contre une autre caisse
            return 1;
            if(tableau[i] == 3 && tableau[i - 1] ==6 && direct == 3 )//caisse poussée contre une autre caisse
            return 1;
            if(tableau[i] == 3 && tableau[i + 1] ==6 && direct == 2 )//caisse poussée contre une autre caisse
            return 1;
            if(tableau[i] == 3 && tableau[i + 10] ==6 && direct == 4 )//caisse poussée contre une autre caisse
            return 1;

            if(tableau[i] == 3 && tableau[i - 10] ==4  && direct == 1)//caisse poussée sur objectif
            return 4;
            if(tableau[i] == 3 && tableau[i - 1] ==4 && direct == 3 )//caisse poussée sur objectif
            return 4;
            if(tableau[i] == 3 && tableau[i + 1] ==4 && direct == 2 )//caisse poussée sur objectif
            return 4;
            if(tableau[i] == 3 && tableau[i + 10] ==4 && direct == 4 )//caisse poussée sur objectif
            return 4;


            if(tableau[i] == 3 && tableau[i - 10] ==2  && direct == 1)//caisse poussée normalement
            return 2;
            if(tableau[i] == 3 && tableau[i - 1] ==2 && direct == 3 )//caisse poussée normalement
            return 2;
            if(tableau[i] == 3 && tableau[i + 1] ==2 && direct == 2 )//caisse poussée normalement
            return 2;
            if(tableau[i] == 3 && tableau[i + 10] ==2 && direct == 4 )//caisse poussée normalement
            return 2;

            if(tableau[i] == 3 && tableau[i - 10] ==5  && direct == 1)//caisse poussée normalement(sur mario)
            return 2;
            if(tableau[i] == 3 && tableau[i - 1] ==5 && direct == 3 )//caisse poussée normalement
            return 2;
            if(tableau[i] == 3 && tableau[i + 1] ==5 && direct == 2 )//caisse poussée normalement
            return 2;
            if(tableau[i] == 3 && tableau[i + 10] ==5 && direct == 4 )//caisse poussée normalement
            return 2;

            if(tableau[i] == 6)//caisse ok
            return 1;
            if(tableau[i] == 6  )//caisse ok
            return 1;
            if(tableau[i] == 6  )//caisse ok
            return 1;
            if(tableau[i] == 6  )//caisse ok(empeche de re bouger la caisse)
            return 1;
            /*/if(tableau[i] == 3 && tableau[i - 10] ==3)//empêche deux caisse d'affillé poussées vers un mur
            return 1;
            if(tableau[i] == 3 && tableau[i - 10] ==1)//empêche une caisse poussée vers un mur
            return 1;*/ //A compléter 8 position

    return 0;

}
int casesmodif(SDL_Rect posimario)
{
    /* Pour une autre fois il existe un moyen de réduire les opération logiques à 18 au pire
    (donc moins de code et meilleurs efficacité(optimise)),pour cela on détermine d'abord l'axe des y
    (if(posi.y>=0 && posi.y <68)posi.y = 0) en procédant par colonne (10).
    Puis on détermine x par ligne colonne et lignes se croise on obtient les coordonnés
    sur le principe de la fonction posicases*/




    if(posimario.y > 0 && posimario.y<68)
    {

        if(posimario.x >= 0 &&posimario.x<68 )
        return 1;
        if(posimario.x >= 68 &&posimario.x<=68*2)
        return 2;
        if(posimario.x >= 68*2 && posimario.x<68*3)
        return 3;
        if(posimario.x >= 68*3 && posimario.x <68*4)
        return 4;
        if(posimario.x >= 68*4 && posimario.x <68*5)
        return 5;
        if(posimario.x >= 68*5 && posimario.x <68*6)
        return 6;
        if(posimario.x >= 68*6 && posimario.x <68*7)
        return 7;
        if(posimario.x >= 68*7 && posimario.x <68*8)
        return 8;
        if(posimario.x >= 68*8 && posimario.x <68*9)
        return 9;
        if(posimario.x >= 68*9 && posimario.x <68*10)
        return 10;

    }
    if(posimario.y >= 68 && posimario.y<68*2)
    {
        if(posimario.x >= 0 && posimario.x <68)
        return 11;
        if(posimario.x >= 68 && posimario.x <68*2)
        return 12;
        if(posimario.x >= 68*2 && posimario.x <68*3)
        return 13;
        if(posimario.x >= 68*3 && posimario.x <68*4)
        return 14;
        if(posimario.x >= 68*4 && posimario.x <68*5)
        return 15;
        if(posimario.x >= 68*5 && posimario.x <68*6)
        return 16;
        if(posimario.x >= 68*6 && posimario.x <68*7)
        return 17;
        if(posimario.x >= 68*7 && posimario.x <68*8)
        return 18;
        if(posimario.x >= 68*8 && posimario.x <68*9)
        return 19;
        if(posimario.x >= 68*9 && posimario.x <68*10)
        return 20;

    }
    if(posimario.y >= 68*2 && posimario.y<68*3)
    {
        if(posimario.x >= 0 && posimario.x <68)
        return 21;
        if(posimario.x >= 68 && posimario.x <68*2)
        return 22;
        if(posimario.x >= 68*2 && posimario.x <68*3)
        return 23;
        if(posimario.x >= 68*3 && posimario.x <68*4)
        return 24;
        if(posimario.x >= 68*4 && posimario.x <68*5)
        return 25;
        if(posimario.x >= 68*5 && posimario.x <68*6)
        return 26;
        if(posimario.x >= 68*6 && posimario.x <68*7)
        return 27;
        if(posimario.x >= 68*7 && posimario.x <68*8)
        return 28;
        if(posimario.x >= 68*8 && posimario.x <68*9)
        return 29;
        if(posimario.x >= 68*9 && posimario.x <68*10)
        return 30;

    }
    if(posimario.y >= 68*3 && posimario.y<68*4)
    {
        if(posimario.x >= 0 && posimario.x <68)
        return 31;
        if(posimario.x >= 68 && posimario.x <68*2)
        return 32;
        if(posimario.x >= 68*2 && posimario.x <68*3)
        return 33;
        if(posimario.x >= 68*3 && posimario.x <68*4)
        return 34;
        if(posimario.x >= 68*4 && posimario.x <68*5)
        return 35;
        if(posimario.x >= 68*5 && posimario.x <68*6)
        return 36;
        if(posimario.x >= 68*6 && posimario.x <68*7)
        return 37;
        if(posimario.x >= 68*7 && posimario.x <68*8)
        return 38;
        if(posimario.x >= 68*8 && posimario.x <68*9)
        return 39;
        if(posimario.x >= 68*9 && posimario.x <68*10)
        return 40;

    }
    if(posimario.y >=68*4 && posimario.y<68*5)
    {
        if(posimario.x >= 0 && posimario.x <68)
        return 41;
        if(posimario.x >= 68 && posimario.x <68*2)
        return 42;
        if(posimario.x >= 68*2 && posimario.x <68*3)
        return 43;
        if(posimario.x >= 68*3 && posimario.x <68*4)
        return 44;
        if(posimario.x >= 68*4 && posimario.x <68*5)
        return 45;
        if(posimario.x >= 68*5 && posimario.x <68*6)
        return 46;
        if(posimario.x >= 68*6 && posimario.x <68*7)
        return 47;
        if(posimario.x >= 68*7 && posimario.x <68*8)
        return 48;
        if(posimario.x >= 68*8 && posimario.x <68*9)
        return 49;
        if(posimario.x >= 68*9 && posimario.x <68*10)
        return 50;

    }
    if(posimario.y >= 68*5 && posimario.y<68*6)
    {
        if(posimario.x >= 0 && posimario.x <68)
        return 51;
        if(posimario.x >= 68 && posimario.x <68*2)
        return 52;
        if(posimario.x >= 68*2 && posimario.x <68*3)
        return 53;
        if(posimario.x >= 68*3 && posimario.x <68*4)
        return 54;
        if(posimario.x >= 68*4 && posimario.x <68*5)
        return 55;
        if(posimario.x >= 68*5 && posimario.x <68*6)
        return 56;
        if(posimario.x >= 68*6 && posimario.x <68*7)
        return 57;
        if(posimario.x >= 68*7 && posimario.x <68*8)
        return 58;
        if(posimario.x >= 68*8 && posimario.x <68*9)
        return 59;
        if(posimario.x >= 68*9 && posimario.x <68*10)
        return 60;

    }
    if(posimario.y >= 68*6 && posimario.y<68*7)
    {
        if(posimario.x >= 0 && posimario.x <68)
        return 61;
        if(posimario.x >= 68 && posimario.x <68*2)
        return 62;
        if(posimario.x >= 68*2 && posimario.x <68*3)
        return 63;
        if(posimario.x >= 68*3 && posimario.x <68*4)
        return 64;
        if(posimario.x >= 68*4 && posimario.x <68*5)
        return 65;
        if(posimario.x >= 68*5 && posimario.x <68*6)
        return 66;
        if(posimario.x >= 68*6 && posimario.x <68*7)
        return 67;
        if(posimario.x >= 68*7 && posimario.x <68*8)
        return 68;
        if(posimario.x >= 68*8 && posimario.x <68*9)
        return 69;
        if(posimario.x >= 68*9 && posimario.x <68*10)
        return 70;

    }
    if(posimario.y >= 68*7 && posimario.y<68*8)
    {
        if(posimario.x >= 0 && posimario.x <68)
        return 71;
        if(posimario.x >= 68 && posimario.x <68*2)
        return 72;
        if(posimario.x >= 68*2 && posimario.x <68*3)
        return 73;
        if(posimario.x >= 68*3 && posimario.x <68*4)
        return 74;
        if(posimario.x >= 68*4 && posimario.x <68*5)
        return 75;
        if(posimario.x >= 68*5 && posimario.x <68*6)
        return 76;
        if(posimario.x >= 68*6 && posimario.x <68*7)
        return 77;
        if(posimario.x >= 68*7 && posimario.x <68*8)
        return 78;
        if(posimario.x >= 68*8 && posimario.x <68*9)
        return 79;
        if(posimario.x >= 68*9 && posimario.x <68*10)
        return 80;

    }
    return 0;



}

int victoire(SDL_Surface* ecran)
{

    TTF_Init();//démarrage de la bibliothèque texte
    SDL_Color couleurindigo = {10, 206, 212};
    SDL_Color couleurrouge = {255,0,0};
    SDL_Color couleurviolette = {165,32,175};
    SDL_Color couleurorange = {255,123,0};
    SDL_Color couleurrose = {255,81,255};
    SDL_Color couleurjaune = {255,233,0};
    SDL_Color couleurvertepomme = {177,255,23};
    SDL_Color couleurbleumarine = {0,0,255};
    SDL_Color couleurblanche = {255,255,255};

    TTF_Font *police = NULL;
    SDL_Surface *texte = NULL;
    SDL_Rect positexte;
    positexte.x = 0;
    positexte.y = 0;
     police = TTF_OpenFont("NotoSans.ttf", 155);

     positexte.x = 0;
    positexte.y = 30;
    texte = TTF_RenderText_Blended(police, "V",couleurrouge);
    SDL_BlitSurface(texte, NULL, ecran, &positexte); /* Blit du texte par-dessus */
    positexte.x = 110;
    texte = TTF_RenderText_Blended(police, "I",couleurrose);
    SDL_BlitSurface(texte, NULL, ecran, &positexte); /* Blit du texte par-dessus */
    positexte.x = 150;
    texte = TTF_RenderText_Blended(police, "C",couleurorange);
    SDL_BlitSurface(texte, NULL, ecran, &positexte); /* Blit du texte par-dessus */
    positexte.x = 235;
    texte = TTF_RenderText_Blended(police, "T",couleurjaune);
    SDL_BlitSurface(texte, NULL, ecran, &positexte); /* Blit du texte par-dessus */
    positexte.x = 330;
    texte = TTF_RenderText_Blended(police, "O",couleurvertepomme);
    SDL_BlitSurface(texte, NULL, ecran, &positexte); /* Blit du texte par-dessus */
    positexte.x = 420;
    texte = TTF_RenderText_Blended(police, "I",couleurindigo);
    SDL_BlitSurface(texte, NULL, ecran, &positexte); /* Blit du texte par-dessus */
    positexte.x = 480;
    texte = TTF_RenderText_Blended(police, "R",couleurbleumarine);
    SDL_BlitSurface(texte, NULL, ecran, &positexte); /* Blit du texte par-dessus */
    positexte.x = 580;
    texte = TTF_RenderText_Blended(police, "E",couleurviolette);
    SDL_BlitSurface(texte, NULL, ecran, &positexte); /* Blit du texte par-dessus */

    TTF_CloseFont(police); /* Doit être avant TTF_Quit() */
    police = TTF_OpenFont("NotoSans.ttf", 75);

    positexte.x = 100;
    positexte.y = 200;
    texte = TTF_RenderText_Shaded(police, "Niveau",couleurorange,couleurbleumarine);
    SDL_BlitSurface(texte, NULL, ecran, &positexte);
    positexte.x = 400;
    positexte.y = 200;
    texte = TTF_RenderText_Shaded(police, "Rejouer",couleurindigo,couleurblanche);
    SDL_BlitSurface(texte, NULL, ecran, &positexte);
    positexte.x = 255;
    positexte.y = 280;
    texte = TTF_RenderText_Shaded(police, "Menu",couleurviolette,couleurrouge);
    SDL_BlitSurface(texte, NULL, ecran, &positexte);



    SDL_Flip(ecran);


    SDL_Rect posiclic;
    posiclic.x = 0;
    posiclic.y = 0;
     int continuer = 1,niveau = 0;
     SDL_Event event;

    while(continuer)
      {
          SDL_WaitEvent(&event);
      switch(event.type)
      {
      case SDL_QUIT:
      continuer = 0;

       break;
       case SDL_MOUSEBUTTONUP: /* Clic de la souris */
       posiclic.x = event.button.x; /* change les coordonnées de posiclic*/
       posiclic.y = event.button.y;
       niveau=choixniveau(posiclic);
       if(niveau != 0)
       {continuer = 0;

       }
       else
       {
           continuer = 1;

       }
       break;
      }
      }


    SDL_FreeSurface(texte);
    TTF_CloseFont(police); /* Doit être avant TTF_Quit() */
    TTF_Quit();//quitte la biblio texte
    return niveau;

}
int choixniveau(SDL_Rect posiclic)
{

    if(posiclic.x >= 100 && posiclic.x <= 265 && posiclic.y >= 200 && posiclic.y <= 280)
    return 1;
    if(posiclic.x >= 400 && posiclic.x <=580  && posiclic.y >= 200 && posiclic.y <= 280)
    return 2;
    if(posiclic.x >= 255 && posiclic.x <= 409 && posiclic.y >= 280 && posiclic.y <=360 )
    return 3;


    return 0;


}
void affvict(SDL_Surface* ecran,SDL_Rect posivict, int fois,SDL_Surface* affiche)
{
        TTF_Init();//démarrage de la bibliothèque texte
        TTF_Font *police = NULL;
    SDL_Color couleurbleulagon = {11,227,206};
    SDL_Color couleurnoirenuance = {51,51,51};
    posivict.x = 0;
    posivict.y = 470;
    police = TTF_OpenFont("NotoSans.ttf", 35);
    /* afficher You Win:*/
    affiche = TTF_RenderText_Blended(police, " YOU WIN:",couleurnoirenuance);
    SDL_BlitSurface(affiche, NULL, ecran, &posivict);

    if(fois == 1)
    {
         posivict.x = 300;
          affiche = TTF_RenderText_Blended(police, "1",couleurbleulagon);
         SDL_BlitSurface(affiche, NULL, ecran, &posivict);

    }
    if(fois == 2)
    {
         posivict.x = 330;
          affiche = TTF_RenderText_Blended(police, "2",couleurbleulagon);
         SDL_BlitSurface(affiche, NULL, ecran, &posivict);

    }
    if(fois == 3)
    {
         posivict.x = 380;
          affiche = TTF_RenderText_Blended(police, "3",couleurbleulagon);
         SDL_BlitSurface(affiche, NULL, ecran, &posivict);

    }
    if(fois == 4)
    {
         posivict.x = 430;
          affiche = TTF_RenderText_Blended(police, "4",couleurbleulagon);
         SDL_BlitSurface(affiche, NULL, ecran, &posivict);

    }
    if(fois == 5)
    {
         posivict.x = 470;
          affiche = TTF_RenderText_Blended(police, "5",couleurbleulagon);
         SDL_BlitSurface(affiche, NULL, ecran, &posivict);

    }
    if(fois == 6)
    {
         posivict.x = 510;
          affiche = TTF_RenderText_Blended(police, "6",couleurbleulagon);
         SDL_BlitSurface(affiche, NULL, ecran, &posivict);

    }


    SDL_Flip(ecran);
      TTF_CloseFont(police); /* Doit être avant TTF_Quit() */
    TTF_Quit();//quitte la biblio texte
}
void save(int niveau)
{
    FILE*score =NULL;
    if(niveau == 1)
    {score = fopen("data/open1.lvl","w+");
    fprintf(score,"%d",1);
    fclose(score);
    }
     if(niveau == 2)
    {score = fopen("data/open2.lvl","w+");
    fprintf(score,"%d",1);
    fclose(score);
    }
     if(niveau == 3)
    {score = fopen("data/open3.lvl","w+");
    fprintf(score,"%d",1);
    fclose(score);
    }
     if(niveau == 4)
    {score = fopen("data/open4.lvl","w+");
    fprintf(score,"%d",1);
    fclose(score);
    }
     if(niveau == 5)
    {score = fopen("data/open5.lvl","w+");
    fprintf(score,"%d",1);
    fclose(score);
    }
     if(niveau == 6)
    {score = fopen("data/open6.lvl","w+");
    fprintf(score,"%d",1);
    fclose(score);
    }

}
void afftrait(int jaune,SDL_Surface* ecran,SDL_Rect positionlignes,SDL_Surface*colleimagecrea )
{
    if(jaune == 1)
    {colleimagecrea= SDL_CreateRGBSurface(SDL_HWSURFACE, 68, 3, 16, 0, 0,0, 0);//dimension et mode de couleur du rectangle
    SDL_FillRect(colleimagecrea, NULL, SDL_MapRGB(ecran->format,255,255, 0));//couleur du rectangle
    SDL_BlitSurface(colleimagecrea, NULL, ecran, &positionlignes); // Collage
    }
    if(jaune==0)
    {
        colleimagecrea= SDL_CreateRGBSurface(SDL_HWSURFACE, 68, 3, 16, 0, 0,0, 0);//dimension et mode de couleur du rectangle
    SDL_FillRect(colleimagecrea, NULL, SDL_MapRGB(ecran->format,255,255,255));//couleur du rectangle
    SDL_BlitSurface(colleimagecrea, NULL, ecran, &positionlignes); // Collage

    }
    SDL_Flip(ecran);
}
int caseposi(int cases,int number)
{
    /* Pour une autre fois il existe un moyen de réduire les opération logiques à 18 au pire
    (donc moins de code et meilleurs efficacité(optimise)),pour cela on détermine d'abord l'axe des y
    (if(posi.y>=0 && posi.y <68)posi.y = 0) en procédant par colonne (10).
    Puis on détermine x par ligne colonne et lignes se croise on obtient les coordonnés
    sur le principe de la fonction posicases*/

    if(number == 1 )
    {
        if(cases <= 10)
        return 0;
        if(cases <= 20)
        return 68;
        if(cases <= 30)
        return 68*2;
        if(cases <= 40)
        return 68*3;
        if(cases <= 50)
        return 68*4;
        if(cases <= 60)
        return 68*5;
        if(cases <= 70)
        return 68*6;
        if(cases <= 80)
        return 68*7;
    }
    else
    {
        if(cases == 1 || cases == 11 || cases == 21 || cases == 31||cases == 41||cases==51||cases==61||cases==71)
        return 0;
        if(cases == 2 || cases == 12 || cases == 22 || cases == 32||cases == 42||cases==52||cases==62||cases==72)
        return 68;
        if(cases == 3 || cases == 13 || cases == 23 || cases == 33||cases == 43||cases==53||cases==63||cases==73)
        return 68*2;
        if(cases == 4 || cases == 14 || cases == 24 || cases == 34||cases == 44||cases==54||cases==64||cases==74)
        return 68*3;
        if(cases == 5 || cases == 15 || cases == 25 || cases == 35||cases == 45||cases==55||cases==65||cases==75)
        return 68*4;
        if(cases == 6 || cases == 16 || cases == 26 || cases == 36||cases == 46||cases==56||cases==66||cases==76)
        return 68*5;
        if(cases == 7 || cases == 17 || cases == 27 || cases == 37||cases == 47||cases==57||cases==67||cases==77)
        return 68*6;
        if(cases == 8 || cases == 18 || cases == 28 || cases == 38||cases == 48||cases==58||cases==68||cases==78)
        return 68*7;
        if(cases == 9 || cases == 19 || cases == 29 || cases == 39||cases == 49||cases==59||cases==69||cases==79)
        return 68*8;
        if(cases == 10 || cases == 20 || cases == 30 || cases == 40||cases == 50||cases==60||cases==70||cases==80)
        return 68*9;
    }
    return 0;
}
int cases(SDL_Rect posimario)
{
    if(posimario.y == 0)
    {

        if(posimario.x == 0)
        return 1;
        if(posimario.x == 68)
        return 2;
        if(posimario.x == 68*2)
        return 3;
        if(posimario.x == 68*3)
        return 4;
        if(posimario.x == 68*4)
        return 5;
        if(posimario.x == 68*5)
        return 6;
        if(posimario.x == 68*6)
        return 7;
        if(posimario.x == 68*7)
        return 8;
        if(posimario.x == 68*8)
        return 9;
        if(posimario.x == 68*9)
        return 10;

    }
    if(posimario.y == 68)
    {
        if(posimario.x == 0)
        return 11;
        if(posimario.x == 68)
        return 12;
        if(posimario.x == 68*2)
        return 13;
        if(posimario.x == 68*3)
        return 14;
        if(posimario.x == 68*4)
        return 15;
        if(posimario.x == 68*5)
        return 16;
        if(posimario.x == 68*6)
        return 17;
        if(posimario.x == 68*7)
        return 18;
        if(posimario.x == 68*8)
        return 19;
        if(posimario.x == 68*9)
        return 20;

    }
    if(posimario.y == 68*2)
    {
        if(posimario.x == 0)
        return 21;
        if(posimario.x == 68*1)
        return 22;
        if(posimario.x == 68*2)
        return 23;
        if(posimario.x == 68*3)
        return 24;
        if(posimario.x == 68*4)
        return 25;
        if(posimario.x == 68*5)
        return 26;
        if(posimario.x == 68*6)
        return 27;
        if(posimario.x == 68*7)
        return 28;
        if(posimario.x == 68*8)
        return 29;
        if(posimario.x == 68*9)
        return 30;

    }
    if(posimario.y == 68*3)
    {
        if(posimario.x == 0)
        return 31;
        if(posimario.x == 68)
        return 32;
        if(posimario.x == 68*2)
        return 33;
        if(posimario.x == 68*3)
        return 34;
        if(posimario.x == 68*4)
        return 35;
        if(posimario.x == 68*5)
        return 36;
        if(posimario.x == 68*6)
        return 37;
        if(posimario.x == 68*7)
        return 38;
        if(posimario.x == 68*8)
        return 39;
        if(posimario.x == 68*9)
        return 40;

    }
    if(posimario.y == 68*4)
    {
        if(posimario.x == 0)
        return 41;
        if(posimario.x == 68)
        return 42;
        if(posimario.x == 68*2)
        return 43;
        if(posimario.x == 68*3)
        return 44;
        if(posimario.x == 68*4)
        return 45;
        if(posimario.x == 68*5)
        return 46;
        if(posimario.x == 68*6)
        return 47;
        if(posimario.x == 68*7)
        return 48;
        if(posimario.x == 68*8)
        return 49;
        if(posimario.x == 68*9)
        return 50;

    }
    if(posimario.y == 68*5)
    {
        if(posimario.x == 0)
        return 51;
        if(posimario.x == 68)
        return 52;
        if(posimario.x == 68*2)
        return 53;
        if(posimario.x == 68*3)
        return 54;
        if(posimario.x == 68*4)
        return 55;
        if(posimario.x == 68*5)
        return 56;
        if(posimario.x == 68*6)
        return 57;
        if(posimario.x == 68*7)
        return 58;
        if(posimario.x == 68*8)
        return 59;
        if(posimario.x == 68*9)
        return 60;

    }
    if(posimario.y == 68*6)
    {
        if(posimario.x == 0)
        return 61;
        if(posimario.x == 68)
        return 62;
        if(posimario.x == 68*2)
        return 63;
        if(posimario.x == 68*3)
        return 64;
        if(posimario.x == 68*4)
        return 65;
        if(posimario.x == 68*5)
        return 66;
        if(posimario.x == 68*6)
        return 67;
        if(posimario.x == 68*7)
        return 68;
        if(posimario.x == 68*8)
        return 69;
        if(posimario.x == 68*9)
        return 70;

    }
    if(posimario.y == 68*7)
    {
        if(posimario.x == 0)
        return 71;
        if(posimario.x == 68)
        return 72;
        if(posimario.x == 68*2)
        return 73;
        if(posimario.x == 68*3)
        return 74;
        if(posimario.x == 68*4)
        return 75;
        if(posimario.x == 68*5)
        return 76;
        if(posimario.x == 68*6)
        return 77;
        if(posimario.x == 68*7)
        return 78;
        if(posimario.x == 68*8)
        return 79;
        if(posimario.x == 68*9)
        return 80;

    }
    return 0;



}
void enregistreniv(int tableau[],int niveaufichier,int eff)
{
    FILE*fichier = NULL;
    int i = 0;
    while(i<80)
    {
        if(tableau[i] == 0)
        tableau[i] = 2;
        i++;
    }
    if(niveaufichier == 1)
    {fichier = fopen("niveaux perso/niveau1.txt", "w+");
    }
    if(niveaufichier == 2)
    fichier = fopen("niveaux perso/niveau2.txt", "w+");
    if(niveaufichier == 3)
    fichier = fopen("niveaux perso/niveau3.txt", "w+");
    if(niveaufichier == 4)
    fichier = fopen("niveaux perso/niveau4.txt", "w+");
    if(niveaufichier == 5)
    fichier = fopen("niveaux perso/niveau5.txt", "w+");
    if(niveaufichier == 6)
    fichier = fopen("niveaux perso/niveau6.txt", "w+");
    if(niveaufichier == 7)
    fichier = fopen("niveaux perso/niveau7.txt", "w+");
    if(niveaufichier == 8)
    fichier = fopen("niveaux perso/niveau8.txt", "w+");
    if(niveaufichier == 9)
    fichier = fopen("niveaux perso/niveau9.txt", "w+");
    if(fichier != NULL && eff == 0)
    fprintf(fichier, " %d",1);
    else if(fichier != NULL && eff == 1)
    fprintf(fichier, " %d",0);
    i = 1;
    while(i<80 && fichier != NULL)
    {fprintf(fichier, " %d", tableau[i]);
    i++;
    }

    fclose(fichier);
}
void lecture(int niveau,int tableauniv[])
{
    FILE* fichier = NULL;
    int i = 0;
    if(niveau == 1)
    {fichier = fopen("niveaux perso/niveau1.txt", "r");
    }
    if(niveau == 2)
    fichier = fopen("niveaux perso/niveau2.txt", "r");
    if(niveau == 3)
    fichier = fopen("niveaux perso/niveau3.txt", "r");
    if(niveau == 4)
    fichier = fopen("niveaux perso/niveau4.txt", "r");
    if(niveau == 5)
    fichier = fopen("niveaux perso/niveau5.txt", "r");
    if(niveau == 6)
    fichier = fopen("niveaux perso/niveau6.txt", "r");
    if(niveau == 7)
    fichier = fopen("niveaux perso/niveau7.txt", "r");
    if(niveau == 8)
    fichier = fopen("niveaux perso/niveau8.txt", "r");
    if(niveau == 9)
    fichier = fopen("niveaux perso/niveau9.txt", "r");
    if(fichier != NULL)
    {while(i<80)
    {fscanf(fichier, " %d" , &tableauniv[i]);//(ou &)
    i++;
    }
    }
}

int niveaucrea(SDL_Surface* ecran,SDL_Rect posiclic)
{
    TTF_Init();//démarrage de la bibliothèque texte

    SDL_Color couleurindigo = {10, 206, 212};
    SDL_Color couleurverte = {128,255,23};
    SDL_Color couleurrouge = {255,0,0};
    SDL_Color couleurviolette = {165,32,175};
    SDL_Color couleurorange = {255,123,0};
    SDL_Color couleurjaune = {255,233,0};
    SDL_Color couleurvertepomme = {177,255,23};
    SDL_Color couleurbleumarine = {0,0,255};
    SDL_Color couleurblanche = {255,255,255};
    SDL_Color couleurNoire = {0, 0, 0};
    SDL_Color couleurbleusature = {132,255,255};
    SDL_Color couleurbleuneutre = {0,204,255};
    SDL_Color couleurvertecremeux = {136,204,0};
    TTF_Font *police = NULL;

    SDL_Event event;

    police = TTF_OpenFont("NotoSans.ttf", 100);
    SDL_Surface *texte = NULL;
    SDL_Rect positexte;
    SDL_Rect niceposition;
niceposition.x = 0;
niceposition.y = 0;
    texte = SDL_LoadBMP ("sprites_mario_sokoban/garden.bmp");//image de fond
    SDL_BlitSurface(texte, NULL, ecran, &niceposition);

    texte = SDL_LoadBMP ("sprites_mario_sokoban/back.bmp");// flèche de retour
    SDL_BlitSurface(texte , NULL, ecran, &niceposition );

    positexte.x =120;
    positexte.y = 50;
    texte = TTF_RenderText_Shaded(police, " 1 ",couleurindigo,couleurbleumarine);
    SDL_BlitSurface(texte, NULL, ecran, &positexte);

    positexte.x =285;
    positexte.y = 50;
    texte = TTF_RenderText_Shaded(police, " 2 ",couleurrouge,couleurjaune);
    SDL_BlitSurface(texte, NULL, ecran, &positexte); /* Blit du texte par-dessus */

    positexte.x =450;
    positexte.y = 50;
    texte = TTF_RenderText_Shaded(police, " 3 ",couleurjaune,couleurrouge);
    SDL_BlitSurface(texte, NULL, ecran, &positexte); /* Blit du texte par-dessus */

    positexte.x =120;
    positexte.y = 250;
    texte = TTF_RenderText_Shaded(police, " 4 ",couleurviolette,couleurvertepomme);
    SDL_BlitSurface(texte, NULL, ecran, &positexte); /* Blit du texte par-dessus */

    positexte.x =285;
    positexte.y = 250;
    texte = TTF_RenderText_Shaded(police, " 5 ",couleurverte,couleurviolette);
    SDL_BlitSurface(texte, NULL, ecran, &positexte); /* Blit du texte par-dessus */

    positexte.x =450;
    positexte.y = 250;
    texte = TTF_RenderText_Shaded(police, " 6 ",couleurblanche,couleurNoire);
    SDL_BlitSurface(texte, NULL, ecran, &positexte); /* Blit du texte par-dessus */

    positexte.x =120;
    positexte.y = 450;
    texte = TTF_RenderText_Shaded(police, " 7 ",couleurorange,couleurbleusature);
    SDL_BlitSurface(texte, NULL, ecran, &positexte); /* Blit du texte par-dessus */

    positexte.x =285;
    positexte.y = 450;
    texte = TTF_RenderText_Shaded(police, " 8 ",couleurblanche,couleurvertecremeux);
    SDL_BlitSurface(texte, NULL, ecran, &positexte); /* Blit du texte par-dessus */

    positexte.x =450;
    positexte.y = 450;
    texte = TTF_RenderText_Shaded(police, " 9 ",couleurbleuneutre,couleurNoire);
    SDL_BlitSurface(texte, NULL, ecran, &positexte); /* Blit du texte par-dessus */

    SDL_Flip(ecran);

     int continuer = 1;
   while (continuer)
    {
       SDL_WaitEvent(&event);
     switch(event.type)
    {
        case SDL_QUIT:
        return 0;
         break;
      case SDL_MOUSEBUTTONUP: /* Clic de la souris */
      posiclic.x = event.button.x; /* change les coordonnées de posiclic*/
      posiclic.y = event.button.y;
      if(posiclic.x >= 0 && posiclic.x <= 68 && posiclic.y >= 0 && posiclic.y <= 68)
      {return 10;
      continuer=0;
      }
      if(posiclic.x >= 120 && posiclic.x <=273  && posiclic.y >= 50 && posiclic.y <= 126)
    {return 1;continuer = 0;}
    if(posiclic.x >= 285 && posiclic.x <= 438 && posiclic.y >= 50 && posiclic.y <= 126)
    {return 2;continuer = 0;}
    if(posiclic.x >= 450 && posiclic.x <= 603 && posiclic.y >= 50 && posiclic.y <= 126)
    {return 3;continuer = 0;}
    if(posiclic.x >= 120 && posiclic.x <= 273 && posiclic.y >= 250 && posiclic.y <=326 )
    {return 4;continuer = 0;}
    if(posiclic.x >= 285 && posiclic.x <= 438 && posiclic.y >= 250 && posiclic.y <=326 )
    {return 5;continuer = 0;}
    if(posiclic.x >= 450 && posiclic.x <= 603 && posiclic.y >= 250 && posiclic.y <=326 )
    {return 6;continuer = 0;}
    if(posiclic.x >= 120 && posiclic.x <= 273 && posiclic.y >= 450 && posiclic.y <=526 )
    {return 7;continuer = 0;}
    if(posiclic.x >= 285 && posiclic.x <= 438 && posiclic.y >= 450 && posiclic.y <=526 )
    {return 8;continuer = 0;}
    if(posiclic.x >= 450 && posiclic.x <= 603 && posiclic.y >= 450 && posiclic.y <=526 )
    {return 9;continuer = 0;}


      break;
       }

    }
    return 0;




    SDL_FreeSurface(texte);
     TTF_CloseFont(police); /* Doit être avant TTF_Quit() */
    TTF_Quit();//quitte la biblio texte

}
int tobesur(SDL_Surface* ecran,SDL_Rect posiclic)
{
    TTF_Init();//démarrage de la bibliothèque texte

    SDL_Color couleurindigo = {10, 206, 212};
    SDL_Color couleurNoire = {0, 0, 0};
    SDL_Color couleurrouge = {255,0,0};

    TTF_Font *police = NULL;

    SDL_Event event;

    SDL_Surface *texte = NULL;
    SDL_Rect positexte;

    police = TTF_OpenFont("NotoSans.ttf", 62);
    positexte.x = 80;
    positexte.y = 125;
    texte = TTF_RenderText_Blended(police, "Veux-tu supprimer ce niveau?",couleurrouge);
    SDL_BlitSurface(texte, NULL, ecran, &positexte); /* Blit du texte par-dessus */

    positexte.x =200;
    positexte.y = 200;
    texte = TTF_RenderText_Shaded(police, "OUI",couleurNoire,couleurindigo);
    SDL_BlitSurface(texte, NULL, ecran, &positexte); /* Blit du texte par-dessus */

    positexte.x =500;
    positexte.y = 200;
    texte = TTF_RenderText_Shaded(police, "NON",couleurNoire,couleurindigo);
    SDL_BlitSurface(texte, NULL, ecran, &positexte); /* Blit du texte par-dessus */


    SDL_Flip(ecran);

     int continuer = 1;
   while (continuer)
    {
       SDL_WaitEvent(&event);
     switch(event.type)
    {
        case SDL_QUIT:
        return 4;
         break;
      case SDL_MOUSEBUTTONUP: /* Clic de la souris */
      posiclic.x = event.button.x; /* change les coordonnées de posiclic*/
      posiclic.y = event.button.y;

     if (posiclic.x >= 199 && posiclic.x <= 285 && posiclic.y >= 200 && posiclic.y <= 270) // grâce aux  coordonnées de posiclic
       {
           return 1;
           continuer = 0;
       }
       else if(posiclic.x >= 500 && posiclic.x <= 605 && posiclic.y >= 200 && posiclic.y <=270)
       {
           return 0;
           continuer = 0;
       }


      break;
       }

    }
    return 0;


    SDL_FreeSurface(texte);
     TTF_CloseFont(police); /* Doit être avant TTF_Quit() */
    TTF_Quit();//quitte la biblio texte
}
int play(int tableau1[],SDL_Surface* caisse,SDL_Rect posicaisse,SDL_Surface* ecran,SDL_Rect posiclic,SDL_Rect posiretour,SDL_Rect posireplay,SDL_Rect posimario,SDL_Surface* mario,SDL_Surface* retour,SDL_Surface* replay)
{
        int finpartie = 2,quit = 0,niveau =0,endwhile = 0,continuer = 1,fois= 0,ouinon =0,mariohere = 0;
        int i =0,a=0,objec =0;
        int objectif[80] ={0};
        int tableaufixe[80]={0};
        tableau1[0] = 2;
        SDL_Event event;

    i = 0;
     while(i<80)
    {
    tableaufixe[i]=tableau1[i];
    i++;
    }

    i = 0;
    a = 0;
    while(i<80)
   {
    if(tableau1[i]== 4)
    {objectif[a] = i;
    a++;
    }
    i++;
    }


        while(finpartie ==2 && quit == 0)
 {
      i = 0;
     while(i<80)//bug résolu : rechargement initial before posi mario avec tableau modulable
    {
    tableau1[i]=tableaufixe[i];
    i++;
    }
      i = 0;
     while(i<80)
    {
    if(tableau1[i]== 5)
    {
        i++;
        posimario.y = caseposi(i,1);
        posimario.x = caseposi(i,2);
        mariohere = 1;
    }
    i++;
    }


    endwhile = 0;
    if(mariohere == 0 || a == 0)
    {ouinon=affcomplet(ecran,posiclic);
     if(ouinon == 4)
    return 1;
    if(ouinon == 1)
    {
        finpartie = 2;
       endwhile=1;
    }
    if(ouinon == 0)
    return 0;
    }

    load(ecran,niveau,tableaufixe);

    retour = SDL_LoadBMP ("sprites_mario_sokoban/back.bmp");
    SDL_BlitSurface(retour , NULL, ecran, &posiretour );

    posireplay.x=9*68;
    replay = SDL_LoadBMP ("sprites_mario_sokoban/retry.bmp");
    SDL_BlitSurface(replay , NULL, ecran, &posireplay );
    SDL_Flip(ecran);


   continuer = 1;

fois=0;



   while (continuer == 1 && quit == 0 && endwhile == 0)
       {
         SDL_WaitEvent(&event);
             switch(event.type)
           {
          case SDL_QUIT:
              continuer = 0;
              return 1;
             break;
         case SDL_KEYDOWN:
         if(event.key.keysym.sym == SDLK_UP || event.key.keysym.sym == SDLK_DOWN
            || event.key.keysym.sym == SDLK_RIGHT || event.key.keysym.sym == SDLK_LEFT)
        {mario = SDL_CreateRGBSurface(SDL_HWSURFACE, 68, 68, 16, 0, 0,0, 0);//on charge un carré blanc à l'ancien emplacement de mario
    SDL_FillRect(mario, NULL, SDL_MapRGB(ecran->format, 255,255,255));//cela permet d'éviter de se servir à chaque boucle de la fonction load(ce qui fait moche et ramer)
    SDL_BlitSurface(mario, NULL, ecran, &posimario); /* On
place mario à sa nouvelle position */
SDL_Flip(ecran); /* On met à jour l'affichage */
}
         if(event.key.keysym.sym == SDLK_UP)
     {

         posimario.y -= 68;

     mario = SDL_LoadBMP("sprites_mario_sokoban/keeper_up.bmp");
      if(verif(niveau,posimario,tableau1,1) == 1 )
           {
               posimario.y += 68;
           }
            if(verif(niveau,posimario,tableau1,1) == 2 )
            {
                tableau1[cases(posimario)-1] = 2;
                posimario.y -= 68;
                tableau1[cases(posimario)-1] = 3;
                posicaisse.y=posimario.y;
                posicaisse.x=posimario.x;
                caisse = SDL_LoadBMP("sprites_mario_sokoban/crate.bmp");
                SDL_BlitSurface(caisse, NULL, ecran, &posicaisse);
                posimario.y += 68;
            }
             if(verif(niveau,posimario,tableau1,1) == 4)
            {
                tableau1[cases(posimario)-1] = 2;
                posimario.y -= 68;
                tableau1[cases(posimario)-1] = 6;
                posicaisse.y=posimario.y;
                posicaisse.x=posimario.x;
                caisse = SDL_LoadBMP("sprites_mario_sokoban/crate_target.bmp");
                SDL_BlitSurface(caisse, NULL, ecran, &posicaisse);
                posimario.y += 68;
            }

     }
     if(event.key.keysym.sym == SDLK_DOWN )
     {

         posimario.y += 68;
     mario = SDL_LoadBMP("sprites_mario_sokoban/keeper_down.bmp");
      if(verif(niveau,posimario,tableau1,4) == 1)
           {
               posimario.y -= 68;
           }
           if(verif(niveau,posimario,tableau1,4) == 2)
            {
                tableau1[cases(posimario)-1] = 2;
                posimario.y += 68;
                tableau1[cases(posimario)-1] = 3;
                posicaisse.y=posimario.y;
                posicaisse.x=posimario.x;
                caisse = SDL_LoadBMP("sprites_mario_sokoban/crate.bmp");
                SDL_BlitSurface(caisse, NULL, ecran, &posicaisse);
                posimario.y -= 68;
            }
            if(verif(niveau,posimario,tableau1,4) == 4)
            {
                tableau1[cases(posimario)-1] = 2;
                posimario.y += 68;
                tableau1[cases(posimario)-1] = 6;
                posicaisse.y=posimario.y;
                posicaisse.x=posimario.x;
                caisse = SDL_LoadBMP("sprites_mario_sokoban/crate_target.bmp");
                SDL_BlitSurface(caisse, NULL, ecran, &posicaisse);
                posimario.y -= 68;
            }
     }
     if(event.key.keysym.sym == SDLK_RIGHT)
     {

         posimario.x += 68;
      if(verif(niveau,posimario,tableau1,2) == 1)
           {
               posimario.x -= 68;
           }
            if(verif(niveau,posimario,tableau1,2) == 2)
            {
                tableau1[cases(posimario)-1] = 2;
                posimario.x += 68;
                tableau1[cases(posimario)-1] = 3;
                posicaisse.y=posimario.y;
                posicaisse.x=posimario.x;
                caisse = SDL_LoadBMP("sprites_mario_sokoban/crate.bmp");
                SDL_BlitSurface(caisse, NULL, ecran, &posicaisse);
                posimario.x -= 68;
            }
            if(verif(niveau,posimario,tableau1,2) == 4)
            {
                tableau1[cases(posimario)-1] = 2;
                posimario.x += 68;
                tableau1[cases(posimario)-1] = 6;
                posicaisse.y=posimario.y;
                posicaisse.x=posimario.x;
                caisse = SDL_LoadBMP("sprites_mario_sokoban/crate_target.bmp");
                SDL_BlitSurface(caisse, NULL, ecran, &posicaisse);
                posimario.x -= 68;
            }
     mario = SDL_LoadBMP("sprites_mario_sokoban/keeper_right.bmp");

     }
     if(event.key.keysym.sym == SDLK_LEFT)
     {

         posimario.x -= 68;
     if(verif(niveau,posimario,tableau1,3) == 1)
           {
               posimario.x += 68;
           }
           if(verif(niveau,posimario,tableau1,3) == 2)
           {
               tableau1[cases(posimario)-1] = 2;
               posimario.x -= 68;
               tableau1[cases(posimario)-1] = 3;
                posicaisse.y=posimario.y;
                posicaisse.x=posimario.x;
                caisse = SDL_LoadBMP("sprites_mario_sokoban/crate.bmp");
                SDL_BlitSurface(caisse, NULL, ecran, &posicaisse);
                posimario.x += 68;
           }
           if(verif(niveau,posimario,tableau1,3) == 4)
           {
               tableau1[cases(posimario)-1] = 2;
               posimario.x -= 68;
               tableau1[cases(posimario)-1] = 6;
                posicaisse.y=posimario.y;
                posicaisse.x=posimario.x;
                caisse = SDL_LoadBMP("sprites_mario_sokoban/crate_target.bmp");
                SDL_BlitSurface(caisse, NULL, ecran, &posicaisse);
                posimario.x += 68;
           }
     mario = SDL_LoadBMP("sprites_mario_sokoban/keeper_left.bmp");
     }
         break;
         case SDL_MOUSEBUTTONUP: /* Clic de la souris */
      posiclic.x = event.button.x; /* change les coordonnées de posiclic*/
      posiclic.y = event.button.y;
      if(posiclic.x >= 0 && posiclic.x <= 68 && posiclic.y >= 0 && posiclic.y <= 68)
      {
          continuer = 0;
      endwhile = 1;
          return 0;

      }
      if(posiclic.x >=68*9  && posiclic.x <= 68*10 && posiclic.y >= 0 && posiclic.y <= 68)
      {finpartie = 2;
       endwhile=1;
      }
      break;
      }

if(posimario.y == 0)
{
    if(posimario.x == 68*8)
    {
    replay = SDL_LoadBMP ("sprites_mario_sokoban/retry.bmp");
    SDL_BlitSurface(replay , NULL, ecran, &posireplay );
    }
    if(posimario.x == 68)
    {
    retour = SDL_LoadBMP ("sprites_mario_sokoban/back.bmp");
    SDL_BlitSurface(retour , NULL, ecran, &posiretour );
    }
}
if(posimario.y == 68)
{
    if(posimario.x == 0)
    {
    retour = SDL_LoadBMP ("sprites_mario_sokoban/back.bmp");
    SDL_BlitSurface(retour , NULL, ecran, &posiretour );
    }
    if(posimario.x == 68*9)
    {
    replay = SDL_LoadBMP ("sprites_mario_sokoban/retry.bmp");
    SDL_BlitSurface(replay , NULL, ecran, &posireplay );
    }
}
i=0;
while(i<a)
{
    if(tableau1[objectif[i]]==6)
    {
        objec++;
    }
    i++;
}
if(objec == a && a != 0)
{
    ouinon=affonlyvictyouinon(ecran,posiclic);
    if(ouinon == 4)
    return 1;
    if(ouinon == 1)
    {
        finpartie = 2;
       endwhile=1;
    }
    if(ouinon == 0)
    return 0;
}



objec = 0;
SDL_BlitSurface(mario, NULL, ecran, &posimario); /* On
place mario à sa nouvelle position */
SDL_Flip(ecran); /* On met à jour l'affichage */


}
}
return 0;

}
int affonlyvictyouinon(SDL_Surface* ecran,SDL_Rect posiclic)
{
    TTF_Init();//démarrage de la bibliothèque texte
    SDL_Color couleurindigo = {10, 206, 212};
    SDL_Color couleurrouge = {255,0,0};
    SDL_Color couleurviolette = {165,32,175};
    SDL_Color couleurorange = {255,123,0};
    SDL_Color couleurrose = {255,81,255};
    SDL_Color couleurjaune = {255,233,0};
    SDL_Color couleurvertepomme = {177,255,23};
    SDL_Color couleurbleumarine = {0,0,255};
    SDL_Color couleurNoire = {0, 0, 0};


    TTF_Font *police = NULL;
    SDL_Surface *texte = NULL;
    SDL_Rect positexte;
    positexte.x = 0;
    positexte.y = 0;
     police = TTF_OpenFont("NotoSans.ttf", 155);

     texte =SDL_LoadBMP ("sprites_mario_sokoban/garden.bmp");
    SDL_BlitSurface(texte , NULL, ecran, &positexte );
    SDL_Flip(ecran);

     positexte.x = 0;
    positexte.y = 30;
    texte = TTF_RenderText_Blended(police, "V",couleurrouge);
    SDL_BlitSurface(texte, NULL, ecran, &positexte); /* Blit du texte par-dessus */
    positexte.x = 110;
    texte = TTF_RenderText_Blended(police, "I",couleurrose);
    SDL_BlitSurface(texte, NULL, ecran, &positexte); /* Blit du texte par-dessus */
    positexte.x = 150;
    texte = TTF_RenderText_Blended(police, "C",couleurorange);
    SDL_BlitSurface(texte, NULL, ecran, &positexte); /* Blit du texte par-dessus */
    positexte.x = 235;
    texte = TTF_RenderText_Blended(police, "T",couleurjaune);
    SDL_BlitSurface(texte, NULL, ecran, &positexte); /* Blit du texte par-dessus */
    positexte.x = 330;
    texte = TTF_RenderText_Blended(police, "O",couleurvertepomme);
    SDL_BlitSurface(texte, NULL, ecran, &positexte); /* Blit du texte par-dessus */
    positexte.x = 420;
    texte = TTF_RenderText_Blended(police, "I",couleurindigo);
    SDL_BlitSurface(texte, NULL, ecran, &positexte); /* Blit du texte par-dessus */
    positexte.x = 480;
    texte = TTF_RenderText_Blended(police, "R",couleurbleumarine);
    SDL_BlitSurface(texte, NULL, ecran, &positexte); /* Blit du texte par-dessus */
    positexte.x = 580;
    texte = TTF_RenderText_Blended(police, "E",couleurviolette);
    SDL_BlitSurface(texte, NULL, ecran, &positexte); /* Blit du texte par-dessus */
    TTF_CloseFont(police);

    police = TTF_OpenFont("NotoSans.ttf", 62);
    positexte.x = 0;
    positexte.y = 175;
    texte = TTF_RenderText_Blended(police, "Veux-tu rejouer?",couleurrouge);
    SDL_BlitSurface(texte, NULL, ecran, &positexte); /* Blit du texte par-dessus */

    positexte.x =20;
    positexte.y = 250;
    texte = TTF_RenderText_Shaded(police, "OUI",couleurNoire,couleurindigo);
    SDL_BlitSurface(texte, NULL, ecran, &positexte); /* Blit du texte par-dessus */

    positexte.x =320;
    positexte.y = 250;
    texte = TTF_RenderText_Shaded(police, "NON",couleurNoire,couleurindigo);
    SDL_BlitSurface(texte, NULL, ecran, &positexte); /* Blit du texte par-dessus */

    SDL_Flip(ecran);
SDL_Event event;
     int continuer = 1;
   while (continuer)
    {
       SDL_WaitEvent(&event);
     switch(event.type)
    {
        case SDL_QUIT:
        return 4;
         break;
      case SDL_MOUSEBUTTONUP: /* Clic de la souris */
      posiclic.x = event.button.x; /* change les coordonnées de posiclic*/
      posiclic.y = event.button.y;

     if (posiclic.x >= 20 && posiclic.x <= 297 && posiclic.y >= 250 && posiclic.y <= 320) // grâce aux  coordonnées de posiclic
       {
           return 1;
           continuer = 0;
       }
       else if(posiclic.x >= 320 && posiclic.x <= 620 && posiclic.y >= 250 && posiclic.y <=320)
       {
           return 0;
           continuer = 0;
       }


      break;
       }

    }
    return 0;




    SDL_FreeSurface(texte);
    TTF_CloseFont(police); /* Doit être avant TTF_Quit() */
    TTF_Quit();//quitte la biblio texte


}
void affefface(SDL_Surface* ecran)
{
    TTF_Init();//démarrage de la bibliothèque texte
    SDL_Color couleurNoire = {0, 0, 0};
     SDL_Surface *texte = NULL;
    SDL_Rect positexte;
    TTF_Font *police = NULL;
     police = TTF_OpenFont("NotoSans.ttf", 62);
    positexte.x = 5;
    positexte.y = 400;
    texte = TTF_RenderText_Blended(police, "Ce niveau remplacera le niveau 9 ",couleurNoire);
    SDL_BlitSurface(texte, NULL, ecran, &positexte); /* Blit du texte par-dessus */

    SDL_Flip(ecran);

     SDL_FreeSurface(texte);
     TTF_CloseFont(police); /* Doit être avant TTF_Quit() */
    TTF_Quit();//quitte la biblio texte


}
int affcomplet(SDL_Surface* ecran,SDL_Rect posiclic)
{
    TTF_Init();//démarrage de la bibliothèque texte
    SDL_Color couleurblanche = {255, 255, 255};
     SDL_Surface *texte = NULL;
    SDL_Rect positexte;
    TTF_Font *police = NULL;
     police = TTF_OpenFont("NotoSans.ttf", 70);
     positexte.x = 0;
    positexte.y = 0;

    texte= SDL_CreateRGBSurface(SDL_HWSURFACE, 680, 544, 16, 0, 0,0, 0);//dimension et mode de couleur du rectangle
    SDL_FillRect(texte, NULL, SDL_MapRGB(ecran->format,0,0, 0));//couleur du rectangle
    SDL_BlitSurface(texte, NULL, ecran, &positexte); // Collage



    positexte.x = 20;
    positexte.y = 300;
    texte = TTF_RenderText_Blended(police, "Niveau incomplet ou vide ",couleurblanche);
    SDL_BlitSurface(texte, NULL, ecran, &positexte); /* Blit du texte par-dessus */

    positexte.x = 0;
    positexte.y = 0;
    texte = SDL_LoadBMP("sprites_mario_sokoban/back.bmp");
    SDL_BlitSurface(texte, NULL, ecran, &positexte); /* Blit du texte par-dessus */

    positexte.x = 68*9;
    texte = SDL_LoadBMP("sprites_mario_sokoban/retry.bmp");
    SDL_BlitSurface(texte, NULL, ecran, &positexte); /* Blit du texte par-dessus */
    SDL_Flip(ecran);

    SDL_Event event;
     int continuer = 1;
   while (continuer)
    {
       SDL_WaitEvent(&event);
     switch(event.type)
    {
        case SDL_QUIT:
        return 4;
         break;
      case SDL_MOUSEBUTTONUP: /* Clic de la souris */
      posiclic.x = event.button.x; /* change les coordonnées de posiclic*/
      posiclic.y = event.button.y;
     if (posiclic.x >= 0 && posiclic.x <= 68 && posiclic.y >= 0 && posiclic.y <= 68) // grâce aux  coordonnées de posiclic
       {
           return 0;
           continuer = 0;
       }
       else if(posiclic.x >= 68*9 && posiclic.x <= 680 && posiclic.y >= 0 && posiclic.y <=68)
       {
           return 1;
           continuer = 0;
       }
      break;
       }
    }
    return 0;




     SDL_FreeSurface(texte);
     TTF_CloseFont(police); /* Doit être avant TTF_Quit() */
    TTF_Quit();//quitte la biblio texte


}



