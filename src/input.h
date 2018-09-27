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

#ifndef STUFFEMU_INPUT_H
#define STUFFEMU_INPUT_H

#include <stdbool.h>
#include <pthread.h>

typedef struct s_input_arg {
    int device;
    int joystick;
} input_arg;


typedef struct s_mouse_ev_data {
    int buttons;
    signed char x;
    signed char y;
    long time;
} mouse_ev_data;

mouse_ev_data mouse_ev;

pthread_mutex_t mouse_mutex;

bool read_event;

void *input_thread(void *arg);

#endif //STUFFEMU_INPUT_H
