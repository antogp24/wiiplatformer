#ifndef __CLOUDS_H
#define __CLOUDS_H

#include <stdlib.h>
#include <grrlib.h>
#include "common.h"

#define MAX_CLOUDS 3

typedef struct {
    f32     x [MAX_CLOUDS];
    f32     y [MAX_CLOUDS];
    int frame [MAX_CLOUDS];
    f32  size [MAX_CLOUDS];

    int count;
} SOA_Clouds;

Rectf clouds_get_rect(SOA_Clouds *c, int index);
void clouds_render(SOA_Clouds *c, int index, GRRLIB_texImg *tex, int time, Vector2f offset);
void clouds_delete(SOA_Clouds *c, int index);
void clouds_append(SOA_Clouds *c, f32 x, f32 y, int frame, f32 size);

#ifdef CLOUDS_IMPL

Rectf clouds_get_rect(SOA_Clouds *c, int index)
{
    f32 scale = c->size[index] * SCALE;
    return (Rectf) {
         c->x[index] - (CLOUDS_SPRITE_W/2 * scale),
         c->y[index] - (CLOUDS_SPRITE_H/2 * scale),
         CLOUDS_SPRITE_W * scale,
         CLOUDS_SPRITE_H * scale,
    };
}

void clouds_render(SOA_Clouds *c, int index, GRRLIB_texImg *tex, int time, Vector2f offset)
{
    anim_update(ANIM_cloud, &c->frame[index], time);
    f32 scale = c->size[index] * SCALE;
    f32 x = c->x[index] - offset.x;
    f32 y = c->y[index] - offset.y;
    GRRLIB_DrawTile(x, y, tex, 0, scale, scale, 0xFFFFFFFF, c->frame[index] % ANIM_cloud.frames);
}

void clouds_delete(SOA_Clouds *c, int index)
{
    memmove(     c->x + index,      c->x + index + 1, sizeof(f32) * (c->count - index));
    memmove(     c->y + index,      c->y + index + 1, sizeof(f32) * (c->count - index));
    memmove( c->frame + index,  c->frame + index + 1, sizeof(int) * (c->count - index));
    memmove(  c->size + index,   c->size + index + 1, sizeof(f32) * (c->count - index));
    c->count -= 1;
}

void clouds_append(SOA_Clouds *c, f32 x, f32 y, int frame, f32 size)
{
    if (c->count + 1 > MAX_CLOUDS) clouds_delete(c, 0);

    c->count += 1;

    int last = c->count - 1;
    c->x[last] = x;
    c->y[last] = y;
    c->frame[last] = frame;
    c->size[last] = size;
}

#endif //  CLOUDS_IMPL

#endif //  __CLOUDS_H
