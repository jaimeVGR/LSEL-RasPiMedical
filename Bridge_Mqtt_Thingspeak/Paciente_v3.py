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
			[self.identificador,self.nombre_usuario,self.dni,RFID] = linea.split(",") #Falta quitar /n del RFID
			self.RFID = RFID.split('\n')[0]
			#espacio = linea.find(" ")
			#self.identificador = int(linea[0:espacio])
			#self.nombre_usuario = linea[espacio+1:len(linea)-1]

		self.API_Write = "1HUIIJ1XJ61ZO06G"


	def getDataFromSensor(self, msg): 

		print msg
			
		[basura,media_y_texto,varianza] = msg.split("=")
		[media,basura,basura] = media_y_texto.split(" ")

		print ("Valores obtenidos: Media Temp = " + media+"  Varianza = "+varianza)
		return (float(media),float(varianza))


		"""
		####Cambar por codigo para obtener los datos de la WeMos
		print "introduce media pulso:"
		Mp = input()
		Vp = Mp
		print "introduce media temperatura:"
		Mt = input()
		Vt = Mt
		print "introduce media oxigeno: "
		Mo = input()
		Vo = Mo 
		"""
		#return (Mp, Vp, Mt, Vt, Mo, Vo) #Pulso, Temperatura, Oxíg



	def UpDataToThingspeak(self, pulsoMedia, pulsoVarianza, temperaturaMedia, temperaturaVarianza, oxigenoMedia, oxigenoVarianza): 

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

	def checkRFID(self, mensaje):
		f = open("NombrePaciente.txt", "r")
		
		rfid = mensaje.split(":")[1].split('\n')[0]
		for line in f: 
			if (line.split(',')[3].split('\n')[0]) == rfid: 
				return 1

		return 0


	def checkDNI(self, mensaje):
		f = open("NombrePaciente.txt", "r")
		print mensaje
		for line in f: 
			print (line.split(',')[2])
			if line.split(',')[2] == mensaje: 
				return 1

		return 0
		
