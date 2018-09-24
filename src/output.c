/*
 * This file is part of the STuffEmu (https://github.com/BigWhale/STuffEmu)
 * Copyright (c) 2018 David Klasinc
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 2.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <stdbool.h>

#include "output.h"
#include "input.h"
#include "helpers.h"
#include "hw_defs.h"

unsigned char m_ev[3] = {0, 0, 0};

/* Atari ST will perform a mouse move each time mouse signal changes state,
 * we need to remember the previous state of both pins for both axes.
 * Mutexes are there to keep */
bool x_state[2] = { false, false };
bool y_state[2] = { false, false };

pthread_mutex_t x_axis_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t y_axis_mutex = PTHREAD_MUTEX_INITIALIZER;

void *output_thread() {
    pthread_t t_x;
    pthread_t t_y;
    pthread_t t_b;

    output_arg x_arg;
    output_arg y_arg;
    button_arg b_arg;

    int button = 0,
        b_left = 0,
        b_right = 0,
        old_r = 0,
        old_l = 0;

    bool send_b = false;

    signed char x, y;

    while (1) {
        /* Grab the mouse data and do stuff with it in peace */
        pthread_mutex_lock(&mouse_mutex);
        memcpy(m_ev, mouse_ev, 3);
        /* Reset axes */
        mouse_ev[1] = 0;
        mouse_ev[2] = 0;
        pthread_mutex_unlock(&mouse_mutex);

        button = m_ev[0];
        b_left = button & 0x1;
        b_right = ( button & 0x2 ) > 0;
        x = m_ev[1];
        y = m_ev[2];

        if (b_left != old_l) {
            b_arg.l_state = b_left;
            b_arg.left = 1;
            old_l = b_left;
            send_b = true;
        } else {
            b_arg.left = 0;
        }

        if (b_right != old_r) {
            b_arg.r_state = b_right;
            b_arg.right = 1;
            old_r = b_right;
            send_b = true;
        } else {
            b_arg.right = 0;
        }

        if (x != 0) {
            x_arg.dir = x < 0 ? LEFT : RIGHT;
            x_arg.distance = abs(x);
            pthread_create(&t_x, NULL, axis_thread, &x_arg);
        }
        if (y != 0) {
            y_arg.dir = y < 0 ? UP : DOWN;
            y_arg.distance = abs(y);
            pthread_create(&t_y, NULL, axis_thread, &y_arg);
        }

        if (send_b) {
            pthread_create(&t_b, NULL, b_thread, &b_arg);
            pthread_join(t_b, NULL);
            send_b = false;
        }

        if (x != 0) {
            pthread_join(t_x, NULL);
        }
        if (y != 0) {
            pthread_join(t_y, NULL);
        }
        //printf("X: %d, y: %d, BTNS: %d - %d\n", x, y, b_left, b_right);
        delay(ATARI_MAX_RATE);
    }
}

void *axis_thread(void *arg) {
    output_arg args;
    args = *((output_arg *)arg);

    if (args.dir == LEFT || args.dir == RIGHT) {
        /* Lock axis mutex for the duration of the pulse */
        pthread_mutex_lock(&x_axis_mutex);
        x_state[A] ^= true;
        x_state[B] ^= true;

        if (args.dir == LEFT) {
            sendCommand(XB, x_state[B]);
            delay(PULSE_DELAY); /* In the future we need some kind of divider for this */
            sendCommand(XA, x_state[A]);
        } else {
            sendCommand(XA, x_state[A]);
            delay(PULSE_DELAY);
            sendCommand(XB, x_state[B]);
        }
        pthread_mutex_unlock(&x_axis_mutex);
    } else {
        /* Lock axis mutex for the duration of the pulse */
        pthread_mutex_lock(&y_axis_mutex);
        y_state[A] ^= true;
        y_state[B] ^= true;

        if (args.dir == UP) {
            sendCommand(YA, y_state[A]);
            delay(PULSE_DELAY);
            sendCommand(YB, y_state[B]);
        } else {
            sendCommand(YB, y_state[B]);
            delay(PULSE_DELAY);
            sendCommand(YA, y_state[A]);
        }
        pthread_mutex_unlock(&y_axis_mutex);
    }
    printf("DIR: %d, DIS: %d\n", args.dir, args.distance);
    return NULL;
}

void *b_thread(void *arg) {
    button_arg args;
    args = *((button_arg *)arg);
    if (args.left) {
        sendCommand(BL, !args.l_state);
    }
    if (args.right) {
        sendCommand(BR, !args.r_state);
    }

    return NULL;
}
