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

#ifndef TESTENV
#include <wiringPi.h>
#endif

#include "output.h"
#include "input.h"
#include "helpers.h"
#include "hw_defs.h"

/*
 *  How things work?
 *
 *  There are four possible pin states for each axis: XA and XB can be either
 *  high or low. Atari will detect movement when one of the pins changes
 *  state. If it's XA before the XB, mouse moves to the right and the other
 *  way around.
 *
 *  Distance is set by the speed of mouse movement, according to what mouse
 *  reports on the USB. Input thread will detect direction change and set
 *  distance to zero which will stop the mouse movement in the previous
 *  direction.
 *
 *  Distance is used as a speed multiplier or rather a delay divider.
 *  ST mouse is quite slow so the initial delay is set 37.5ms for each pixel.
 *  Minimum delay is 375 microseconds anything below that and Atari starts
 *  to behave unpredictable. Maximum distance a mouse can report is 127 so it
 *  had to be limited.
 *
 *  The result is a surprisingly smooth mouse movement. :)
 *
 */
int x_state = 0;
void *x_thread() {
    int runs, delay;
    while(1) {
        /* Make this thread wait until the mouse_thread raises motion condition */
        pthread_mutex_lock(&mouse_motion_mtx);
        pthread_cond_wait(&mouse_motion, &mouse_motion_mtx);

        for (runs = 0; runs < x_dist; runs++) {
            if (x_dir == LEFT) x_state--; else x_state++;

            if ((x_dir == RIGHT) && x_state > 3) x_state = 0;
            if ((x_dir == LEFT) && x_state < 0) x_state = 3;

            if (x_state == 0) send_command(XA, HIGH);
            if (x_state == 1) send_command(XB, HIGH);
            if (x_state == 2) send_command(XA, LOW);
            if (x_state == 3) send_command(XB, LOW);
            if (x_dist > 1) {
                delay = (MAX_PULSE_DELAY / x_dist > MIN_PULSE_DELAY) ? MAX_PULSE_DELAY / x_dist : MIN_PULSE_DELAY;
                delayMicroseconds(delay);
            }
            x_dist--;
        }

        pthread_mutex_unlock(&mouse_motion_mtx);
    }
}

int y_state = 0;
void *y_thread() {
    int runs, delay;
    while(1) {
        for (runs = 0; runs < y_dist; runs++) {
            if (y_dir == UP) y_state--; else y_state++;

            if ((y_dir == DOWN) && y_state > 3) y_state = 0;
            if ((y_dir == UP) && y_state < 0) y_state = 3;

            if (y_state == 0) send_command(YA, HIGH);
            if (y_state == 1) send_command(YB, HIGH);
            if (y_state == 2) send_command(YA, LOW);
            if (y_state == 3) send_command(YB, LOW);
            if (y_dist > 1) {
                delay = (MAX_PULSE_DELAY / y_dist > MIN_PULSE_DELAY) ? MAX_PULSE_DELAY / y_dist : MIN_PULSE_DELAY;
                delayMicroseconds(delay);
            }
            y_dist--;
        }
    }
}
