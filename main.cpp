#ifdef __cplusplus
    #include <cstdlib>
#else
    #include <stdlib.h>
#endif
#include <SDL.h>
#include "bitmap_font.hpp"
#include <string.h>
#include <string>
#include <vector>
#ifdef _WIN32
#include <windows.h>
#include <wincodec.h>
#else
#include <unistd.h>
#endif
//code source retouché: rechercher "//bug résolu :" pour voir les retouches
void attendre_fermeture();
bool enregistreniv(int tableau[],int niveaufichier,int eff);
void lecture(int niveau,int tableauniv[]);
void reset_custom_level(int niveau);
void clear_best_times(int level);
int choixniveau(SDL_Rect posiclic);
void choixniv(SDL_Surface* ecran);
void load(SDL_Surface *ecran , int niveau, int tableau1[]);
int tableau(int niveau , int fois);
int cases(SDL_Rect posimario);
int casesmodif(SDL_Rect posimario);
int verif(int niveau ,SDL_Rect posimario, int tableau[],int direct);
int victoire(SDL_Surface* ecran, unsigned int elapsedSeconds);
void affvict(SDL_Surface* ecran,SDL_Rect posivict, int fois,SDL_Surface* affiche);
void save(int niveau);
void afftrait(int jaune,SDL_Surface* ecran,SDL_Rect positionlignes,SDL_Surface*colleimagecrea);
int caseposi(int cases,int number);
int niveaucrea(SDL_Surface* ecran,SDL_Rect posiclic);
int tobesur(SDL_Surface* ecran,SDL_Rect posiclic);
int play(int tableau1[],SDL_Surface* caisse,SDL_Rect posicaisse,SDL_Surface* ecran,SDL_Rect posiclic,SDL_Rect posiretour,SDL_Rect posireplay,SDL_Rect posimario,SDL_Surface* mario,SDL_Surface* retour,SDL_Surface* replay,int customLevel);
int affonlyvictyouinon(SDL_Surface* ecran,SDL_Rect posiclic,unsigned int elapsedSeconds);
int affcomplet(SDL_Surface* ecran,SDL_Rect posiclic);
void draw_ui_button(SDL_Surface* ecran, TTF_Font* font, const char* label, SDL_Rect bounds, bool blue=false);
void draw_game_text(SDL_Surface* ecran, TTF_Font* font, const char* label, SDL_Rect at, SDL_Color color);
void draw_centered_game_text(SDL_Surface* ecran, TTF_Font* font, const char* label, SDL_Rect bounds, SDL_Color color);
void draw_multicolor_victory(SDL_Surface* ecran, TTF_Font* font, int centerX, int y);
void draw_player_on_floor(SDL_Surface* ecran, SDL_Surface* player, SDL_Rect at);
void draw_level_timer(SDL_Surface* ecran, Uint32 startedTicks);
void draw_go_label(SDL_Surface* ecran);
void blit_half_size(SDL_Surface* source, SDL_Surface* target, int x, int y);
void load_best_times(int level, unsigned int times[3]);
void record_best_time(int level, unsigned int seconds);
void draw_best_times(SDL_Surface* ecran);
void draw_centered_game_text(SDL_Surface* ecran, TTF_Font* font, const char* label, SDL_Rect bounds, SDL_Color color)
{
    if (!ecran || !font || !label) return;
    SDL_Surface *text = TTF_RenderText_Blended(font, label, color);
    if (!text) return;
    SDL_Rect at = {static_cast<Sint16>(bounds.x + (bounds.w - text->w) / 2),
                   static_cast<Sint16>(bounds.y + (bounds.h - text->h) / 2), 0, 0};
    SDL_FreeSurface(text);
    draw_game_text(ecran, font, label, at, color);
}

void draw_level_thumbnail(SDL_Surface* ecran, const int map[80], int x, int y, int width, int height);
void redraw_board_cell(SDL_Surface* ecran, int value, SDL_Rect at);
SDL_Surface* load_character_png(const char* pngPath);

// ---------------------------------------------------------------------------
// Presentation layer: the game always draws into a fixed 680x544 canvas. That
// canvas is scaled (aspect ratio preserved) and centred on the real video
// surface, so fullscreen and resized windows stay centred instead of being
// drawn in the top-left corner of a larger display mode.
// ---------------------------------------------------------------------------
static const int GAME_WIDTH = 680;
static const int GAME_HEIGHT = 544;
static SDL_Surface *g_display = NULL;
static SDL_Surface *g_canvas = NULL;
static SDL_Surface *g_scaled = NULL;
static SDL_Rect g_viewport = {0, 0, GAME_WIDTH, GAME_HEIGHT};
static bool g_fullscreen = false;
static int g_desktopW = 0, g_desktopH = 0;
static int g_windowW = GAME_WIDTH, g_windowH = GAME_HEIGHT;

static void video_update_viewport()
{
    if (!g_display) return;
    const int dw = g_display->w, dh = g_display->h;
    int vw = dw, vh = dw * GAME_HEIGHT / GAME_WIDTH;
    if (vh > dh) { vh = dh; vw = dh * GAME_WIDTH / GAME_HEIGHT; }
    if (vw < 1) vw = 1;
    if (vh < 1) vh = 1;
    g_viewport.x = static_cast<Sint16>((dw - vw) / 2);
    g_viewport.y = static_cast<Sint16>((dh - vh) / 2);
    g_viewport.w = static_cast<Uint16>(vw);
    g_viewport.h = static_cast<Uint16>(vh);
    if (g_scaled) { SDL_FreeSurface(g_scaled); g_scaled = NULL; }
    if (vw != GAME_WIDTH || vh != GAME_HEIGHT)
        g_scaled = SDL_CreateRGBSurface(SDL_SWSURFACE, vw, vh, 32,
                                        0x00ff0000, 0x0000ff00, 0x000000ff, 0);
}

static bool video_set_mode(bool fullscreen, int width, int height)
{
    SDL_Surface *display = NULL;
    if (fullscreen) {
        // Use the desktop resolution: the canvas is centred inside it.
        if (g_desktopW > 0 && g_desktopH > 0)
            display = SDL_SetVideoMode(g_desktopW, g_desktopH, 0, SDL_SWSURFACE | SDL_FULLSCREEN);
        if (!display) {
            SDL_Rect **modes = SDL_ListModes(NULL, SDL_FULLSCREEN);
            if (modes && modes != reinterpret_cast<SDL_Rect**>(-1) && modes[0])
                display = SDL_SetVideoMode(modes[0]->w, modes[0]->h, 0, SDL_SWSURFACE | SDL_FULLSCREEN);
        }
        if (!display) display = SDL_SetVideoMode(GAME_WIDTH, GAME_HEIGHT, 0, SDL_SWSURFACE | SDL_FULLSCREEN);
    } else {
        if (width < 170) width = 170;
        if (height < 136) height = 136;
        display = SDL_SetVideoMode(width, height, 0, SDL_SWSURFACE | SDL_RESIZABLE);
    }
    if (!display) return false;
    g_display = display;
    g_fullscreen = fullscreen;
    if (!fullscreen) { g_windowW = display->w; g_windowH = display->h; }
    video_update_viewport();
    return true;
}

static void video_present()
{
    if (!g_display || !g_canvas) return;
    if (g_viewport.w != g_display->w || g_viewport.h != g_display->h)
        SDL_FillRect(g_display, NULL, SDL_MapRGB(g_display->format, 0, 0, 0));
    SDL_Rect at = g_viewport;
    if (!g_scaled) {
        SDL_BlitSurface(g_canvas, NULL, g_display, &at);
    } else {
        // Nearest-neighbour scaling, both surfaces are 32-bit XRGB.
        SDL_LockSurface(g_canvas);
        SDL_LockSurface(g_scaled);
        std::vector<int> sourceX(g_scaled->w);
        for (int x = 0; x < g_scaled->w; ++x) sourceX[x] = x * GAME_WIDTH / g_scaled->w;
        for (int y = 0; y < g_scaled->h; ++y) {
            const Uint32 *src = reinterpret_cast<const Uint32*>(
                static_cast<const Uint8*>(g_canvas->pixels) + (y * GAME_HEIGHT / g_scaled->h) * g_canvas->pitch);
            Uint32 *dst = reinterpret_cast<Uint32*>(static_cast<Uint8*>(g_scaled->pixels) + y * g_scaled->pitch);
            for (int x = 0; x < g_scaled->w; ++x) dst[x] = src[sourceX[x]];
        }
        SDL_UnlockSurface(g_scaled);
        SDL_UnlockSurface(g_canvas);
        SDL_BlitSurface(g_scaled, NULL, g_display, &at);
    }
    SDL_Flip(g_display);
}

static void video_toggle_fullscreen()
{
    if (!video_set_mode(!g_fullscreen, g_windowW, g_windowH))
        video_set_mode(g_fullscreen, g_windowW, g_windowH);
    video_present();
}

