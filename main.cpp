#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>

#include <array>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

namespace {
constexpr int kColumns = 10;
constexpr int kRows = 8;
constexpr int kCellCount = kColumns * kRows;
constexpr float kTileSize = 60.0F;
constexpr float kBoardX = 42.0F;
constexpr float kBoardY = 142.0F;
constexpr int kWindowWidth = 920;
constexpr int kWindowHeight = 660;

struct Level {
    std::array<bool, kCellCount> walls{};
    std::array<bool, kCellCount> goals{};
    std::array<bool, kCellCount> boxes{};
    int player = -1;
};

struct Assets {
    SDL_Texture *floor = nullptr;
    SDL_Texture *wall = nullptr;
    SDL_Texture *goal = nullptr;
    SDL_Texture *box = nullptr;
    SDL_Texture *boxOnGoal = nullptr;
    SDL_Texture *playerDown = nullptr;
    SDL_Texture *playerUp = nullptr;
    SDL_Texture *playerLeft = nullptr;
    SDL_Texture *playerRight = nullptr;
    SDL_Texture *background = nullptr;
    TTF_Font *font = nullptr;
};

SDL_Texture *loadTexture(SDL_Renderer *renderer, const char *path)
{
    SDL_Surface *surface = SDL_LoadBMP(path);
    if (!surface) {
        std::cerr << "Unable to load " << path << ": " << SDL_GetError() << '\n';
        return nullptr;
    }
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_DestroySurface(surface);
    if (!texture) {
        std::cerr << "Unable to create texture for " << path << ": " << SDL_GetError() << '\n';
    }
    return texture;
}

bool loadLevel(int number, Level &level)
{
    const std::string path = "niveaux perso/niveau" + std::to_string(number) + ".txt";
    std::ifstream file(path);
    if (!file) {
        std::cerr << "Unable to open level " << path << '\n';
        return false;
    }

    std::array<int, kCellCount> map{};
    for (int &cell : map) {
        if (!(file >> cell)) {
            std::cerr << "Level " << path << " must contain " << kCellCount << " cells.\n";
            return false;
        }
    }

    level = {};
    for (int i = 0; i < kCellCount; ++i) {
        switch (map[i]) {
        case 1: level.walls[i] = true; break;
        case 3: level.boxes[i] = true; break;
        case 4: level.goals[i] = true; break;
        case 5: level.player = i; break;
        case 6: level.goals[i] = true; level.boxes[i] = true; break;
        default: break;
        }
    }
    if (level.player < 0) {
        std::cerr << "Level " << path << " has no player.\n";
        return false;
    }
    return true;
}

bool saveLevel(const Level &level, int number)
{
    if (level.player < 0) return false;
    const std::string path = "niveaux perso/niveau" + std::to_string(number) + ".txt";
    std::ofstream file(path, std::ios::trunc);
    if (!file) {
        std::cerr << "Unable to save level " << path << '\n';
        return false;
    }
    for (int i = 0; i < kCellCount; ++i) {
        int cell = 2;
        if (level.walls[i]) cell = 1;
        else if (i == level.player) cell = 5;
        else if (level.boxes[i]) cell = level.goals[i] ? 6 : 3;
        else if (level.goals[i]) cell = 4;
        file << cell << (i + 1 == kCellCount ? '\n' : ' ');
    }
    return static_cast<bool>(file);
}

bool loadAssets(SDL_Renderer *renderer, Assets &assets)
{
    assets.floor = loadTexture(renderer, "sprites_mario_sokoban/garden.bmp");
    assets.wall = loadTexture(renderer, "sprites_mario_sokoban/wall.bmp");
    assets.goal = loadTexture(renderer, "sprites_mario_sokoban/target.bmp");
    assets.box = loadTexture(renderer, "sprites_mario_sokoban/crate.bmp");
    assets.boxOnGoal = loadTexture(renderer, "sprites_mario_sokoban/crate_target.bmp");
    assets.playerDown = loadTexture(renderer, "sprites_mario_sokoban/keeper_down.bmp");
    assets.playerUp = loadTexture(renderer, "sprites_mario_sokoban/keeper_up.bmp");
    assets.playerLeft = loadTexture(renderer, "sprites_mario_sokoban/keeper_left.bmp");
    assets.playerRight = loadTexture(renderer, "sprites_mario_sokoban/keeper_right.bmp");
    assets.background = loadTexture(renderer, "sprites_mario_sokoban/garden_detail.bmp");
    assets.font = TTF_OpenFont("NotoSans.ttf", 23.0F);
    if (!assets.font) {
        std::cerr << "Unable to load NotoSans.ttf: " << SDL_GetError() << '\n';
        return false;
    }
    return assets.floor && assets.wall && assets.goal && assets.box && assets.boxOnGoal &&
        assets.playerDown && assets.playerUp && assets.playerLeft && assets.playerRight && assets.background;
}

void destroyAssets(Assets &assets)
{
    for (SDL_Texture *texture : {assets.floor, assets.wall, assets.goal, assets.box, assets.boxOnGoal,
             assets.playerDown, assets.playerUp, assets.playerLeft, assets.playerRight, assets.background}) {
        SDL_DestroyTexture(texture);
    }
    if (assets.font) TTF_CloseFont(assets.font);
}

void drawTexture(SDL_Renderer *renderer, SDL_Texture *texture, float x, float y, float width, float height)
{
    const SDL_FRect destination{x, y, width, height};
    SDL_RenderTexture(renderer, texture, nullptr, &destination);
}

void drawText(SDL_Renderer *renderer, TTF_Font *font, const std::string &text,
              float x, float y, SDL_Color color)
{
    SDL_Surface *surface = TTF_RenderText_Blended(font, text.c_str(), 0, color);
    if (!surface) return;
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (texture) {
        const SDL_FRect destination{x, y, static_cast<float>(surface->w), static_cast<float>(surface->h)};
        SDL_RenderTexture(renderer, texture, nullptr, &destination);
        SDL_DestroyTexture(texture);
    }
    SDL_DestroySurface(surface);
}

bool isSolved(const Level &level)
{
    for (int i = 0; i < kCellCount; ++i) {
        if (level.goals[i] && !level.boxes[i]) return false;
    }
    return true;
}

void movePlayer(Level &level, int dx, int dy)
{
    const int x = level.player % kColumns;
    const int y = level.player / kColumns;
    const int nextX = x + dx;
    const int nextY = y + dy;
    if (nextX < 0 || nextX >= kColumns || nextY < 0 || nextY >= kRows) return;
    const int next = nextY * kColumns + nextX;
    if (level.walls[next]) return;

    if (level.boxes[next]) {
        const int boxX = nextX + dx;
        const int boxY = nextY + dy;
        if (boxX < 0 || boxX >= kColumns || boxY < 0 || boxY >= kRows) return;
        const int destination = boxY * kColumns + boxX;
        if (level.walls[destination] || level.boxes[destination]) return;
        level.boxes[next] = false;
        level.boxes[destination] = true;
    }
    level.player = next;
}

void drawGame(SDL_Renderer *renderer, const Assets &assets, const Level &level,
              int levelNumber, SDL_Texture *playerTexture, bool solved,
              bool editorMode, int selectedTool)
{
    SDL_SetRenderDrawColor(renderer, 30, 48, 39, 255);
    SDL_RenderClear(renderer);
    drawTexture(renderer, assets.background, 0, 0, static_cast<float>(kWindowWidth), static_cast<float>(kWindowHeight));

    drawText(renderer, assets.font, "Mario Sokoban", 42, 30, SDL_Color{255, 245, 205, 255});
    drawText(renderer, assets.font, std::string(editorMode ? "Editeur" : "Level") + " " + std::to_string(levelNumber), 42, 88, SDL_Color{235, 244, 225, 255});
    if (editorMode) {
        drawText(renderer, assets.font, "1 Mur  2 Sol  3 Caisse", 690, 188, SDL_Color{244, 245, 228, 255});
        drawText(renderer, assets.font, "4 But  5 Gardien", 690, 228, SDL_Color{244, 245, 228, 255});
        drawText(renderer, assets.font, "Clic : placer la case", 690, 268, SDL_Color{244, 245, 228, 255});
        drawText(renderer, assets.font, "S : sauvegarder niveau 10", 690, 308, SDL_Color{244, 245, 228, 255});
        drawText(renderer, assets.font, "O : ouvrir niveau 10", 690, 348, SDL_Color{244, 245, 228, 255});
        drawText(renderer, assets.font, "E : quitter l'editeur", 690, 388, SDL_Color{244, 245, 228, 255});
        drawText(renderer, assets.font, "Outil actif : " + std::to_string(selectedTool), 690, 428, SDL_Color{255, 231, 149, 255});
    } else {
        drawText(renderer, assets.font, "Fleches / ZQSD : deplacer", 690, 188, SDL_Color{244, 245, 228, 255});
        drawText(renderer, assets.font, "R : recommencer", 690, 228, SDL_Color{244, 245, 228, 255});
        drawText(renderer, assets.font, "1-9 : choisir un niveau", 690, 268, SDL_Color{244, 245, 228, 255});
        drawText(renderer, assets.font, "N : niveau suivant", 690, 308, SDL_Color{244, 245, 228, 255});
        drawText(renderer, assets.font, "E : editeur de niveaux", 690, 348, SDL_Color{244, 245, 228, 255});
        drawText(renderer, assets.font, "Echap : quitter", 690, 388, SDL_Color{244, 245, 228, 255});
    }

    for (int i = 0; i < kCellCount; ++i) {
        const int x = i % kColumns;
        const int y = i / kColumns;
        const float px = kBoardX + static_cast<float>(x) * kTileSize;
        const float py = kBoardY + static_cast<float>(y) * kTileSize;
        drawTexture(renderer, assets.floor, px, py, kTileSize, kTileSize);
        if (level.walls[i]) drawTexture(renderer, assets.wall, px, py, kTileSize, kTileSize);
        else if (level.boxes[i]) drawTexture(renderer, level.goals[i] ? assets.boxOnGoal : assets.box, px, py, kTileSize, kTileSize);
        else if (level.goals[i]) drawTexture(renderer, assets.goal, px, py, kTileSize, kTileSize);
    }
    if (level.player >= 0) {
        const float px = kBoardX + static_cast<float>(level.player % kColumns) * kTileSize;
        const float py = kBoardY + static_cast<float>(level.player / kColumns) * kTileSize;
        drawTexture(renderer, playerTexture, px, py, kTileSize, kTileSize);
    }
    if (solved) {
        const SDL_FRect panel{190, 300, 540, 78};
        SDL_SetRenderDrawColor(renderer, 17, 54, 40, 235);
        SDL_RenderFillRect(renderer, &panel);
        drawText(renderer, assets.font, "Bravo ! Appuyez sur N pour continuer", 220, 320,
                 SDL_Color{255, 248, 204, 255});
    }
    SDL_RenderPresent(renderer);
}
} // namespace

