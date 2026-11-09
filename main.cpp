// Mario Sokoban - SDL 3 version.
//
// This port reproduces the screens, graphics and texts of the SDL 1.2 version
// (branch master) pixel for pixel: everything is drawn on a 680x544 software
// canvas with the same sprites and the same 5x7 bitmap typeface, then the
// canvas is presented through an SDL 3 renderer (scaled and centred).
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include "bitmap_font.hpp"

#include <array>
#include <cstdio>
#include <cstring>
#include <filesystem>
#include <map>
#include <string>

namespace {

constexpr int kCols = 10, kRows = 8, kCells = kCols * kRows, kTile = 68;
constexpr int kWidth = kCols * kTile, kHeight = kRows * kTile;
constexpr int kBuiltinLevels = 6;
constexpr int kLevelsPerPage = 9, kPageCount = 3, kCustomLevels = kLevelsPerPage * kPageCount;
constexpr int kBestTimeRows = kBuiltinLevels + kCustomLevels;

// Cell values, identical to the level files: 1 wall, 2 floor, 3 crate,
// 4 target, 5 player, 6 crate on a target (0 = never painted = floor).
enum Cell { Empty = 0, Wall = 1, Floor = 2, Crate = 3, Target = 4, Player = 5, CrateOnTarget = 6 };
using Map = std::array<int, kCells>;

constexpr SDL_Color kInk{29, 43, 61, 255};
constexpr SDL_Color kCream{250, 239, 205, 255};
constexpr SDL_Color kGold{255, 225, 110, 255};
constexpr SDL_Color kLight{255, 245, 205, 255};

// ---------------------------------------------------------------------------
// Window, canvas and events
// ---------------------------------------------------------------------------
struct App {
    SDL_Window *window = nullptr;
    SDL_Renderer *renderer = nullptr;
    SDL_Texture *texture = nullptr;
    SDL_Surface *canvas = nullptr;
    std::map<std::string, SDL_Surface *> sprites;
    bool fullscreen = false;
    bool quit = false;
    int customPage = 0;
};

void present(App &app)
{
    SDL_UpdateTexture(app.texture, nullptr, app.canvas->pixels, app.canvas->pitch);
    SDL_SetRenderDrawColor(app.renderer, 0, 0, 0, 255);
    SDL_RenderClear(app.renderer);
    SDL_RenderTexture(app.renderer, app.texture, nullptr, nullptr);
    SDL_RenderPresent(app.renderer);
}

void toggleFullscreen(App &app)
{
    app.fullscreen = !app.fullscreen;
    SDL_SetWindowFullscreen(app.window, app.fullscreen);
    present(app);
}

// Waits for the next game event (timeoutMs < 0: no timeout). Fullscreen keys,
// resizes and redraw requests are handled here. Mouse coordinates are
// converted to canvas coordinates. Returns false on timeout. A closed window
// sets app.quit and returns true.
bool waitEvent(App &app, SDL_Event &event, int timeoutMs = -1)
{
    for (;;) {
        const bool received = timeoutMs < 0 ? SDL_WaitEvent(&event) : SDL_WaitEventTimeout(&event, timeoutMs);
        if (!received) return false;
        switch (event.type) {
        case SDL_EVENT_QUIT:
            app.quit = true;
            return true;
        case SDL_EVENT_KEY_DOWN:
            if (event.key.repeat) break;
            if (event.key.key == SDLK_F11 || (event.key.key == SDLK_RETURN && (event.key.mod & SDL_KMOD_ALT))) {
                toggleFullscreen(app);
                continue;
            }
            if (event.key.key == SDLK_ESCAPE && app.fullscreen) {
                toggleFullscreen(app);
                continue;
            }
            break;
        case SDL_EVENT_WINDOW_EXPOSED:
        case SDL_EVENT_WINDOW_RESIZED:
        case SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED:
            present(app);
            continue;
        case SDL_EVENT_MOUSE_BUTTON_DOWN:
        case SDL_EVENT_MOUSE_BUTTON_UP:
        case SDL_EVENT_MOUSE_MOTION:
            SDL_ConvertEventToRenderCoordinates(app.renderer, &event);
            break;
        default:
            break;
        }
        return true;
    }
}

bool isClick(const SDL_Event &event) { return event.type == SDL_EVENT_MOUSE_BUTTON_UP; }
bool clickIn(const SDL_Event &event, int x0, int y0, int x1, int y1)
{
    if (!isClick(event)) return false;
    const float x = event.button.x, y = event.button.y;
    return x >= x0 && x <= x1 && y >= y0 && y <= y1;
}
bool keyIs(const SDL_Event &event, SDL_Keycode key)
{
    return event.type == SDL_EVENT_KEY_DOWN && event.key.key == key;
}

// ---------------------------------------------------------------------------
// Drawing helpers (same look as the SDL 1.2 helpers of master)
// ---------------------------------------------------------------------------
SDL_Surface *sprite(App &app, const std::string &name)
{
    auto found = app.sprites.find(name);
    if (found != app.sprites.end()) return found->second;
    const std::string path = "sprites_mario_sokoban/" + name;
    SDL_Surface *loaded = nullptr;
    if (name.size() > 4 && name.compare(name.size() - 4, 4, ".png") == 0) {
        loaded = SDL_LoadPNG(path.c_str());
        if (!loaded) loaded = SDL_LoadBMP((path.substr(0, path.size() - 4) + ".bmp").c_str());
    } else {
        loaded = SDL_LoadBMP(path.c_str());
    }
    SDL_Surface *result = nullptr;
    if (loaded) {
        const bool alpha = SDL_ISPIXELFORMAT_ALPHA(loaded->format);
        result = SDL_ConvertSurface(loaded, alpha ? SDL_PIXELFORMAT_ARGB8888 : SDL_PIXELFORMAT_XRGB8888);
        SDL_DestroySurface(loaded);
        if (result) SDL_SetSurfaceBlendMode(result, alpha ? SDL_BLENDMODE_BLEND : SDL_BLENDMODE_NONE);
    } else {
        SDL_Log("Cannot load %s: %s", path.c_str(), SDL_GetError());
    }
    app.sprites[name] = result;
    return result;
}

void blit(App &app, const std::string &name, int x, int y)
{
    if (SDL_Surface *image = sprite(app, name)) {
        SDL_Rect at{x, y, image->w, image->h};
        SDL_BlitSurface(image, nullptr, app.canvas, &at);
    }
}

// Linear filtering: halving a 68 px icon averages each 2x2 block instead of
// dropping every other line of the artwork.
void blitScaled(App &app, const std::string &name, SDL_Rect at)
{
    if (SDL_Surface *image = sprite(app, name)) SDL_BlitSurfaceScaled(image, nullptr, app.canvas, &at, SDL_SCALEMODE_LINEAR);
}

void fill(App &app, SDL_Rect rect, Uint8 r, Uint8 g, Uint8 b)
{
    SDL_FillSurfaceRect(app.canvas, &rect, SDL_MapSurfaceRGB(app.canvas, r, g, b));
}

void fillAll(App &app, Uint8 r, Uint8 g, Uint8 b)
{
    SDL_FillSurfaceRect(app.canvas, nullptr, SDL_MapSurfaceRGB(app.canvas, r, g, b));
}

// Outlined text, as draw_game_text().
void gameText(App &app, int size, const char *label, int x, int y, SDL_Color color)
{
    // A 2 px outline swallows the 1 px strokes of small text: use 1 px there.
    const int step = BitmapScale(size) >= 2 ? 2 : 1;
    for (int dy = -step; dy <= step; dy += step)
        for (int dx = -step; dx <= step; dx += step) BitmapDrawText(app.canvas, size, label, x + dx, y + dy, kInk);
    BitmapDrawText(app.canvas, size, label, x, y, color);
}

void centeredText(App &app, int size, const char *label, SDL_Rect bounds, SDL_Color color)
{
    gameText(app, size, label, bounds.x + (bounds.w - BitmapTextWidth(size, label)) / 2,
             bounds.y + (bounds.h - BitmapTextHeight(size)) / 2, color);
}

void button(App &app, int size, const char *label, SDL_Rect bounds, bool blue = false)
{
    fill(app, bounds, 29, 43, 61);
    fill(app, {bounds.x + 3, bounds.y + 3, bounds.w - 6, bounds.h - 6}, blue ? 42 : 82, blue ? 105 : 147, blue ? 188 : 99);
    fill(app, {bounds.x + 7, bounds.y + 6, bounds.w - 14, 3}, blue ? 133 : 147, blue ? 194 : 190, blue ? 255 : 105);
    BitmapDrawText(app.canvas, size, label, bounds.x + (bounds.w - BitmapTextWidth(size, label)) / 2,
                   bounds.y + (bounds.h - BitmapTextHeight(size)) / 2, kCream);
}

void panel(App &app, SDL_Rect outer)
{
    fill(app, outer, 29, 43, 61);
    fill(app, {outer.x + 4, outer.y + 4, outer.w - 8, outer.h - 8}, 32, 55, 84);
}

void multicolorVictory(App &app, int size, int centerX, int y)
{
    static const SDL_Color colors[] = {{255, 93, 87, 255}, {255, 177, 66, 255}, {255, 230, 90, 255}, {118, 220, 115, 255},
                                       {91, 190, 255, 255}, {164, 139, 255, 255}, {255, 124, 193, 255}, {255, 245, 205, 255}};
    const char *title = "VICTOIRE";
    int x = centerX - BitmapTextWidth(size, title) / 2;
    for (int i = 0; title[i]; ++i) {
        const char glyph[2] = {title[i], 0};
        gameText(app, size, glyph, x, y, colors[i % 8]);
        x += BitmapTextWidth(size, glyph);
    }
}

void timerPanel(App &app, Uint64 startedTicks)
{
    const unsigned seconds = static_cast<unsigned>((SDL_GetTicks() - startedTicks) / 1000);
    char label[32];
    std::snprintf(label, sizeof(label), "TIME %02u:%02u", seconds / 60, seconds % 60);
    fill(app, {272, 7, 136, 40}, 29, 43, 61);
    const SDL_Rect inset{275, 10, 130, 34};
    fill(app, inset, 32, 55, 84);
    centeredText(app, 17, label, inset, kGold);
}

void thumbnail(App &app, const Map &map, int x, int y, int width, int height)
{
    fill(app, {x, y, width, height}, 29, 43, 61);
    const int cellW = (width - 8) / 10, cellH = (height - 8) / 8;
    for (int i = 0; i < kCells; ++i) {
        const SDL_Rect cell{x + 4 + (i % 10) * cellW, y + 4 + (i / 10) * cellH, cellW, cellH};
        const int value = map[i];
        Uint8 r = 181, g = 205, b = 158;
        if (value == Wall) { r = 48; g = 81; b = 104; }
        else if (value == Crate || value == CrateOnTarget) { r = 174; g = 112; b = 61; }
        else if (value == Target) { r = 244; g = 198; b = 93; }
        else if (value == Player) { r = 202; g = 83; b = 79; }
        fill(app, cell, r, g, b);
        if (value == CrateOnTarget) fill(app, {cell.x + cell.w / 4, cell.y + cell.h / 4, cell.w / 2, cell.h / 2}, 255, 220, 115);
    }
}

// ---------------------------------------------------------------------------
// Levels, progress and best times (same files as master)
// ---------------------------------------------------------------------------
Map builtinLevel(int number)
{
    static const int levels[kBuiltinLevels][kCells] = {
        {1,1,1,1,1,1,1,1,1,1,1,1,1,1,4,2,1,1,1,1,1,4,2,2,3,2,1,1,1,1,1,1,2,2,3,4,1,1,1,1,1,1,2,3,2,2,1,1,1,1,1,1,1,5,2,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
        {1,1,1,1,1,1,1,1,1,1,1,5,2,2,1,1,1,1,1,1,1,2,3,3,1,1,1,1,1,1,1,2,3,2,1,1,1,1,1,1,1,1,1,2,1,1,1,4,4,4,1,1,1,2,2,2,2,2,2,2,1,2,2,2,2,1,2,2,2,2,4,3,2,2,2,1,1,1,1,1},
        {1,1,2,2,2,2,2,1,1,1,1,1,3,1,1,1,2,2,2,1,2,5,2,2,3,2,2,3,2,1,2,2,2,2,1,2,3,2,1,1,4,4,4,4,1,2,2,2,1,1,1,1,1,1,4,1,1,2,1,1,1,1,2,2,3,1,2,3,2,2,1,1,4,2,2,3,2,2,2,4},
        {1,1,1,1,1,1,4,2,2,2,1,1,2,2,1,4,3,2,3,2,1,5,3,2,1,4,3,2,1,2,1,1,3,2,1,2,2,1,2,2,1,1,2,3,2,1,2,2,3,2,1,4,3,2,2,1,1,1,1,2,1,4,2,3,2,2,2,2,2,2,1,1,4,4,4,1,2,2,2,4},
        {1,1,1,2,2,1,2,4,1,1,1,2,2,3,2,1,2,2,1,1,1,5,2,1,2,1,2,2,1,1,1,4,2,2,2,2,2,3,2,1,1,1,1,1,2,1,2,2,1,1,1,4,2,2,3,1,2,2,1,1,1,2,3,1,2,1,2,2,1,1,4,2,2,3,2,2,2,4,1,1},
        {2,2,2,2,2,2,2,4,1,2,1,2,2,1,1,1,1,1,1,1,1,1,3,2,3,2,2,4,1,1,2,2,2,3,2,2,4,1,1,1,2,2,3,2,3,2,1,1,1,1,1,1,2,2,1,2,1,1,1,1,1,4,2,2,4,2,2,2,5,1,1,1,1,1,1,1,1,1,1,1},
    };
    Map map{};
    for (int i = 0; i < kCells; ++i) map[i] = levels[number - 1][i];
    return map;
}

std::string customPath(int slot) { return "niveaux perso/niveau" + std::to_string(slot) + ".txt"; }

// Reads a personal level. An empty (reset) or missing slot gives an all-zero map.
Map readCustomLevel(int slot)
{
    Map map{};
    FILE *file = std::fopen(customPath(slot).c_str(), "r");
    if (!file) return map;
    int format = 0;
    if (std::fscanf(file, "%d", &format) == 1) {
        if (format >= 2) {
            for (int &cell : map) if (std::fscanf(file, "%d", &cell) != 1) cell = Floor;
        } else if (format == 1) {
            // Legacy files used the first entry as the enabled flag and could not store cell 0 (blank floor).
            map[0] = Floor;
            for (int cell = 1; cell < kCells; ++cell) if (std::fscanf(file, "%d", &map[cell]) != 1) map[cell] = Floor;
        }
    }
    std::fclose(file);
    return map;
}

bool writeCustomLevel(Map map, int slot, bool empty)
{
    if (slot < 1 || slot > kCustomLevels) return false;
    for (int &cell : map) if (cell == Empty) cell = Floor;
    FILE *file = std::fopen(customPath(slot).c_str(), "w");
    if (!file) return false;
    bool okay = std::fprintf(file, " %d", empty ? 0 : 2) >= 0;
    for (int cell : map) if (std::fprintf(file, " %d", cell) < 0) okay = false;
    if (std::fflush(file) != 0) okay = false;
    if (std::fclose(file) != 0) okay = false;
    return okay;
}

using BestTimes = std::array<std::array<unsigned, 3>, kBestTimeRows>;

BestTimes readBestTimes()
{
    BestTimes records{};
    if (FILE *file = std::fopen("data/best_times.txt", "r")) {
        for (auto &row : records)
            if (std::fscanf(file, "%u %u %u", &row[0], &row[1], &row[2]) != 3) break;
        std::fclose(file);
    }
    return records;
}

void writeBestTimes(const BestTimes &records)
{
    if (FILE *file = std::fopen("data/best_times.txt", "w")) {
        for (const auto &row : records) std::fprintf(file, "%u %u %u\n", row[0], row[1], row[2]);
        std::fclose(file);
    }
}

void recordBestTime(int row, unsigned seconds)
{
    if (row < 1 || row > kBestTimeRows) return;
    if (seconds == 0) seconds = 1;
    BestTimes records = readBestTimes();
    auto &times = records[row - 1];
    for (int rank = 0; rank < 3; ++rank)
        if (times[rank] == 0 || seconds < times[rank]) {
            for (int shift = 2; shift > rank; --shift) times[shift] = times[shift - 1];
            times[rank] = seconds;
            break;
        }
    writeBestTimes(records);
}

std::string completedPath(bool custom, int number)
{
    return std::string(custom ? "data/custom_completed" : "data/completed") + std::to_string(number) + ".lvl";
}

bool isCompleted(bool custom, int number)
{
    int completed = 0;
    if (FILE *file = std::fopen(completedPath(custom, number).c_str(), "r")) {
        if (std::fscanf(file, "%d", &completed) != 1) completed = 0;
        std::fclose(file);
    }
    return completed != 0;
}

void markCompleted(bool custom, int number)
{
    if (FILE *file = std::fopen(completedPath(custom, number).c_str(), "w")) {
        std::fputs("1", file);
        std::fclose(file);
    }
}

void resetCustomLevel(int slot)
{
    writeCustomLevel(Map{}, slot, true);
    std::remove(completedPath(true, slot).c_str());
    BestTimes records = readBestTimes();
    records[kBuiltinLevels + slot - 1] = {0, 0, 0};
    writeBestTimes(records);
}

// ---------------------------------------------------------------------------
// Board drawing
// ---------------------------------------------------------------------------
const char *cellSprite(int value)
{
    switch (value) {
    case Wall: return "wall.bmp";
    case Crate: return "crate.bmp";
    case Target: return "target.bmp";
    case CrateOnTarget: return "crate_target.bmp";
    default: return nullptr;
    }
}

// Same layering as load(): floor, walls, targets, crates, crates on targets, player.
void drawBoard(App &app, const Map &map, const char *playerSprite)
{
    for (int cell = 0; cell < kCells; ++cell) blit(app, "floor.bmp", (cell % kCols) * kTile, (cell / kCols) * kTile);
    for (int value : {Wall, Target, Crate, CrateOnTarget, Player})
        for (int cell = 0; cell < kCells; ++cell) {
            if (map[cell] != value) continue;
            const int x = (cell % kCols) * kTile, y = (cell / kCols) * kTile;
            if (value == Player) { if (playerSprite) blit(app, playerSprite, x, y); }
            else blit(app, cellSprite(value), x, y);
        }
}

// ---------------------------------------------------------------------------
// Screens
// ---------------------------------------------------------------------------
enum class Choice { MainMenu, LevelSelect, Back, Replay, Quit };

int mainMenu(App &app)
{
    fillAll(app, 0, 0, 0);
    blit(app, "menu.bmp", 0, 0);
    fill(app, {10, 482, 660, 52}, 29, 43, 61);
    fill(app, {13, 485, 654, 46}, 32, 55, 84);
    centeredText(app, 32, "CREATED BY FROMTHERAGS", {13, 485, 654, 46}, kGold);
    centeredText(app, 40, "MARIO SOKOBAN", {157, 35, 366, 94}, kLight);
    button(app, 26, "1 - JOUER", {176, 210, 304, 45});
    button(app, 26, "2 - CREER", {176, 257, 304, 45}, true);
    present(app);
    SDL_Event event;
    while (waitEvent(app, event)) {
        if (app.quit) return 0;
        if (keyIs(event, SDLK_1) || keyIs(event, SDLK_KP_1) || clickIn(event, 176, 210, 480, 255)) return 1;
        if (keyIs(event, SDLK_2) || keyIs(event, SDLK_KP_2) || clickIn(event, 176, 257, 480, 302)) return 2;
    }
    return 0;
}

void drawBestTimes(App &app)
{
    panel(app, {12, 425, 656, 109});
    centeredText(app, 12, "TOP 3 - MEILLEURS TEMPS", {180, 429, 320, 17}, kGold);
    static const SDL_Color rankColors[3] = {{255, 225, 110, 255}, {210, 225, 239, 255}, {236, 166, 112, 255}};
    const BestTimes records = readBestTimes();
    for (int level = 1; level <= kBuiltinLevels; ++level) {
        const int x = 22 + (level - 1) * 106;
        fill(app, {x, 450, 100, 78}, 38, 66, 96);
        char heading[16];
        std::snprintf(heading, sizeof(heading), "NIVEAU %d", level);
        centeredText(app, 12, heading, {x, 451, 100, 13}, kLight);
        for (int rank = 0; rank < 3; ++rank) {
            const unsigned t = records[level - 1][rank];
            char line[24];
            if (t) std::snprintf(line, sizeof(line), "%d. %02u:%02u", rank + 1, t / 60, t % 60);
            else std::snprintf(line, sizeof(line), "%d. --:--", rank + 1);
            gameText(app, 12, line, x + 18, 466 + rank * 18, rankColors[rank]);
        }
    }
}

// Built-in level selection (choixniv). Returns 1..6, 7 = personal levels, 0 = back.
int levelSelect(App &app)
{
    blit(app, "garden_detail.bmp", 0, 0);
    gameText(app, 30, "CHOISIS TON NIVEAU", 202, 48, kLight);
    const int xs[6] = {120, 285, 450, 120, 285, 450}, ys[6] = {150, 150, 150, 250, 250, 250};
    for (int level = 1; level <= kBuiltinLevels; ++level) {
        const int x = xs[level - 1], y = ys[level - 1];
        button(app, 30, " ", {x, y, 74, 76});
        thumbnail(app, builtinLevel(level), x + 7, y + 7, 60, 39);
        const std::string label = std::to_string(level);
        gameText(app, 30, label.c_str(), x + 31, y + 49, kLight);
        if (isCompleted(false, level)) blit(app, "medal_small.bmp", x + 48, y + 5);
    }
    button(app, 30, "NIVEAUX PERSO", {120, 350, 404, 70});
    drawBestTimes(app);
    blit(app, "back.bmp", 0, 0);
    present(app);
    SDL_Event event;
    while (waitEvent(app, event)) {
        if (app.quit) return 0;
        if (event.type == SDL_EVENT_KEY_DOWN && event.key.key >= SDLK_1 && event.key.key <= SDLK_6)
            return static_cast<int>(event.key.key - SDLK_0);
        if (clickIn(event, 0, 0, 68, 68)) return 0;
        for (int level = 1; level <= kBuiltinLevels; ++level)
            if (clickIn(event, xs[level - 1], ys[level - 1], xs[level - 1] + 74, ys[level - 1] + 76)) return level;
        if (clickIn(event, 120, 350, 524, 420)) return 7;
    }
    return 0;
}

constexpr int kListBack = 100;

// Personal level list, 3 pages of 9 (niveaucrea). Returns 1..27 or kListBack.
int customLevelList(App &app)
{
    const int cardX[3] = {20, 245, 470}, cardY[3] = {96, 236, 376};
    const SDL_Rect prev{20, 504, 96, 34}, next{564, 504, 96, 34};
    app.customPage = SDL_clamp(app.customPage, 0, kPageCount - 1);
    for (;;) {
        const int first = app.customPage * kLevelsPerPage;
        const BestTimes records = readBestTimes();
        blit(app, "garden_detail.bmp", 0, 0);
        blit(app, "back.bmp", 0, 0);
        gameText(app, 28, "NIVEAUX PERSONNELS", 202, 44, kLight);
        for (int slot = 1; slot <= kLevelsPerPage; ++slot) {
            const int level = first + slot, col = (slot - 1) % 3, row = (slot - 1) / 3;
            const int x = cardX[col], y = cardY[row];
            button(app, 28, " ", {x, y, 190, 124});
            thumbnail(app, readCustomLevel(level), x + 8, y + 8, 174, 58);
            char label[24];
            std::snprintf(label, sizeof(label), "NIVEAU %d", level);
            gameText(app, 28, label, x + 12, y + 68, kLight);
            for (int rank = 0; rank < 3; ++rank) {
                const unsigned t = records[kBuiltinLevels + level - 1][rank];
                char line[24];
                if (t) std::snprintf(line, sizeof(line), "%d   %02u:%02u", rank + 1, t / 60, t % 60);
                else std::snprintf(line, sizeof(line), "%d   --:--", rank + 1);
                gameText(app, 8, line, x + 12, y + 81 + rank * 9, rank == 0 ? kGold : kCream);
            }
            if (isCompleted(true, level)) blit(app, "medal_small.bmp", x + 132, y + 4);
        }
        char pageLabel[24];
        std::snprintf(pageLabel, sizeof(pageLabel), "PAGE %d / %d", app.customPage + 1, kPageCount);
        const SDL_Rect pagePanel{250, 504, 180, 34};
        fill(app, pagePanel, 29, 43, 61);
        centeredText(app, 20, pageLabel, pagePanel, kGold);
        if (app.customPage > 0) button(app, 20, "< PREC", prev);
        if (app.customPage < kPageCount - 1) button(app, 20, "SUIV >", next, true);
        present(app);

        int change = 0;
        SDL_Event event;
        while (change == 0 && waitEvent(app, event)) {
            if (app.quit) return kListBack;
            if ((keyIs(event, SDLK_LEFT) || keyIs(event, SDLK_PAGEUP)) && app.customPage > 0) change = -1;
            if ((keyIs(event, SDLK_RIGHT) || keyIs(event, SDLK_PAGEDOWN)) && app.customPage < kPageCount - 1) change = 1;
            if (!isClick(event)) continue;
            if (clickIn(event, 0, 0, 68, 68)) return kListBack;
            if (app.customPage > 0 && clickIn(event, prev.x, prev.y, prev.x + prev.w, prev.y + prev.h)) change = -1;
            else if (app.customPage < kPageCount - 1 && clickIn(event, next.x, next.y, next.x + next.w, next.y + next.h)) change = 1;
            else
                for (int slot = 1; slot <= kLevelsPerPage; ++slot) {
                    const int x = cardX[(slot - 1) % 3], y = cardY[(slot - 1) / 3];
                    if (clickIn(event, x, y, x + 190, y + 124)) return first + slot;
                }
        }
        if (app.quit) return kListBack;
        app.customPage += change;
    }
}

// Built-in victory (victoire). Returns Replay, LevelSelect or MainMenu.
Choice builtinVictory(App &app, unsigned seconds)
{
    blit(app, "garden_detail.bmp", 0, 0);
    blit(app, "medal.bmp", 306, 98);
    multicolorVictory(app, 54, 340, 28);
    centeredText(app, 54, "NIVEAU TERMINE", {70, 170, 540, 40}, kCream);
    char elapsed[40];
    std::snprintf(elapsed, sizeof(elapsed), "TEMPS %02u:%02u", seconds / 60, seconds % 60);
    centeredText(app, 40, elapsed, {170, 216, 340, 34}, kGold);
    button(app, 36, "REJOUER", {100, 270, 165, 80});
    button(app, 36, "NIVEAU", {400, 270, 180, 80});
    button(app, 36, "MENU", {255, 370, 154, 80});
    present(app);
    SDL_Event event;
    while (waitEvent(app, event)) {
        if (app.quit) return Choice::Quit;
        if (clickIn(event, 100, 270, 265, 350)) return Choice::Replay;
        if (clickIn(event, 400, 270, 580, 350)) return Choice::LevelSelect;
        if (clickIn(event, 255, 370, 409, 450)) return Choice::MainMenu;
    }
    return Choice::Quit;
}

// Personal level victory (affonlyvictyouinon). Returns Replay or Back.
Choice customVictory(App &app, unsigned seconds)
{
    blit(app, "garden_detail.bmp", 0, 0);
    blit(app, "medal.bmp", 306, 84);
    multicolorVictory(app, 52, 340, 35);
    char elapsed[40];
    std::snprintf(elapsed, sizeof(elapsed), "TEMPS %02u:%02u", seconds / 60, seconds % 60);
    centeredText(app, 52, elapsed, {170, 158, 340, 38}, kGold);
    gameText(app, 44, "VEUX-TU REJOUER ?", 160, 205, kCream);
    button(app, 44, "REJOUER", {20, 250, 277, 70});
    button(app, 44, "MENU", {320, 250, 300, 70});
    present(app);
    SDL_Event event;
    while (waitEvent(app, event)) {
        if (app.quit) return Choice::Quit;
        if (clickIn(event, 20, 250, 297, 320)) return Choice::Replay;
        if (clickIn(event, 320, 250, 620, 320)) return Choice::Back;
    }
    return Choice::Quit;
}

// "Niveau incomplet ou vide" (affcomplet). Returns Back or Replay.
Choice incompleteLevel(App &app)
{
    blit(app, "garden_detail.bmp", 0, 0);
    blit(app, "back.bmp", 0, 0);
    blit(app, "retry.bmp", 9 * kTile, 0);
    panel(app, {70, 170, 540, 200});
    centeredText(app, 40, "NIVEAU INCOMPLET", {74, 192, 532, 40}, kGold);
    centeredText(app, 40, "OU VIDE", {74, 240, 532, 40}, kGold);
    centeredText(app, 14, "IL FAUT UN JOUEUR, DES CAISSES ET DES OBJECTIFS", {74, 310, 532, 20}, kCream);
    present(app);
    SDL_Event event;
    while (waitEvent(app, event)) {
        if (app.quit) return Choice::Quit;
        if (clickIn(event, 0, 0, 68, 68)) return Choice::Back;
        if (clickIn(event, 9 * kTile, 0, kWidth, 68)) return Choice::Replay;
    }
    return Choice::Quit;
}

// Reset confirmation drawn over the level list (tobesur). Returns true for OUI.
bool confirmReset(App &app)
{
    panel(app, {70, 150, 540, 230});
    centeredText(app, 26, "REINITIALISER CE NIVEAU ?", {74, 172, 532, 30}, kGold);
    centeredText(app, 14, "LE NIVEAU, SA MEDAILLE ET SES TEMPS SERONT EFFACES", {74, 215, 532, 20}, kCream);
    button(app, 36, "OUI", {130, 270, 180, 70});
    button(app, 36, "NON", {370, 270, 180, 70}, true);
    present(app);
    SDL_Event event;
    while (waitEvent(app, event)) {
        if (app.quit) return false;
        if (clickIn(event, 130, 270, 310, 340)) return true;
        if (clickIn(event, 370, 270, 550, 340)) return false;
    }
    return false;
}

// Moves the player like verif() on master: walls and targets block the player,
// a crate moves onto floor (or onto a target, where it is locked).
void movePlayer(Map &map, int &player, int dx, int dy)
{
    const int x = player % kCols + dx, y = player / kCols + dy;
    if (x < 0 || x >= kCols || y < 0 || y >= kRows) return;
    const int next = y * kCols + x;
    const int value = map[next];
    if (value == Wall || value == Target || value == CrateOnTarget) return;
    if (value == Crate) {
        const int bx = x + dx, by = y + dy;
        if (bx < 0 || bx >= kCols || by < 0 || by >= kRows) return;
        const int beyond = by * kCols + bx;
        if (map[beyond] == Floor || map[beyond] == Player) map[beyond] = Crate;
        else if (map[beyond] == Target) map[beyond] = CrateOnTarget;
        else return;
        map[next] = Floor;
    }
    player = next;
}

// Plays a level. builtin: 1..6; otherwise a personal slot (record = false when
// the level is test-played from the editor).
Choice playLevel(App &app, const Map &start, bool builtin, int number, bool record)
{
    for (;;) {
        Map map = start;
        int player = -1, targets = 0;
        for (int cell = 0; cell < kCells; ++cell) {
            if (map[cell] == Empty) map[cell] = Floor; // never painted in the editor = floor
            if (map[cell] == Player) { player = cell; map[cell] = Floor; }
            if (map[cell] == Target) ++targets;
        }
        if (!builtin && (player < 0 || targets == 0)) {
            const Choice choice = incompleteLevel(app);
            if (choice == Choice::Replay) continue;
            return choice;
        }
        Map initial = start;
        for (int &cell : initial) if (cell == Empty) cell = Floor;
        const char *facing = "keeper_up.png";
        const Uint64 started = SDL_GetTicks();
        bool restart = false;
        while (!restart) {
            // board, buttons, player, then timer (same layering as master)
            drawBoard(app, map, nullptr);
            blit(app, "back.bmp", 0, 0);
            blit(app, "retry.bmp", 9 * kTile, 0);
            const int px = (player % kCols) * kTile, py = (player / kCols) * kTile;
            blit(app, "floor.bmp", px, py);
            blit(app, facing, px, py);
            timerPanel(app, started);
            present(app);

            bool won = true;
            for (int cell = 0; cell < kCells; ++cell)
                if (initial[cell] == Target && map[cell] != CrateOnTarget) won = false;
            if (won) {
                const unsigned seconds = static_cast<unsigned>((SDL_GetTicks() - started) / 1000);
                if (builtin) {
                    recordBestTime(number, seconds);
                    markCompleted(false, number);
                    const Choice choice = builtinVictory(app, seconds);
                    if (choice == Choice::Replay) break;
                    return choice;
                }
                if (record) {
                    recordBestTime(kBuiltinLevels + number, seconds);
                    markCompleted(true, number);
                }
                const Choice choice = customVictory(app, seconds);
                if (choice == Choice::Replay) break;
                return choice;
            }

            SDL_Event event;
            if (!waitEvent(app, event, 100)) continue; // refresh the timer
            if (app.quit) return Choice::Quit;
            if (event.type == SDL_EVENT_KEY_DOWN) {
                switch (event.key.key) {
                case SDLK_UP: facing = "keeper_up.png"; movePlayer(map, player, 0, -1); break;
                case SDLK_DOWN: facing = "keeper_down.png"; movePlayer(map, player, 0, 1); break;
                case SDLK_LEFT: facing = "keeper_left.png"; movePlayer(map, player, -1, 0); break;
                case SDLK_RIGHT: facing = "keeper_right.png"; movePlayer(map, player, 1, 0); break;
                default: break;
                }
            }
            if (clickIn(event, 0, 0, 68, 68)) return Choice::Back;
            if (clickIn(event, 9 * kTile, 0, kWidth, 68)) restart = true;
        }
    }
}

// Level editor for one personal slot.
Choice editLevel(App &app, int slot)
{
    Map map = readCustomLevel(slot);
    for (int &cell : map) if (cell == Empty) cell = Floor;
    static const char *toolbar[] = {"back.bmp", "save.bmp", "crate_editor.bmp", "keeper_editor.bmp", "wall_editor.bmp",
                                    "target_editor.bmp", "eraser.bmp", "floor.bmp", "go.bmp", "retry.bmp"};
    int tool = 0, highlight = 0;
    std::string message;
    bool saved = true;
    for (;;) {
        fillAll(app, 255, 255, 255);
        for (int cell = 0; cell < kCells; ++cell) {
            const int x = (cell % kCols) * kTile, y = (cell / kCols) * kTile;
            blit(app, "floor.bmp", x, y);
            if (map[cell] == Player) blit(app, "keeper_down.png", x, y);
            else if (const char *name = cellSprite(map[cell])) blit(app, name, x, y);
        }
        for (int row = 0; row < kRows; ++row) fill(app, {0, row * kTile, kWidth, 1}, 0, 0, 0);
        fill(app, {kTile, 34, 544, 1}, 0, 0, 0);
        for (int col = 1; col <= kCols; ++col) fill(app, {col * kTile, 0, 1, kHeight}, 0, 0, 0);
        for (int i = 0; i < 10; ++i) blitScaled(app, toolbar[i], {i * kTile + 17, 0, 34, 34});
        centeredText(app, 16, "GO", {544, 0, 68, 34}, {255, 250, 230, 255});
        if (highlight) {
            for (int i = 1; i <= 6; ++i) {
                if (i == highlight) fill(app, {i * kTile, 31, 68, 3}, 235, 55, 65);
                else fill(app, {i * kTile, 31, 68, 3}, 255, 255, 255);
            }
        }
        if (!message.empty()) {
            fill(app, {150, 486, 380, 40}, 29, 43, 61);
            const SDL_Rect inset{153, 489, 374, 34};
            fill(app, inset, 32, 55, 84);
            centeredText(app, 24, message.c_str(), inset, saved ? kGold : SDL_Color{255, 90, 90, 255});
        }
        present(app);

        SDL_Event event;
        if (!waitEvent(app, event)) return Choice::Quit;
        if (app.quit) return Choice::Quit;
        if (!isClick(event)) continue;
        const int x = static_cast<int>(event.button.x), y = static_cast<int>(event.button.y);
        if (x < 0 || x >= kWidth || y < 0 || y >= kHeight) continue;
        if (y < 34) {
            const int index = x / kTile;
            message.clear();
            if (index == 0) return Choice::Back;
            if (index == 1) {
                saved = writeCustomLevel(map, slot, false);
                message = saved ? "NIVEAU SAUVEGARDE" : "ERREUR DE SAUVEGARDE";
                tool = highlight = 1;
            } else if (index >= 2 && index <= 6) {
                tool = highlight = index;
            } else if (index == 7) {
                tool = 7;
            } else if (index == 8) {
                const Choice choice = playLevel(app, map, false, slot, false);
                if (choice == Choice::Quit) return choice;
            } else if (index == 9) {
                // Reload the last saved version (empty floor if never saved).
                map = readCustomLevel(slot);
                for (int &value : map) if (value == Empty) value = Floor;
            }
            continue;
        }
        const int cell = (y / kTile) * kCols + x / kTile;
        int value = map[cell];
        if (tool == 3) {
            for (int &other : map) if (other == Player) other = Floor;
            value = Player;
        } else if (tool == 2) value = (value == Target || value == CrateOnTarget) ? CrateOnTarget : Crate;
        else if (tool == 4) value = Wall;
        else if (tool == 5) value = (value == Crate || value == CrateOnTarget) ? CrateOnTarget : Target;
        else if (tool == 6 || tool == 7) value = Floor;
        else continue;
        map[cell] = value;
    }
}

// Editor menu: MODIFIER and REINITIALISATION. Returns MainMenu or Quit.
Choice editorMenu(App &app)
{
    for (;;) {
        blit(app, "editor.bmp", 0, 0);
        blit(app, "back.bmp", 0, 0);
        gameText(app, 36, "EDITEUR DE NIVEAUX", 225, 60, {255, 231, 149, 255});
        button(app, 36, "MODIFIER", {180, 170, 320, 68});
        button(app, 36, "REINITIALISATION", {150, 290, 380, 68}, true);
        present(app);
        int mode = 0;
        SDL_Event event;
        while (mode == 0 && waitEvent(app, event)) {
            if (app.quit) return Choice::Quit;
            if (clickIn(event, 0, 0, 68, 68)) return Choice::MainMenu;
            if (clickIn(event, 180, 170, 500, 238)) mode = 1;
            if (clickIn(event, 150, 290, 530, 358)) mode = 2;
        }
        if (app.quit) return Choice::Quit;
        for (;;) {
            const int slot = customLevelList(app);
            if (app.quit) return Choice::Quit;
            if (slot == kListBack) break;
            if (mode == 1) {
                // The back arrow of the editor returns to the level list.
                if (editLevel(app, slot) == Choice::Quit) return Choice::Quit;
                continue;
            }
            if (confirmReset(app)) resetCustomLevel(slot);
            if (app.quit) return Choice::Quit;
        }
    }
}

// "1 - JOUER": built-in levels and personal levels.
Choice playMenu(App &app)
{
    for (;;) {
        const int choice = levelSelect(app);
        if (app.quit) return Choice::Quit;
        if (choice == 0) return Choice::MainMenu;
        if (choice == 7) {
            const int slot = customLevelList(app);
            if (app.quit) return Choice::Quit;
            if (slot == kListBack) continue;
            if (playLevel(app, readCustomLevel(slot), false, slot, true) == Choice::Quit) return Choice::Quit;
            continue;
        }
        const Choice result = playLevel(app, builtinLevel(choice), true, choice, true);
        if (result == Choice::Quit || result == Choice::MainMenu) return result;
    }
}

// Assets use relative paths. When the game is launched from another working
// directory (CLion run configuration, shortcut...), use the executable folder,
// where CMake copies every asset.
void useExecutableDirectoryForAssets()
{
    std::error_code error;
    if (std::filesystem::exists("sprites_mario_sokoban/floor.bmp", error)) return;
    if (const char *base = SDL_GetBasePath()) std::filesystem::current_path(std::filesystem::u8path(base), error);
}

} // namespace

int main(int argc, char *argv[])
{
    useExecutableDirectoryForAssets();
    bool fullscreen = false;
    for (int arg = 1; arg < argc; ++arg)
        if (std::strcmp(argv[arg], "--fullscreen") == 0 || std::strcmp(argv[arg], "-f") == 0) fullscreen = true;

    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_Log("SDL_Init: %s", SDL_GetError());
        return 1;
    }
    App app;
    app.window = SDL_CreateWindow("Mario Sokoban !", kWidth, kHeight, SDL_WINDOW_RESIZABLE);
    app.renderer = app.window ? SDL_CreateRenderer(app.window, nullptr) : nullptr;
    app.texture = app.renderer ? SDL_CreateTexture(app.renderer, SDL_PIXELFORMAT_XRGB8888, SDL_TEXTUREACCESS_STREAMING, kWidth, kHeight) : nullptr;
    app.canvas = SDL_CreateSurface(kWidth, kHeight, SDL_PIXELFORMAT_XRGB8888);
    if (!app.window || !app.renderer || !app.texture || !app.canvas) {
        SDL_Log("Initialisation: %s", SDL_GetError());
        SDL_Quit();
        return 1;
    }
    SDL_SetTextureScaleMode(app.texture, SDL_SCALEMODE_NEAREST);
    SDL_SetRenderLogicalPresentation(app.renderer, kWidth, kHeight, SDL_LOGICAL_PRESENTATION_LETTERBOX);
    if (SDL_Surface *icon = SDL_LoadBMP("caisse.bmp")) {
        SDL_SetWindowIcon(app.window, icon);
        SDL_DestroySurface(icon);
    }
    if (fullscreen) toggleFullscreen(app);
    SDL_CreateDirectory("data");
    SDL_CreateDirectory("niveaux perso");

    while (!app.quit) {
        const int choice = mainMenu(app);
        if (app.quit || choice == 0) break;
        if (choice == 1) playMenu(app);
        else editorMenu(app);
    }

    for (auto &entry : app.sprites) if (entry.second) SDL_DestroySurface(entry.second);
    SDL_DestroySurface(app.canvas);
    SDL_DestroyTexture(app.texture);
    SDL_DestroyRenderer(app.renderer);
    SDL_DestroyWindow(app.window);
    SDL_Quit();
    return 0;
}
