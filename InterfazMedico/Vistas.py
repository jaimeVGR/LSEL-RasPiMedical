#!/usr/bin/env python
# -*- coding: utf-8 -*-
import sys
from functools import partial

from PyQt5.QtWidgets import *
from PyQt5.QtGui import *
from PyQt5 import QtCore

from matplotlib.backends.backend_qt5agg import FigureCanvasQTAgg as FigureCanvas
from matplotlib.figure import Figure
import matplotlib.pyplot as plt

from Medico import Medico
from Modelo import *


class General(QWidget): 
	def __init__(self):
		super(General, self).__init__()

		self.vista = QGraphicsView()
		self.layout = QVBoxLayout()

		self.bienvenido = QLabel("Bienvenido a Raspi-Medical")
		self.bienvenido.setFont(QFont("Helvetica [Cronyx]", 42, QFont.Bold))

		self.anyadir = QPushButton("Añadir paciente")
		self.anyadir.setMinimumWidth(150)
		self.anyadir.setMaximumWidth(200)
		self.anyadir.clicked.connect( partial(self.changeWindow, "Añadir paciente"))
		self.VisualizarDatos = QPushButton("Visualizar Datos")
		self.VisualizarDatos.setMinimumWidth(150)
		self.VisualizarDatos.setMaximumWidth(200)
		self.VisualizarDatos.clicked.connect( partial(self.changeWindow, "Visualizar Datos"))
		self.visualizarPacientes = QPushButton("Visualizar Pacientes")
		self.visualizarPacientes.setMinimumWidth(150)
		self.visualizarPacientes.setMaximumWidth(200)
		self.visualizarPacientes.clicked.connect( partial(self.changeWindow, "Visualizar Pacientes"))

		self.layout.addWidget(self.bienvenido)
		self.layout.addWidget(self.anyadir)
		self.layout.addWidget(self.VisualizarDatos)
		self.layout.addWidget(self.visualizarPacientes)

		self.layout.setAlignment(self.anyadir,QtCore.Qt.AlignCenter)
		self.layout.setAlignment(self.VisualizarDatos,QtCore.Qt.AlignCenter)
		self.layout.setAlignment(self.visualizarPacientes,QtCore.Qt.AlignCenter)

		
		self.vista.setGeometry(200, 200, 700, 350)

		self.vista.setLayout(self.layout)

	def setModel(self, m): 
		self.miModelo = m

	def showWindow(self): 
		self.vista.show()

	def closeWindow(self): 
		self.vista.hide()

	def changeWindow(self, name): 
		if name == "Añadir paciente": 
			self.miModelo.showWindow(name)

		elif name == "Visualizar Datos":
			self.miModelo.showWindow(name)

		elif name == "Visualizar Pacientes": 
			self.miModelo.showWindow(name)


