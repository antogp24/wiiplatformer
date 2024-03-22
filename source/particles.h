#ifndef __PARTICLES_H
#define __PARTICLES_H

#include <stdlib.h>
#include <grrlib.h>
#include "common.h"

#define MAX_PARTICLES 64

typedef struct {
    f32     x [MAX_PARTICLES];
    f32     y [MAX_PARTICLES];
    f32  velx [MAX_PARTICLES];
    f32  vely [MAX_PARTICLES];
    f32  size [MAX_PARTICLES];
    u32 color [MAX_PARTICLES];

    int count;
} SOA_Particles;

void particles_update(SOA_Particles *p, int index, f32 gravity, f32 deletion_speed);
void particles_render(SOA_Particles *p, int index, Vector2f offset);
void particles_append(SOA_Particles *p, f32 x, f32 y, f32 vx, f32 vy, f32 size, u32 color);
void particles_delete(SOA_Particles *p, int index);

#ifdef PARTICLES_IMPL

void particles_update(SOA_Particles *p, int index, f32 gravity, f32 deletion_speed)
{
    p->x[index] += p->velx[index];
    p->y[index] += p->vely[index];

    if (gravity > 0) p->vely[index] += gravity;
    
    p->size[index] -= deletion_speed;

    if (p->size[index] <= 0) {
        particles_delete(p, index);
    }
}

void particles_render(SOA_Particles *p, int index, Vector2f offset)
{
    GRRLIB_Rectangle(p->x[index] - p->size[index]/2 - offset.x,
                     p->y[index] - p->size[index]/2 - offset.y,
                     p->size[index],
                     p->size[index],
                     p->color[index],
                     true);
}

void particles_append(SOA_Particles *p, f32 x, f32 y, f32 vx, f32 vy, f32 size, u32 color)
{
    if (p->count + 1 > MAX_PARTICLES) return;

    p->count += 1;

    int last = p->count - 1;
    p->x[last] = x;
    p->y[last] = y;
    p->velx[last] = vx;
    p->vely[last] = vy;
    p->size[last] = size;
    p->color[last] = color;
}

void particles_delete(SOA_Particles *p, int index)
{
    memmove(    p->x + index,     p->x + index + 1, sizeof(f32) * (p->count - index));
    memmove(    p->y + index,     p->y + index + 1, sizeof(f32) * (p->count - index));
    memmove( p->velx + index,  p->velx + index + 1, sizeof(f32) * (p->count - index));
    memmove( p->vely + index,  p->vely + index + 1, sizeof(f32) * (p->count - index));
    memmove( p->size + index,  p->size + index + 1, sizeof(f32) * (p->count - index));
    memmove(p->color + index, p->color + index + 1, sizeof(u32) * (p->count - index));
    p->count -= 1;
}

#endif // PARTICLES_IMPL

#endif // __PARTICLES_H
