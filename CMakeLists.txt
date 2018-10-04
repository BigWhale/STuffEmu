#
# This file is part of the STuffEmu (https://github.com/BigWhale/STuffEmu)
# Copyright (c) 2018 David Klasinc
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, version 2.
#
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
# General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program. If not, see <http://www.gnu.org/licenses/>.
#

cmake_minimum_required(VERSION 3.7)
project(stuffemu C)

set(CMAKE_C_STANDARD 99)

set (SYSTEMD_DIR "/lib/systemd/system/")

#
# If compiling on a PC assume it's just a test and disable RPi stuff
#
if (CMAKE_SYSTEM_PROCESSOR MATCHES "(x86)|(X86)|(x86_64)|(X86_64)")
    add_definitions(-DTESTENV)
endif ()

add_executable(stuffemu src/main.c src/main.h src/input.c src/helpers.c src/helpers.h src/hw_defs.h src/hw_defs.c src/input.h src/output.c src/output.h)
target_link_libraries(stuffemu pthread)

#
# TODO.md: Check if this works on RPi v1 and v3
#
if (CMAKE_SYSTEM_PROCESSOR MATCHES "armv7l")
    target_link_libraries(stuffemu wiringPi)
endif ()

set(CMAKE_INSTALL_PREFIX "/opt/stuffemu/")
install(TARGETS stuffemu DESTINATION bin)
install(FILES systemd/stuffemu.path DESTINATION ${SYSTEMD_DIR})
install(FILES systemd/stuffemu.service DESTINATION ${SYSTEMD_DIR})
