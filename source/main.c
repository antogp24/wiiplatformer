#include <stdio.h>              // stdio
#include <stdlib.h>             // stdlib
#include <time.h>               // stdtime
#include <math.h>               // stdmath
#include <gccore.h>             // idk
#include <wiiuse/wpad.h>        // wii remotes input
#include <fat.h>                // idk
#include <asndlib.h>            // initializing sound library
#include <grrlib.h>             // wii graphics library

// Resources baked in.
#include "boing_ogg.h"
#include "cloud_ogg.h"
#include "coin_ogg.h"
#include "dash_ogg.h"
#include "jump_ogg.h"
#include "font_png.h"
#include "logo_png.h"
#include "cloud_png.h"
#include "coin_png.h"
#include "heart_png.h"
#include "cursor_png.h"
#include "shrooms_png.h"
#include "entities_png.h"

// Other source files.
#include "oggplayer.h"

#define COMMON_IMPL
#include "common.h"

#define PARTICLES_IMPL
#include "particles.h"

#define ANIMATION_IMPL
#include "animation.h"

#define CLOUDS_IMPL
#include "clouds.h"

#define CAMERA_IMPL
#include "camera.h"

// Constants.

#define MAX_PLAYER_HEARTS 3
#define MAX_PLAYER_DASHES 2
#define MAX_PLAYER_PUFFS 3
#define GRAVITY 0.5f

// Types.

typedef struct {
    b32 left;
    b32 right;
    b32 jump;
    b32 dash;
    b32 puff;
} Keys;

typedef struct {
    Vector2f pos;
    int frame;
} Shroom;

typedef struct {
    Vector2f pos;
    Vector2f vel;

    Keys keys;
    int air_time;

    Animation anim;
    int frame;
    int flip;

    f32 dash_time;
    b32 dashing;
    int available_dashes;

    f32 puff_time;
    b32 puffing;
    int available_puffs;

    SOA_Particles particles;
    SOA_Clouds clouds;

    int hearts;
} Player;


