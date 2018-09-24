#ifndef X86
#include <wiringPi.h>
#else
#define HIGH 1
#define LOW 0
#endif

#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <signal.h>
#include <linux/input.h>
#include <linux/joystick.h>
#include <pthread.h>
#include<sys/time.h>


#include "main.h"

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

int main_control = true;

/* Mouse states */
int mouseDirX = 0;
int mouseDirY = 0;
int mouseDistX = 0;
int mouseDistY = 0;

#define A 0
#define B 1

bool xState[2] = { false, false };
bool yState[2] = { false, false };

int globalDelay = 500;
int pulseDelay = 5000;
int signalDelay = 1000;

long long count = 0, oldCount = 0;

int oldL = 0,
    oldR = 0;

void sig_handler(int signum) {
    if (signum == SIGUSR1) {
        printf("Got signal USR1, bailing out!\n");
        main_control = false;
    }
}

void *mouseThread(void *vargp) {
    // Note to self: pretend you never heard of thread safety ;)
    while (main_control) {
        if (mouseDistX > 0) {
            xState[B] ^= true;
            xState[A] ^= true;
            if (mouseDirX) {
                sendCommand(XB, xState[B]);
                bwDelay(pulseDelay);
                sendCommand(XA, xState[A]);
            } else {
                sendCommand(XA, xState[B]);
                bwDelay(pulseDelay);
                sendCommand(XB, xState[A]);
            }
            mouseDistX--;
        }
        if (mouseDistY > 0) {
            yState[B] ^= true;
            yState[A] ^= true;
            if (mouseDirY) {
                sendCommand(YA, yState[A]);
                bwDelay(pulseDelay);
                sendCommand(YB, yState[B]);
            } else {
                sendCommand(YB, yState[B]);
                bwDelay(pulseDelay);
                sendCommand(YA, yState[A]);
            }
            mouseDistY--;
        }
        bwDelay(signalDelay);
    }
}

int main( int argc, char *argv[]) {
    bool joy = false;
    int mouse_dev;
    int joy_dev = 0;
    struct js_event joy_ev;
    pthread_t mouset_id;
    unsigned char mouse_ev[3] = {0, 0, 0};
    long long td = 0,
              tdOld = 0;

    long long startSample = 0;

    signal(SIGINT, sig_handler);

#ifndef X86
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

    if (argc == 2) {
        if (strncmp(argv[1], "-j", 2) == 0) {
            joy = true;
        }
    }

    if ((mouse_dev = open(MOUSE_DEV, O_RDONLY)) == -1) {
        printf("Unable to open mouse.\n");
        exit(EXIT_FAILURE);
    }

    if (joy) {
        if ((joy_dev = open(JOY_DEV, O_RDONLY)) == -1) {
            printf("Unable to open joystick.\n");
            exit(EXIT_FAILURE);
        }
    }

    pthread_create(&mouset_id, NULL, mouseThread, NULL);
    /* Perhaps turn this into async operations so we can read both at the same time? */

    td = milliS();
    startSample = td;

    printf("Starting emulation\n");
    while(main_control) {
        td = milliS();
        tdOld = td;
        if (td > (startSample + 100)) {
            printf("RATE: %lld\n", count - oldCount);
            startSample = td;
            oldCount = count;
        }
        if (joy) {
            if (read(joy_dev, &joy_ev, sizeof(joy_ev)) > 0) {
                process_joystick(&joy_ev);
            }
        } else {
            if (read(mouse_dev, &mouse_ev, sizeof(mouse_ev)) > 0) {
                process_mouse(mouse_ev);
                count++;
            }
        }
    }
    pthread_join(mouset_id, NULL);
    return 0;
}

void process_mouse(unsigned char ev[3]) {
    int button = 0,
        bLeft = 0,
        bMiddle = 0,
        bRight = 0;
    signed char x, y;

    button = ev[0];
    bLeft = button & 0x1;
    bMiddle = ( button & 0x4 ) > 0;
    bRight = ( button & 0x2 ) > 0;
    x = ev[1];
    y = ev[2];

//    printf("X: %d, y: %d, BTNS: %d - %d - %d | CMD: %lld \n", x, y, bLeft, bMiddle, bRight, count);

    if (bLeft != oldL) {
        sendCommand(BL, bLeft == 1 ? LOW : HIGH );
        oldL = bLeft;
    }

    if ( bRight != oldR) {
        sendCommand(BL, bRight == 1 ? LOW : HIGH );
        oldR = bRight;
    }

    /* Mouse has moved on the X axis, do something */
    if (count % 2 ) {
        if (x != 0) {
            mouseDirX = x < 0 ? true : false;
            mouseDistX++;
        }

        /* Upsies and downies are happening here */
        if (y != 0) {
            mouseDirY = y < 0 ? true : false;
            mouseDistY++;
        }
    }
//    printf("MX: %d | %d ", mouseDirX, mouseDistX);
//    printf("MY: %d | %d\n", mouseDirY, mouseDistY);
}

void process_joystick(struct js_event *ev) {
    printf("EV: %d - %d - %d\n", ev->type, ev->value, ev->number);
}

void bwDelay(int time) {
#ifdef X86
    usleep(time);
#else
    delayMicroseconds(time);
#endif
}

void sendCommand(int pin, int value) {
#ifdef X86
    // printf("P: %d (%d)\n", pin, value);
#else
    digitalWrite(pin, value);
#endif
}

long long milliS(void) {
    struct timeval t;
    gettimeofday(&t, NULL);
    return (((long long)t.tv_sec) * 1000) + (t.tv_usec / 1000);
}
