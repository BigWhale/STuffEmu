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
#ifndef TESTENV
#include <wiringPi.h>
#endif

#include "hw_defs.h"

void start_gpio() {
#ifndef TESTENV
    wiringPiSetupGpio() ;
    sendCommand(BL, HIGH);
    sendCommand(BR, HIGH);
    sendCommand(XA, LOW) ;
    sendCommand(XB, LOW) ;
    sendCommand(YA, LOW) ;
    sendCommand(YB, LOW) ;
    pinMode (XA, OUTPUT) ;
    pinMode (XB, OUTPUT) ;
    pinMode (YA, OUTPUT) ;
    pinMode (YB, OUTPUT) ;
    pinMode (BL, OUTPUT) ;
    pinMode (BR, OUTPUT) ;
#endif
}

void sendCommand(int pin, int value) {
#ifdef TESTENV
    printf("P: %d (%d)\n", pin, value);
#else
    digitalWrite(pin, value);
#endif
}
