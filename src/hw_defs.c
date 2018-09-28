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

    send_command(JU, HIGH);
    send_command(JD, HIGH);
    send_command(JL, HIGH);
    send_command(JR, HIGH);
    send_command(JB, HIGH);

    pinMode(JU, OUTPUT);
    pinMode(JD, OUTPUT);
    pinMode(JL, OUTPUT);
    pinMode(JR, OUTPUT);
    pinMode(JB, OUTPUT);

#endif
}

void send_command(int pin, int value) {
#ifdef TESTENV
    printf("P: %d (%d)\n", pin, value);
#else
    digitalWrite(pin, value);
#endif
}
