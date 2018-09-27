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
#include <stdbool.h>
#include <stdlib.h>

#ifndef TESTENV
#include <wiringPi.h>
#endif

#include "helpers.h"
#include "hw_defs.h"

void start_gpio() {
#ifndef TESTENV
    wiringPiSetupGpio();
    send_command(BL, HIGH);
    send_command(BR, HIGH);
    send_command(XA, LOW);
    send_command(XB, LOW);
    send_command(YA, LOW);
    send_command(YB, HIGH);
    pinMode(XA, OUTPUT);
    pinMode(XB, OUTPUT);
    pinMode(YA, OUTPUT);
    pinMode(YB, OUTPUT);
    pinMode(BL, OUTPUT);
    pinMode(BR, OUTPUT);
#endif
}

void send_command(int pin, int value) {
#ifdef TESTENV
    printf("P: %d (%d)\n", pin, value);
#else
    digitalWrite(pin, value);
#endif
}

int x_state = 0;
int y_state = 0;
/* TODO: Threads, both axes need to send commands simultaneously */
void move_x(int dir, int dist) {
    int runs, delay;
    dir_change = false;
    for (runs = 0; runs < dist; runs++) {
        if (dir == LEFT) x_state--; else x_state++;

        if ((dir == RIGHT) && x_state > 3) x_state = 0;
        if ((dir == LEFT) && x_state < -0) x_state = 3;

        if (x_state == 0) send_command(XA, HIGH);
        if (x_state == 1) send_command(XB, HIGH);
        if (x_state == 2) send_command(XA, LOW);
        if (x_state == 3) send_command(XB, LOW);
        if (dist > 1) {
            delay = (MAX_PULSE_DELAY / dist > MIN_PULSE_DELAY) ? MAX_PULSE_DELAY / dist : MIN_PULSE_DELAY;
            delayMicroseconds(delay);
        }
    }
}

void move_y(int dir, int dist) {
    int runs, delay;
    dir_change = false;
    for (runs = 0; runs < dist; runs++) {
        if (dir == UP) y_state--; else y_state++;

        if ((dir == DOWN) && y_state > 3) y_state = 0;
        if ((dir == UP) && y_state < -0) y_state = 3;

        if (y_state == 0) send_command(YA, HIGH);
        if (y_state == 1) send_command(YB, HIGH);
        if (y_state == 2) send_command(YA, LOW);
        if (y_state == 3) send_command(YB, LOW);
        if (dist > 1) {
            delay = (MAX_PULSE_DELAY / dist > MIN_PULSE_DELAY) ? MAX_PULSE_DELAY / dist : MIN_PULSE_DELAY;
            delayMicroseconds(delay);
        }
        if (dir_change) {
            break;
        }
    }
}
