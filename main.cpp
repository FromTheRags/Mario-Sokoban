#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>

#include <array>
#include <fstream>
#include <iostream>
#include <string>

namespace {
constexpr int kCols = 10, kRows = 8, kCells = kCols * kRows, kTile = 68;
constexpr int kWidth = kCols * kTile, kHeight = kRows * kTile;
enum class Screen { Menu, Levels, Game };

struct Level {
    std::array<bool, kCells> walls{}, goals{}, boxes{};
    int player = -1;
};
struct Assets {
    SDL_Texture *menu = nullptr, *background = nullptr, *floor = nullptr, *wall = nullptr;
    SDL_Texture *goal = nullptr, *box = nullptr, *boxGoal = nullptr;
    SDL_Texture *playerDown = nullptr, *playerUp = nullptr, *playerLeft = nullptr, *playerRight = nullptr;
    SDL_Texture *back = nullptr, *retry = nullptr, *save = nullptr, *eraser = nullptr, *play = nullptr;
    SDL_Texture *crateTool = nullptr, *keeperTool = nullptr, *wallTool = nullptr, *targetTool = nullptr;
    TTF_Font *font = nullptr;
};

SDL_Texture *texture(SDL_Renderer *renderer, const char *path) {
    SDL_Surface *surface = SDL_LoadBMP(path);
    if (!surface) { std::cerr << "Cannot load " << path << ": " << SDL_GetError() << '\n'; return nullptr; }
    SDL_Texture *result = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_DestroySurface(surface);
    return result;
}
bool loadAssets(SDL_Renderer *r, Assets &a) {
#define LOAD(field, file) a.field = texture(r, "sprites_mario_sokoban/" file)
    LOAD(menu,"menu.bmp"); LOAD(background,"garden_detail.bmp"); LOAD(floor,"floor.bmp");
    LOAD(wall,"wall.bmp"); LOAD(goal,"target.bmp"); LOAD(box,"crate.bmp"); LOAD(boxGoal,"crate_target.bmp");
    LOAD(playerDown,"keeper_down.bmp"); LOAD(playerUp,"keeper_up.bmp"); LOAD(playerLeft,"keeper_left.bmp"); LOAD(playerRight,"keeper_right.bmp");
    LOAD(back,"back.bmp"); LOAD(retry,"retry.bmp"); LOAD(save,"save.bmp"); LOAD(eraser,"eraser.bmp"); LOAD(play,"play.bmp");
    LOAD(crateTool,"crate_editor.bmp"); LOAD(keeperTool,"keeper_editor.bmp"); LOAD(wallTool,"wall_editor.bmp"); LOAD(targetTool,"target_editor.bmp");
#undef LOAD
    a.font = TTF_OpenFont("NotoSans.ttf", 26.0F);
    return a.menu && a.background && a.floor && a.wall && a.goal && a.box && a.boxGoal &&
        a.playerDown && a.playerUp && a.playerLeft && a.playerRight && a.back && a.retry &&
        a.save && a.eraser && a.play && a.crateTool && a.keeperTool && a.wallTool && a.targetTool && a.font;
}
void destroyAssets(Assets &a) {
    for (SDL_Texture *t : {a.menu,a.background,a.floor,a.wall,a.goal,a.box,a.boxGoal,a.playerDown,a.playerUp,
         a.playerLeft,a.playerRight,a.back,a.retry,a.save,a.eraser,a.play,a.crateTool,a.keeperTool,a.wallTool,a.targetTool})
        if (t) SDL_DestroyTexture(t);
    if (a.font) TTF_CloseFont(a.font);
}
void blit(SDL_Renderer *r, SDL_Texture *t, float x, float y, float w, float h) {
    if (t) { const SDL_FRect dst{x,y,w,h}; SDL_RenderTexture(r,t,nullptr,&dst); }
}
void text(SDL_Renderer *r, TTF_Font *font, const std::string &s, float x, float y, SDL_Color color={250,239,205,255}) {
    SDL_Surface *surface = TTF_RenderText_Blended(font,s.c_str(),0,color);
    if (!surface) return;
    SDL_Texture *t = SDL_CreateTextureFromSurface(r,surface);
    if (t) { blit(r,t,x,y,static_cast<float>(surface->w),static_cast<float>(surface->h)); SDL_DestroyTexture(t); }
    SDL_DestroySurface(surface);
}
void button(SDL_Renderer *r, TTF_Font *font, const std::string &label, SDL_FRect b, bool blue=false) {
    SDL_SetRenderDrawColor(r,29,43,61,255); SDL_RenderFillRect(r,&b);
    SDL_FRect face{b.x+3,b.y+3,b.w-6,b.h-6}; SDL_SetRenderDrawColor(r,blue?75:82,blue?133:147,blue?173:99,255); SDL_RenderFillRect(r,&face);
    SDL_FRect shine{b.x+8,b.y+7,b.w-16,3}; SDL_SetRenderDrawColor(r,147,190,105,255); SDL_RenderFillRect(r,&shine);
    SDL_Surface *s=TTF_RenderText_Blended(font,label.c_str(),0,SDL_Color{250,239,205,255});
    if(s){SDL_Texture *t=SDL_CreateTextureFromSurface(r,s); if(t){blit(r,t,b.x+(b.w-s->w)/2,b.y+(b.h-s->h)/2,(float)s->w,(float)s->h);SDL_DestroyTexture(t);}SDL_DestroySurface(s);}
}
bool readLevel(int n, Level &level) {
    std::ifstream f("niveaux perso/niveau"+std::to_string(n)+".txt");
    if(!f) return false;
    std::array<int,kCells> map{}; for(int &v:map) if(!(f>>v)) return false;
    level={};
    for(int i=0;i<kCells;i++) switch(map[i]) {
        case 1: level.walls[i]=true; break; case 3: level.boxes[i]=true; break;
        case 4: level.goals[i]=true; break; case 5: level.player=i; break;
        case 6: level.boxes[i]=level.goals[i]=true; break; default: break;
    }
    return level.player>=0;
}
bool writeLevel(const Level &l,int n) {
    if(l.player<0) return false;
    std::ofstream f("niveaux perso/niveau"+std::to_string(n)+".txt",std::ios::trunc);
    if(!f) return false;
    for(int i=0;i<kCells;i++){int v=l.walls[i]?1:(i==l.player?5:(l.boxes[i]?(l.goals[i]?6:3):(l.goals[i]?4:2)));f<<v<<(i+1==kCells?'\n':' ');}
    return bool(f);
}
void move(Level &l,int dx,int dy) {
    if(l.player<0)return; int x=l.player%kCols+dx,y=l.player/kCols+dy;
    if(x<0||x>=kCols||y<0||y>=kRows)return; int n=y*kCols+x;if(l.walls[n])return;
    if(l.boxes[n]){int bx=x+dx,by=y+dy;if(bx<0||bx>=kCols||by<0||by>=kRows)return;int b=by*kCols+bx;if(l.walls[b]||l.boxes[b])return;l.boxes[n]=false;l.boxes[b]=true;}
    l.player=n;
}
void drawMenu(SDL_Renderer *r,const Assets &a) {
    blit(r,a.menu,0,0,kWidth,kHeight);
    text(r,a.font,"MARIO SOKOBAN",213,64,{29,43,61,255});
    button(r,a.font,"JOUER",SDL_FRect{176,210,304,45});
    button(r,a.font,"CREER UN NIVEAU",SDL_FRect{176,257,304,45},true);
    text(r,a.font,"Un petit jeu de caisses au jardin",165,390,{29,43,61,255});
}
void drawLevels(SDL_Renderer *r,const Assets &a) {
    blit(r,a.background,0,0,kWidth,kHeight);
    text(r,a.font,"CHOISIS TON NIVEAU",177,60,{29,43,61,255});
    for(int i=0;i<9;i++){int x=120+(i%3)*165,y=145+(i/3)*92;button(r,a.font,std::to_string(i+1),SDL_FRect{(float)x,(float)y,74,68},i%2);}
    button(r,a.font,"RETOUR",SDL_FRect{210,450,260,56},true);
}
void drawGame(SDL_Renderer *r,const Assets &a,const Level &l,SDL_Texture *player,int number,bool editor,int tool) {
    SDL_SetRenderDrawColor(r,250,239,205,255); SDL_RenderClear(r);
    blit(r,a.background,0,0,kWidth,kHeight);
    for(int i=0;i<kCells;i++){
        float x=(float)(i%kCols*kTile),y=(float)(i/kCols*kTile);
        blit(r,a.floor,x,y,kTile,kTile);
        if(l.walls[i]) blit(r,a.wall,x,y,kTile,kTile);
        else {
            if(l.goals[i]) blit(r,a.goal,x,y,kTile,kTile);
            if(l.boxes[i]) blit(r,l.goals[i]?a.boxGoal:a.box,x,y,kTile,kTile);
        }
    }
    if(l.player>=0) blit(r,player,(float)(l.player%kCols*kTile),(float)(l.player/kCols*kTile),kTile,kTile);
    if(editor){
        SDL_Texture *icons[]={a.back,a.save,a.crateTool,a.keeperTool,a.wallTool,a.targetTool,a.eraser,a.play,a.retry};
        int xs[]={0,68,136,204,272,340,408,476,612};
        for(int i=0;i<9;i++)blit(r,icons[i],(float)xs[i],0,68,68);
        text(r,a.font,"EDITION - clic pour placer | S sauvegarder | E jouer",88,490,{29,43,61,255});
        text(r,a.font,"Outil "+std::to_string(tool),490,490,{29,43,61,255});
    }else{
        button(r,a.font,"MENU",SDL_FRect{10,8,100,50},true);
        button(r,a.font,"RECOMMENCER",SDL_FRect{122,8,220,50});
        text(r,a.font,"Niveau "+std::to_string(number)+"   Fleches/ZQSD : bouger   E : editeur",165,493,{29,43,61,255});
    }
    SDL_RenderPresent(r);
}
} // namespace

