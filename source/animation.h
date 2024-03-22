#ifndef __ANIMATION_H
#define __ANIMATION_H

#include "common.h"

typedef struct {
    int row;      /* On which row of the spritesheet it is */
    int frames;   /* How many frames the animation has */
    bool loop;    /* Loop the animation? */
    int wait;     /* How many game loop iterations to wait between each animation frame. */
} Animation;

#define ANIM_player_idle (Animation){0,  6, true,  5}
#define ANIM_player_run  (Animation){1,  7, true,  5}
#define ANIM_player_puff (Animation){2, 11, false, 5}
#define ANIM_player_jump (Animation){3,  6, false, 5}
#define ANIM_player_dash (Animation){4, 11, false, 5}
#define ANIM_enemy_idle  (Animation){5,  6, true,  5}
#define ANIM_enemy_run   (Animation){6,  7, true,  5}
#define ANIM_shroom      (Animation){0,  3, false, 5}
#define ANIM_cloud       (Animation){0,  6, false, 8}

void anim_update(Animation anim, int *frame, int time);
void anim_set(Animation *old, Animation new, int *frame);

#ifdef ANIMATION_IMPL

bool is_anim_done(Animation anim, int frame)
{
    return frame + 1 >= anim.frames;
}

void anim_set(Animation *old, Animation new, int *frame)
{
    if (old->row == new.row) return;

    old->row = new.row;
    old->frames = new.frames;
    old->loop = new.loop;
    old->wait = new.wait;
    *frame = 0;
}

void anim_update(Animation anim, int *frame, int time)
{
    if (time % anim.wait != 0) return;

    if (anim.loop) {
        *frame = (*frame + 1) % anim.frames;
    } else {
        *frame = MIN(*frame + 1, anim.frames - 1);
    }
}

#endif // ANIMATION_IMPL

#endif //  __ANIMATION_H