// Returns true when the event was consumed by the presentation layer.
static bool video_filter_event(SDL_Event *event)
{
    switch (event->type) {
    case SDL_KEYDOWN:
        if (event->key.keysym.sym == SDLK_F11 ||
            (event->key.keysym.sym == SDLK_RETURN && (event->key.keysym.mod & KMOD_ALT))) {
            video_toggle_fullscreen();
            return true;
        }
        if (event->key.keysym.sym == SDLK_ESCAPE && g_fullscreen) {
            video_toggle_fullscreen();
            return true;
        }
        return false;
    case SDL_VIDEORESIZE:
        if (!g_fullscreen) { video_set_mode(false, event->resize.w, event->resize.h); video_present(); }
        return true;
    case SDL_VIDEOEXPOSE:
        video_present();
        return true;
    case SDL_MOUSEBUTTONDOWN:
    case SDL_MOUSEBUTTONUP: {
        const int x = event->button.x - g_viewport.x, y = event->button.y - g_viewport.y;
        if (x < 0 || y < 0 || x >= g_viewport.w || y >= g_viewport.h) return true; // click in the black bars
        event->button.x = static_cast<Uint16>(x * GAME_WIDTH / g_viewport.w);
        event->button.y = static_cast<Uint16>(y * GAME_HEIGHT / g_viewport.h);
        return false;
    }
    case SDL_MOUSEMOTION: {
        int x = event->motion.x - g_viewport.x, y = event->motion.y - g_viewport.y;
        if (x < 0) x = 0;
        if (y < 0) y = 0;
        if (x >= g_viewport.w) x = g_viewport.w - 1;
        if (y >= g_viewport.h) y = g_viewport.h - 1;
        event->motion.x = static_cast<Uint16>(x * GAME_WIDTH / g_viewport.w);
        event->motion.y = static_cast<Uint16>(y * GAME_HEIGHT / g_viewport.h);
        return false;
    }
    default:
        return false;
    }
}

static int video_wait_event(SDL_Event *event)
{
    for (;;) {
        if (!SDL_WaitEvent(event)) return 0;
        if (!video_filter_event(event)) return 1;
    }
}

static int video_poll_event(SDL_Event *event)
{
    while (SDL_PollEvent(event))
        if (!video_filter_event(event)) return 1;
    return 0;
}

static SDL_Surface *video_init(bool fullscreen)
{
    const SDL_VideoInfo *info = SDL_GetVideoInfo();
    if (info) { g_desktopW = info->current_w; g_desktopH = info->current_h; }
    if (!video_set_mode(fullscreen, GAME_WIDTH, GAME_HEIGHT) &&
        !(fullscreen && video_set_mode(false, GAME_WIDTH, GAME_HEIGHT)))
        return NULL;
    g_canvas = SDL_CreateRGBSurface(SDL_SWSURFACE, GAME_WIDTH, GAME_HEIGHT, 32,
                                    0x00ff0000, 0x0000ff00, 0x000000ff, 0);
    if (g_canvas) SDL_FillRect(g_canvas, NULL, 0);
    return g_canvas;
}

// Every screen of the game draws on the canvas; route presentation and events
// through the layer above so they are scaled, centred and mapped back.
#define SDL_Flip(surface) (video_present(), 0)
#define SDL_WaitEvent(event) video_wait_event(event)
#define SDL_PollEvent(event) video_poll_event(event)

// ---------------------------------------------------------------------------
// Personal levels: up to 3 pages of 9 levels, stored as
// "niveaux perso/niveau1.txt" .. "niveaux perso/niveau27.txt".
// ---------------------------------------------------------------------------
static const int CUSTOM_LEVELS_PER_PAGE = 9;
static const int CUSTOM_PAGE_COUNT = 3;
static const int CUSTOM_LEVEL_COUNT = CUSTOM_LEVELS_PER_PAGE * CUSTOM_PAGE_COUNT;
static const int CUSTOM_BACK = 100;          // niveaucrea(): "back" button
static const int BUILTIN_LEVEL_COUNT = 6;
static const int BEST_TIME_ROWS = BUILTIN_LEVEL_COUNT + CUSTOM_LEVEL_COUNT;
static int g_customPage = 0;

static void custom_level_path(int level, char *path, size_t size)
{
    snprintf(path, size, "niveaux perso/niveau%d.txt", level);
}

// Assets are loaded with relative paths. When the game is started from another
// working directory (an IDE run configuration, a shortcut...), switch to the
// directory of the executable, where CMake copies every asset.
static void use_executable_directory_for_assets()
{
    FILE *probe = fopen("sprites_mario_sokoban/floor.bmp", "rb");
    if (probe) { fclose(probe); return; }
#ifdef _WIN32
    char path[MAX_PATH];
    const DWORD length = GetModuleFileNameA(NULL, path, MAX_PATH);
    if (length > 0 && length < MAX_PATH) {
        char *slash = strrchr(path, '\\');
        if (slash) { *slash = 0; SetCurrentDirectoryA(path); }
    }
#else
    char path[4096];
    const ssize_t length = readlink("/proc/self/exe", path, sizeof(path) - 1);
    if (length > 0) {
        path[length] = 0;
        char *slash = strrchr(path, '/');
        if (slash) { *slash = 0; if (chdir(path) != 0) return; }
    }
#endif
}

