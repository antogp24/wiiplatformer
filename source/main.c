#include <stdio.h>              // stdio
#include <stdlib.h>             // stdlib
#include <math.h>               // stdmath
#include <gccore.h>             // idk
#include <ogc/lwp_watchdog.h>   // gettime and ticks_to_millisecs
#include <wiiuse/wpad.h>        // wii remotes input
#include <fat.h>                // idk
#include <asndlib.h>            // initializing sound library
#include <grrlib.h>             // wii graphics library
#include "oggplayer.h"

// Resources baked in.
#include "boing_ogg.h"
#include "cloud_ogg.h"
#include "coin_ogg.h"
#include "dash_ogg.h"
#include "jump_ogg.h"
#include "font_png.h"
#include "logo_png.h"
#include "cursor_png.h"

// Constants for the Spritesheet.
#define SPRITE_ROWS 12
#define SPRITE_COLS 8

// Pico-8 colors.
#define PICO8_BLACK       0x000000FF
#define PICO8_DARK_BLUE   0x1D2B53FF
#define PICO8_DARK_PURPLE 0x7E2553FF
#define PICO8_DARK_GREEN  0x008751FF
#define PICO8_BROWN       0xAB5236FF
#define PICO8_DARK_GREY   0x5F574FFF
#define PICO8_LIGHT_GREY  0xC2C3C7FF
#define PICO8_WHITE       0xFFF1E8FF
#define PICO8_RED         0xFF004DFF
#define PICO8_ORANGE      0xFFA300FF
#define PICO8_YELLOW      0xFFEC27FF
#define PICO8_GREEN       0x00E436FF
#define PICO8_BLUE        0x29ADFFFF
#define PICO8_LAVENDER    0x83769CFF
#define PICO8_PINK        0xFF77A8FF
#define PICO8_LIGHT_PEACH 0xFFCCAAFF

typedef struct { f32 x, y; } Vector2f;
typedef struct { int x, y; } Vector2i;

typedef struct {
    Vector2f pos;
} Player;

#define PI 3.1415926f
#define array_size(a) (sizeof(a) / sizeof(*a))

