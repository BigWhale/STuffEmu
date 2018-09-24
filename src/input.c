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
    unsigned char m_ev[3] = {0, 0, 0};

    args = *((input_arg *)arg);
    printf("Starting input thread.\n");
    printf("ARG: %d / %d\n", args.device, args.joystick);

    while(1) {
        if (args.joystick) {
            if (read(args.device, &j_ev, sizeof(j_ev)) > 0) {
                // Process joystick
            }
        } else {
            /* Read the value and store it some place safe */
            /* m_ev is used so that we can release the lock asap */
            if (read(args.device, &m_ev, sizeof(m_ev)) > 0) {
                pthread_mutex_lock(&mouse_mutex);
                memcpy(mouse_ev, m_ev, 3);
                pthread_mutex_unlock(&mouse_mutex);
                /* Reading only as fast as output can process the signals */
                delay(ATARI_MAX_RATE);
            }
        }
    }
}
