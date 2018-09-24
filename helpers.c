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

#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>

#ifndef TESTENV
#include <wiringPi.h>
#endif

void delay(useconds_t microseconds) {
#ifdef TESTENV
    usleep(microseconds);
#else
    delayMicroseconds(microseconds);
#endif
}

long long milis(void) {
    struct timeval t;
    gettimeofday(&t, NULL);
    return (((long long)t.tv_sec) * 1000) + (t.tv_usec / 1000);
}
