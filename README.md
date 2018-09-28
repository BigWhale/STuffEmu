# STuffEmu

This is a simple mouse emulator for Atari ST type of computers. Old
Atari mouse can get broken and can be a pain to use. There are various
hardware emulators for Atari mouse but none of them support a modern USB
mouse (some probably do, but are a bit pricey or hard to get).

This started as a fun side project after watching [Backoffice show video](https://www.youtube.com/watch?v=ED3PSM4Mwss) on
using RPi to emulate Atari mouse. It was a proof of concept and I decided
to make a POCv2. :)


## How it's done?

RPi serves as a USB HID interface as an easy access to ```/dev/input/mice```.
Input is read, converted to proper signals for Atari ST and then sent out via
GPIO to Atari ST. A seven wire cable with half a dozen resistors and a DB9
connector is all that you need for connecting these two.


## What's missing?

Atari can't cope with the speed of modern USB mice that's why a lot of data
has to be thrown away. Right now there's a hard limit on read and on input.
In a nutshell, USB input needs to be converted into variable frequency pulses
for Atari ST.

(And they aren't even pulses, just state changes.)


## How to use it?

Unzip or untar or whatever you downloaded. If you're simply goofing out on a PC
then you don't need anything else if you're on Raspberry Pi, then you need
WiringPi libraries installed. You also need GCC compiler working.

```
$ tar -xvzf stuffemu.tar.gz
$ cd STuffEmu
$ mkdir build
$ cd build
$ cmake ..
$ make
```

If everything compiled you can now run the program with ```./STuffEmu```,
rejoice! Don't worry if you did not connect your Atari to RPi nothing will
blow up. If you did, it might.

If you move your mouse around, a mouse that is connected to the computer
you're running STuffEmu on, a bunch of debug data will print in your
terminal.

When you're done you'll have to kill STuffEmu. There's no quit option.


## How to connect Atari ST with Raspberry PI?

Take DB9 female connector, solder 330 Ohm resistors on pins 1, 2, 3, 4, 6, 9.
Attach some wires to the resistors and connect them to GPIO pins 0, 5, 6, 13,
19 and 26. Then also connect DB9 pin 8 to one of the GND pins on RPi. Joystick
pinout is similar, see diagram below.

GPIO pins on RPI B+ are mapped on the GPIO header like this:

### Mouse
```
DB9 | GPIO | HDR
 1 ---- 0 --- 27 
 2 ---- 5 --- 29
 3 ---- 6 --- 31
 4 --- 13 --- 33
 6 --- 19 --- 35
 8 -- GND --- 39
 9 --- 26 --- 37 
```

### Joystick
```
DB9 | GPIO | HDR
 1 ----25 --- 22 
 2 ---- 8 --- 24
 3 ---- 7 --- 36
 4 ---  1 --- 38
 6 --- 16 --- 36
 8 -- GND --- 34 
```

I will provide a more detailed pinout with later releases.


## Why?

Why not? My idea is to actually power Raspberry Pi from the Atari's power
supply. Unfortunately 5V from Atari mouse/joy port isn't enough to power
RPi2. It might be enough to power RPi Zero.


## Final thoughts

I want an RPi in my Atari. It will emulate mouse, joystick and will provide
an USB mount point for GoTEK floppy emulator running FlashFloppy.

Yes, I know about CosmosEx for Atari, but I don't need that much. Altho
HDD emulation would be nice and I'll look into that. :)

Yes, I know that having a Raspberry Pi inside an Atari ST is an overkill and
that RPi could easily emulate whole Atari ST, but I like my Atari.

## Random trivia

STuffEmu should also work with Amiga mouse port. There are just a few minor
code modifications and it should be working. I'll add a command line switch
but I have no way to test it. If you have a spare Amiga 1200, let me know. :)


# Disclaimer

Connecting various stuff and wires to your computer, let it be a new PC,
small Raspberry Pi or an old Atari ST, can damage your computer. This thing
works and it worked for me. It might not work for you. You're at your own here.
 
 
