import sys

from PyQt5.QtWidgets import QApplication, QVBoxLayout, QWidget, QPushButton, QComboBox, QGroupBox, QRadioButton, QGridLayout, QGraphicsView, QMessageBox, QLineEdit, QHBoxLayout
from PyQt5.QtGui import QIcon

from matplotlib.backends.backend_qt5agg import FigureCanvasQTAgg as FigureCanvas
from matplotlib.figure import Figure
import matplotlib.pyplot as plt

from Medico import Medico


class RaspiMedical(QWidget):
	"""docstring for RaspiMedical"""
	def __init__(self):
		super(RaspiMedical, self).__init__()

		self.medico = Medico()
		self.pacientes = self.medico.getNameFromFile()
		self.medico.getDataFromThingspeak()

		self.vista = QGraphicsView()
		self.layout = QGridLayout()

		self.layout.setSpacing(30)

	#Adicion de Pacientes
		vbox2 = QVBoxLayout()
		self.line = QLineEdit("Escriba Nombre del Paciente")
		self.line.setMaximumWidth(400)
		self.AnyadirPaciente = QPushButton("Insertar Paciente")
		self.AnyadirPaciente.clicked.connect(self.anyadirUsuario)
		vbox2.addWidget(self.line)
		vbox2.addWidget(self.AnyadirPaciente)

	#Lista de Pacientes
		hbox = QHBoxLayout()
		self.listaNombres = QComboBox()
		for i in range(len(self.pacientes)): 
			self.listaNombres.addItem(self.pacientes[i])

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

		hbox.addWidget(self.listaNombres)
		hbox.addWidget(self.grupo)

	#Boton
		self.mostrarBoton = QPushButton("Mostrar Graficas")
		self.mostrarBoton.clicked.connect(self.drawGraphs)

	#Grafica Izquierda
		self.mediaGraph = PlotCanvas(self, width=4, height=3, Titulo="Media")

	#Grafica Derecha
		self.desvGraph = PlotCanvas(self, width=4, height=3, Titulo="Desviacion")

	##Colocacion
		self.layout.addLayout(vbox2, 0, 0, 1, 1)
		self.layout.addLayout(hbox, 0, 1, 1, 1)
		self.layout.addWidget(self.mostrarBoton, 0, 2, 1, 1)
		self.layout.addWidget(self.mediaGraph, 1, 0, 1, 3)
		self.layout.addWidget(self.desvGraph, 2, 0, 1, 3)

		self.vista.setLayout(self.layout)
		self.vista.showMaximized()

	def drawGraphs(self): 
		self.mediaGraph.cleanGraph()
		self.desvGraph.cleanGraph()

		nombre = self.listaNombres.currentText()
		id = self.medico.findUser(nombre)

		if self.pulsoOpcion.isChecked() :
			(media, desv) = self.medico.getData(id, "Pulso")
		elif self.temperaturaOpcion.isChecked() : 
			(media, desv) = self.medico.getData(id, "Temperatura")
		elif self.oxigenoOpcion.isChecked() : 
			(media, desv) = self.medico.getData(id, "Oxigeno")

		if len(media) == 0 or len(desv) == 0:
			QMessageBox.about(self, "Aviso", "Este Usuario no tiene ningun registro en la Base de Datos de Pacientes")
		else: 
			self.mediaGraph.drawGraphs(media)
			self.desvGraph.drawGraphs(desv)

	def anyadirUsuario(self): 
		self.medico.updateFile(self.line.text())



class PlotCanvas(FigureCanvas):
	"""docstring for ClassName"""
	def __init__(self, parent=None, width=5, height=4, dpi=100, Titulo=""):
		fig = Figure(figsize=(width, height), dpi=dpi)
		fig.suptitle(Titulo)
		self.axes = fig.add_subplot(111)


		FigureCanvas.__init__(self, fig)
		self.setParent(parent)

		#FigureCanvas.setSizePolicy(self,
			#QSizePolicy.Expanding, 
			#QSizePolicy.Expanding)

		FigureCanvas.updateGeometry(self)
		self.ax = self.figure.add_subplot(111)


	def drawGraphs(self, data): 
		self.ax.plot(data, 'r-')
		self.draw()

	def cleanGraph(self): 
		self.ax.cla()


if __name__== '__main__': 
	app = QApplication(sys.argv)
	ex = RaspiMedical()
	sys.exit(app.exec_())
