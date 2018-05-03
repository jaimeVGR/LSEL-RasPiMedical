#!/usr/bin/env python
# coding=utf-8
import sys
import os
import json
import urllib2
import numpy as np
import matplotlib.pyplot as plt

code = 480292
API_Write = "1HUIIJ1XJ61ZO06G"
API_Read = "KQWUKI7T8BPL3G6R"

bpm = []
temp = []
s02 = []
entry_id = []

def getDataFromSensor(): 
#	os.system('clear')
	print "introduce pulso:"
	p = input()
	print "introduce temperatura:"
	t = input()
	print "introduce oxigeno: "
	o = input()
	return (p,t,o) #Pulso, Temperatura, Oxígeno


def UpDataToThingspeak(): 
	pulso, temperatura, oxigeno = getDataFromSensor()
	URL = "https://api.thingspeak.com/update?api_key=%s&field1=%s&field2=%s&field3=%s" % (API_Write, pulso, temperatura, oxigeno)
	try: 
		f = urllib2.urlopen(URL)
	except urllib2.HTTPError,e: 
		print "ERROR, ha cambiado la API de escritura"
		f.close()
		return
	except urllib2.URLError,e: 
		print "ERROR, ha cambiado la API de escritura"
		f.close()
		return
	print f.read()
	f.close()


def GetDataFromThingspeak(): 
	print "Getting Data..."


	URLstatus = "https://api.thingspeak.com/channels/%s/status.json?api_key=%s" % (code, API_Read)
	status = json.loads(urllib2.urlopen(URLstatus).read())
	last_entry_id = len(status['feeds'])
	
	print status
	print last_entry_id

	URL = "https://api.thingspeak.com/channels/%s/feeds.json?api_key=%s&results=%s" % (code, API_Read, last_entry_id)
	try:
		f = urllib2.urlopen(URL)
	except urllib2.HTTPError,e: 
		print "ERROR, ha cambiado la API de escritura"
		f.close()
		return
	except urllib2.URLError,e: 
		print "ERROR, ha cambiado la API de escritura"
		f.close()
		return
	response = f.read()
	print response
	data = json.loads(response)

	print response

	for i in range (0, len(data['feeds'])):
		bpm.append(data['feeds'][i]['field1'])
		temp.append(data['feeds'][i]['field2'])
		s02.append(data['feeds'][i]['field3'])
		entry_id.append(data['feeds'][i]['entry_id'])

	f.close()

def ShowData(): 
#	os.system('clear')
	print "Elija la variable a representar"
	print "1- Pulso"
	print "2- Temperatura"
	print "3- Oxigeno"

	codigo = input()
	if codigo == 1: 
		plt.plot(entry_id, bpm, label="Pulso")
	elif codigo == 2: 
		plt.plot(entry_id, temp, label="Pulso")
	elif codigo==3: 
		plt.plot(entry_id, s02, label="Pulso")
	else: 
		return

	plt.show()
	return


def main(): 
	print 'Starting'

	code = 1
	while (code==1 or code==2 or code==3): 
#		os.system('clear')
		print "Elige una opción a realizar"
		print "1- Subir Datos al Servidor"
		print "2- Obtener Datos del Servidor y Representarlos en gráfica"
		print "3- Representar los Datos"
		print "Otro - Salir de la App"
		code = input()
	
		if code == 1: 
			UpDataToThingspeak()
			print "Data Upload"
		elif code==2: 
			GetDataFromThingspeak()
			print "Data Got"
		elif code==3: 
			print "Showing Data"
			ShowData()
		else:
			exit

	

	print "Saliendo de la App..."

main()
