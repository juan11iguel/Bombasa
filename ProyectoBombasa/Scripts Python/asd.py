#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import serial
import time
import select
import sys
import matplotlib.pyplot as plt
from matplotlib import style
from datetime import datetime
import numpy as np
import urllib
import time
from drawnow import *

timeout = 0.01
condicion = True
tiempo = []
referencia = []
sc = []
salida = []
tiempo_inicial = time.time()


ser = serial.Serial('/dev/ttyUSB0', 9600, timeout=1)
time.sleep(3)
style.use('fivethirtyeight')


plt.ion()

def procesarValor():
	ser.write(b'g')
	nuevoPunto1 = ser.readline()
	nuevoPunto2 = ser.readline()
	nuevoPunto3 = ser.readline()

	nuevoPunto1 = nuevoPunto1.decode()
	nuevoPunto2 = nuevoPunto2.decode()
	nuevoPunto3 = nuevoPunto3.decode()

	nuevoPunto = [nuevoPunto1, nuevoPunto2, nuevoPunto3]
	
	return nuevoPunto

def representarValor():

	ax1 = plt.subplot(211)
	plt.plot(tiempo, referencia, linewidth=1, label='Referencia')
	plt.plot(tiempo, salida, linewidth=1, label='Salida')

	plt.title('Control del caudal propulsado')
	plt.grid(True)
	plt.ylabel('Caudal (mL/seg)')
	plt.legend(loc='lower left', shadow=True)
	plt.subplots_adjust(left=0.2, bottom=0.2)
	plt.setp(ax1.get_xticklabels(), visible=False)

	ax2 = plt.subplot(212,sharex = ax1)
	plt.plot(tiempo, sc, linewidth=1, label='S.control')

	plt.grid(True)
	plt.ylabel('Voltaje (V)')
	plt.xlabel('Tiempo (seg)')
	plt.xticks(rotation=45, ha='right')
	

while condicion == True:
	rlist, wlist, xlist = select.select([sys.stdin], [], [], timeout)
	if len(rlist):
		try:
			var = float(sys.stdin.readline().rstrip('\n'))
			ser.write(str(var))
			print('Referencia recibida')
			print(var)
		except ValueError:
			condicion = False
		
	data=procesarValor()	
	data = list(map(float, data))
	#Mostrar en terminal
	print(data)

	#Mostrar gráfica
	tiempo_transc = time.time() - tiempo_inicial
	tiempo_transc = time.strftime("%M:%S", time.gmtime(tiempo_transc))


	tiempo.append(tiempo_transc)
	referencia.append(float(data[0]))
	sc.append(float(data[1]))
	salida.append(float(data[2]))

	# Hace que el eje x se desplace continuamente
	tiempo = tiempo[-20:]
	referencia = referencia[-20:]
	sc = sc[-20:]
	salida = salida[-20:]


	drawnow(representarValor)
	

