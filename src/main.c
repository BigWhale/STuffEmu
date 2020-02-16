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

#define _GNU_SOURCE

#include <argp.h>
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <stdbool.h>
#include <sys/time.h>

#include "main.h"
#include "input.h"
#include "output.h"
#include "hw_defs.h"

static char *joy_dev = "/dev/input/js0";  /* default joystick device */
static char *mouse_dev = "/dev/input/mice";  /* default mouse device */
const char *argp_program_version = "STuffEmu 0.2";
const char *argp_program_bug_address = "<bigwhale@lubica.net>";
static char doc[] = "Atari ST mouse emulator for RaspberryPi.";
static char args_doc[] = "argle";
static struct argp_option options[] = {
        { "daemon", 'd', 0, 0, "Run in background."},
        { "amiga", 'a', 0, 0, "Switch mouse to Amiga mode." },
        { "mouse", 'm', "MOUSE_DEV", 0, "Specify mouse device (default: /dev/input/mice)." },
        { "joystick", 'j', "JOY_DEV", OPTION_ARG_OPTIONAL, "Enable joystick emulation and/or specify device (default: /dev/input/js0)." },
        { 0 }
};

struct arguments {
    char *args[4];
    int daemon;
    int amiga;
    char *mouse_dev;
    char *joystick_dev;
};


static error_t parse_opt(int key, char *arg, struct argp_state *state) {
    struct arguments *arguments = state->input;
    switch (key) {
        case 'd':
            arguments->daemon = true;
            break;
        case 'a':
            arguments->amiga = true;
            break;
        case 'm':
            arguments->mouse_dev = arg;
            break;
        case 'j':
            if (arg == NULL) {
                arguments->joystick_dev = joy_dev;
            } else {
                arguments->joystick_dev = arg;
            }
            break;
        default:
            return ARGP_ERR_UNKNOWN;
    }
    return 0;
}


static struct argp argp = { options, parse_opt, args_doc, doc };

pthread_mutex_t mouse_mutex = PTHREAD_MUTEX_INITIALIZER;

void sig_handler(int signum) {
    if (signum == SIGINT) {
        printf("\nGot signal INT, bailing out!\n");
        exit(0);
    }
}

int main(int argc, char **argv) {
    pid_t pid;

    struct arguments arguments;

    int mouse_dev;
    int joystick_dev = 0;

    input_arg mouse_arg;
    input_arg joystick_arg;
    pthread_t t_mouse_input;
    pthread_t t_joystick_input;
    pthread_t t_xout;
    pthread_t t_yout;

    arguments.daemon = false;
    arguments.amiga = false;
    arguments.joystick_dev = NULL;
    arguments.mouse_dev = mouse_dev;

    argp_parse(&argp, argc, argv, 0, 0, &arguments);


    if (arguments.daemon) {
        printf("Running in background.\n");
        printf("Mouse device: %s\n", arguments.mouse_dev);

        if (arguments.joystick_dev) {
            printf("Joystick device: %s\n", arguments.joystick_dev);
        }

        pid = fork();

        /* Something was foobar */
        if (pid < 0) {
            exit(EXIT_FAILURE);
        }

        /* Kill the parent */
        if (pid > 0) {
            exit(EXIT_SUCCESS);
        }

        if (setsid() < 0) {
            exit(EXIT_FAILURE);
        }

        signal(SIGINT, sig_handler);

        pid = fork();

        /* Something was foobar */
        if (pid < 0) {
            exit(EXIT_FAILURE);
        }

        /* Kill the parent */
        if (pid > 0) {
            exit(EXIT_SUCCESS);
        }
    }

    if (arguments.amiga == true) {
        if (!arguments.daemon) {
            printf("Amiga mode active.\n");
        }
        set_pins(1);
    } else {
        if (!arguments.daemon) {
            printf("Atari ST mode active.\n");
        }
        set_pins(0);
    }

    if (arguments.joystick_dev != NULL) {
        if ((joystick_dev = open(arguments.joystick_dev, O_RDONLY | O_NONBLOCK)) == -1) {
            if (!arguments.daemon) {
                printf("Unable to open joystick device.\n");
            }
            exit(EXIT_FAILURE);
        }
    }

    if ((mouse_dev = open(arguments.mouse_dev, O_RDONLY | O_NONBLOCK)) == -1) {
        if (!arguments.daemon) {
            printf("Unable to open mouse device: %s\n", arguments.mouse_dev);
        }
        exit(EXIT_FAILURE);
    }

    mouse_arg.device = mouse_dev;
    joystick_arg.device = joystick_dev;

    /* All is well, prepare GPIO and fire up input & output threads. */
    start_gpio();
    if (!arguments.daemon) {
        printf("Starting emulation.\n");
    }
    pthread_create(&t_mouse_input, NULL, mouse_input_thread, &mouse_arg);
    pthread_setname_np(t_mouse_input, "STuffEmu: mouse input");

    if (joystick_dev) {
        pthread_create(&t_joystick_input, NULL, joystick_input_thread, &joystick_arg);
        pthread_setname_np(t_joystick_input, "STuffEmu: joystick input");
    }

    pthread_create(&t_xout, NULL, x_thread, NULL);
    pthread_setname_np(t_xout, "STuffEmu: mouse x motion");
    pthread_create(&t_yout, NULL, y_thread, NULL);
    pthread_setname_np(t_yout, "STuffEmu: mouse y motion");

    if (!arguments.daemon) {
        printf("Waiting for threads (forever).\n");
    }

    /* Wait for threads to stop, once we decide on how to quit. */
    pthread_join(t_mouse_input, NULL);
    if (joystick_dev) {
        pthread_join(t_joystick_input, NULL);
    }
    pthread_join(t_xout, NULL);
    pthread_join(t_yout, NULL);

    return 0;
}
