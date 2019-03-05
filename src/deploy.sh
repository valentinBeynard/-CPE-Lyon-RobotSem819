#!/bin/bash

sudo chmod a+rw /dev/ttyACM0;
cd ../build;
avrdude -F -V -c arduino -p ATMEGA328P -P /dev/ttyACM0 -b 115200 -U flash:w:robotSem819.hex
