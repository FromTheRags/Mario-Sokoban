#pragma once

// SDL 1.2-shaped adapter for the original UI code. Every operation is backed
// by SDL 3 / SDL_image 3 / SDL_ttf 3; this keeps the screen and game logic
// intact while translating the removed 1.2 window, event, and text APIs.
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_ttf/SDL_ttf.h>

using NativeSDLColor = SDL_Color;
using NativeSDLEvent = SDL_Event;

struct LegacyColor { Uint8 r, g, b; };
struct LegacyEvent {
    int type;
    union {
        struct { struct { SDL_Keycode sym; } keysym; } key;
        struct { int x, y; } button;
    };
};

enum { LEGACY_QUIT = 1, LEGACY_KEYDOWN = 2, LEGACY_MOUSEBUTTONUP = 3 };
#define SDL_QUIT LEGACY_QUIT
#define SDL_KEYDOWN LEGACY_KEYDOWN
#define SDL_MOUSEBUTTONUP LEGACY_MOUSEBUTTONUP
#define SDL_HWSURFACE 0
#define SDL_DOUBLEBUF 0
#define SDL_WM_SetCaption LegacySetCaption
#define SDL_WM_SetIcon LegacySetIcon
#define SDL_SetVideoMode LegacySetVideoMode
#define SDL_EnableKeyRepeat LegacyEnableKeyRepeat
#define SDL_Flip LegacyFlip
#define SDL_CreateRGBSurface LegacyCreateRGBSurface
#define SDL_FillRect LegacyFillRect
#define SDL_FreeSurface SDL_DestroySurface
#define SDL_MapRGB(surface, r, g, b) SDL_MapSurfaceRGB((surface), (r), (g), (b))
#define SDLK_KP1 SDLK_KP_1
#define SDLK_KP2 SDLK_KP_2
#define SDLK_KP3 SDLK_KP_3
#define SDLK_KP4 SDLK_KP_4
#define SDLK_KP5 SDLK_KP_5
#define SDLK_KP6 SDLK_KP_6
#define SDLK_KP7 SDLK_KP_7
#define SDLK_KP8 SDLK_KP_8
#define SDLK_KP9 SDLK_KP_9

inline SDL_Window *legacy_sdl3_window = nullptr;
inline SDL_Surface *legacy_sdl3_icon = nullptr;
inline SDL_Surface *legacy_sdl3_screen = nullptr;
inline const char *legacy_sdl3_title = "Mario Sokoban";

inline int LegacyInit(SDL_InitFlags flags)
{
    return SDL_Init(flags) ? 0 : -1;
}

inline void LegacySetCaption(const char *title, const char *)
{
    legacy_sdl3_title = title;
    if (legacy_sdl3_window) SDL_SetWindowTitle(legacy_sdl3_window, title);
}

inline void LegacySetIcon(SDL_Surface *icon, const Uint8 *)
{
    legacy_sdl3_icon = icon;
    if (legacy_sdl3_window && icon) SDL_SetWindowIcon(legacy_sdl3_window, icon);
}

inline SDL_Surface *LegacySetVideoMode(int width, int height, int, Uint32)
{
    legacy_sdl3_window = SDL_CreateWindow(legacy_sdl3_title, width, height, 0);
    if (legacy_sdl3_window && legacy_sdl3_icon) SDL_SetWindowIcon(legacy_sdl3_window, legacy_sdl3_icon);
    legacy_sdl3_screen = legacy_sdl3_window ? SDL_GetWindowSurface(legacy_sdl3_window) : nullptr;
    return legacy_sdl3_screen;
}

inline int LegacyEnableKeyRepeat(int, int) { return 0; }

inline int LegacyWaitEvent(LegacyEvent *legacy)
{
    NativeSDLEvent event;
    if (!legacy || !SDL_WaitEvent(&event)) return 0;
    legacy->type = 0;
    if (event.type == SDL_EVENT_QUIT) legacy->type = LEGACY_QUIT;
    else if (event.type == SDL_EVENT_KEY_DOWN) {
        legacy->type = LEGACY_KEYDOWN;
        legacy->key.keysym.sym = event.key.key;
    } else if (event.type == SDL_EVENT_MOUSE_BUTTON_UP) {
        legacy->type = LEGACY_MOUSEBUTTONUP;
        legacy->button.x = static_cast<int>(event.button.x);
        legacy->button.y = static_cast<int>(event.button.y);
    }
    return 1;
}

inline int LegacyFlip(SDL_Surface *)
{
    return legacy_sdl3_window && SDL_UpdateWindowSurface(legacy_sdl3_window) ? 0 : -1;
}

inline SDL_Surface *LegacyCreateRGBSurface(Uint32, int width, int height, int,
                                            Uint32, Uint32, Uint32, Uint32)
{
    const SDL_PixelFormat format = legacy_sdl3_screen ? legacy_sdl3_screen->format : SDL_PIXELFORMAT_ARGB8888;
    return SDL_CreateSurface(width, height, format);
}

inline int LegacyFillRect(SDL_Surface *surface, const SDL_Rect *rect, Uint32 color)
{
    return SDL_FillSurfaceRect(surface, rect, color) ? 0 : -1;
}

inline NativeSDLColor LegacySDLColor(LegacyColor color)
{
    return NativeSDLColor{color.r, color.g, color.b, 255};
}

inline SDL_Surface *LegacyRenderBlended(TTF_Font *font, const char *text, LegacyColor color)
{
    return TTF_RenderText_Blended(font, text, 0, LegacySDLColor(color));
}

inline SDL_Surface *LegacyRenderShaded(TTF_Font *font, const char *text,
                                       LegacyColor foreground, LegacyColor background)
{
    return TTF_RenderText_Shaded(font, text, 0, LegacySDLColor(foreground), LegacySDLColor(background));
}

#define SDL_Init LegacyInit
#define SDL_Color LegacyColor
#define SDL_Event LegacyEvent
#define SDL_WaitEvent LegacyWaitEvent
#define TTF_RenderText_Blended LegacyRenderBlended
#define TTF_RenderText_Shaded LegacyRenderShaded
