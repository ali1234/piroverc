/* control.c -- control packet buffer
 *
 * Copyright (C) 2015 Alistair Buxton <a.j.buxton@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdio.h>

#include <glib.h>

#include <SDL.h>

#include "control.h"

signed short motors[4];
unsigned short lights;
unsigned short flags;

SDL_GameController *gc;

static GMutex control_mutex;

void control_set_motors(signed short *m)
{
    g_mutex_lock (&control_mutex);

    motors[0] = m[0];
    motors[1] = m[1];
    motors[2] = m[2];
    motors[3] = m[3];

    g_mutex_unlock (&control_mutex);
}

void control_set_lights(signed short l)
{
    g_mutex_lock (&control_mutex);

    lights = l;

    g_mutex_unlock (&control_mutex);
}

void control_set_headlights(gboolean on)
{
    g_mutex_lock (&control_mutex);

    if (on)
        lights |= 1;
    else
        lights &= ~1;

    g_mutex_unlock (&control_mutex);
}

void control_set_taillights(gboolean on)
{
    g_mutex_lock (&control_mutex);

    if (on)
        lights |= 2;
    else
        lights &= ~2;

    g_mutex_unlock (&control_mutex);
}

void control_set_hazardlights(gboolean on)
{
    g_mutex_lock (&control_mutex);

    if (on)
        lights |= 4;
    else
        lights &= ~4;

    g_mutex_unlock (&control_mutex);
}

void control_set_flags(signed short f)
{
    g_mutex_lock (&control_mutex);

    flags = f;

    g_mutex_unlock (&control_mutex);
}

void control_get_packet(char *buf)
{
    g_mutex_lock (&control_mutex);

    buf[0] = motors[0] >> 8;
    buf[1] = motors[0] & 0xff;
    buf[2] = motors[1] >> 8;
    buf[3] = motors[1] & 0xff;
    buf[4] = motors[2] >> 8;
    buf[5] = motors[2] & 0xff;
    buf[6] = motors[3] >> 8;
    buf[7] = motors[3] & 0xff;

    buf[8] = lights >> 8;
    buf[9] = lights & 0xff;

    buf[10] = flags >> 8;
    buf[11] = flags & 0xff;

    g_mutex_unlock (&control_mutex);
}

#define DZ (10000)

static gboolean get_controls (gpointer unused, gboolean ignored)
{
    SDL_JoystickUpdate();
    int a = -SDL_GameControllerGetAxis(gc, SDL_CONTROLLER_AXIS_RIGHTY );
    int b = -SDL_GameControllerGetAxis(gc, SDL_CONTROLLER_AXIS_LEFTY);
    unsigned short da = 0, db = 0;

    if(a < -DZ) {
        da = 0x8000;
        a = -a;
    }
    if(b < -DZ) {
        db = 0x8000;
        b = -b;
    }
    if(a < DZ) a = 0;
    if(b < DZ) b = 0;
    a = a / 7;
    b = b / 7;
    da |= a;
    db |= b;

    g_mutex_lock (&control_mutex);

    motors[0] = motors[2] = da;//0;
    motors[1] = motors[3] = db;//0;

    g_mutex_unlock (&control_mutex);

    return TRUE;
}

void control_start(void)
{
    SDL_Init(SDL_INIT_JOYSTICK|SDL_INIT_GAMECONTROLLER);
    SDL_GameControllerAddMapping("030000005e0400008902000021010000,Classic XBOX Controller,a:b0,b:b1,y:b4,x:b3,start:b7,guide:,back:b6,leftstick:b8,rightstick:b9,dpup:h0.1,dpleft:h0.8,dpdown:h0.4,dpright:h0.2,leftx:a0,lefty:a1,rightx:a3,righty:a4,lefttrigger:a2,righttrigger:a5,leftshoulder:b5,rightshoulder:b2,");

    int nj = SDL_NumJoysticks();
    int cj = 0;
    while(cj < nj) { // Make racers for each valid game controller
        if(SDL_IsGameController(cj)) {
            printf("%s\n", SDL_JoystickNameForIndex(cj));
            gc = SDL_GameControllerOpen(cj);
        }
        cj++;
    }
    g_timeout_add(100, (GSourceFunc)get_controls, NULL);
}

void control_stop(void)
{
    ;
}
