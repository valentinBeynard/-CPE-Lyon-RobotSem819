#!/bin/bash

#avrdude -c arduino -p ATMEGA328P -P /dev/ttyACM0 -U lfuse:r:lfuse:h -U hfuse:r:hfuse:h -U efuse:r:efuse:h
avrdude -P /dev/ttyACM0 -b 19200 -c stk500v1 -p m328p -v