int main ( int argc, char* argv[] )
{
    use_executable_directory_for_assets();
    SDL_putenv("SDL_VIDEO_CENTERED=1");
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
    bool fullscreen = false;
    for (int arg = 1; arg < argc; ++arg) {
        if (strcmp(argv[arg], "--fullscreen") == 0 || strcmp(argv[arg], "-f") == 0) fullscreen = true;
    }
    SDL_Surface* ecran = video_init(fullscreen);
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
    int retourListe = 0;//retour de l'editeur vers la liste des niveaux
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
    SDL_Rect creditPlaque = {10, 482, 660, 52};
    SDL_FillRect(ecran, &creditPlaque, SDL_MapRGB(ecran->format, 29, 43, 61));
    SDL_Rect creditInset = {13, 485, 654, 46};
    SDL_FillRect(ecran, &creditInset, SDL_MapRGB(ecran->format, 32, 55, 84));
    police = TTF_OpenFont("NotoSans.ttf", 32);
    draw_centered_game_text(ecran,police,"CREATED BY FROMTHERAGS",SDL_Rect{13,485,654,46},SDL_Color{255,225,110});
    TTF_CloseFont(police);
    police = TTF_OpenFont("NotoSans.ttf", 40);
    draw_centered_game_text(ecran, police, "MARIO SOKOBAN", SDL_Rect{157,35,366,94}, SDL_Color{255,245,205});
    TTF_CloseFont(police);
    police = TTF_OpenFont("NotoSans.ttf", 26);
    draw_ui_button(ecran, police, "1 - JOUER", SDL_Rect{176,210,304,45});
    draw_ui_button(ecran, police, "2 - CREER", SDL_Rect{176,257,304,45}, true);
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
      if(posiclic.x >= 176 && posiclic.x <= 480 && posiclic.y >= 210 && posiclic.y <= 255)
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
      if(posiclic.x >= 0 && posiclic.x <= 68 && posiclic.y <= 68)
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
        if(quit == 0 && accesniv != CUSTOM_BACK)
        {int customLevel=accesniv;
        lecture(customLevel,tableaucrea);
        accesniv = 0;
          accesniv =play(tableaucrea,caisse,posicaisse,ecran,posiclic,posiretour,posireplay,posimario,mario,retour,replay,customLevel);
          if(accesniv)
          {quit = 1;
          }
          else
          {
              boucleniv = 1;
          }
        }
        if(accesniv == CUSTOM_BACK)
        {
            boucleniv = 1;
        }

     }
     }while(boucleniv == 1 && quit == 0);


    while(finpartie ==2 && quit == 0)
 {
    Uint32 levelStartTicks = SDL_GetTicks();
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



   int shownSeconds=-1;
   while (continuer == 1 && quit == 0 && endwhile == 0)
       {
         if(!SDL_PollEvent(&event)){
             int seconds=static_cast<int>((SDL_GetTicks()-levelStartTicks)/1000);
             if(seconds!=shownSeconds){draw_level_timer(ecran,levelStartTicks);SDL_Flip(ecran);shownSeconds=seconds;}
             SDL_Delay(100);
             continue;
         }

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
         redraw_board_cell(ecran, 2, posimario);
     }
         if(event.key.keysym.sym == SDLK_UP)
     {

         posimario.y -= 68;

     mario = load_character_png("sprites_mario_sokoban/keeper_up.png");
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
     mario = load_character_png("sprites_mario_sokoban/keeper_down.png");
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
     mario = load_character_png("sprites_mario_sokoban/keeper_right.png");

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
     mario = load_character_png("sprites_mario_sokoban/keeper_left.png");
     }
         break;
         case SDL_MOUSEBUTTONUP: /* Clic de la souris */
      posiclic.x = event.button.x; /* change les coordonnées de posiclic*/
      posiclic.y = event.button.y;
      if(posiclic.x >= 0 && posiclic.x <= 68 && posiclic.y >= 0 && posiclic.y <= 68)
      {finpartie = 1;
       endwhile=1;
      }
      if(posiclic.x >=68*9 && posiclic.x < 68*10 && posiclic.y < 68)
      {finpartie = 2;
       levelStartTicks = SDL_GetTicks();
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


draw_player_on_floor(ecran, mario, posimario);
draw_level_timer(ecran,levelStartTicks); /* On
place mario à sa nouvelle position */
SDL_Flip(ecran); /* On met à jour l'affichage */

 if(niveau == 1)
 {
     if(tableau1[14]==6 && tableau1[21]==6 && tableau1[35]==6)
     {
         record_best_time(niveau,(SDL_GetTicks()-levelStartTicks)/1000);
         finpartie = victoire(ecran,(SDL_GetTicks()-levelStartTicks)/1000);
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
         record_best_time(niveau,(SDL_GetTicks()-levelStartTicks)/1000);
         finpartie = victoire(ecran,(SDL_GetTicks()-levelStartTicks)/1000);
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
         record_best_time(niveau,(SDL_GetTicks()-levelStartTicks)/1000);
         finpartie = victoire(ecran,(SDL_GetTicks()-levelStartTicks)/1000);
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
         record_best_time(niveau,(SDL_GetTicks()-levelStartTicks)/1000);
         finpartie = victoire(ecran,(SDL_GetTicks()-levelStartTicks)/1000);
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
         record_best_time(niveau,(SDL_GetTicks()-levelStartTicks)/1000);
         finpartie = victoire(ecran,(SDL_GetTicks()-levelStartTicks)/1000);
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
         record_best_time(niveau,(SDL_GetTicks()-levelStartTicks)/1000);
         finpartie = victoire(ecran,(SDL_GetTicks()-levelStartTicks)/1000);
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

  if(!retourListe)
  {
   ecran_partie = SDL_LoadBMP("sprites_mario_sokoban/editor.bmp");
    if(ecran_partie){SDL_BlitSurface(ecran_partie,NULL,ecran,&posi_partie);SDL_FreeSurface(ecran_partie);ecran_partie=NULL;}
    retour=SDL_LoadBMP("sprites_mario_sokoban/back.bmp");
    if(retour){SDL_BlitSurface(retour,NULL,ecran,&posiretour);SDL_FreeSurface(retour);retour=NULL;}
    police=TTF_OpenFont("NotoSans.ttf",36);
    if(police){
        SDL_Rect title={225,60,0,0};
        draw_game_text(ecran,police,"EDITEUR DE NIVEAUX",title,SDL_Color{255,231,149});
        draw_ui_button(ecran,police,"MODIFIER",SDL_Rect{180,170,320,68});
        draw_ui_button(ecran,police,"REINITIALISATION",SDL_Rect{150,290,380,68},true);
        TTF_CloseFont(police);
    }
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
      if(posiclic.x >= 180 && posiclic.x <= 500 && posiclic.y >= 170 && posiclic.y <= 238)
      {
       jouer = 2;//modifier (un emplacement vide cree un nouveau niveau)
       creationend = 0;
       continuer = 0;
      }
      if(posiclic.x >= 150 && posiclic.x <= 530 && posiclic.y >= 290 && posiclic.y <= 358)
      {
       jouer = 3;//reinitialisation
       creationend = 0;
       continuer = 0;
      }


      break;
       }

    }
  }
  retourListe = 0;
    level = CUSTOM_LEVEL_COUNT;
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
         if(clicniv<CUSTOM_BACK)
         level = clicniv;

     }
     if(jouer == 3 && clicniv >0 && clicniv <CUSTOM_BACK)
     {
         tobechoix = tobesur(ecran,posiclic);
         if(tobechoix == 1)
         {reset_custom_level(clicniv);
         boucle = 1;
         }
         if(tobechoix == 0)
         {
         boucle = 1;
         }
         if(tobechoix == 4)
         quit = 1;
     }
}while(boucle == 1 && quit == 0);


    if(quit == 0 && jouer != 4 && clicniv <CUSTOM_BACK && sauteif == 0)
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

    if(jouer == 2) lecture(clicniv,tableaucrea);
    for(int cell=0;cell<80;++cell) if(tableaucrea[cell]==0) tableaucrea[cell]=2;
    load(ecran,clicniv,tableaucrea);

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

    // Redraw the complete editor toolbar after the grid so every tool remains in front.
    const char *toolbar[] = {"back.bmp", "save.bmp", "crate_editor.bmp", "keeper_editor.bmp",
        "wall_editor.bmp", "target_editor.bmp", "eraser.bmp", "floor.bmp", "go.bmp", "retry.bmp"};
    const int toolbarX[] = {0, 68, 136, 204, 272, 340, 408, 476, 544, 612};
    for (int tool = 0; tool < 10; ++tool) {
        SDL_Surface *icon = SDL_LoadBMP((std::string("sprites_mario_sokoban/") + toolbar[tool]).c_str());
        if (icon) {
            SDL_Surface *converted = SDL_ConvertSurface(icon, ecran->format, SDL_SWSURFACE);
            SDL_Rect iconPos = {static_cast<Sint16>(toolbarX[tool] + 17), 0, 34, 34};
            if (converted) {
                blit_half_size(converted, ecran, iconPos.x, iconPos.y);
                SDL_FreeSurface(converted);
            }
            SDL_FreeSurface(icon);
        }
    }
    draw_go_label(ecran);

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
      if(posiclic.x >= 0 && posiclic.x < 68 && posiclic.y < 34)
      {
       debutcrea = 1;
       creationend = 3;
       continuer = 0;
       quita = 1;
       retourListe = (jouer == 2);//fleche retour : revenir au choix du niveau a editer
      }
      if(posiclic.x >=68*9 && posiclic.x < 68*10 && posiclic.y < 34)
      {
          // Recharge la derniere sauvegarde du niveau (sol vide s'il n'a jamais ete sauvegarde).
          for(int cell=0;cell<80;++cell) tableaucrea[cell]=0;
          lecture(level,tableaucrea);
          for(int cell=0;cell<80;++cell){
              if(tableaucrea[cell]==0) tableaucrea[cell]=2;
              SDL_Rect at={static_cast<Sint16>((cell%10)*68),static_cast<Sint16>((cell/10)*68),0,0};
              redraw_board_cell(ecran,tableaucrea[cell],at);
          }
          for(int row=0;row<8;++row){
              SDL_Rect line={0,static_cast<Sint16>(row*68),680,1};
              SDL_FillRect(ecran,&line,SDL_MapRGB(ecran->format,0,0,0));
          }
          SDL_Rect middle={68,34,544,1};
          SDL_FillRect(ecran,&middle,SDL_MapRGB(ecran->format,0,0,0));
          nombfois=1; // redessine ensuite la grille et la barre d'outils
          continuer=0;
      }
      if(posiclic.x >= 68 && posiclic.x <= 68*2 && posiclic.y >= 0 && posiclic.y < 34)
      {
          cursus = 1;
          nombfois = 0;
          continuer = 0;
          const bool saved = enregistreniv(tableaucrea,level,0);
          TTF_Font *savedFont=TTF_OpenFont("NotoSans.ttf",24);
          SDL_Rect savedPanel={150,486,380,40};
          SDL_FillRect(ecran,&savedPanel,SDL_MapRGB(ecran->format,29,43,61));
          SDL_Rect savedInset={153,489,374,34};
          SDL_FillRect(ecran,&savedInset,SDL_MapRGB(ecran->format,32,55,84));
          if(savedFont){draw_centered_game_text(ecran,savedFont,saved?"NIVEAU SAUVEGARDE":"ERREUR DE SAUVEGARDE",savedInset,saved?SDL_Color{255,225,110}:SDL_Color{255,90,90});TTF_CloseFont(savedFont);SDL_Flip(ecran);}
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
      if(posiclic.x >= 68*2 && posiclic.x <= 68*3 && posiclic.y >= 0 && posiclic.y < 34)
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
       if(posiclic.x >= 68*3 && posiclic.x <= 68*4 && posiclic.y >= 0 && posiclic.y < 34)
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
      if(posiclic.x >= 68*4 && posiclic.x <= 68*5 && posiclic.y >= 0 && posiclic.y < 34)
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
      if(posiclic.x >= 68*5 && posiclic.x <= 68*6 && posiclic.y >= 0 && posiclic.y < 34)
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
      if(posiclic.x >= 68*6 && posiclic.x < 68*7 && posiclic.y >= 0 && posiclic.y < 34)
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
      if(posiclic.x >= 68*7 && posiclic.x < 68*8 && posiclic.y >= 0 && posiclic.y < 34)
      {
          cursus = 7;
          nombfois = 0;
          continuer = 0;
      }
      if(posiclic.x >= 68*8 && posiclic.x < 68*9 && posiclic.y >= 0 && posiclic.y < 34)
      {
          //appelle fonction jouer
          int testMap[80];
          memcpy(testMap,tableaucrea,sizeof(testMap));
          // Test depuis l'editeur : pas de medaille ni de record (niveau pas forcement sauvegarde).
          returnplay =play(testMap,caisse,posicaisse,ecran,posiclic,posiretour,posireplay,posimario,mario,retour,replay,0);
          if(returnplay)
          {quit = 1;}
          else
          {
              load(ecran,level,tableaucrea);
              nombfois = 1;
              continuer = 0;
          }



      }
      continuer = 0;
      break;
       }

    }

    if(nombfois>0)
    {
      if(posiclic.x>=0 && posiclic.x<680 && posiclic.y>=34 && posiclic.y<544)
      {
        const int column=posiclic.x/68;
        const int row=posiclic.y/68;
        const int cell=row*10+column;
        int value=tableaucrea[cell];
        if(cursus==3){
            for(int old=0;old<80;++old){
                if(tableaucrea[old]==5){
                    tableaucrea[old]=2;
                    SDL_Rect oldAt={static_cast<Sint16>((old%10)*68),static_cast<Sint16>((old/10)*68),0,0};
                    redraw_board_cell(ecran,2,oldAt);
                }
            }
            value=5;
        } else if(cursus==2){value=(value==4||value==6)?6:3;}
        else if(cursus==4){value=1;}
        else if(cursus==5){value=(value==3||value==6)?6:4;}
        else if(cursus==6){value=2;}
        else if(cursus==7){value=2;}
        if(cursus>=2 && cursus<=7){
            tableaucrea[cell]=value;
            SDL_Rect at={static_cast<Sint16>(column*68),static_cast<Sint16>(row*68),0,0};
            redraw_board_cell(ecran,value,at);
        }
    }

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

    const char *toolbar[] = {"back.bmp", "save.bmp", "crate_editor.bmp", "keeper_editor.bmp",
        "wall_editor.bmp", "target_editor.bmp", "eraser.bmp", "floor.bmp", "go.bmp", "retry.bmp"};
    const int toolbarX[] = {0, 68, 136, 204, 272, 340, 408, 476, 544, 612};
    for (int tool = 0; tool < 10; ++tool) {
        SDL_Surface *icon = SDL_LoadBMP((std::string("sprites_mario_sokoban/") + toolbar[tool]).c_str());
        if (icon) {
            SDL_Surface *converted = SDL_ConvertSurface(icon, ecran->format, SDL_SWSURFACE);
            SDL_Rect iconPos = {static_cast<Sint16>(toolbarX[tool] + 17), 0, 34, 34};
            if (converted) { blit_half_size(converted, ecran, iconPos.x, iconPos.y); SDL_FreeSurface(converted); }
            SDL_FreeSurface(icon);
        }
    }
    draw_go_label(ecran);

    }
    SDL_Flip(ecran);