class NuevoSistema(QWidget): 
	def __init__(self):
		super(NuevoSistema, self).__init__()
		self.vista = QGraphicsView()
		self.layout = QGridLayout()

		## Desplegable
		hbox = QHBoxLayout()
		systemText = QLabel("Numero del sistema que va a asignar: ")
		self.SystemsList = QComboBox()
		hbox.addWidget(systemText)
		hbox.addWidget(self.SystemsList)
		hbox.setSpacing(30)
		self.layout.addLayout(hbox, 0, 0, 2, 1)
		#self.layout.addWidget(self.SystemsList, 0, 0, 2, 4)

		## Boton
		vbox = QVBoxLayout()
		self.AddButton = QPushButton("Añadir")
		self.AddButton.setMinimumWidth(150)
		self.AddButton.setMaximumWidth(150)
		self.AddButton.clicked.connect(self.addPaciente)

		self.ClearButton = QPushButton("Limpiar Campos")
		self.ClearButton.setMinimumWidth(150)
		self.ClearButton.setMaximumWidth(150)
		self.ClearButton.clicked.connect(self.clear)

		vbox.addWidget(self.AddButton)
		vbox.addWidget(self.ClearButton)

		self.layout.addLayout(vbox, 0, 1, 2, 1)
		self.layout.setAlignment(vbox,QtCore.Qt.AlignCenter)

		#Recordatorio
		aviso = QLabel("AVISO! \n\n\nAntes de dar al botón de añadir deberá tener rellenados los datos de los pacientes que vaya a registrar y seleccionado de manera correcta el systema. Tenga en cuenta también que es necesario que tenga encendida y conectada al Router la placa que se le ha proporcionada y que se llevará el paciente. Esto es indispensable para poder guardar en ella los datos de cada paciente.")
		aviso.setWordWrap(True)
		aviso.setFont(QFont("Helvetica [Cronyx]", 16, QFont.Bold))
		self.layout.addWidget(aviso, 0, 2, 2, 2)

		#Datos de Paciente
		self.MaxPacientes = 4
		self.nombre = []
		self.dni = []
		self.edad = []
		self.sexo = []
		self.telefono = []
 		self.tarjeta = []
 		self.grupos = []

 		for i in range(self.MaxPacientes): 
 			grupo = QGroupBox("Paciente %s" % (i+1))
 			grupo.setLayout(self.PacienteForm())
 			self.grupos.append(grupo)
 			self.layout.addWidget(self.grupos[i], 5, i, 1, 1)
 			self.layout.setAlignment(self.grupos[i],QtCore.Qt.AlignBottom)

 		self.BackHome = QPushButton("Volver")
 		self.BackHome.clicked.connect(self.back)
 		self.layout.addWidget(self.BackHome, 6, 1, 1, 2)

 		self.vista.setLayout(self.layout)


	def setModel(self, m): 
		self.miModelo = m

	def showWindow(self): 
		self.miModelo.GetSystemsAvailable()
		self.fillSystems()
		self.vista.showMaximized()

	def closeWindow(self): 
		self.vista.hide()

	def PacienteForm(self):

		nombre = QLabel("Nombre: ")
		nombre.setMaximumWidth(75)
		nombre_line = QLineEdit()
		nombre_line.setMinimumWidth(150)
		dni = QLabel("Dni (sin letra): ")
		dni.setMaximumWidth(125)
		dni_line = QLineEdit()
		edad = QLabel("Edad: ")
		edad.setMaximumWidth(75)
		edad_line = QLineEdit()
		sexo = QLabel("Sexo: ")
		sexo.setMaximumWidth(75)
		sexo_line = QLineEdit()
		telefono = QLabel("Telefono: ")
		telefono.setMaximumWidth(75)
		telefono_line = QLineEdit()
		tarjeta = QLabel("Tarjeta: ")
		tarjeta.setMaximumWidth(75)
		tarjeta_line = QLineEdit()

		grid = QGridLayout()
		grid.setSpacing(50)
		grid.addWidget(nombre, 0, 0, 1, 1)
		grid.addWidget(nombre_line, 0, 1, 1, 2)
		grid.addWidget(dni, 1, 0, 1, 1)
		grid.addWidget(dni_line, 1, 1, 1, 2)
		grid.addWidget(edad, 2, 0, 1, 1)
		grid.addWidget(edad_line, 2, 1, 1, 2)
		grid.addWidget(sexo, 3, 0, 1, 1)
		grid.addWidget(sexo_line, 3, 1, 1, 2)
		grid.addWidget(telefono, 4, 0, 1, 1)
		grid.addWidget(telefono_line, 4, 1, 1, 2)
		grid.addWidget(tarjeta, 5, 0, 1, 1)
		grid.addWidget(tarjeta_line, 5, 1, 1, 2)

		grid.setVerticalSpacing(30)

		self.nombre.append(nombre_line)
		self.dni.append(dni_line)
		self.edad.append(edad_line)
		self.sexo.append(sexo_line)
		self.telefono.append(telefono_line)
		self.tarjeta.append(tarjeta_line)

		return grid

	def fillSystems(self): 
		for i in range(self.miModelo.GetAmountOfSystems()): 
			text = str(self.miModelo.getId(i))
			self.SystemsList.addItem(text)

	def removeSystems(self): 	
		self.SystemsList.clear()

	def addPaciente(self): 
		self.miModelo.deleteFile()
		for i in range(len(self.nombre)):
			if self.nombre[i].text() != "" and self.dni[i].text() != "" and self.edad[i].text() != "" and self.sexo[i].text() != "" and self.telefono[i].text() != "" and self.tarjeta[i].text() != "":
				self.miModelo.addSystem(self.SystemsList.currentText(), self.nombre[i].text(),self.dni[i].text(), self.edad[i].text(), self.sexo[i].text(), self.telefono[i].text(), self.tarjeta[i].text())


	def getSystemSelected(self): 
		return self.SystemsList.currentText()

	def back(self): 
		self.miModelo.showWindow("General")

	def clear(self): 
		for i in range(len(self.nombre)): 
			self.nombre[i].setText("")

		for i in range(len(self.dni)): 
			self.dni[i].setText("")

		for i in range(len(self.edad)): 
			self.edad[i].setText("")

		for i in range(len(self.sexo)): 
			self.sexo[i].setText("")

		for i in range(len(self.telefono)): 
			self.telefono[i].setText("")

		for i in range(len(self.tarjeta)): 
			self.tarjeta[i].setText("")

