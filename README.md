# Assembly Watchface for Pebble
Kenneth Falck <kennu@iki.fi> 2014

## Description

Shows the current [Assembly.org](http://assembly.org) event schedule on Pebble.

![Screenshot](https://raw.githubusercontent.com/kennu/asmpebble/master/screenshots/asmpebble.jpg)

## Installation

Install Pebble SDK from https://developer.getpebble.com/ and run:

    ./loadschedule.py > src/schedule.c
    pebble build
    pebble install --phone (developer mode phone ip address)