nombfois++;

    }while(quita == 0 && quit == 0);
     arrete = 0;
     lignesvar = 0;
    }
    }
    }while((clicniv == CUSTOM_BACK && quit == 0 && jouer == 2)||(clicniv == CUSTOM_BACK && quit == 0 && jouer == 3)||(retourListe && quit == 0));
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
#if defined(_WIN32)
    // SDL 1.2's x64 Windows shutdown path can call stale 32-bit atexit hooks.
    // End the process directly after releasing game-owned surfaces.
    std::_Exit(EXIT_SUCCESS);
#else
    SDL_Quit();
    return EXIT_SUCCESS;
#endif


}
void draw_ui_button(SDL_Surface* ecran, TTF_Font* font, const char* label, SDL_Rect bounds, bool blue)
{
    if (!ecran || !font) return;
    SDL_Rect outer = bounds;
    SDL_FillRect(ecran, &outer, SDL_MapRGB(ecran->format, 29, 43, 61));
    SDL_Rect face = {static_cast<Sint16>(bounds.x + 3), static_cast<Sint16>(bounds.y + 3),
                     static_cast<Uint16>(bounds.w - 6), static_cast<Uint16>(bounds.h - 6)};
    SDL_FillRect(ecran, &face, SDL_MapRGB(ecran->format, blue?42:82, blue?105:147, blue?188:99));
    SDL_Rect highlight = {static_cast<Sint16>(bounds.x + 7), static_cast<Sint16>(bounds.y + 6),
                          static_cast<Uint16>(bounds.w - 14), 3};
    SDL_FillRect(ecran, &highlight, SDL_MapRGB(ecran->format, blue?133:147, blue?194:190, blue?255:105));
    SDL_Color ink = {250, 239, 205};
    SDL_Surface *text = TTF_RenderText_Blended(font, label, ink);
    if (text) {
        SDL_Rect at = {static_cast<Sint16>(bounds.x + (bounds.w - text->w) / 2),
                       static_cast<Sint16>(bounds.y + (bounds.h - text->h) / 2), 0, 0};
        SDL_BlitSurface(text, NULL, ecran, &at);
        SDL_FreeSurface(text);
    }
}

void draw_game_text(SDL_Surface* ecran, TTF_Font* font, const char* label, SDL_Rect at, SDL_Color color)
{
    if (!ecran || !font || !label) return;
    SDL_Color outline = {29, 43, 61};
    // A 2 px outline swallows the 1 px strokes of small text: use 1 px there.
    const int step = BitmapScale(font) >= 2 ? 2 : 1;
    for (int dy = -step; dy <= step; dy += step)
        for (int dx = -step; dx <= step; dx += step) {
            SDL_Surface *edge = TTF_RenderText_Blended(font, label, outline);
            if (edge) {
                SDL_Rect p = {static_cast<Sint16>(at.x + dx), static_cast<Sint16>(at.y + dy), 0, 0};
                SDL_BlitSurface(edge, NULL, ecran, &p);
                SDL_FreeSurface(edge);
            }
        }
    SDL_Surface *face = TTF_RenderText_Blended(font, label, color);
    if (face) { SDL_BlitSurface(face, NULL, ecran, &at); SDL_FreeSurface(face); }
}

// Halves a 32-bit surface by averaging each 2x2 block (smooth toolbar icons,
// nearest-neighbour shrinking would drop every other line of the artwork).
void blit_half_size(SDL_Surface* source, SDL_Surface* target, int x, int y)
{
    if (!source || !target || source->format->BytesPerPixel != 4) return;
    SDL_Surface *half = SDL_CreateRGBSurface(SDL_SWSURFACE, source->w / 2, source->h / 2, 32,
        source->format->Rmask, source->format->Gmask, source->format->Bmask, 0);
    if (!half) return;
    SDL_LockSurface(source);
    SDL_LockSurface(half);
    for (int row = 0; row < half->h; ++row)
        for (int column = 0; column < half->w; ++column) {
            int sum[3] = {0, 0, 0};
            for (int dy = 0; dy < 2; ++dy)
                for (int dx = 0; dx < 2; ++dx) {
                    const Uint32 pixel = *reinterpret_cast<Uint32*>(static_cast<Uint8*>(source->pixels)
                        + (row * 2 + dy) * source->pitch + (column * 2 + dx) * 4);
                    Uint8 r, g, b;
                    SDL_GetRGB(pixel, source->format, &r, &g, &b);
                    sum[0] += r; sum[1] += g; sum[2] += b;
                }
            *reinterpret_cast<Uint32*>(static_cast<Uint8*>(half->pixels) + row * half->pitch + column * 4) =
                SDL_MapRGB(half->format, static_cast<Uint8>(sum[0] / 4), static_cast<Uint8>(sum[1] / 4), static_cast<Uint8>(sum[2] / 4));
        }
    SDL_UnlockSurface(half);
    SDL_UnlockSurface(source);
    SDL_Rect at = {static_cast<Sint16>(x), static_cast<Sint16>(y), 0, 0};
    SDL_BlitSurface(half, NULL, target, &at);
    SDL_FreeSurface(half);
}

void draw_go_label(SDL_Surface* ecran)
{
    TTF_Font *font=TTF_OpenFont("NotoSans.ttf",16);
    if(font){draw_centered_game_text(ecran,font,"GO",SDL_Rect{544,0,68,34},SDL_Color{255,250,230});TTF_CloseFont(font);}
}

void draw_level_timer(SDL_Surface* ecran, Uint32 startedTicks)
{
    if(!ecran) return;
    const unsigned int seconds=(SDL_GetTicks()-startedTicks)/1000;
    char label[32];
    snprintf(label,sizeof(label),"TIME %02u:%02u",seconds/60,seconds%60);
    SDL_Rect panel={272,7,136,40};
    SDL_FillRect(ecran,&panel,SDL_MapRGB(ecran->format,29,43,61));
    SDL_Rect inset={275,10,130,34};
    SDL_FillRect(ecran,&inset,SDL_MapRGB(ecran->format,32,55,84));
    TTF_Font *font=TTF_OpenFont("NotoSans.ttf",17);
    if(font){draw_centered_game_text(ecran,font,label,inset,SDL_Color{255,225,110});TTF_CloseFont(font);}
}

void load_best_times(int level, unsigned int times[3])
{
    times[0]=times[1]=times[2]=0;
    if(level<1 || level>BEST_TIME_ROWS) return;
    FILE *file=fopen("data/best_times.txt","r");
    if(!file) return;
    for(int row=1;row<=BEST_TIME_ROWS;++row){
        unsigned int values[3]={0,0,0};
        if(fscanf(file,"%u %u %u",&values[0],&values[1],&values[2])!=3) break;
        if(row==level){times[0]=values[0];times[1]=values[1];times[2]=values[2];break;}
    }
    fclose(file);
}

