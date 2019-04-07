#!/usr/bin/env python
# -*- coding: latin-1 -*-

import serial
import time # Para poder usar la funcion de delay
import os


ArduinoSerial = serial.Serial('/dev/serial/by-id/usb-1a86_USB2.0-Serial-if00-port0', 9600) # cambiar dev/ en funcion del puerto donde se conecte
#Este nombre lo he encontrado con el comando "ls /dev/serial/by-id/"

MAX = 10;
MIN = 0;

time.sleep(2) # 2 seg de espera para dar tiempo a la comunicacion serie a establecerse

#Prueba para mandar valor a Arduino--------------------------------
ArduinoSerial.write(b'5') 
ArduinoSerial.write('5') # Manda valor al arduino que lo recibe mientras este en Serial.read()




print (ArduinoSerial.readline()) #Imprime en pantalla cualquier cosa que se introduzca en el Arduino


while 1: # Estara en este bucle siempre leyendo valores
	print ('Introduzca un valor comprendido entre ',MIN, 'y ', 		MAX,': ')
	
	var = input() 
	ArduinoSerial.write(str(var))

		