class Visualizacion(QWidget): 
	def __init__(self):
		super(Visualizacion, self).__init__()

		#self.medico = Medico()
		#self.pacientes = self.medico.getNameFromFile()
		#self.medico.getDataFromThingspeak()

		self.vista = QGraphicsView()
		self.layout = QGridLayout()

		self.layout.setSpacing(30)

	#Lista de Pacientes
		self.listaNombres = QComboBox()
#		if self.pacientes != -1: 
#			for i in range(len(self.pacientes)): 
#				self.listaNombres.addItem(self.pacientes[i])

	#Opciones Datos
		self.grupo = QGroupBox("Variable a Mostrar")
		self.pulsoOpcion = QRadioButton("Pulso")
		self.temperaturaOpcion = QRadioButton("Temperatura")
		self.oxigenoOpcion = QRadioButton("Oxigeno")
		self.pulsoOpcion.setChecked(True)
		vbox = QVBoxLayout()
		vbox.addWidget(self.pulsoOpcion)
		vbox.addWidget(self.temperaturaOpcion)
		vbox.addWidget(self.oxigenoOpcion)
		self.grupo.setLayout(vbox)

	#Boton
		vboxButtons = QVBoxLayout()
		self.mostrarBoton = QPushButton("Mostrar Graficas")
		self.mostrarBoton.clicked.connect(self.drawGraphs)
		self.Volver = QPushButton("Volver")
		self.Volver.clicked.connect(self.back)
		vboxButtons.addWidget(self.mostrarBoton)
		vboxButtons.addWidget(self.Volver)


	#Grafica Izquierda
		self.mediaGraph = PlotCanvas(self, width=4, height=3, Titulo="Media")

	#Grafica Derecha
		self.desvGraph = PlotCanvas(self, width=4, height=3, Titulo="Desviacion")

	##Colocacion
		self.layout.addWidget(self.listaNombres, 0, 0, 1, 1)
		self.layout.addWidget(self.grupo, 0, 1, 1, 1)
		self.layout.addLayout(vboxButtons, 0, 2, 1, 1)
		self.layout.addWidget(self.mediaGraph, 1, 0, 1, 3)
		self.layout.addWidget(self.desvGraph, 2, 0, 1, 3)

		self.vista.setLayout(self.layout)

	def back(self): 
		self.miModelo.showWindow("General")

	def drawGraphs(self): 
		self.mediaGraph.cleanGraph()
		self.desvGraph.cleanGraph()

		nombre = self.listaNombres.currentText()
		id_number = self.miModelo.findUser(nombre)

		media = []
		desv = []
	
		if self.pulsoOpcion.isChecked():
			(media, desv) = self.miModelo.getDataFromPatient(id_number, "Pulso")
		elif self.temperaturaOpcion.isChecked(): 
			(media, desv) = self.miModelo.getDataFromPatient(id_number, "Temperatura")
		elif self.oxigenoOpcion.isChecked(): 
			(media, desv) = self.miModelo.getDataFromPatient(id_number, "Oxigeno")


		if len(media) == 0 or len(desv) == 0:
			QMessageBox.about(self, "Aviso", "Este Usuario no tiene ningun registro en la Base de Datos de Pacientes")
		else: 
			minValue = float(media[0])
			maxValue = float(media[0])

			for i in range(len(media)): 
				if minValue > float(media[i]):
					minValue = float(media[i])
				if maxValue < float(media[i]):
					maxValue = float(media[i])	
			self.mediaGraph.drawGraphs(media, minValue, maxValue)

			minValue = float(desv[0])
			maxValue = float(desv[0])

			for i in range(len(desv)): 
				if minValue > float(desv[i]):		
					minValue = float(desv[i])
				if maxValue < float(desv[i]):
					maxValue = float(desv[i])	
			self.desvGraph.drawGraphs(desv, minValue, maxValue)


	def setModel(self, m): 
		self.miModelo = m

	def showWindow(self): 
		if self.listaNombres.count() > 0: 
			self.listaNombres.clear()
		for i in range(self.miModelo.getAmountOfPacientes()):
			self.listaNombres.addItem(self.miModelo.getPaciente(i)[1])
		self.miModelo.getDataFromThingspeak()
		self.vista.showMaximized()

	def closeWindow(self): 
		self.vista.hide()

	def removeSystems(self): 	
		self.listaNombres.clear()