void record_best_time(int level, unsigned int seconds)
{
    if(level<1 || level>BEST_TIME_ROWS) return;
    if(seconds==0) seconds=1;
    unsigned int records[BEST_TIME_ROWS][3]={{0}};
    FILE *file=fopen("data/best_times.txt","r");
    if(file){
        for(int row=0;row<BEST_TIME_ROWS;++row)
            if(fscanf(file,"%u %u %u",&records[row][0],&records[row][1],&records[row][2])!=3) break;
        fclose(file);
    }
    unsigned int *times=records[level-1];
    for(int rank=0;rank<3;++rank){
        if(times[rank]==0 || seconds<times[rank]){
            for(int shift=2;shift>rank;--shift)times[shift]=times[shift-1];
            times[rank]=seconds;
            break;
        }
    }
    file=fopen("data/best_times.txt","w");
    if(!file) return;
    for(int row=0;row<BEST_TIME_ROWS;++row)
        fprintf(file,"%u %u %u\n",records[row][0],records[row][1],records[row][2]);
    fclose(file);
}

void clear_best_times(int level)
{
    if(level<1 || level>BEST_TIME_ROWS) return;
    unsigned int records[BEST_TIME_ROWS][3]={{0}};
    FILE *file=fopen("data/best_times.txt","r");
    if(!file) return;
    for(int row=0;row<BEST_TIME_ROWS;++row)
        if(fscanf(file,"%u %u %u",&records[row][0],&records[row][1],&records[row][2])!=3) break;
    fclose(file);
    records[level-1][0]=records[level-1][1]=records[level-1][2]=0;
    file=fopen("data/best_times.txt","w");
    if(!file) return;
    for(int row=0;row<BEST_TIME_ROWS;++row)
        fprintf(file,"%u %u %u\n",records[row][0],records[row][1],records[row][2]);
    fclose(file);
}

void draw_best_times(SDL_Surface* ecran)
{
    if(!ecran) return;
    SDL_Rect panel={12,425,656,109};
    SDL_FillRect(ecran,&panel,SDL_MapRGB(ecran->format,29,43,61));
    SDL_Rect inset={15,428,650,103};
    SDL_FillRect(ecran,&inset,SDL_MapRGB(ecran->format,32,55,84));
    TTF_Font *font=TTF_OpenFont("NotoSans.ttf",12);
    if(!font) return;
    draw_centered_game_text(ecran,font,"TOP 3 - MEILLEURS TEMPS",SDL_Rect{180,429,320,17},SDL_Color{255,225,110});
    const SDL_Color rankColors[3]={{255,225,110},{210,225,239},{236,166,112}};
    for(int level=1;level<=6;++level){
        const int x=22+(level-1)*106;
        SDL_Rect card={static_cast<Sint16>(x),450,100,78};
        SDL_FillRect(ecran,&card,SDL_MapRGB(ecran->format,38,66,96));
        char heading[16];snprintf(heading,sizeof(heading),"NIVEAU %d",level);
        draw_centered_game_text(ecran,font,heading,SDL_Rect{static_cast<Sint16>(x),451,100,13},SDL_Color{255,245,205});
        unsigned int times[3];load_best_times(level,times);
        for(int rank=0;rank<3;++rank){
            char line[24];
            if(times[rank])snprintf(line,sizeof(line),"%d. %02u:%02u",rank+1,times[rank]/60,times[rank]%60);
            else snprintf(line,sizeof(line),"%d. --:--",rank+1);
            draw_game_text(ecran,font,line,SDL_Rect{static_cast<Sint16>(x+18),static_cast<Sint16>(466+rank*18),0,0},rankColors[rank]);
        }
    }
    TTF_CloseFont(font);
}

void draw_level_thumbnail(SDL_Surface* ecran, const int map[80], int x, int y, int width, int height)
{
    SDL_Rect frame = {static_cast<Sint16>(x), static_cast<Sint16>(y),
                      static_cast<Uint16>(width), static_cast<Uint16>(height)};
    SDL_FillRect(ecran, &frame, SDL_MapRGB(ecran->format, 29, 43, 61));
    const int cellW = (width - 8) / 10, cellH = (height - 8) / 8;
    for (int i = 0; i < 80; ++i) {
        SDL_Rect cell = {static_cast<Sint16>(x + 4 + (i % 10) * cellW),
                         static_cast<Sint16>(y + 4 + (i / 10) * cellH),
                         static_cast<Uint16>(cellW), static_cast<Uint16>(cellH)};
        int value = map[i]; Uint8 r=181,g=205,b=158;
        if (value==1) {r=48;g=81;b=104;}
        else if (value==3||value==6) {r=174;g=112;b=61;}
        else if (value==4) {r=244;g=198;b=93;}
        else if (value==5) {r=202;g=83;b=79;}
        SDL_FillRect(ecran,&cell,SDL_MapRGB(ecran->format,r,g,b));
        if (value==6) {
            SDL_Rect goal={static_cast<Sint16>(cell.x+cell.w/4),static_cast<Sint16>(cell.y+cell.h/4),
                           static_cast<Uint16>(cell.w/2),static_cast<Uint16>(cell.h/2)};
            SDL_FillRect(ecran,&goal,SDL_MapRGB(ecran->format,255,220,115));
        }
    }
}

void redraw_board_cell(SDL_Surface* ecran, int value, SDL_Rect at)
{
    SDL_Surface *tile=SDL_LoadBMP("sprites_mario_sokoban/floor.bmp");
    if(tile){SDL_BlitSurface(tile,NULL,ecran,&at);SDL_FreeSurface(tile);}
    const char *asset=NULL;
    if(value==1)asset="sprites_mario_sokoban/wall.bmp";
    else if(value==3)asset="sprites_mario_sokoban/crate.bmp";
    else if(value==4)asset="sprites_mario_sokoban/target.bmp";
    else if(value==6)asset="sprites_mario_sokoban/crate_target.bmp";
    if(value==5){tile=load_character_png("sprites_mario_sokoban/keeper_down.png");if(tile){SDL_BlitSurface(tile,NULL,ecran,&at);SDL_FreeSurface(tile);}}
    else if(asset){tile=SDL_LoadBMP(asset);if(tile){SDL_BlitSurface(tile,NULL,ecran,&at);SDL_FreeSurface(tile);}}
}

void draw_player_on_floor(SDL_Surface* ecran, SDL_Surface* player, SDL_Rect at)
{
    if (!ecran || !player) return;
    SDL_Surface *floor=SDL_LoadBMP("sprites_mario_sokoban/floor.bmp");
    if(floor){SDL_BlitSurface(floor,NULL,ecran,&at);SDL_FreeSurface(floor);}
    SDL_BlitSurface(player,NULL,ecran,&at);
}

void draw_multicolor_victory(SDL_Surface* ecran, TTF_Font* font, int centerX, int y)
{
    if(!ecran || !font) return;
    const char *title="VICTOIRE";
    const SDL_Color colors[]={{255,93,87},{255,177,66},{255,230,90},{118,220,115},
                              {91,190,255},{164,139,255},{255,124,193},{255,245,205}};
    SDL_Surface *measure=TTF_RenderText_Blended(font,title,SDL_Color{255,255,255});
    if(!measure) return;
    const int width=measure->w;
    SDL_FreeSurface(measure);
    int x=centerX-width/2;
    for(int i=0;title[i];++i){
        char glyph[2]={title[i],0};
        SDL_Surface *letter=TTF_RenderText_Blended(font,glyph,colors[i%8]);
        if(!letter) continue;
        SDL_Rect at={static_cast<Sint16>(x),static_cast<Sint16>(y),0,0};
        draw_game_text(ecran,font,glyph,at,colors[i%8]);
        x+=letter->w;
        SDL_FreeSurface(letter);
    }
}

SDL_Surface* load_character_png(const char* pngPath)
{
#ifdef _WIN32
    HRESULT comInit = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
    bool uninitializeCom = SUCCEEDED(comInit);
    IWICImagingFactory *factory = NULL;
    IWICBitmapDecoder *decoder = NULL;
    IWICBitmapFrameDecode *frame = NULL;
    IWICFormatConverter *converter = NULL;
    SDL_Surface *surface = NULL;
    WCHAR widePath[MAX_PATH] = {0};
    if (MultiByteToWideChar(CP_UTF8, 0, pngPath, -1, widePath, MAX_PATH) > 0 &&
        SUCCEEDED(CoCreateInstance(CLSID_WICImagingFactory, NULL, CLSCTX_INPROC_SERVER,
                                   IID_PPV_ARGS(&factory))) &&
        SUCCEEDED(factory->CreateDecoderFromFilename(widePath, NULL, GENERIC_READ,
                                                      WICDecodeMetadataCacheOnLoad, &decoder)) &&
        SUCCEEDED(decoder->GetFrame(0, &frame)) &&
        SUCCEEDED(factory->CreateFormatConverter(&converter))) {
        UINT width = 0, height = 0;
        if (SUCCEEDED(frame->GetSize(&width, &height)) && width > 0 && height > 0 &&
            width <= 4096 && height <= 4096 &&
            SUCCEEDED(converter->Initialize(frame, GUID_WICPixelFormat32bppRGBA,
                         WICBitmapDitherTypeNone, NULL, 0.0, WICBitmapPaletteTypeCustom))) {
            std::vector<Uint8> rgba(static_cast<size_t>(width) * height * 4);
            if (SUCCEEDED(converter->CopyPixels(NULL, width * 4,
                    static_cast<UINT>(rgba.size()), rgba.data()))) {
                surface = SDL_CreateRGBSurface(SDL_SWSURFACE, width, height, 32,
                    0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000);
                if (surface) {
                    if (SDL_MUSTLOCK(surface)) SDL_LockSurface(surface);
                    for (UINT row = 0; row < height; ++row)
                        memcpy(static_cast<Uint8*>(surface->pixels) + row * surface->pitch,
                               rgba.data() + static_cast<size_t>(row) * width * 4, width * 4);
                    if (SDL_MUSTLOCK(surface)) SDL_UnlockSurface(surface);
                    SDL_SetAlpha(surface, SDL_SRCALPHA, 255);
                }
            }
        }
    }
    if (converter) converter->Release();
    if (frame) frame->Release();
    if (decoder) decoder->Release();
    if (factory) factory->Release();
    if (uninitializeCom) CoUninitialize();
    if (surface) return surface;
#endif
    std::string bmpPath = pngPath ? pngPath : "";
    const std::string suffix = ".png";
    if (bmpPath.size() >= suffix.size() && bmpPath.compare(bmpPath.size()-suffix.size(), suffix.size(), suffix) == 0)
        bmpPath.replace(bmpPath.size()-suffix.size(), suffix.size(), ".bmp");
    return SDL_LoadBMP(bmpPath.c_str());
}

