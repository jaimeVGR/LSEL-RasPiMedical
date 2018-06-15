#!/usr/bin/env python
# -*- coding: utf-8 -*-

from matplotlib.backends.backend_qt5agg import FigureCanvasQTAgg as FigureCanvas
from matplotlib.figure import Figure
import matplotlib.pyplot as plt


from Modelo import *
from Vistas import *

if __name__== '__main__': 
	app = QApplication(sys.argv)

	general = General()
	nuevosistema = NuevoSistema()
	visualizacion = Visualizacion()
	versistemas = VerSistemas()

	m = Modelo(general, nuevosistema, visualizacion, versistemas)
	general.setModel(m)
	nuevosistema.setModel(m)
	visualizacion.setModel(m)
	versistemas.setModel(m)


	sys.exit(app.exec_())