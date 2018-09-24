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

#ifndef STUFFEMU_OUTPUT_H
#define STUFFEMU_OUTPUT_H

typedef struct s_output_arg {
    int dir;
    int distance;
} output_arg;

typedef struct s_button_arg {
    int left;
    int l_state;
    int right;
    int r_state;
} button_arg;

void *output_thread();
void *axis_thread(void *arg);
void *b_thread(void *arg);

#endif //STUFFEMU_OUTPUT_H
