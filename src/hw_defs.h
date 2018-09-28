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

/* GPIO mouse pins */
#define XB  0
#define XA  5
#define YB  6
#define YA 13
#define BL 19
#define BR 26

/* Mouse directions */
#define UP    1
#define DOWN  2
#define LEFT  3
#define RIGHT 4


/* Joystick events */
#define JOY_BUTTON 1
#define JOY_AXIS 2

/* Joystick axes; these are safe assumptions */
#define JOY_X 0
#define JOY_Y 1

/* Joystick deadzones in case of analog joystick */
#define JOY_DEAD_MIN -1500
#define JOY_DEAD_MAX 1500

/* GPIO joystick pins */
#define JU 25
#define JD  8
#define JL  7
#define JR  1
#define JB 16

/* GPIO stuff */
void start_gpio();
void send_command(int pin, int value);

#endif //STUFFEMU_HW_DEFS_H