class PlotCanvas(FigureCanvas):
	"""docstring for ClassName"""
	def __init__(self, parent=None, width=5, height=4, dpi=100, Titulo=""):
		fig = Figure(figsize=(width, height), dpi=dpi)
		fig.suptitle(Titulo)
		self.axes = fig.add_subplot(111)


		FigureCanvas.__init__(self, fig)
		self.setParent(parent)

		FigureCanvas.updateGeometry(self)
		self.ax = self.figure.add_subplot(111)

	def drawGraphs(self, data, minValue, maxValue): 
		self.ax.set_ylim(bottom=float(minValue)-2, top=float(maxValue)+2)
		
		data_float = []
		for i in range(len(data)): 
			data_float.append(float(data[i]))
		
		self.ax.plot(data_float, 'r-')
		self.draw()

	def cleanGraph(self): 
		self.ax.cla()



class VerSistemas(QWidget): 
	def __init__(self):
		super(VerSistemas, self).__init__()
		self.vista = QGraphicsView()
		self.layout = QGridLayout()


		self.amount_columns = 7

		self.table = QTableWidget()
		self.table.setColumnCount(self.amount_columns)

		self.header = ["No Sistema", "Paciente","DNI", "Edad", "Sexo", "Telefono", "No Tarjeta"]
		self.table.setHorizontalHeaderLabels(self.header)
		self.table.verticalHeader().setVisible(False)
		self.table.setEditTriggers(QAbstractItemView.NoEditTriggers)
		self.table.setSelectionBehavior(QAbstractItemView.SelectRows)
		self.table.setSelectionMode(QAbstractItemView.SingleSelection)
		self.table.setShowGrid(True)



		self.volver = QPushButton("Volver")
		self.volver.clicked.connect(self.back)


		self.layout.addWidget(self.table, 0, 0, 6, 3)
		self.layout.addWidget(self.volver, 6, 1, 1, 1)

		self.vista.setLayout(self.layout)


	def setModel(self, m): 
		self.miModelo = m

	def showWindow(self):
		#self.readFile() 
		self.fillRowsOfTable()
		self.vista.showMaximized()

	def closeWindow(self): 
		self.vista.hide()

	#def readFile(self): 
	#	self.miModelo.readPacientes()

	def back(self): 
		self.miModelo.showWindow("General")

	def fillRowsOfTable(self):
		for i in range(self.miModelo.getAmountOfPacientes()): 
			self.table.removeRow(self.miModelo.getAmountOfPacientes()-i-1) 

		for i in range(self.miModelo.getAmountOfPacientes()): 
			self.table.insertRow(i)
			(system, name, dni, age, sex, tfn, number) = self.miModelo.getPaciente(i)			
			self.table.setItem(i, 0, QTableWidgetItem(system))
			self.table.setItem(i, 1, QTableWidgetItem(name))
			self.table.setItem(i, 2, QTableWidgetItem(dni))
			self.table.setItem(i, 3, QTableWidgetItem(age))
			self.table.setItem(i, 4, QTableWidgetItem(sex))
			self.table.setItem(i, 5, QTableWidgetItem(tfn))
			self.table.setItem(i, 6, QTableWidgetItem(number))
			self.table.resizeRowsToContents()
			self.table.resizeColumnsToContents()