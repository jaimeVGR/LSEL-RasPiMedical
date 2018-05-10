#!/usr/bin/env python
# coding=utf-8
import sys
import os
import json
import urllib2
import numpy as np
import matplotlib.pyplot as plt

class Medico:
	"""
	Clase creada para realizar las acciones desde el punto de vista del medico y poder ver los datos
	de cada paciente de manera independiente. 
	"""
	def __init__(self):
		self.code = 480292
		self.API_Read = "KQWUKI7T8BPL3G6R"

		#### Listas para guardar los datos de ThingSpeak
		self.bpm = []
		self.temp = []
		self.s02 = []
		self.user_id = []
		self.entry_id = []

		#### Listas para obtener nombre e identificador del txt
		self.nombre_usuario = []
		self.identificador_usuario = []

		self.graph = []


	def getDataFromThingspeak(self): 
		URL = "https://api.thingspeak.com/channels/%s/feeds.json?api_key=%s&results=0" % (self.code, self.API_Read)
		try:
			f = urllib2.urlopen(URL)
		except urllib2.HTTPError,e: 
			print "ERROR, ha cambiado la API de lectura"
			f.close()
			return
		except urllib2.URLError,e: 
			print "ERROR, ha cambiado la API de lectura"
			f.close()
			return
		response = f.read()
		data = json.loads(response)

		for i in range (0, len(data['feeds'])):
			self.bpm.append(data['feeds'][i]['field1'])
			self.temp.append(data['feeds'][i]['field2'])
			self.s02.append(data['feeds'][i]['field3'])
			self.user_id.append(data['feeds'][i]['field4'])
			self.entry_id.append(data['feeds'][i]['entry_id'])

		f.close()
		print "Datos obtenidos"


	def getData(self, identificador, etiqueta): 
		mean = []
		desv = []

		for i in range(len(self.bpm)): 
			if int(self.user_id[i]) == identificador:
				if etiqueta == "Pulso":  
					mean.append(int(self.bpm[i])) 
					desv.append(int(self.bpm[i]))
				elif etiqueta == "Temperatura": 
					mean.append(int(self.temp[i]))
					desv.append(int(self.temp[i]))
				elif etiqueta == "Oxigeno":
					mean.append(int(self.s02[i]))
					desv.append(int(self.s02[i]))

		return (mean, desv)

		

	def showGraph(self, data, codigoGrafico, etiqueta): 
		minimo = data[0]
		maximo = data[0]
		for i in range(len(data)): 
			if data[i] > maximo: 
				maximo = data[i]
			if data[i] < minimo: 
				minimo = data[i]

		self.graph.append(plt.figure(codigoGrafico))
		plt.ylabel(etiqueta)
		plt.ylim(minimo-10, maximo+10)
		plt.plot(data)
		self.graph[len(self.graph)-1].show()


	def getNameFromFile(self):
		f = open("Names.txt", "r")
		for linea in f: 
			espacio = linea.find(" ")
			self.identificador_usuario.append(int(linea[:espacio]))
			self.nombre_usuario.append(linea[espacio+1:len(linea)-1])	
		f.close()
		return self.nombre_usuario

	def updateFile(self, name): 
		f = open("Names.txt", "a")	
		f.write("%s %s\n" % (str(len(self.identificador_usuario)+1), name))
		f.close()

		self.identificador_usuario.append(len(self.identificador_usuario))
		self.nombre_usuario.append(name)

		print "Nombres Actualizados"

	def findUser(self, user): 
		for i in range(len(self.nombre_usuario)): 
			if (user == self.nombre_usuario[i]): 
				return self.identificador_usuario[i]

		return -1