void attendre_fermeture()
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
    TTF_Init();
    SDL_Surface *fond=SDL_LoadBMP("sprites_mario_sokoban/garden_detail.bmp");
    if(fond){SDL_Rect origin={0,0,0,0};SDL_BlitSurface(fond,NULL,ecran,&origin);SDL_FreeSurface(fond);}
    TTF_Font *font=TTF_OpenFont("NotoSans.ttf",30);
    if(font){
        SDL_Rect title={202,48,0,0};
        draw_game_text(ecran,font,"CHOISIS TON NIVEAU",title,SDL_Color{255,245,205});
        const int xs[6]={120,285,450,120,285,450};
        const int ys[6]={150,150,150,250,250,250};
        SDL_Surface *badge=SDL_LoadBMP("sprites_mario_sokoban/medal_small.bmp");
        for(int level=1;level<=6;++level){
            int index=level-1, map[80];
            for(int cell=0;cell<80;++cell)map[cell]=tableau(level,cell);
            SDL_Rect card={static_cast<Sint16>(xs[index]),static_cast<Sint16>(ys[index]),74,76};
            draw_ui_button(ecran,font," ",card);
            draw_level_thumbnail(ecran,map,xs[index]+7,ys[index]+7,60,39);
            char label[4];snprintf(label,sizeof(label),"%d",level);
            SDL_Rect number={static_cast<Sint16>(xs[index]+31),static_cast<Sint16>(ys[index]+49),0,0};
            draw_game_text(ecran,font,label,number,SDL_Color{255,245,205});
            char progressPath[64]; snprintf(progressPath,sizeof(progressPath),"data/completed%d.lvl",level);
            FILE *progress=fopen(progressPath,"r"); int completed=0;
            if(progress){fscanf(progress,"%d",&completed);fclose(progress);}
            if(completed && badge){SDL_Rect medalAt={static_cast<Sint16>(xs[index]+48),static_cast<Sint16>(ys[index]+5),0,0};SDL_BlitSurface(badge,NULL,ecran,&medalAt);}
        }
        if(badge)SDL_FreeSurface(badge);
        draw_ui_button(ecran,font,"NIVEAUX PERSO",SDL_Rect{120,350,404,70});
        TTF_CloseFont(font);
        draw_best_times(ecran);
    }
    SDL_Flip(ecran);
    TTF_Quit();
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

    carre = SDL_LoadBMP("sprites_mario_sokoban/floor.bmp");
    if (carre) {
        for (int cell = 0; cell < 80; ++cell) {
            posicarre.x = static_cast<Sint16>((cell % 10) * 68);
            posicarre.y = static_cast<Sint16>((cell / 10) * 68);
            SDL_BlitSurface(carre, NULL, ecran, &posicarre);
        }
        SDL_FreeSurface(carre);
        carre = NULL;
    }
    const int drawOrder[] = {1, 4, 3, 6, 5};
    int order = 0;
    while(order < 5)
    {
        a = drawOrder[order];
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
                    if( a == 6)
                    {
                        carre = SDL_LoadBMP ("sprites_mario_sokoban/crate_target.bmp");
                        SDL_BlitSurface(carre , NULL, ecran, &posicarre );
                    }
                    if( a == 5)
                    {

                        carre = load_character_png("sprites_mario_sokoban/keeper_up.png");
                         SDL_BlitSurface(carre , NULL, ecran, &posicarre );


                    }

                }
                i++;
            }
            order++;
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

int victoire(SDL_Surface* ecran,unsigned int elapsedSeconds)
{
    TTF_Init();
    SDL_Surface *background=SDL_LoadBMP("sprites_mario_sokoban/garden_detail.bmp");
    if(background){SDL_Rect at={0,0,0,0};SDL_BlitSurface(background,NULL,ecran,&at);SDL_FreeSurface(background);}
    SDL_Surface *medal=SDL_LoadBMP("sprites_mario_sokoban/medal.bmp");
    if(medal){SDL_Rect at={306,98,0,0};SDL_BlitSurface(medal,NULL,ecran,&at);SDL_FreeSurface(medal);}
    TTF_Font *font=TTF_OpenFont("NotoSans.ttf",54);
    if(font){
        draw_multicolor_victory(ecran,font,340,28);
        draw_centered_game_text(ecran,font,"NIVEAU TERMINE",SDL_Rect{70,170,540,40},SDL_Color{250,239,205});
        TTF_CloseFont(font);
        font=TTF_OpenFont("NotoSans.ttf",40);
        char elapsed[40];snprintf(elapsed,sizeof(elapsed),"TEMPS %02u:%02u",elapsedSeconds/60,elapsedSeconds%60);
        if(font)draw_centered_game_text(ecran,font,elapsed,SDL_Rect{170,216,340,34},SDL_Color{255,225,110});
        if(font)TTF_CloseFont(font);
        font=TTF_OpenFont("NotoSans.ttf",36);
        draw_ui_button(ecran,font,"REJOUER",SDL_Rect{100,270,165,80});
        draw_ui_button(ecran,font,"NIVEAU",SDL_Rect{400,270,180,80});
        draw_ui_button(ecran,font,"MENU",SDL_Rect{255,370,154,80});
        if(font)TTF_CloseFont(font);
    }
    SDL_Flip(ecran);
    SDL_Event event;
    for(;;){
        SDL_WaitEvent(&event);


        if(event.type==SDL_QUIT){TTF_Quit();return 4;}
        if(event.type==SDL_MOUSEBUTTONUP){
            SDL_Rect click={static_cast<Sint16>(event.button.x),static_cast<Sint16>(event.button.y),0,0};
            int choice=choixniveau(click);
            if(choice){TTF_Quit();return choice;}
        }
    }
}

int choixniveau(SDL_Rect posiclic)
{
    if(posiclic.x>=100&&posiclic.x<=265&&posiclic.y>=270&&posiclic.y<=350)return 2;
    if(posiclic.x>=400&&posiclic.x<=580&&posiclic.y>=270&&posiclic.y<=350)return 1;
    if(posiclic.x>=255&&posiclic.x<=409&&posiclic.y>=370&&posiclic.y<=450)return 3;
    return 0;
}

void affvict(SDL_Surface* ecran,SDL_Rect posivict, int fois,SDL_Surface* affiche)
{
    TTF_Init();
    SDL_Rect panel={18,452,644,80},inner={22,456,636,72};
    SDL_FillRect(ecran,&panel,SDL_MapRGB(ecran->format,29,43,61));
    SDL_FillRect(ecran,&inner,SDL_MapRGB(ecran->format,32,55,84));
    SDL_Surface *medal=SDL_LoadBMP("sprites_mario_sokoban/medal.bmp");
    if(medal){SDL_Rect at={30,458,0,0};SDL_BlitSurface(medal,NULL,ecran,&at);SDL_FreeSurface(medal);}
    TTF_Font *font=TTF_OpenFont("NotoSans.ttf",27);
    if(font){
        posivict.x=110;posivict.y=476;
        draw_multicolor_victory(ecran,font,215,476);
        char count[40];snprintf(count,sizeof(count),"NIVEAU %d TERMINE",fois);
        posivict.x=270;draw_game_text(ecran,font,count,posivict,SDL_Color{250,239,205});
        TTF_CloseFont(font);
    }
    SDL_Flip(ecran);TTF_Quit();
}

