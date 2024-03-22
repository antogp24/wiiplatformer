#ifndef __COMMON_H
#define __COMMON_H

#define PI 3.1415926f
#define b32 int
typedef struct { f32 x, y; } Vector2f;
typedef struct { int x, y; } Vector2i;
typedef struct { f32 x, y, w, h; } Rectf;

#define ENTITIES_SPRITE_W 7  /* Number of tile horizontally */
#define ENTITIES_SPRITE_H 7  /* Number of tile vertically */

#define SHROOMS_SPRITE_W 16  /* Number of tile horizontally */
#define SHROOMS_SPRITE_H 9   /* Number of tile vertically */

#define CLOUDS_SPRITE_W 14   /* Number of tile horizontally */
#define CLOUDS_SPRITE_H 5    /* Number of tile vertically */

#define FONT_SPRITE_W 6      /* Number of tile horizontally */
#define FONT_SPRITE_H 9      /* Number of tile vertically */

#define HEARTS_SPRITE_W 5  /* Number of tile horizontally */
#define HEARTS_SPRITE_H 5  /* Number of tile vertically */

#define PLAYER_SPRITE_W 7    /* Number of tiles horizontally */
#define PLAYER_SPRITE_H 7    /* Number of tiles vertically */

#define SPRITE_ROWS 12       /* Number of tiles horizontally */
#define SPRITE_COLS 8        /* Number of tiles vertically */

#define SCALE 4              /* Scale of all textures in the game*/

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

u32 PICO8[16] = { PICO8_BLACK, PICO8_DARK_BLUE, PICO8_DARK_PURPLE, PICO8_DARK_GREEN, PICO8_BROWN, PICO8_DARK_GREY, PICO8_LIGHT_GREY, PICO8_WHITE, PICO8_RED, PICO8_ORANGE, PICO8_YELLOW, PICO8_GREEN, PICO8_BLUE, PICO8_LAVENDER, PICO8_PINK, PICO8_LIGHT_PEACH};

#define array_size(a) (sizeof(a) / sizeof(*a))
#define print_tex(tex)                                           \
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

bool AABB(f32 r1x, f32 r1y, f32 r1w, f32 r1h,
          f32 r2x, f32 r2y, f32 r2w, f32 r2h);
float ease_out_elastic(float x);
f32 randf();             // f32 from  0 to 1.
f32 randf1();            // f32 from -1 to 1.
int randi(int a, int b); // int from (a >= 0) to (b >= 0).

#ifdef COMMON_IMPL

bool AABB(f32 r1x, f32 r1y, f32 r1w, f32 r1h,
          f32 r2x, f32 r2y, f32 r2w, f32 r2h)
{
    return (r1x + r1w >= r2x && r1x <= r2x + r2w) &&
           (r1y + r1h >= r2y && r1y <= r2y + r2h);
}

float ease_out_elastic(float x)
{
    // 2^(-10x) * sin((2pi/3) * (10x - 3/4)) + 1
    const float c4 = (2.0f * PI) / 3.0f;

    if (x <= 0.0f) return 0.0f;
    if (x >= 1.0f) return 1.0f;

    return powf(2.0f, -10.0f*x) * sinf((10.0f*x - 0.75f) * c4) + 1.0f;
}

int randi(int a, int b)
{
    return (b - a) + (rand() % b); // int from (a >= 0) to (b >= 0).
}

f32 randf()
{
    return (f32)(rand() % 100) / 100.f; // f32 from 0 to 1
}

f32 randf1()
{
    return (f32)(rand() % 20) / 10.f - 1.0f; // f32 from -1 to 1
}

#endif // COMMON_IMPL

#endif //  __COMMON_H
