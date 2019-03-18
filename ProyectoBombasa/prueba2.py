#!/usr/bin/env python
import time
import os

MIN = 0;
MAX = 10;


while 1: # Estara en este bucle siempre leyendo valores
	print 'Introduzca un valor comprendido entre ',MIN, 'y ', 		MAX,': '


	var = input() 
	print "Ha introducido", var
	
	if	var >= MIN and var <= MAX:
		print 'Vale, vale...'
		time.sleep(1)
		raw_input('Press <ENTER> to continue')
	else:
		print("El valor introducido no es correcto")
		time.sleep(1)
		raw_input('Press <ENTER> to continue')
		

