#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import serial
import time
import sys
import matplotlib.pyplot as plt
from matplotlib import style
from datetime import datetime
import numpy as np
from drawnow import *
from matplotlib.pyplot import figure
import threading
import signal
import os


#Inicialización de variables
timeout = 0.01
condicion = True
nombre = time.strftime("%Y%m%d-%H%M%S") + '_registroBombasa.csv'
#puerto = input('Puerto usado por el arduino (COM4, /dev/ttyUSB0, etc): ')
tiempo = []
referencia = []
sc = []
salida = []

tiempo_inicial = time.time()
tiempo_inicial2 = tiempo_inicial

#Conexión serie
ser = serial.Serial('/dev/ttyUSB0', 9600, timeout=1)

print("La referencia se puede introducir en cualquier momento por teclado. Para cerrar el programa introducir cualquier carácter no numérico\n")
time.sleep(3)

#Parámetros de la gráfica
figure(num=None, figsize=(16, 10), dpi=90, facecolor='#C0C0C0', edgecolor='k')
style.use('fivethirtyeight')

plt.ion()

# Definición de funciones
#--------------------------------------------------------------------
def procesarValor():
	ser.write(b'g')
	nuevoPunto1 = ser.readline()
	nuevoPunto2 = ser.readline()
	nuevoPunto3 = ser.readline()

	nuevoPunto1 = nuevoPunto1.decode().replace('\r\n', "")
	nuevoPunto2 = nuevoPunto2.decode().replace('\r\n', "")
	nuevoPunto3 = nuevoPunto3.decode().replace('\r\n', "")

	nuevoPunto = [nuevoPunto1, nuevoPunto2, nuevoPunto3]
	
	for i in range(0,3):
		if nuevoPunto[i] == "":
			nuevoPunto[i] = '0'		
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

def leerReferencia():
	while True:
		try:
			referencia = input()
			numeroReal = referencia
			print('Referencia enviada: {}'.format(referencia))
			referencia = bytes('h ' + str(referencia) + '\n', encoding='utf-8')
			ser.write(referencia)
			print(referencia)
			numeroReal=float(numeroReal)
		except ValueError:
			os.kill(os.getpid(), signal.SIGUSR1)

#Para cerrar programa
class ExitCommand(Exception):
	pass
	
#visto en: https://stackoverflow.com/questions/1489669/how-to-exit-the-entire-application-from-a-python-thread

#Para cerrar programa	
def signal_handler(signal, frame):
	raise ExitCommand()


#---------------------------------------------------------------------
signal.signal(signal.SIGUSR1, signal_handler) #Para cerrar programa
t1 = threading.Thread(target = leerReferencia)
t1.setDaemon(True)
t1.start()





#Abre el archivo en modo escritura
dataFile = open(nombre, 'w')

if __name__ == "__main__":
	try:
		while True:
			if(time.time() - tiempo_inicial2 > 0.1):
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

				#Mostrar en terminal
				print('\n\n\nReferencia [mL/min]: ')
				print(data[0])
				print('\nSeñal de control [V]: ')
				print(data[1])
				print('\nSalida [mL/min]: ')
				print(data[2])
				tiempo_inicial2 = time.time()
		pass
	except ExitCommand:
		pass
	finally:
		ser.close()
		dataFile.close()
		print('Programa finalizado, registro de variables en ' + nombre)
		