int main() {
    if(!SDL_Init(SDL_INIT_VIDEO)){std::cerr<<SDL_GetError()<<'\n';return 1;}
    if(!TTF_Init()){std::cerr<<SDL_GetError()<<'\n';SDL_Quit();return 1;}
    SDL_Window *window=SDL_CreateWindow("Mario Sokoban",kWidth,kHeight,0);
    SDL_Renderer *renderer=window?SDL_CreateRenderer(window,nullptr):nullptr;
    if(!window||!renderer){std::cerr<<SDL_GetError()<<'\n';if(renderer)SDL_DestroyRenderer(renderer);if(window)SDL_DestroyWindow(window);TTF_Quit();SDL_Quit();return 1;}
    Assets a;
    if(!loadAssets(renderer,a)){std::cerr<<"Assets manquants: "<<SDL_GetError()<<'\n';destroyAssets(a);SDL_DestroyRenderer(renderer);SDL_DestroyWindow(window);TTF_Quit();SDL_Quit();return 1;}
    Screen screen=Screen::Menu; Level level{},initial{}; int number=1,tool=3; SDL_Texture *player=a.playerDown; bool run=true,editor=false;
    while(run){
        SDL_Event e;
        while(SDL_PollEvent(&e)){
            if(e.type==SDL_EVENT_QUIT){run=false;continue;}
            if(screen==Screen::Menu){
                if(e.type==SDL_EVENT_KEY_DOWN&&!e.key.repeat){if(e.key.key==SDLK_ESCAPE)run=false;else if(e.key.key==SDLK_1||e.key.key==SDLK_RETURN)screen=Screen::Levels;else if(e.key.key==SDLK_2){level={};level.player=44;initial=level;number=10;editor=true;screen=Screen::Game;}}
                if(e.type==SDL_EVENT_MOUSE_BUTTON_UP){float x=e.button.x,y=e.button.y;if(x>=176&&x<=480&&y>=210&&y<=255)screen=Screen::Levels;else if(x>=176&&x<=480&&y>=257&&y<=302){level={};level.player=44;initial=level;number=10;editor=true;screen=Screen::Game;}}
            } else if(screen==Screen::Levels){
                if(e.type==SDL_EVENT_KEY_DOWN&&!e.key.repeat){if(e.key.key==SDLK_ESCAPE)screen=Screen::Menu;else if(e.key.key>=SDLK_1&&e.key.key<=SDLK_9){number=(int)(e.key.key-SDLK_0);if(readLevel(number,level)){initial=level;editor=false;player=a.playerDown;screen=Screen::Game;}}}
                if(e.type==SDL_EVENT_MOUSE_BUTTON_UP){float x=e.button.x,y=e.button.y;if(y>=450){screen=Screen::Menu;}else for(int i=0;i<9;i++){float bx=(float)(120+(i%3)*165),by=(float)(145+(i/3)*92);if(x>=bx&&x<=bx+74&&y>=by&&y<=by+68){number=i+1;if(readLevel(number,level)){initial=level;editor=false;player=a.playerDown;screen=Screen::Game;}break;}}}
            } else {
                if(e.type==SDL_EVENT_MOUSE_BUTTON_UP){
                    int x=(int)e.button.x,y=(int)e.button.y;
                    if(editor&&y<68){
                        if(x<68){screen=Screen::Menu;editor=false;}
                        else if(x<136){writeLevel(level,10);initial=level;}
                        else if(x<204)tool=3; else if(x<272)tool=5; else if(x<340)tool=1;
                        else if(x<408)tool=4; else if(x<476)tool=2;
                        else if(x<544){editor=false;initial=level;} else if(x>=612)level=initial;
                    } else if(editor&&x>=0&&x<kWidth&&y>=0&&y<kHeight){
                        int i=(y/kTile)*kCols+x/kTile;
                        if(tool==1){level.walls[i]=true;level.boxes[i]=level.goals[i]=false;if(level.player==i)level.player=-1;}
                        if(tool==2){level.walls[i]=level.boxes[i]=level.goals[i]=false;if(level.player==i)level.player=-1;}
                        if(tool==3){level.walls[i]=false;level.boxes[i]=true;}
                        if(tool==4){level.walls[i]=false;level.goals[i]=true;}
                        if(tool==5){level.walls[i]=false;level.boxes[i]=false;level.player=i;}
                        if(tool==6){level.boxes[i]=level.goals[i]=false;if(level.player==i)level.player=-1;}
                    } else if(!editor&&y<64){if(x<115)screen=Screen::Menu;else if(x<350)level=initial;}
                }
                if(e.type==SDL_EVENT_KEY_DOWN&&!e.key.repeat){
                    SDL_Keycode k=e.key.key;
                    if(k==SDLK_ESCAPE){screen=Screen::Menu;editor=false;}
                    else if(k==SDLK_E){editor=!editor;if(editor){number=10;}}
                    else if(editor&&k==SDLK_S){writeLevel(level,10);initial=level;}
                    else if(editor&&k>=SDLK_1&&k<=SDLK_6)tool=(int)(k-SDLK_0);
                    else if(!editor&&k==SDLK_R)level=initial;
                    else if(!editor&&(k==SDLK_UP||k==SDLK_W||k==SDLK_Z)){player=a.playerUp;move(level,0,-1);}
                    else if(!editor&&(k==SDLK_DOWN||k==SDLK_S)){player=a.playerDown;move(level,0,1);}
                    else if(!editor&&(k==SDLK_LEFT||k==SDLK_A||k==SDLK_Q)){player=a.playerLeft;move(level,-1,0);}
                    else if(!editor&&(k==SDLK_RIGHT||k==SDLK_D)){player=a.playerRight;move(level,1,0);}
                }
            }
        }
        if(screen==Screen::Menu)drawMenu(renderer,a);
        else if(screen==Screen::Levels)drawLevels(renderer,a);
        else drawGame(renderer,a,level,player,number,editor,tool);
        SDL_Delay(16);
    }
    destroyAssets(a);SDL_DestroyRenderer(renderer);SDL_DestroyWindow(window);TTF_Quit();SDL_Quit();return 0;
}
