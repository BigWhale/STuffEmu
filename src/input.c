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
#include <string.h>
#include <unistd.h>
#include <linux/joystick.h>

#ifndef TESTENV
#include <wiringPi.h>
#endif

#include "input.h"
#include "helpers.h"
#include "hw_defs.h"

/**
 *
 * @param arg input_arg device FD and true/false if device is a joystick
 * @return none
 */
void *input_thread(void *arg) {
    input_arg args;
    struct js_event j_ev;
    signed char m_ev[3] = {0, 0, 0};
    long timer = 0, old_timer = 0;
    int b_left, b_right;
    args = *((input_arg *)arg);
    printf("Starting input thread.\n");

    /* Input while loop doesn't require any delay or filtering. RPi needs 16ms to read the mouse device */
    while(1) {
        timer = millis();
        if (args.joystick) {
            if (read(args.device, &j_ev, sizeof(j_ev)) > 0) {
                // Process joystick
            }
        } else {
            /* Read the value and store it some place safe. */
            /* m_ev is used so that we can release the lock ASAP. */
            if (read(args.device, &m_ev, sizeof(m_ev)) > 0) {
                b_left = m_ev[0] & 0x1;
                b_right = m_ev[0] & 0x2;
                printf("RX: %ld, X: %d, Y: %d, B: %d %d\n", timer - old_timer, m_ev[1], m_ev[2], b_left, b_right);
                mouse_ev.buttons = m_ev[0];
                mouse_ev.x = m_ev[1];
                mouse_ev.y = m_ev[2];
                mouse_ev.time = timer - old_timer;
                pthread_mutex_lock(&mouse_mutex);
                read_event = true;
                dir_change = true;
                pthread_mutex_unlock(&mouse_mutex);
                old_timer = timer;
            }
        }
    }
}