void save(int niveau)
{
    if(niveau>=1 && niveau<=6){
        char completedPath[64];snprintf(completedPath,sizeof(completedPath),"data/completed%d.lvl",niveau);
        FILE *completed=fopen(completedPath,"w");
        if(completed){fputs("1",completed);fclose(completed);}
    }
}
void afftrait(int jaune,SDL_Surface* ecran,SDL_Rect positionlignes,SDL_Surface*colleimagecrea )
{
    (void)colleimagecrea;
    // Underline of the editor toolbar: red for the selected tool, white otherwise.
    SDL_Rect bar={positionlignes.x,positionlignes.y,68,3};
    if(jaune == 1) SDL_FillRect(ecran,&bar,SDL_MapRGB(ecran->format,235,55,65));
    else SDL_FillRect(ecran,&bar,SDL_MapRGB(ecran->format,255,255,255));
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
bool enregistreniv(int tableau[],int niveaufichier,int eff)
{
    FILE* fichier = NULL;
    if(niveaufichier<1 || niveaufichier>CUSTOM_LEVEL_COUNT) return false;
    for(int i=0;i<80;++i) if(tableau[i]==0) tableau[i]=2;
    char path[64];
    custom_level_path(niveaufichier,path,sizeof(path));
    fichier=fopen(path,"w");
    if(!fichier) return false;
    bool okay=fprintf(fichier," %d",eff==0?2:0)>=0;
    for(int i=0;i<80;++i) if(fprintf(fichier," %d",tableau[i])<0) okay=false;
    if(fflush(fichier)!=0) okay=false;
    if(fclose(fichier)!=0) okay=false;
    return okay;
}
void reset_custom_level(int niveau)
{
    if(niveau<1 || niveau>CUSTOM_LEVEL_COUNT) return;
    int empty[80]={0};
    enregistreniv(empty,niveau,1);
    char completedPath[64];snprintf(completedPath,sizeof(completedPath),"data/custom_completed%d.lvl",niveau);
    remove(completedPath);
    clear_best_times(niveau+BUILTIN_LEVEL_COUNT);
}
void lecture(int niveau,int tableauniv[])
{
    FILE* fichier = NULL;
    if(niveau >= 1 && niveau <= CUSTOM_LEVEL_COUNT)
    {
        char path[64];
        custom_level_path(niveau,path,sizeof(path));
        fichier = fopen(path, "r");
    }
    if(fichier){
        int format=0;
        if(fscanf(fichier,"%d",&format)==1){
            if(format>=2){
                for(int cell=0;cell<80;++cell)
                    if(fscanf(fichier,"%d",&tableauniv[cell])!=1) tableauniv[cell]=2;
            }else if(format==1){
                // Legacy files used the first entry as the enabled flag and could not store cell 0.
                tableauniv[0]=1;
                for(int cell=1;cell<80;++cell)
                    if(fscanf(fichier,"%d",&tableauniv[cell])!=1) tableauniv[cell]=2;
            }
        }
        fclose(fichier);
    }
}

int niveaucrea(SDL_Surface* ecran,SDL_Rect posiclic)
{
    TTF_Init();
    const int cardX[3]={20,245,470}, cardY[3]={96,236,376};
    const SDL_Rect prevButton={20,504,96,34}, nextButton={564,504,96,34};
    const int pages=CUSTOM_PAGE_COUNT;
    if(g_customPage<0) g_customPage=0;
    if(g_customPage>=pages) g_customPage=pages-1;
    for(;;){
    const int firstLevel=g_customPage*CUSTOM_LEVELS_PER_PAGE;
    SDL_Surface *background=SDL_LoadBMP("sprites_mario_sokoban/garden_detail.bmp");
    if(background){SDL_Rect at={0,0,0,0};SDL_BlitSurface(background,NULL,ecran,&at);SDL_FreeSurface(background);}
    SDL_Surface *back=SDL_LoadBMP("sprites_mario_sokoban/back.bmp");
    if(back){SDL_Rect at={0,0,0,0};SDL_BlitSurface(back,NULL,ecran,&at);SDL_FreeSurface(back);}
    TTF_Font *font=TTF_OpenFont("NotoSans.ttf",28);
    if(font){
        SDL_Rect title={202,44,0,0};
        draw_game_text(ecran,font,"NIVEAUX PERSONNELS",title,SDL_Color{255,245,205});
        for(int slot=1;slot<=CUSTOM_LEVELS_PER_PAGE;++slot){
            const int level=firstLevel+slot;
            int map[80]={0}; lecture(level,map);
            int col=(slot-1)%3,row=(slot-1)/3;
            SDL_Rect card={static_cast<Sint16>(cardX[col]),static_cast<Sint16>(cardY[row]),190,124};
            draw_ui_button(ecran,font," ",card);
            draw_level_thumbnail(ecran,map,cardX[col]+8,cardY[row]+8,174,58);
            char label[24]; snprintf(label,sizeof(label),"NIVEAU %d",level);
            SDL_Rect labelAt={static_cast<Sint16>(cardX[col]+12),static_cast<Sint16>(cardY[row]+68),0,0};
            draw_game_text(ecran,font,label,labelAt,SDL_Color{255,245,205});
            unsigned int best[3];load_best_times(level+BUILTIN_LEVEL_COUNT,best);
            TTF_Font *tiny=TTF_OpenFont("NotoSans.ttf",8);
            if(tiny){for(int rank=0;rank<3;++rank){char timeLine[24];
                if(best[rank])snprintf(timeLine,sizeof(timeLine),"%d   %02u:%02u",rank+1,best[rank]/60,best[rank]%60);
                else snprintf(timeLine,sizeof(timeLine),"%d   --:--",rank+1);
                draw_game_text(ecran,tiny,timeLine,SDL_Rect{static_cast<Sint16>(cardX[col]+12),static_cast<Sint16>(cardY[row]+81+rank*9),0,0},rank==0?SDL_Color{255,225,110}:SDL_Color{250,239,205});}
                TTF_CloseFont(tiny);}

            char completedPath[64];snprintf(completedPath,sizeof(completedPath),"data/custom_completed%d.lvl",level);
            FILE *progress=fopen(completedPath,"r");int completed=0;
            if(progress){fscanf(progress,"%d",&completed);fclose(progress);}
            if(completed){SDL_Surface *badge=SDL_LoadBMP("sprites_mario_sokoban/medal_small.bmp");if(badge){SDL_Rect medalAt={static_cast<Sint16>(cardX[col]+132),static_cast<Sint16>(cardY[row]+4),0,0};SDL_BlitSurface(badge,NULL,ecran,&medalAt);SDL_FreeSurface(badge);}}
        }
        TTF_CloseFont(font);
    }
    TTF_Font *pageFont=TTF_OpenFont("NotoSans.ttf",20);
    if(pageFont){
        char pageLabel[24]; snprintf(pageLabel,sizeof(pageLabel),"PAGE %d / %d",g_customPage+1,pages);
        SDL_Rect pagePanel={250,504,180,34};
        SDL_FillRect(ecran,&pagePanel,SDL_MapRGB(ecran->format,29,43,61));
        draw_centered_game_text(ecran,pageFont,pageLabel,pagePanel,SDL_Color{255,225,110});
        if(g_customPage>0) draw_ui_button(ecran,pageFont,"< PREC",prevButton);
        if(g_customPage<pages-1) draw_ui_button(ecran,pageFont,"SUIV >",nextButton,true);
        TTF_CloseFont(pageFont);
    }
    SDL_Flip(ecran);
    SDL_Event event;
    int changePage=0;
    while(changePage==0){
        SDL_WaitEvent(&event);
        if(event.type==SDL_QUIT){TTF_Quit();return 0;}
        if(event.type==SDL_KEYDOWN){
            const SDLKey key=event.key.keysym.sym;
            if((key==SDLK_LEFT||key==SDLK_PAGEUP)&&g_customPage>0) changePage=-1;
            if((key==SDLK_RIGHT||key==SDLK_PAGEDOWN)&&g_customPage<pages-1) changePage=1;
            continue;
        }
        if(event.type!=SDL_MOUSEBUTTONUP)continue;
        posiclic.x=event.button.x;posiclic.y=event.button.y;
        if(posiclic.x<=68&&posiclic.y<=68){TTF_Quit();return CUSTOM_BACK;}
        if(g_customPage>0&&posiclic.x>=prevButton.x&&posiclic.x<=prevButton.x+prevButton.w&&
           posiclic.y>=prevButton.y&&posiclic.y<=prevButton.y+prevButton.h){changePage=-1;continue;}
        if(g_customPage<pages-1&&posiclic.x>=nextButton.x&&posiclic.x<=nextButton.x+nextButton.w&&
           posiclic.y>=nextButton.y&&posiclic.y<=nextButton.y+nextButton.h){changePage=1;continue;}
        for(int slot=1;slot<=CUSTOM_LEVELS_PER_PAGE;++slot){
            int col=(slot-1)%3,row=(slot-1)/3;
            if(posiclic.x>=cardX[col]&&posiclic.x<=cardX[col]+190&&
               posiclic.y>=cardY[row]&&posiclic.y<=cardY[row]+124){TTF_Quit();return firstLevel+slot;}
        }
    }
    g_customPage+=changePage;
    }
}

int tobesur(SDL_Surface* ecran,SDL_Rect posiclic)
{
    TTF_Init();
    SDL_Rect panel={70,150,540,230};
    SDL_FillRect(ecran,&panel,SDL_MapRGB(ecran->format,29,43,61));
    SDL_Rect inset={74,154,532,222};
    SDL_FillRect(ecran,&inset,SDL_MapRGB(ecran->format,32,55,84));
    TTF_Font *font=TTF_OpenFont("NotoSans.ttf",26);
    if(font){
        draw_centered_game_text(ecran,font,"REINITIALISER CE NIVEAU ?",SDL_Rect{74,172,532,30},SDL_Color{255,225,110});
        TTF_CloseFont(font);
    }
    font=TTF_OpenFont("NotoSans.ttf",14);
    if(font){
        draw_centered_game_text(ecran,font,"LE NIVEAU, SA MEDAILLE ET SES TEMPS SERONT EFFACES",SDL_Rect{74,215,532,20},SDL_Color{250,239,205});
        TTF_CloseFont(font);
    }
    font=TTF_OpenFont("NotoSans.ttf",36);
    if(font){
        draw_ui_button(ecran,font,"OUI",SDL_Rect{130,270,180,70});
        draw_ui_button(ecran,font,"NON",SDL_Rect{370,270,180,70},true);
        TTF_CloseFont(font);
    }
    SDL_Flip(ecran);
    SDL_Event event;
    for(;;){
        SDL_WaitEvent(&event);
        if(event.type==SDL_QUIT){TTF_Quit();return 4;}
        if(event.type!=SDL_MOUSEBUTTONUP) continue;
        posiclic.x=event.button.x; posiclic.y=event.button.y;
        if(posiclic.x>=130&&posiclic.x<=310&&posiclic.y>=270&&posiclic.y<=340){TTF_Quit();return 1;}
        if(posiclic.x>=370&&posiclic.x<=550&&posiclic.y>=270&&posiclic.y<=340){TTF_Quit();return 0;}
    }
}
int play(int tableau1[],SDL_Surface* caisse,SDL_Rect posicaisse,SDL_Surface* ecran,SDL_Rect posiclic,SDL_Rect posiretour,SDL_Rect posireplay,SDL_Rect posimario,SDL_Surface* mario,SDL_Surface* retour,SDL_Surface* replay,int customLevel)
{
        int finpartie = 2,quit = 0,niveau =0,endwhile = 0,continuer = 1,fois= 0,ouinon =0,mariohere = 0;
        int i =0,a=0,objec =0;
        int objectif[80] ={0};
        int tableaufixe[80]={0};
        SDL_Event event;

    i = 0;
     while(i<80)
    {
    if(tableau1[i]==0) tableau1[i]=2; // case jamais peinte dans l'editeur = sol
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
      Uint32 customTimerStarted=SDL_GetTicks();
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
    draw_level_timer(ecran,customTimerStarted);
    SDL_Flip(ecran);


   continuer = 1;
   int customShownSeconds=-1;

fois=0;



   while (continuer == 1 && quit == 0 && endwhile == 0)
       {
         if(!SDL_PollEvent(&event)){
             int seconds=static_cast<int>((SDL_GetTicks()-customTimerStarted)/1000);
             if(seconds!=customShownSeconds){draw_level_timer(ecran,customTimerStarted);SDL_Flip(ecran);customShownSeconds=seconds;}
             SDL_Delay(100);
             continue;
         }


             switch(event.type)
           {
          case SDL_QUIT:
              continuer = 0;
              return 1;
             break;
         case SDL_KEYDOWN:
         if(event.key.keysym.sym == SDLK_UP || event.key.keysym.sym == SDLK_DOWN
            || event.key.keysym.sym == SDLK_RIGHT || event.key.keysym.sym == SDLK_LEFT)
        {
         redraw_board_cell(ecran, 2, posimario);
     }
         if(event.key.keysym.sym == SDLK_UP)
     {

         posimario.y -= 68;

     mario = load_character_png("sprites_mario_sokoban/keeper_up.png");
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
     mario = load_character_png("sprites_mario_sokoban/keeper_down.png");
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
     mario = load_character_png("sprites_mario_sokoban/keeper_right.png");

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
     mario = load_character_png("sprites_mario_sokoban/keeper_left.png");
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
       customTimerStarted=SDL_GetTicks();
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
    if(customLevel>=1 && customLevel<=CUSTOM_LEVEL_COUNT){
        record_best_time(customLevel+BUILTIN_LEVEL_COUNT,(SDL_GetTicks()-customTimerStarted)/1000);
        char completedPath[64];snprintf(completedPath,sizeof(completedPath),"data/custom_completed%d.lvl",customLevel);
        FILE *completed=fopen(completedPath,"w");
        if(completed){fputs("1",completed);fclose(completed);}
    }
    ouinon=affonlyvictyouinon(ecran,posiclic,(SDL_GetTicks()-customTimerStarted)/1000);
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
draw_player_on_floor(ecran, mario, posimario);
draw_level_timer(ecran,customTimerStarted); /* On
place mario à sa nouvelle position */
SDL_Flip(ecran); /* On met à jour l'affichage */


}
}
return 0;

}
int affonlyvictyouinon(SDL_Surface* ecran,SDL_Rect posiclic,unsigned int elapsedSeconds)
{
    TTF_Init();
    SDL_Surface *background=SDL_LoadBMP("sprites_mario_sokoban/garden_detail.bmp");
    if(background){SDL_Rect origin={0,0,0,0};SDL_BlitSurface(background,NULL,ecran,&origin);SDL_FreeSurface(background);}
    SDL_Surface *medal=SDL_LoadBMP("sprites_mario_sokoban/medal.bmp");
    if(medal){SDL_Rect at={306,84,0,0};SDL_BlitSurface(medal,NULL,ecran,&at);SDL_FreeSurface(medal);}
    TTF_Font *font=TTF_OpenFont("NotoSans.ttf",52);
    SDL_Rect title={232,35,0,0};
    draw_multicolor_victory(ecran,font,340,35);
    char elapsed[40];snprintf(elapsed,sizeof(elapsed),"TEMPS %02u:%02u",elapsedSeconds/60,elapsedSeconds%60);
    draw_centered_game_text(ecran,font,elapsed,SDL_Rect{170,158,340,38},SDL_Color{255,225,110});
    if(font)TTF_CloseFont(font);
    font=TTF_OpenFont("NotoSans.ttf",44);
    SDL_Rect prompt={160,205,0,0};
    draw_game_text(ecran,font,"VEUX-TU REJOUER ?",prompt,SDL_Color{250,239,205});
    draw_ui_button(ecran,font,"REJOUER",SDL_Rect{20,250,277,70});
    draw_ui_button(ecran,font,"MENU",SDL_Rect{320,250,300,70});
    if(font)TTF_CloseFont(font);
    SDL_Flip(ecran);
    SDL_Event event;
    for(;;){
        SDL_WaitEvent(&event);


        if(event.type==SDL_QUIT){TTF_Quit();return 4;}
        if(event.type==SDL_MOUSEBUTTONUP){
            if(event.button.x>=20&&event.button.x<=297&&event.button.y>=250&&event.button.y<=320){TTF_Quit();return 1;}
            if(event.button.x>=320&&event.button.x<=620&&event.button.y>=250&&event.button.y<=320){TTF_Quit();return 0;}
        }
    }
}

int affcomplet(SDL_Surface* ecran,SDL_Rect posiclic)
{
    TTF_Init();
    SDL_Surface *background=SDL_LoadBMP("sprites_mario_sokoban/garden_detail.bmp");
    if(background){SDL_Rect at={0,0,0,0};SDL_BlitSurface(background,NULL,ecran,&at);SDL_FreeSurface(background);}
    SDL_Surface *icon=SDL_LoadBMP("sprites_mario_sokoban/back.bmp");
    if(icon){SDL_Rect at={0,0,0,0};SDL_BlitSurface(icon,NULL,ecran,&at);SDL_FreeSurface(icon);}
    icon=SDL_LoadBMP("sprites_mario_sokoban/retry.bmp");
    if(icon){SDL_Rect at={68*9,0,0,0};SDL_BlitSurface(icon,NULL,ecran,&at);SDL_FreeSurface(icon);}
    SDL_Rect panel={70,170,540,200};
    SDL_FillRect(ecran,&panel,SDL_MapRGB(ecran->format,29,43,61));
    SDL_Rect inset={74,174,532,192};
    SDL_FillRect(ecran,&inset,SDL_MapRGB(ecran->format,32,55,84));
    TTF_Font *font=TTF_OpenFont("NotoSans.ttf",40);
    if(font){
        draw_centered_game_text(ecran,font,"NIVEAU INCOMPLET",SDL_Rect{74,192,532,40},SDL_Color{255,225,110});
        draw_centered_game_text(ecran,font,"OU VIDE",SDL_Rect{74,240,532,40},SDL_Color{255,225,110});
        TTF_CloseFont(font);
    }
    font=TTF_OpenFont("NotoSans.ttf",14);
    if(font){
        draw_centered_game_text(ecran,font,"IL FAUT UN JOUEUR, DES CAISSES ET DES OBJECTIFS",SDL_Rect{74,310,532,20},SDL_Color{250,239,205});
        TTF_CloseFont(font);
    }
    SDL_Flip(ecran);
    SDL_Event event;
    for(;;){
        SDL_WaitEvent(&event);
        if(event.type==SDL_QUIT){TTF_Quit();return 4;}
        if(event.type!=SDL_MOUSEBUTTONUP) continue;
        posiclic.x=event.button.x; posiclic.y=event.button.y;
        if(posiclic.x>=0&&posiclic.x<=68&&posiclic.y>=0&&posiclic.y<=68){TTF_Quit();return 0;}
        if(posiclic.x>=68*9&&posiclic.x<=680&&posiclic.y>=0&&posiclic.y<=68){TTF_Quit();return 1;}
    }
}
