#!/usr/bin/env python
# Prueba con el de la web italiana

import serial
import time

ArduinoSerial = serial.Serial('/dev/ttyACM1', 9600)
file = open('valores.csv')

while 1:		#no-break space, espacio que no se rompe
&nbsp; &nbsp;line = file.readline()
&nbsp; &nbsp;if not line:
&nbsp; &nbsp; &nbsp; &nbsp; &nbsp;break
&nbsp; &nbsp;ArduinoSerial.write(line)
&nbsp; &nbsp;time.sleep(3)


file.close
