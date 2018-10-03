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
void *mouse_input_thread(void *arg) {
    input_arg args;
    signed char m_ev[3] = {0, 0, 0};
    int l_butt, r_butt, l_old = 0, r_old = 0;
    args = *((input_arg *)arg);
    /* Input while loop doesn't require any delay or filtering. RPi needs 16ms to read the mouse device */
    while(1) {
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

void *joystick_input_thread(void *arg) {
    input_arg args;
    struct js_event j_ev;
    int j_butt = 0, j_old = 0;
    args = *((input_arg *)arg);
    while(1) {
       if (read(args.device, &j_ev, sizeof(j_ev)) > 0) {
            switch (j_ev.type) {
                case JOY_BUTTON: /* One of the buttons was pressed, react to ALL the buttons */
                    j_butt = j_ev.value;
                    if (j_butt != j_old) {
                        /* Mouse right button and joystick button are shared */
                        send_command(JB, (j_butt == true) ? LOW : HIGH);
                        send_command(BR, (j_butt == true) ? LOW : HIGH);
                        j_old = j_butt;
                    }
                    break;
                case JOY_AXIS: /* Joystick movement detected */
                    switch (j_ev.number) {
                        case JOY_X:
                            if (JOY_DEAD_MIN < j_ev.value && j_ev.value < JOY_DEAD_MAX ) {
                                send_command(JL, LOW);
                                send_command(JR, LOW);
                            }
                            if (j_ev.value > JOY_DEAD_MAX) {
                                send_command(JL, HIGH);
                                send_command(JR, LOW);
                            }
                            if (j_ev.value < JOY_DEAD_MIN) {
                                send_command(JL, LOW);
                                send_command(JR, HIGH);
                            }
                            break;
                        case JOY_Y:
                            if (JOY_DEAD_MIN < j_ev.value && j_ev.value < JOY_DEAD_MAX ) {
                                send_command(JU, LOW);
                                send_command(JD, LOW);
                            }
                            if (j_ev.value > JOY_DEAD_MAX) {
                                send_command(JD, LOW);
                                send_command(JU, HIGH);
                            }
                            if (j_ev.value < JOY_DEAD_MIN) {
                                send_command(JD, HIGH);
                                send_command(JU, LOW);
                            }
                            break;
                        default:  /* Ignore all other bazillion axes */
                            break;
                    }
                    break;
                default:
                    break;
            }
        }
    }
}
