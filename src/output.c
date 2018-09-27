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

mouse_ev_data m_ev;

void *output_thread() {
    pthread_t t_b;

    button_arg b_arg;

    int button = 0,
        b_left = 0,
        b_right = 0,
        old_r = 0,
        old_l = 0;

    bool send_b = false;
    printf("Starting output thread.\n");
    /* Keep running the loop and read only when there's something new to read */
    while (1) {
        if (read_event) {
            /* Grab the mouse data and do stuff with it in peace */
            pthread_mutex_lock(&mouse_mutex);
            m_ev = mouse_ev;
            /* Reset axes */
            mouse_ev.x = 0;
            mouse_ev.y = 0;
            pthread_mutex_unlock(&mouse_mutex);

            button = m_ev.buttons;
            b_left = button & 0x1;
            b_right = (button & 0x2) > 0;

            /* TODO: This is most likely a bit too complicated */
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

            /* Perhaps just send direction to move_ function and figure it out there? */
            if (m_ev.x != 0) {
                if (m_ev.x < 0) {
                    move_x(LEFT, abs(m_ev.x));
                } else {
                    move_x(RIGHT, abs(m_ev.x));
                }
            }

            if (m_ev.y != 0) {
                if (m_ev.y < 0) {
                    move_y(DOWN, abs(m_ev.y));
                } else {
                    move_y(UP, abs(m_ev.y));
                }
            }

            if (send_b) {
                pthread_create(&t_b, NULL, b_thread, &b_arg);
                pthread_join(t_b, NULL);
                send_b = false;
            }

            if ( m_ev.x || m_ev.y ) {
                printf("TX: %ld, X: %d, Y: %d, B: %d %d\n", m_ev.time, m_ev.x, m_ev.y, b_left, b_right);
            }

            pthread_mutex_lock(&mouse_mutex);
            read_event = false;
            pthread_mutex_unlock(&mouse_mutex);
        }
    }
}

void *b_thread(void *arg) {
    button_arg args;
    args = *((button_arg *)arg);
    if (args.left) {
        send_command(BL, !args.l_state);
    }
    if (args.right) {
        send_command(BR, !args.r_state);
    }
    return NULL;
}