int main(int, char **)
{
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        std::cerr << "SDL_Init failed: " << SDL_GetError() << '\n';
        return 1;
    }
    if (!TTF_Init()) {
        std::cerr << "TTF_Init failed: " << SDL_GetError() << '\n';
        SDL_Quit();
        return 1;
    }

    SDL_Window *window = SDL_CreateWindow("Mario Sokoban", kWindowWidth, kWindowHeight, SDL_WINDOW_RESIZABLE);
    SDL_Renderer *renderer = window ? SDL_CreateRenderer(window, nullptr) : nullptr;
    if (!window || !renderer) {
        std::cerr << "Unable to create SDL window/renderer: " << SDL_GetError() << '\n';
        if (renderer) SDL_DestroyRenderer(renderer);
        if (window) SDL_DestroyWindow(window);
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    Assets assets;
    if (!loadAssets(renderer, assets)) {
        destroyAssets(assets);
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    int levelNumber = 1;
    Level level;
    loadLevel(levelNumber, level);
    Level initialLevel = level;
    SDL_Texture *player = assets.playerDown;
    bool editorMode = false;
    int selectedTool = 2;
    bool running = true;
    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) running = false;
            if (editorMode && event.type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
                const bool insideBoard = event.button.x >= kBoardX && event.button.x < kBoardX + kColumns * kTileSize &&
                    event.button.y >= kBoardY && event.button.y < kBoardY + kRows * kTileSize;
                if (insideBoard) {
                    const int column = static_cast<int>((event.button.x - kBoardX) / kTileSize);
                    const int row = static_cast<int>((event.button.y - kBoardY) / kTileSize);
                    const int cell = row * kColumns + column;
                    if (selectedTool == 1) {
                        level.walls[cell] = true;
                        level.goals[cell] = false;
                        level.boxes[cell] = false;
                        if (level.player == cell) level.player = -1;
                    } else if (selectedTool == 2) {
                        level.walls[cell] = false;
                        level.goals[cell] = false;
                        level.boxes[cell] = false;
                        if (level.player == cell) level.player = -1;
                    } else if (selectedTool == 3) {
                        level.walls[cell] = false;
                        level.boxes[cell] = true;
                    } else if (selectedTool == 4) {
                        level.walls[cell] = false;
                        level.goals[cell] = true;
                    } else if (selectedTool == 5) {
                        level.walls[cell] = false;
                        level.boxes[cell] = false;
                        level.player = cell;
                    }
                }
            }
            if (event.type != SDL_EVENT_KEY_DOWN || event.key.repeat) continue;
            const SDL_Keycode key = event.key.key;
            if (key == SDLK_ESCAPE) running = false;
            else if (key == SDLK_E) editorMode = !editorMode;
            else if (editorMode && key >= SDLK_1 && key <= SDLK_5) {
                selectedTool = static_cast<int>(key - SDLK_0);
            } else if (editorMode && key == SDLK_S) {
                if (saveLevel(level, 10)) {
                    levelNumber = 10;
                    initialLevel = level;
                }
            } else if (editorMode && key == SDLK_O) {
                if (loadLevel(10, level)) {
                    levelNumber = 10;
                    initialLevel = level;
                }
            } else if (key >= SDLK_1 && key <= SDLK_9) {
                levelNumber = static_cast<int>(key - SDLK_0);
                if (loadLevel(levelNumber, level)) initialLevel = level;
            } else if (key == SDLK_0) {
                if (loadLevel(10, level)) {
                    levelNumber = 10;
                    initialLevel = level;
                }
            } else if (key == SDLK_R) {
                level = initialLevel;
            } else if (key == SDLK_N) {
                const int nextLevel = levelNumber % 10 + 1;
                if (loadLevel(nextLevel, level)) {
                    levelNumber = nextLevel;
                    initialLevel = level;
                }
            } else if (key == SDLK_UP || key == SDLK_W || key == SDLK_Z) {
                player = assets.playerUp;
                movePlayer(level, 0, -1);
            } else if (key == SDLK_DOWN || key == SDLK_S) {
                player = assets.playerDown;
                movePlayer(level, 0, 1);
            } else if (key == SDLK_LEFT || key == SDLK_A || key == SDLK_Q) {
                player = assets.playerLeft;
                movePlayer(level, -1, 0);
            } else if (key == SDLK_RIGHT || key == SDLK_D) {
                player = assets.playerRight;
                movePlayer(level, 1, 0);
            }
        }
        drawGame(renderer, assets, level, levelNumber, player, isSolved(level), editorMode, selectedTool);
        SDL_Delay(16);
    }

    destroyAssets(assets);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();
    return 0;
}
