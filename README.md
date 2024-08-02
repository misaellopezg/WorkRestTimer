# Work/Rest Timer

This project was done for buildspace nights & weekends. This is a prototype work/rest timer, where users can set
a work time and a rest time. The timer has two neopixel strips that count down time visually by fading.

## Table of Contents
1. [Introduction](#introduction)
2. [Features](#features)
3. [Getting Started](#getting-started)
4. [Installation](#installation)
5. [Usage](#usage)
6. [Schematics and Design](#schematics-and-design)
7. [License](#license)

## Introduction

## Features
- Utilizes an Atmega 328p for easy deployment with Arduino.
- Uses a compact 3.7V Lipo battery with a boost converter to 5V to power all devices.
- Battery charging system with usb-c port for charing.
- Compact circuit board for interconnecting all devices.

## Getting Started

### Prequisites
Arduino needs to be installed to run this code.
Clone this repository.
Using Arduino, open the work_rest_timer.ino file.
Upload the sketch onto your Arduino-based microcontroller.

## Usage
When launching for the first time, the work/rest timer will be pulsing white slowly. To set the work and rest time, 
press the encoder button. The light strip will turn solid white, and as you rotate the encoder, the color will change
from white to blue to indicate work time being set. Every blue led indicats +5 minutes. For example, if two blue leds
are present, that means the work time will be set to 10 minutes. After this, press the encoder button again to set the rest time. 
Same thing applies as before, except the color will be light orange. After pressing the button again, the light bar will turn 
blue or orange depending on the time counting down. As work or rest time counts down, the light bar will fade to black, switching
to the next time to be counted down to after completely fading. To exit, press the encoder button again.

## Schematics


## License