#define print_texture_data(tex)                                  \
    do {                                                         \
        printf(#tex".w: %u\n", tex->w);                          \
        printf(#tex".h: %u\n", tex->h);                          \
        printf(#tex".format: %u\n", tex->format);                \
        printf(#tex".handlex: %i\n", tex->handlex);              \
        printf(#tex".handley: %i\n", tex->handley);              \
        printf(#tex".offsetx: %i\n", tex->offsetx);              \
        printf(#tex".offsety: %i\n", tex->offsety);              \
        printf(#tex".tiledtex: %d\n", tex->tiledtex);            \
        printf(#tex".tilew: %u\n", tex->tilew);                  \
        printf(#tex".tileh: %u\n", tex->tileh);                  \
        printf(#tex".nbtilew: %u\n", tex->nbtilew);              \
        printf(#tex".nbtileh: %u\n", tex->nbtileh);              \
        printf(#tex".tilestart: %u\n", tex->tilestart);          \
        printf(#tex".ofnormaltexx: %.2f\n", tex->ofnormaltexx);  \
        printf(#tex".ofnormaltexy: %.2f\n", tex->ofnormaltexy);  \
        printf(#tex".data: %p\n", tex->data);                    \
    } while(0)


int main(int argc, char **argv)
{
    unsigned int time = 0;
    ir_t wiimoteP1;

    Player player = {0};

    GRRLIB_Init();
    GRRLIB_SetAntiAliasing(false);
    const u16 screenW = rmode->fbWidth;
    const u16 screenH = rmode->efbHeight;
    WPAD_Init();
    WPAD_SetIdleTimeout(60*10);
    WPAD_SetDataFormat(WPAD_CHAN_0, WPAD_FMT_BTNS_ACC_IR);
	ASND_Init();

    // Loading resources from baked in memory.
    // min width: 20, min height: 28
    // otherwise textures don't load correctly.

    GRRLIB_texImg *tex_cursor = GRRLIB_LoadTexture(cursor_png);
    GRRLIB_SetMidHandle(tex_cursor, true);

    GRRLIB_texImg *tex_logo = GRRLIB_LoadTexture(logo_png);
    GRRLIB_texImg *tex_font = GRRLIB_LoadTexture(font_png);

    GRRLIB_InitTileSet(tex_font, 7, 9, 32);

	// PlayOgg(boing_ogg, boing_ogg_size, 0, OGG_ONE_TIME);
	// PlayOgg(cloud_ogg, cloud_ogg_size, 0, OGG_ONE_TIME);
	// PlayOgg(coin_ogg, coin_ogg_size, 0, OGG_ONE_TIME);
	// PlayOgg(dash_ogg, dash_ogg_size, 0, OGG_ONE_TIME);
	// PlayOgg(jump_ogg, jump_ogg_size, 0, OGG_ONE_TIME);

    for (int i = 0; i <= 255; i += 1) {
        GRRLIB_FillScreen(PICO8_BLUE);
        GRRLIB_Printf(screenW/2-array_size("Made with")*tex_font->tilew, 160, tex_font, (PICO8_WHITE&0xFFFFFF00)|i, 2, "Made with");
        GRRLIB_DrawImg(screenW/2-tex_logo->w/2, screenH/2-tex_logo->h/2, tex_logo, 0, 1, 1, (PICO8_WHITE&0xFFFFFF00)|i);
        GRRLIB_Printf(screenW/2-array_size("version "GRRLIB_VER_STRING)*tex_font->tilew, 300, tex_font, (PICO8_WHITE&0xFFFFFF00)|i, 2, "version %s", GRRLIB_VER_STRING);
        GRRLIB_Render();
    }
    for (int i = 255; i >= 0; i -= 2) {
        GRRLIB_FillScreen(PICO8_BLUE);
        GRRLIB_Printf(screenW/2-array_size("Made with")*tex_font->tilew, 160, tex_font, (PICO8_WHITE&0xFFFFFF00)|i, 2, "Made with");
        GRRLIB_DrawImg(screenW/2-tex_logo->w/2, screenH/2-tex_logo->h/2, tex_logo, 0, 1, 1, (PICO8_WHITE&0xFFFFFF00)|i);
        GRRLIB_Printf(screenW/2-array_size("version "GRRLIB_VER_STRING)*tex_font->tilew, 300, tex_font, (PICO8_WHITE&0xFFFFFF00)|i, 2, "version %s", GRRLIB_VER_STRING);
        GRRLIB_Render();
    }

    while (true) {
        time += 1;

        WPAD_ScanPads();
        WPAD_SetVRes(WPAD_CHAN_0, screenW, screenH);
        WPAD_IR(WPAD_CHAN_0, &wiimoteP1);

        // Viewport IR correction.
        Vector2i P1 = {wiimoteP1.sx - 200, wiimoteP1.sy - 213};

        const u32 wpaddown = WPAD_ButtonsDown(0);
        const u32 wpadheld = WPAD_ButtonsHeld(0);

        // Close the Game with the HOME button.
        if (wpaddown & WPAD_BUTTON_HOME) {
            break;
        }

        // Moving the player.
        const f32 speed = 1.5f;
        if (wpadheld & WPAD_BUTTON_LEFT) {
            player.pos.x -= speed;
        }
        if (wpadheld & WPAD_BUTTON_RIGHT) {
            player.pos.x += speed;
        }
        if (wpadheld & WPAD_BUTTON_UP) {
            player.pos.y -= speed;
        }
        if (wpadheld & WPAD_BUTTON_DOWN) {
            player.pos.y += speed;
        }

        // Opening menus.
        if (wpaddown & WPAD_BUTTON_MINUS) {
        }
        if (wpaddown & WPAD_BUTTON_PLUS) {
        }

        if (wpadheld & WPAD_BUTTON_1 && wpadheld & WPAD_BUTTON_2) {
            WPAD_Rumble(WPAD_CHAN_0, 1); // Rumble on
            WPAD_Rumble(WPAD_CHAN_0, 0); // Rumble off
        }

        WPAD_Rumble(WPAD_CHAN_0, 0);
        GRRLIB_FillScreen(PICO8_BLUE);

        // Rendering Code
        {
            // GRRLIB_DrawTile(320+player.pos.x, 240+player.pos.y, tex_sprite_png, 0, 2, 2, PICO8_WHITE, frame);
            GRRLIB_DrawImg(P1.x, P1.y, tex_cursor, 0, 1, 1, 0xFFFFFFFF);
        }
        GRRLIB_Render();
    }
    StopOgg();
    GRRLIB_FreeTexture(tex_font);
    GRRLIB_FreeTexture(tex_logo);
    GRRLIB_FreeTexture(tex_cursor);
    GRRLIB_Exit();
    exit(0);
}

