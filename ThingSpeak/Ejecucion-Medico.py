#!/usr/bin/env python
# coding=utf-8
import sys
import os
import json
import urllib2
import numpy as np
import matplotlib.pyplot as plt
import Medico


def main(): 
	medico = Medico.Medico()
	medico.getNameFromFile()
	medico.getDataFromThingspeak()


	codigo = 1
	while (codigo != 0): 		
		print "Introduzca Usuario del que quiere ver los datos:"
		name = raw_input()

		id = medico.findUser(name)

		if not(id == -1): 
			medico.showData(id)
		else: 
			print "No existe este Usuario"
		
		string = raw_input("¿Quiere seguir viendo Datos? (Yes/No)")
		if string == "Yes" or string == "Y": 
			codigo = 1
		else: 
			codigo = 0

	print "Fin de Visualizacion de Datos"

if __name__ == "__main__":
    main()