#pragma once

// Tiny original 5x7 typeface renderer for the SDL 1.2 build. This avoids a
// separate SDL_ttf binary while keeping the old surface-based call sites.
#include <SDL.h>
#include <algorithm>
#include <cstdint>
#include <new>
#include <string>
#include <vector>

struct TTF_Font { int point_size; };

inline int TTF_Init() { return 0; }
inline void TTF_Quit() {}
inline TTF_Font *TTF_OpenFont(const char *, int point_size)
{
    return new (std::nothrow) TTF_Font{std::max(8, point_size)};
}
inline void TTF_CloseFont(TTF_Font *font) { delete font; }

struct BitmapGlyph { char key; Uint8 rows[7]; };
inline const BitmapGlyph *BitmapGlyphFor(char key)
{
    static const BitmapGlyph glyphs[] = {
        {'A',{14,17,17,31,17,17,17}}, {'B',{30,17,17,30,17,17,30}},
        {'C',{14,17,16,16,16,17,14}}, {'D',{30,17,17,17,17,17,30}},
        {'E',{31,16,16,30,16,16,31}}, {'F',{31,16,16,30,16,16,16}},
        {'G',{14,17,16,23,17,17,15}}, {'H',{17,17,17,31,17,17,17}},
        {'I',{14,4,4,4,4,4,14}}, {'J',{7,2,2,2,18,18,12}},
        {'K',{17,18,20,24,20,18,17}}, {'L',{16,16,16,16,16,16,31}},
        {'M',{17,27,21,21,17,17,17}}, {'N',{17,25,21,19,17,17,17}},
        {'O',{14,17,17,17,17,17,14}}, {'P',{30,17,17,30,16,16,16}},
        {'Q',{14,17,17,17,21,18,13}}, {'R',{30,17,17,30,20,18,17}},
        {'S',{15,16,16,14,1,1,30}}, {'T',{31,4,4,4,4,4,4}},
        {'U',{17,17,17,17,17,17,14}}, {'V',{17,17,17,17,17,10,4}},
        {'W',{17,17,17,21,21,21,10}}, {'X',{17,17,10,4,10,17,17}},
        {'Y',{17,17,10,4,4,4,4}}, {'Z',{31,1,2,4,8,16,31}},
        {'0',{14,17,19,21,25,17,14}}, {'1',{4,12,4,4,4,4,14}},
        {'2',{14,17,1,2,4,8,31}}, {'3',{30,1,1,14,1,1,30}},
        {'4',{2,6,10,18,31,2,2}}, {'5',{31,16,16,30,1,1,30}},
        {'6',{14,16,16,30,17,17,14}}, {'7',{31,1,2,4,8,8,8}},
        {'8',{14,17,17,14,17,17,14}}, {'9',{14,17,17,15,1,1,14}},
        {'.',{0,0,0,0,0,12,12}}, {',',{0,0,0,0,4,4,8}},
        {'!',{4,4,4,4,4,0,4}}, {'?',{14,17,1,2,4,0,4}},
        {'-',{0,0,0,31,0,0,0}}, {'_',{0,0,0,0,0,0,31}},
        {'+',{0,4,4,31,4,4,0}}, {':',{0,12,12,0,12,12,0}},
        {';',{0,12,12,0,4,4,8}}, {'/',{1,2,2,4,8,8,16}},
        {'\\',{16,8,8,4,2,2,1}}, {'\'',{4,4,8,0,0,0,0}},
        {'"',{10,10,20,0,0,0,0}}, {'(',{2,4,8,8,8,4,2}},
        {')',{8,4,2,2,2,4,8}}, {'=',{0,31,0,31,0,0,0}},
        {'%',{17,2,4,8,17,0,0}}, {'*',{0,21,14,31,14,21,0}},
        {' ',{0,0,0,0,0,0,0}}
    };
    if (key >= 'a' && key <= 'z') key = static_cast<char>(key - 'a' + 'A');
    for (const BitmapGlyph &glyph : glyphs) if (glyph.key == key) return &glyph;
    return BitmapGlyphFor('?');
}