int main(int argc, char **argv)
{
    srand(time(NULL));
    GRRLIB_Init();
    GRRLIB_SetAntiAliasing(false);

    const u16 screenW = rmode->fbWidth;
    const u16 screenH = rmode->efbHeight;

    WPAD_Init();
    WPAD_SetIdleTimeout(60*10);
    WPAD_SetDataFormat(WPAD_CHAN_0, WPAD_FMT_BTNS_ACC_IR);
	ASND_Init();

    u64 time = 0;
    ir_t wiimoteP1;
    expansion_t wiimote_expansion;

    Player player = {0};
    player.hearts = MAX_PLAYER_HEARTS;
    player.available_dashes = MAX_PLAYER_DASHES;
    player.available_puffs = MAX_PLAYER_PUFFS;
    player.anim = ANIM_player_idle;
    player.pos.x = screenW/2;
    player.pos.y = screenH/2;
    player.flip = 1;

    Camera2D cam = {0};

    // Loading resources from baked in memory.

    GRRLIB_texImg *tex_entities = GRRLIB_LoadTexture(entities_png);
    GRRLIB_texImg *tex_shrooms = GRRLIB_LoadTexture(shrooms_png);
    GRRLIB_texImg *tex_cursor = GRRLIB_LoadTexture(cursor_png);
    GRRLIB_texImg *tex_clouds = GRRLIB_LoadTexture(cloud_png);
    GRRLIB_texImg *tex_heart = GRRLIB_LoadTexture(heart_png);
    GRRLIB_texImg *tex_coin = GRRLIB_LoadTexture(coin_png);
    GRRLIB_texImg *tex_logo = GRRLIB_LoadTexture(logo_png);
    GRRLIB_texImg *tex_font = GRRLIB_LoadTexture(font_png);

    GRRLIB_InitTileSet(tex_entities, PLAYER_SPRITE_W, PLAYER_SPRITE_H, 0);
    GRRLIB_InitTileSet(tex_shrooms, SHROOMS_SPRITE_W, SHROOMS_SPRITE_H, 0);
    GRRLIB_InitTileSet(tex_clouds, CLOUDS_SPRITE_W, CLOUDS_SPRITE_H, 0);
    GRRLIB_InitTileSet(tex_font, FONT_SPRITE_W, FONT_SPRITE_H, 32);

    GRRLIB_SetMidHandle(tex_clouds, true);
    GRRLIB_SetMidHandle(tex_cursor, true);
    GRRLIB_SetMidHandle(tex_entities, true);

	// PlayOgg(boing_ogg, boing_ogg_size, 0, OGG_ONE_TIME);
	// PlayOgg(coin_ogg, coin_ogg_size, 0, OGG_ONE_TIME);

#if 0  /* Intro gets annoying for developing the game */
    for (int i = 0; i <= 255; i += 1) {
        GRRLIB_FillScreen(PICO8_BLUE);
        GRRLIB_Printf(screenW/2-array_size("Made with")*tex_font->tilew, 160, tex_font, (PICO8_WHITE&0xFFFFFF00)|i, 2, "Made with");
        GRRLIB_DrawImg(screenW/2-tex_logo->w/2, screenH/2-tex_logo->h/2, tex_logo, 0, 1, 1, (PICO8_WHITE&0xFFFFFF00)|i);
        GRRLIB_Printf(screenW/2-array_size("version "GRRLIB_VER_STRING)*tex_font->tilew, 300, tex_font, (PICO8_WHITE&0xFFFFFF00)|i, 2, "version %s", GRRLIB_VER_STRING);
        GRRLIB_Render();
    }
    for (int i = 255; i >= 0; i -= 3) {
        GRRLIB_FillScreen(PICO8_BLUE);
        GRRLIB_Printf(screenW/2-array_size("Made with")*tex_font->tilew, 160, tex_font, (PICO8_WHITE&0xFFFFFF00)|i, 2, "Made with");
        GRRLIB_DrawImg(screenW/2-tex_logo->w/2, screenH/2-tex_logo->h/2, tex_logo, 0, 1, 1, (PICO8_WHITE&0xFFFFFF00)|i);
        GRRLIB_Printf(screenW/2-array_size("version "GRRLIB_VER_STRING)*tex_font->tilew, 300, tex_font, (PICO8_WHITE&0xFFFFFF00)|i, 2, "version %s", GRRLIB_VER_STRING);
        GRRLIB_Render();
    }
#endif

    while (true) {
        time += 1;

        WPAD_ScanPads();
        WPAD_SetVRes(WPAD_CHAN_0, screenW, screenH);
        WPAD_IR(WPAD_CHAN_0, &wiimoteP1);
        WPAD_Expansion(WPAD_CHAN_0, &wiimote_expansion);

        switch (wiimote_expansion.type) {
            case EXP_NONE:    break;
            case EXP_CLASSIC: break;
        }

        // Viewport IR correction.
        Vector2i P1 = {wiimoteP1.sx - 200, wiimoteP1.sy - 213};

        const u32 wpaddown = WPAD_ButtonsDown(0);
        const u32 wpadheld = WPAD_ButtonsHeld(0);

        // Close the Game with the HOME button.
        if (wpaddown & WPAD_BUTTON_HOME || wpaddown & WPAD_CLASSIC_BUTTON_HOME) {
            break;
        }

        // Updating the camera.
        cam.target = (Vector2f){player.pos.x - screenW/2, player.pos.y - screenH/2};
        camera_update(&cam);

        // Gravity.
        if (!player.dashing && !player.puffing) {
            player.vel.y += GRAVITY;
            player.pos.y += player.vel.y;
            if (player.pos.y > screenH - SCALE*PLAYER_SPRITE_H - 10) player.pos.y = screenH - SCALE*PLAYER_SPRITE_H - 10;
        }

        // Checking if the player is on the ground.
        Rectf player_rect = {
            player.pos.x - SCALE*PLAYER_SPRITE_W/2,
            player.pos.y - SCALE*PLAYER_SPRITE_H/2,
            PLAYER_SPRITE_W * SCALE,
            PLAYER_SPRITE_H * SCALE,
        };

        bool player_on_ground = (player.pos.y >= screenH - SCALE*PLAYER_SPRITE_H - 10);

        for (int i = 0; i < player.clouds.count; i++) {
            Rectf cloud_rect = clouds_get_rect(&player.clouds, i);

            bool colliding = AABB(player_rect.x, player_rect.y, player_rect.w, player_rect.h,
                                   cloud_rect.x,  cloud_rect.y,  cloud_rect.w,  cloud_rect.h);

            if (colliding && player.vel.y > 0 && player.pos.y <= cloud_rect.y) {
                player_on_ground = true;
                player.pos.y = MIN(player.pos.y, cloud_rect.y - player_rect.h/2);
            }
        }

        if (player_on_ground) {
            player.vel.y = 0;
            player.air_time = 0;
            player.available_dashes = MAX_PLAYER_DASHES;
            player.available_puffs = MAX_PLAYER_PUFFS;
        } else {
            player.air_time++;
        }

        // Checking for input.
        player.keys.left  = (wpadheld & WPAD_BUTTON_UP   || wpadheld & WPAD_CLASSIC_BUTTON_LEFT);
        player.keys.right = (wpadheld & WPAD_BUTTON_DOWN || wpadheld & WPAD_CLASSIC_BUTTON_RIGHT);
        player.keys.jump  = (wpaddown & WPAD_BUTTON_2    || wpaddown & WPAD_CLASSIC_BUTTON_B);
        player.keys.dash  = (wpaddown & WPAD_BUTTON_1    || wpaddown & WPAD_CLASSIC_BUTTON_Y);
        player.keys.puff  = (wpaddown & WPAD_BUTTON_B    || wpaddown & WPAD_CLASSIC_BUTTON_A);

        // Moving the player.
        const f32 speed = 2.5f;
        if (player.keys.left) {
            player.flip = -1;
            player.pos.x -= speed;
        }
        if (player.keys.right) {
            player.flip = 1;
            player.pos.x += speed;
        }
        if (player.keys.jump) {
            if (player_on_ground) {
                PlayOgg(jump_ogg, jump_ogg_size, 0, OGG_ONE_TIME);
                player.vel.y = -3*SCALE;
            }
        }
        if (player.keys.dash && player.available_dashes > 0) {
            PlayOgg(dash_ogg, dash_ogg_size, 0, OGG_ONE_TIME);
            player.vel.y = 0;
            player.dashing = true;
            player.available_dashes--;
        }
        if (player.keys.puff && player.available_puffs > 0) {
            PlayOgg(cloud_ogg, cloud_ogg_size, 0, OGG_ONE_TIME);
            player.vel.y = 0;
            player.puffing = true;
            player.available_puffs--;
            clouds_append(&player.clouds, player.pos.x, player.pos.y, 0, 1.0f);
        }

        // Player actions
        if (player.dashing) {
            player.dash_time += 0.02f;
            player.pos.x += player.flip * 2.0f * SCALE * ease_out_elastic(player.dash_time);
            if (player.dash_time > 0.7f) {
                player.dash_time = 0;
                player.dashing = false;
                player.vel.y = 0;
            }
        }
        if (player.puffing) {
            player.puff_time += 0.01f;
            player.pos.y -= 2.0f * ease_out_elastic(player.puff_time);
            if (player.puff_time > 0.3f) {
                player.puff_time = 0;
                player.puffing = false;
                player.vel.y = 0;
            }
        }

        // Handling animation
        if (player.dashing) {
            anim_set(&player.anim, ANIM_player_dash, &player.frame);

        } else if (player.puffing) {
            anim_set(&player.anim, ANIM_player_puff, &player.frame);

        } else if (player.air_time > SCALE) {
            anim_set(&player.anim, ANIM_player_jump, &player.frame);

        } else if (player.keys.left || player.keys.right) {
            anim_set(&player.anim, ANIM_player_run, &player.frame);

        } else {
            anim_set(&player.anim, ANIM_player_idle, &player.frame);
        }
        anim_update(player.anim, &player.frame, time);

        // Adding particles.
        if (player.dashing) {
            f32 x = player.pos.x + (randf1() * SCALE * PLAYER_SPRITE_W);
            f32 y = player.pos.y + (randf1() * SCALE * PLAYER_SPRITE_H);
            f32 velx = randf() * player.flip * SCALE * -1;
            particles_append(&player.particles, x, y, velx, randf1(), randi(SCALE, SCALE+2), PICO8_YELLOW);

        } else if (player.puffing) {
            int last = player.clouds.count - 1;
            f32 x = player.clouds.x[last] + (randf1() * SCALE * CLOUDS_SPRITE_W/2);
            f32 y = player.clouds.y[last] + (randf1() * SCALE * CLOUDS_SPRITE_H/2);
            particles_append(&player.particles, x, y, randf1(), randf1(), randi(SCALE, SCALE+2), PICO8_WHITE);

        } else if (time % 15 == 0) {
            f32 x = player.pos.x + (randf1() * SCALE * PLAYER_SPRITE_W/2);
            f32 y = player.pos.y + (randf1() * SCALE * PLAYER_SPRITE_H/2);
            particles_append(&player.particles, x, y, randf1()/2, randf1()/2, randi(SCALE/2, SCALE), PICO8_WHITE);
        }

        // Updating the clouds size to remove them.
        for (int i = 0; i < player.clouds.count; i++) {
            if (time % 30 == 0) {
                player.clouds.size[i] -= 0.015f;
                if (player.clouds.size[i] <= 0.30f) {
                    clouds_delete(&player.clouds, i);
                }
            }
        }

        // Opening menus.
        if (wpaddown & WPAD_BUTTON_MINUS) {
            WPAD_Rumble(WPAD_CHAN_0, 1);
            WPAD_Rumble(WPAD_CHAN_0, 0);
        }
        if (wpaddown & WPAD_BUTTON_PLUS) {
            WPAD_Rumble(WPAD_CHAN_0, 1);
            WPAD_Rumble(WPAD_CHAN_0, 0);
        }

        WPAD_Rumble(WPAD_CHAN_0, 0);
        GRRLIB_FillScreen(PICO8_BLUE);

        // Rendering Code
        {
            for (int i = 0; i < player.particles.count; i++) {
                particles_update(&player.particles, i, 0.01f, randf()*0.2f);
                particles_render(&player.particles, i, cam.offset);
            }

            for (int i = 0; i < player.clouds.count; i++) {
                clouds_render(&player.clouds, i, tex_clouds, time, cam.offset);
            }

            GRRLIB_DrawImg(screenW/2 - cam.offset.x, screenH/2+sinf(time/15.0f)*SCALE - cam.offset.y, tex_coin, 0, SCALE, SCALE, 0xFFFFFFFF);

            // GRRLIB_DrawTile(shroom.pos.x - cam.offset.x, shroom.pos.y - cam.offset.y, tex_shrooms, 0, SCALE, SCALE, 0xFFFFFFFF, shroom.frame % ANIM_shroom.frames);
            GRRLIB_DrawTile(player.pos.x - cam.offset.x, player.pos.y - cam.offset.y, tex_entities, 0, player.flip*SCALE, SCALE, 0xFFFFFFFF, player.anim.row * SPRITE_ROWS + player.frame);

            // HUD.
            for (int i = 0; i < player.hearts; i++) {
                f32 x = i * SCALE * (HEARTS_SPRITE_W + 1) + SCALE;
                f32 y = SCALE;
                GRRLIB_DrawImg(x, y, tex_heart, 0, SCALE, SCALE, 0xFFFFFFFF);
            }
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
