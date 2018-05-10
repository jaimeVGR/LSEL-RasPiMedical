#!/usr/bin/env python
# coding=utf-8
import sys
import os
import urllib2
import numpy as np

class Paciente:
	"""
	Clase creada para obtener los datos desde los sensores y subirlos a la web de Thingspeak.
	Cada usuario tendrá creado un identificador y conocerá sólamente la API para poder escribir 
	los datos. Estos irán asociados a su identificador, que es único.
	"""
	def __init__(self):
		f = open("NombrePaciente.txt", "r")
		for linea in f: 
			espacio = linea.find(" ")
			self.identificador = int(linea[0:espacio])
			self.nombre_usuario = linea[espacio+1:len(linea)-1]

		self.API_Write = "1HUIIJ1XJ61ZO06G"


	def getDataFromSensor(self, msg): 

		"""if msg.startswith("Temperatura"):
			
			[basura,media_y_texto,varianza] = msg.split("=")
			[media,basura,basura] = media_y_texto.split()
			return (0,int(media),int(varianza))
		else:
			print("Mensaje de Oxigeno o Pulso, O error...")
			return (0,0,0)
		"""

		####Cambar por codigo para obtener los datos de la WeMos
		print "introduce media pulso:"
		Mp = input()
		Vp = Mp
		print "introduce temperatura:"
		Mt = input()
		Vt = Mt
		print "introduce oxigeno: "
		Mo = input()
		Vo = Mo 

		return (Mp, Vp, Mt, Vt, Mo, Vo) #Pulso, Temperatura, Oxíg



	def UpDataToThingspeak(self, pulso, temperatura, oxigeno): 

		self.URL = "https://api.thingspeak.com/update?api_key=%s&field1=%s&field2=%s&field3=%s&field4=%s&field5=%s&field6=%s&field7=%s" % (self.API_Write, pulsoMedia, pulsoVarianza, temperaturaMedia, temperaturaVarianza, oxigenoMedia, oxigenoVarianza, self.identificador)
		try: 
			f = urllib2.urlopen(self.URL)
		except urllib2.HTTPError,e: 
			print "Error"
			f.close()
			return
		except urllib2.URLError,e: 
			print "Error"
			f.close()
			return

		f.close()
		print "Datos subidos con exito"

	def getName(self): 
		return self.nombre_usuario 
		