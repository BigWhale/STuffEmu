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

int x_state = 0;
void *x_thread() {
    int runs, delay;
    printf("Starting the X thread. ** E X T E R M I N A T E **\n");
    while(1) {
        for (runs = 0; runs < x_dist; runs++) {
            if (x_dir == LEFT) x_state--; else x_state++;

            if ((x_dir == RIGHT) && x_state > 3) x_state = 0;
            if ((x_dir == LEFT) && x_state < -0) x_state = 3;

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
    }
}

int y_state = 0;
void *y_thread() {
    int runs, delay;
    printf("Starting the Y thread. Why, oh why?\n");
    while(1) {
        for (runs = 0; runs < y_dist; runs++) {
            if (y_dir == UP) y_state--; else y_state++;

            if ((y_dir == DOWN) && y_state > 3) y_state = 0;
            if ((y_dir == UP) && y_state < -0) y_state = 3;

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
