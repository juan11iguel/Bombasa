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
from matplotlib.pyplot import figure


#Inicialización de variables
timeout = 0.01
condicion = True
nombre = time.strftime("%Y%m%d-%H%M%S") + '_registroBombasa.csv'
puerto = input('Puerto usado por el arduino (COM4, /dev/ttyUSB0, etc): ')
tiempo = []
referencia = []
sc = []
salida = []
tiempo_inicial = time.time()

#Conexión serie
ser = serial.Serial(puerto, 9600, timeout=1)

print("[Para cerrar el programa introducir cualquier carácter no numérico]\n")
time.sleep(3)

#Parámetros de la gráfica
figure(num=None, figsize=(16, 10), dpi=90, facecolor='#C0C0C0', edgecolor='k')
style.use('fivethirtyeight')

plt.ion()

# Definición de funciones
#--------------------------------------------------------------------
def procesarValor():
	
	nuevoPunto1 = ser.readline()
	nuevoPunto2 = ser.readline()
	nuevoPunto3 = ser.readline()
	print(nuevoPunto1)
	nuevoPunto1 = nuevoPunto1.decode().replace('\r\n', "")
	nuevoPunto2 = nuevoPunto2.decode().replace('\r\n', "")
	nuevoPunto3 = nuevoPunto3.decode().replace('\r\n', "")
	

	nuevoPunto = [nuevoPunto1, nuevoPunto2, nuevoPunto3]
	for i in range(0, 3):
		if(nuevopunto[i] == ''):
			nuevoPunto[i] = 0
	return nuevoPunto

def representarValor():

	ax1 = plt.subplot(211)
	plt.plot(tiempo, referencia, linewidth=1, label='Referencia')
	plt.plot(tiempo, salida, linewidth=1, label='Salida')
	
	plt.suptitle('Control del caudal propulsado', fontsize=20, fontweight='bold')
	#plt.title('Control del caudal propulsado')
	plt.grid(True)
	plt.ylabel('Caudal (mL/seg)')
	plt.legend(loc='lower left', shadow=True)
	plt.subplots_adjust(left=0.1, bottom=0.15)
	plt.setp(ax1.get_xticklabels(), visible=False)

	ax2 = plt.subplot(212,sharex = ax1)
	plt.plot(tiempo, sc, linewidth=1, label='S.control')
	
	plt.title('Señal de control', fontsize=15)
	plt.grid(True)
	plt.ylabel('Voltaje (V)')
	plt.xticks(rotation=45, ha='right')
#---------------------------------------------------------------------

#Abre el archivo en modo escritura
dataFile = open(nombre, 'w')


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
			
	data = procesarValor()
	#Tratar para escritura en archivo
	dataFile.write(str(datetime.now().time()))
	dataFile.write(', ')
	dataFile.write(data[0])
	dataFile.write(', ')
	dataFile.write(data[1])
	dataFile.write(', ')
	dataFile.write(data[2])
	dataFile.write('\n')
	
	data = list(map(float, data))
	#Mostrar en terminal
	print('\n\n\nReferencia [mL/min]: ')
	print(data[0])
	print('\nSeñal de control [V]: ')
	print(data[1])
	print('\nSalida [mL/min]: ')
	print(data[2])
	
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
	

dataFile.close()



