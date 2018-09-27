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
#include <stdlib.h>
#include <unistd.h>
#include <linux/joystick.h>

#ifndef TESTENV
#include <wiringPi.h>
#endif

#include "input.h"
#include "output.h"
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
    int l_butt, r_butt, l_old = 0, r_old = 0;
    args = *((input_arg *)arg);
    printf("Starting input thread.\n");

    /* Input while loop doesn't require any delay or filtering. RPi needs 16ms to read the mouse device */
    while(1) {
        if (args.joystick) {
            if (read(args.device, &j_ev, sizeof(j_ev)) > 0) {
                // Process joystick
            }
        } else {
            /* Read the value and store it some place safe. */
            if (read(args.device, &m_ev, sizeof(m_ev)) > 0) {
                l_butt = (m_ev[0] & 0x1) > 0;
                r_butt = (m_ev[0] & 0x2) > 0;
                pthread_mutex_lock(&mouse_mutex);
                if (m_ev[1] != 0) {
                    x_dir = (m_ev[1] < 0) ? LEFT : RIGHT;
                    x_dist = abs(m_ev[1]);
                }

                if (m_ev[2] != 0) {
                    y_dir = (m_ev[2] < 0) ? DOWN : UP;
                    y_dist = abs(m_ev[2]);
                }

                if (l_butt != l_old) {
                    send_command(BL, (l_butt == true) ? LOW : HIGH);
                    l_old = l_butt;
                }

                if (r_butt != r_old) {
                    send_command(BR, (r_butt == true) ? LOW : HIGH);
                    r_old = r_butt;
                }

                pthread_mutex_unlock(&mouse_mutex);
            }
        }
    }
}
