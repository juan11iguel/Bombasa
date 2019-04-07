#!/usr/bin/env python
# -*- coding: utf-8 -*-


import serial
import time
import select
import sys
import matplotlib.pyplot as plt
import matplotlib.animation as animation
from matplotlib import style
from datetime import datetime
import numpy as np
import urllib
import matplotlib.dates as mdates
import matplotlib.ticker as miarma


ser = serial.Serial('/dev/ttyUSB0', 9600, timeout = 1)


style.use('fivethirtyeight')

fig = plt.figure()



ax1 = fig.add_subplot(2,1,1)
#Customización gráfica 1
plt.title('Control del caudal propulsado')
plt.ylabel('Caudal (mL/seg)')
legend = plt.legend(loc='best', shadow=True, fontsize='x-large')

ax2 = fig.add_subplot(2,1,2, sharex = ax1)
#Customización gráfica 2
plt.ylabel('S. de control (V)')
plt.xlabel('Tiempo (seg)')
plt.margins(0.05)
plt.subplots_adjust(bottom=0.3)


def animate(i):
	graph_data = open('output.csv', 'r').read()
	lines = graph_data.split('\n')

	tiempo = []
	referencia = []
	sc = []
	salida = []
	
	for line in lines:
		if len(line)>1:
			t,x,y,z = line.split(',')
			referencia.append(float(x))
			sc.append(float(y))
			salida.append(float(z))
			tiempo.append(t)
	
	ax1.clear
	ax2.clear


	ax1.plot(tiempo, referencia, linewidth=1, label='Referencia')
	ax1.plot(tiempo, salida, linewidth=1, label='Salida')
	ax2.plot(tiempo, sc, linewidth=1)
	
	#Configuración de la gráfica
	#legend = ax1.legend(loc='best', shadow=True, fontsize='x-large')
	plt.setp(ax1.get_xticklabels(), visible=False)

	#ax2.xaxis.set_major_formatter(m_dates.DateFormatter('%S'))
	#ax2.xaxis.set_major_locator(miarma.AutoLocator(nbins=5))
	ax2.grid(True)

	xmin, xmax = ax2.get_xlim()
	ax2.set_xticks(np.round(np.linspace(xmin, xmax, 10), 2)
	for label in ax2.xaxis.get_ticklabels():
		label.set_rotation(45)


ani = animation.FuncAnimation(fig, animate, interval = 5000)

plt.show()
plt.draw()
plt.pause(0.001)
