#!/usr/bin/env python
# -*- coding: utf-8 -*-
from Vistas import *
from SCPCommand import SCPCommand
from Pacientes import *
import os

import sys
import json
import urllib2
import numpy as np
import matplotlib.pyplot as plt


class Modelo(object): 
	def __init__(self, g, ns, v, vs): 
		"""
		Constructor. Se crean todos los atributos de la clase, donde cada uno de ellos se refiere a una de las vistas creadas.
			Las vistas creadas son las siguientes. 
				- General
				- Añadir
				- Visualizar Datos
				- Ver Sistemas
			Se empieza viendo la pantalla General. 
		"""
		self.general = g
		self.nuevosistema = ns
		self.visualizacion = v
		self.versistemas = vs

		self.vectorPacientes = []

		self.readPacientes()
		self.deleteFile()

		self.showWindow("General")

	def showWindow(self, name): 
		"""
			Método de la case que enseña la pantalla seleccionada y cierra las demás. Se selecciona la ventana en función del botón pulsado. 
		"""
		if name == "General": 
			self.nuevosistema.closeWindow()
			self.visualizacion.closeWindow()
			self.versistemas.closeWindow()
			self.general.showWindow()

		elif name == "Añadir paciente":
			self.nuevosistema.removeSystems()
			self.general.closeWindow()
			self.visualizacion.closeWindow()
			self.versistemas.closeWindow()
			self.nuevosistema.showWindow()

		elif name == "Visualizar Datos":
			self.nuevosistema.removeSystems()
			self.general.closeWindow()
			self.versistemas.closeWindow()
			self.nuevosistema.closeWindow()
			self.visualizacion.showWindow()

		elif name == "Visualizar Pacientes": 
			self.general.closeWindow()
			self.nuevosistema.closeWindow()
			self.visualizacion.closeWindow()
			self.versistemas.showWindow()

	def GetSystemsAvailable(self): 
		"""
			Se obtienen los sistemas disponibles en el fichero "Sistemas.txt", donde está presente el número de sistema y su IP. 
		"""
		f = open("Sistemas.txt", "r")
		
		self.systems_id = []
		self.systems_ip = []
		for linea in f: 
			self.systems_id.append(int(linea.split(" ")[0]))
			self.systems_ip.append(linea.split(" ")[1].split("\n")[0])
		f.close()


	def GetAmountOfSystems(self): 
		"""
			Devuelve la cantidad de sistemas disponibles en el fichero "Sistemas.txt"
		"""
		if len(self.systems_ip) == len(self.systems_id): 
			return len(self.systems_id)
		else:
			if (len(self.systems_id) > len(self.systems_ip)): 
				return len(self.systems_ip)
			else:
				return len(self.systems_id)

	def getId(self, id): 
		"""
			Método que devuelve el id de sistema para rellenar la pantalla de Añadir
		"""
		return self.systems_id[id]

	def readPacientes(self):
		"""
			Método que lee el archivo de "Pacientes.txt" y parsea las líneas escritas en éste para guardar en la base de datos los datos de los pacientes
			que se encuentran guardados en el sistema y sus datos. 
		""" 
		if os.path.isfile("Pacientes.txt"): 
			f = open("Pacientes.txt", "r")
			
			for linea in f: 
				sistema = linea.split("$ ")[0]
				nombre = linea.split("$ ")[1]
				dni = linea.split("$ ")[2]
				edad = linea.split("$ ")[3]
				sexo = linea.split("$ ")[4]
				telefono = linea.split("$ ")[5]
				tarjeta = linea.split("$ ")[6].split("\n")[0]

				paciente = Patient(sistema, nombre, dni, edad, sexo, telefono, tarjeta)
				self.vectorPacientes.append(paciente)

			#self.getMaximumIdent()
		#else: 
		#	self.nextIdent = 1

	def getAmountOfPacientes(self): 
		"""
			Devuelve la cantidad de Pacientes de la base de datos leída
		"""
		return len(self.vectorPacientes)

	def getPaciente(self, pos): 
		"""
			Método que devuelve los datos del paciente que se encuentra en la posición "pos"
		"""
		return self.vectorPacientes[pos].getPersonalData()

	def addSystem(self, sistema, nombre, dni, edad, sexo, telefono, tarjeta): 
		"""
			Método empleado para añadir un sistema a la BBDD
		"""

		#Se añade el Paciente a la BBDD
		paciente = Patient(sistema, nombre, dni, edad, sexo, telefono, tarjeta)
		self.vectorPacientes.append(paciente)

		f = open("NombrePaciente.txt", "a")
		data = "%s,%s,%s\n" % (self.vectorPacientes[len(self.vectorPacientes)-1].getNombre(),  self.vectorPacientes[len(self.vectorPacientes)-1].getDNI(), self.vectorPacientes[len(self.vectorPacientes)-1].getTarjeta())
		f.write(data)
		f.close()


		f = open("Pacientes.txt", "w")
		for i in range(len(self.vectorPacientes)): 
			line = "%s$ %s$ %s$ %s$ %s$ %s$ %s\n" % (self.vectorPacientes[i].getSistema(), self.vectorPacientes[i].getNombre(), self.vectorPacientes[i].getDNI(), self.vectorPacientes[i].getEdad(), self.vectorPacientes[i].getSexo(), self.vectorPacientes[i].getTelefono(), self.vectorPacientes[i].getTarjeta())
			f.write(line)
		f.close()

		#self.nextIdent = self.nextIdent + 1
		self.sendToSystem(self.nuevosistema.getSystemSelected())


	def sendToSystem(self, sistema): 
		"""
			Método que lee la dirección ip del sistema seleccionado en la ventana de "Añadir" y manda el archivo de "NombrePaciente.txt", donde se encuentran
			los datos necesarios para cada sistema, al sistema concreto por SCP
		"""
		f = open("Sistemas.txt", "r")
		for linea in f: 
			if linea.split(" ")[0] == str(sistema): 
				ip = linea.split(" ")[1].split("\n")[0]

		##Envio de Datos por SCP
		scp = SCPCommand(ip)
		scp.sendFile("NombrePaciente.txt")

	def deleteFile(self): 
		"""
			Método empleado para eliminar el archio de "NombrePaciente.txt" para que no interfiera en otro sistema añadido
		"""
		if os.path.isfile("NombrePaciente.txt"): 
			os.remove("NombrePaciente.txt")


	def getMaximumIdent(self): 
		#	Método que busca el máximo identificador atribuido y automáticamente determina que el próximo identificador 
		#	a seleccionar será el máximo actual más uno. 
		
		if len(self.vectorPacientes) == 0: 
			self.nextIdent = 1
		else: 
			maximum = self.vectorPacientes[0].getIdentificador()
			for i in range(len(self.vectorPacientes)): 
				if maximum <= self.vectorPacientes[i].getIdentificador(): 
					maximum = self.vectorPacientes[i].getIdentificador()

			self.nextIdent = int(maximum) + 1

	def getDataFromThingspeak(self): 
		"""
			Método empleado para obtener los datos desde Thingspeak
		"""

		for i in range(len(self.vectorPacientes)): 
			self.vectorPacientes[i].cleanValues()

		self.code = 480292
		self.API_Read = "KQWUKI7T8BPL3G6R"


		URL = "https://api.thingspeak.com/channels/%s/feeds.json?api_key=%s&results=0" % (self.code, self.API_Read)
		try:
			f = urllib2.urlopen(URL)
		except urllib2.HTTPError,e: 
			print "ERROR, ha cambiado la API de lectura"
			return
		except urllib2.URLError,e: 
			print "ERROR, ha cambiado la API de lectura"
			return

		response = f.read()
		data = json.loads(response)

		for i in range (0, len(data['feeds'])):
			Mbpm = data['feeds'][i]['field1']
			Dbpm = data['feeds'][i]['field2']
			Mtemp = data['feeds'][i]['field3']
			Dtemp = data['feeds'][i]['field4']
			Ms02 = data['feeds'][i]['field5']
			Ds02 = data['feeds'][i]['field6']
			user_id = data['feeds'][i]['field7']
			entry_id = data['feeds'][i]['entry_id']

			for j in range(len(self.vectorPacientes)): 
				if user_id == self.vectorPacientes[j].getDNI(): 
					self.vectorPacientes[j].addDataToPatient(Mbpm, Dbpm, Mtemp, Dtemp, Ms02, Ds02, entry_id)

		f.close()


	def getDataFromPatient(self, id_user, valor):
		mean = []
		desv = []

		if (valor == "Pulso"):
			for i in range(len(self.vectorPacientes)): 
				if id_user == self.vectorPacientes[i].getDNI():
					(mean, desv) = self.vectorPacientes[i].getPulsoPatient()

		elif (valor == "Temperatura"): 
			for i in range(len(self.vectorPacientes)): 
				if id_user == self.vectorPacientes[i].getDNI():
					(mean, desv) = self.vectorPacientes[i].getTemperaturaPatient()

		elif (valor == "Oxigeno"): 
			for i in range(len(self.vectorPacientes)): 
				if id_user == self.vectorPacientes[i].getDNI():
					(mean, desv) = self.vectorPacientes[i].getOxigenoPatient()

		return (mean, desv)


	def findUser(self, nombre): 
		for i in range(len(self.vectorPacientes)): 
			if nombre == self.vectorPacientes[i].getNombre(): 
				return self.vectorPacientes[i].getDNI()

		return 0
 
