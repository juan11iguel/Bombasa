#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import serial
import time
import select
import sys
from datetime import datetime
import numpy as np
import urllib
import time
from drawnow import *
from matplotlib.pyplot import figure
import threading


#Conexión serie
ser = serial.Serial('/dev/ttyUSB0', 9600, timeout=1)

print("Para cerrar el programa introducir cualquier carácter no numérico\n")
time.sleep(3)


def generarReferencia():
	referencia = input('Referencia: ')
	#print('Referencia enviada: {}'.format(referencia))
	referencia = float(referencia)
	referencia = bytes(str(referencia), encoding='utf-8')
	print(referencia)
	ser.write(referencia)


while True:
	generarReferencia();
