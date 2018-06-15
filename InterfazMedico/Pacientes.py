#!/usr/bin/env python
# -*- coding: utf-8 -*-
import sys
import os

class Patient(object):
	"""docstring for Paciente"""
	def __init__(self, sistema, nombre, dni, edad, sexo, telefono, tarjeta):

		self.sistema = sistema
		self.nombre = nombre	
		self.dni = dni
		self.edad = edad
		self.sexo = sexo
		self.telefono = telefono
		self.tarjeta = tarjeta
		
		self.bpmMean = []
		self.bpmDesv = []

		self.TMean = []
		self.TDesv = []

		self.so2Mean = []
		self.so2Desv = []

		self.entry_id = []

	def addDataToPatient(self, mbpm, dbpm, mt, dt, mso2, dso2, entry_id): 

		self.bpmMean.append(mbpm)
		self.bpmDesv.append(dbpm)

		self.TMean.append(mt)
		self.TDesv.append(dt)

		self.so2Mean.append(mso2)
		self.so2Desv.append(dso2)

		self.entry_id.append(entry_id)


	def getDataOfPatient(self, variable): 
		if variable == "Pulso": 
			return (self.bpmMean, self.bpmDesv)
		elif variable == "Temperatura": 
			return (self.TMean, self.TDesv)
		elif variable == "Oxigeno": 
			return (self.so2Mean, self.so2Desv)

	def getPatientObject(self): 
		return self

	def getSistema(self): 
		return self.sistema

	def getNombre(self): 
		return self.nombre

	def getDNI(self): 
		return self.dni

	def getEdad(self):
		return self.edad 

	def getSexo(self): 
		return self.sexo

	def getTelefono(self): 
		return self.telefono

	def getTarjeta(self): 
		return self.tarjeta

	def getPersonalData(self): 
		return (self.getSistema(), self.getNombre(), self.getDNI(), self.getEdad(), self.getSexo(), self.getTelefono(), self.getTarjeta())

	def getPulsoPatient(self): 
		return (self.bpmMean, self.bpmDesv)

	def getTemperaturaPatient(self): 
		return (self.TMean, self.TDesv)

	def getOxigenoPatient(self): 
		return (self.so2Mean, self.so2Desv)

	def cleanValues(self): 
		
		if (len(self.bpmDesv)> 0):
			del self.bpmMean[:]
			del self.bpmDesv[:]

			del self.TMean[:]
			del self.TDesv[:]

			del self.so2Mean[:]
			del self.so2Desv[:]
