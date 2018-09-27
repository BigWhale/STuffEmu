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

#ifndef STUFFEMU_HW_DEFS_H
#define STUFFEMU_HW_DEFS_H

#ifdef TESTENV
#define HIGH 1
#define LOW 0
#endif

/* Max/Min rate at which Atari can/should receive signals */
#define MAX_PULSE_DELAY 37500
#define MIN_PULSE_DELAY 375

/* Make these configurable */
#define MOUSE_DEV "/dev/input/mice"
#define JOY_DEV "/dev/input/js0"

/* GPIO pins */
#define XB  0
#define XA  5
#define YB  6
#define YA 13
#define BL 19
#define BR 26

/* Directions */
#define UP    1
#define DOWN  2
#define LEFT  3
#define RIGHT 4

/* GPIO stuff down */

void start_gpio();
void send_command(int pin, int value);

#endif //STUFFEMU_HW_DEFS_H
