#!/usr/bin/env python
# -*- coding: latin-1 -*-
##############
## Script listens to serial port and writes contents into a file
##############
## requires pySerial to be installed 
import serial
import sys
import select 	
import os

timeout = 0.01

serial_port = '/dev/ttyUSB0';
baud_rate = 9600; #In arduino, Serial.begin(baud_rate)
write_to_file_path = "output.txt";

output_file = open(write_to_file_path, "w+");
ser = serial.Serial(serial_port, baud_rate)
while True:
	rlist, wlist, xlist = select.select([sys.stdin], [], [], timeout)
	if len(rlist):
		var = float(sys.stdin.readline().rstrip('\n'))
		ser.write(str(var))
		print('Referencia recibida')

	line = ser.readline();
	line = line.decode("latin-1") #ser.readline returns a binary, 	convert to string
	print(line);
	output_file.write(line.encode("utf-8"));



    

