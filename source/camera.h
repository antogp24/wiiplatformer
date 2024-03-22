#ifndef __CAMERA_H
#define __CAMERA_H

#include "common.h"

#define CAMERA_DELAY 30.0f

typedef struct {
    Vector2f target; /* The camera will follow this position */
    Vector2f offset; /* The render offset for all objects in the game */
} Camera2D;

void camera_update(Camera2D *cam);

#ifdef CAMERA_IMPL

void camera_update(Camera2D *cam)
{
    cam->offset.x += (cam->target.x - cam->offset.x) / CAMERA_DELAY;
    cam->offset.y += (cam->target.y - cam->offset.y) / CAMERA_DELAY;
}

#endif // CAMERA_IMPL

#endif // __CAMERA_H
