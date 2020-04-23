# STuffEmu

This is a simple mouse emulator for Atari ST type of computers. Old
Atari mouse can get broken and can be a pain to use. There are various
hardware emulators for Atari mouse but none of them support a modern USB
mouse (some probably do, but are a bit pricey or hard to get).

This started as a fun side project after watching [Backoffice show video](https://www.youtube.com/watch?v=ED3PSM4Mwss) on
using RPi to emulate Atari mouse. It was a proof of concept and I decided
to make a POCv2. :)

## How it's done?

RPi serves as a USB HID interface as an easy access to `/dev/input/mice`.
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
WiringPi libraries installed. You also need GCC compiler and cmake.

### Prerequisites

```shell
$ sudo apt install wiringpi
$ sudo apt install cmake
```

### Build the STuffEmu

```shell
$ tar -xvzf stuffemu.tar.gz
$ cd STuffEmu
$ mkdir build
$ cd build
$ cmake ..
$ make
```

If everything compiled you can now run the program with `./stuffemu`,
rejoice! Don't worry if you did not connect your Atari to RPi nothing will
blow up. If you did, it might.

If you move your mouse around, a mouse that is connected to the computer
you're running STuffEmu on, a bunch of debug data will print in your
terminal.

When you're done you can press CTRL-C to end stuffemu.

If you want a system wide installation and you want to start STuffEmu on startup
you can run these commands:

```
$ sudo make install
$ sudo systemctl enable stuffemu.path
$ sudo systemctl start stuffemu.path
```

This will install stuffemu in `/opt/stuffemu` and all the systemd scripts in
`/lib/systemd/system` then it will enable stuffemu at boot and start it.
Your mouse should be working now and after a reboot.

## Permissions

If you're running stuffemu as user pi or root (and you probably should not run
it as root) then everything should work. If you're running it as nother user
then you'll have to take care of the necessary permissions for devices.

The easiest was for this is to edit the `/etc/group` file and add your username
to the input and gpio group.

## How to connect Atari ST with Raspberry PI?

Take DB9 female connector, solder 330 Ohm resistors on pins 1, 2, 3, 4, 6, 9.
Attach some wires to the resistors and connect them to GPIO pins 0, 5, 6, 13,
19 and 26. Then also connect DB9 pin 8 to one of the GND pins on RPi. Joystick
pinout is similar, see diagram below.

GPIO pins on RPI B+ are mapped on the GPIO header like this:

### Mouse

| DB9 | GPIO | HDR |
|-----|------|-----|
| 1   | 0    | 27  |
| 2   | 5    | 29  |
| 3   | 6    | 31  |
| 4   | 13   | 33  |
| 6   | 19   | 35  |
| 8   | GND  | 39  |
| 9   | 26   | 37  |

### Joystick

| DB9 | GPIO | HDR |
|-----|------|-----|
| 1   | 25   | 22  |
| 2   | 8    | 24  |
| 3   | 7    | 26  |
| 4   | 1    | 28  |
| 6   | 16   | 36  |
| 8   | GND  | 34  |

Wiring diagrams are in the doc directory. Stock DB9 connector in Fritzing was
a little bit strange, so there's an alternative version and you'll probably need
to import it if you want to open the .fzz file.

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

## Amiga support

STuffEmu is now also working on Amiga. It was tested on Amiga 600. You will have to
use `-a` switch when running the program to turn on the Amiga mode. Amiga and Atari
modes are incompatible so you can't have one stuffemu running for both computers.

### Amiga hardware issues

The Amiga's right and middle mouse buttons intersect with the Y and X axis of analogue
joystick inputs respectively owing to the ports being compatible with multiple types of
input device, and are potentiometer inputs. In spite of the mouse-mode inputs being read
digitally, if the voltage level is too low the Amiga-side CIA won't register the
required "high" to trigger the input and it will be ignored.

As a result, if there is a 330 ohm resistor on pin 9, the right mouse button will not
work. You might try with a smaller value resistor. I just removed it and it seems that
everything is fine. Resistors on all the other pins aren't causing any problems.

The Amiga DB9 pin 7 is a 5V pin with a maximum load of 50mA which is not sufficient to
power any form of Raspberry Pi.

# Disclaimer

Connecting various stuff and wires to your computer, let it be a new PC,
small Raspberry Pi or an old Atari ST, can damage your computer. This thing
works and it worked for me. It might not work for you. You're at your own here.