inline std::vector<char> BitmapTextCharacters(const char *text)
{
    std::vector<char> result;
    if (!text) return result;
    const unsigned char *cursor = reinterpret_cast<const unsigned char *>(text);
    while (*cursor) {
        unsigned char first = *cursor++;
        if (first < 0x80) { result.push_back(static_cast<char>(first)); continue; }
        if ((first & 0xE0) == 0xC0 && *cursor) {
            unsigned char second = *cursor++;
            char replacement = '?';
            if (first == 0xC3) {
                if (second == 0xA0 || second == 0xA1 || second == 0xA2 || second == 0xA4 || second == 0x80 || second == 0x81) replacement='A';
                else if (second == 0xA7 || second == 0x87) replacement='C';
                else if (second == 0xA8 || second == 0xA9 || second == 0xAA || second == 0xAB || second == 0x88 || second == 0x89) replacement='E';
                else if (second == 0xAC || second == 0xAD || second == 0xAE || second == 0xAF || second == 0x8E || second == 0x8F) replacement='I';
                else if (second == 0xB1 || second == 0x91) replacement='N';
                else if (second == 0xB2 || second == 0xB3 || second == 0xB4 || second == 0xB6 || second == 0x92 || second == 0x93 || second == 0x94) replacement='O';
                else if (second == 0xB9 || second == 0xBA || second == 0xBB || second == 0xBC || second == 0x99 || second == 0x9A) replacement='U';
                else if (second == 0xBD || second == 0x9D) replacement='Y';
            }
            result.push_back(replacement);
        } else {
            while ((*cursor & 0xC0) == 0x80) ++cursor;
            result.push_back('?');
        }
    }
    return result;
}

inline SDL_Surface *BitmapRenderText(TTF_Font *font, const char *text,
                                     SDL_Color foreground, const SDL_Color *background)
{
    if (!font || !text) return NULL;
    const std::vector<char> characters = BitmapTextCharacters(text);
    const int scale = std::max(1, font->point_size / 12);
    const int advance = 6 * scale;
    const int width = std::max(1, static_cast<int>(characters.size()) * advance);
    const int height = 9 * scale;
    SDL_Surface *surface = SDL_CreateRGBSurface(SDL_SWSURFACE, width, height, 32,
        0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);
    if (!surface) return NULL;
    if (background) SDL_FillRect(surface, NULL, SDL_MapRGB(surface->format, background->r, background->g, background->b));
    else SDL_FillRect(surface, NULL, SDL_MapRGBA(surface->format, 0, 0, 0, 0));
    const Uint32 ink = SDL_MapRGB(surface->format, foreground.r, foreground.g, foreground.b);
    for (size_t index = 0; index < characters.size(); ++index) {
        const BitmapGlyph *glyph = BitmapGlyphFor(characters[index]);
        for (int row = 0; row < 7; ++row) for (int column = 0; column < 5; ++column) {
            if (!(glyph->rows[row] & (1u << (4 - column)))) continue;
            SDL_Rect pixel = {static_cast<Sint16>(static_cast<int>(index) * advance + column * scale),
                              static_cast<Sint16>(row * scale), static_cast<Uint16>(scale), static_cast<Uint16>(scale)};
            SDL_FillRect(surface, &pixel, ink);
        }
    }
    if (!background) SDL_SetAlpha(surface, SDL_SRCALPHA, SDL_ALPHA_OPAQUE);
    return surface;
}

inline SDL_Surface *TTF_RenderText_Blended(TTF_Font *font, const char *text, SDL_Color foreground)
{
    return BitmapRenderText(font, text, foreground, NULL);
}

inline SDL_Surface *TTF_RenderText_Shaded(TTF_Font *font, const char *text,
                                          SDL_Color foreground, SDL_Color background)
{
    return BitmapRenderText(font, text, foreground, &background);
}
