#!/usr/bin/env python
# coding=utf-8
import Paciente
import EnvioTelegram

def main(): 
	print "Escriba los datos a subir"
	paciente = Paciente.Paciente()

	(pulso, temperatura, oxigeno) = paciente.getDataFromSensor()

	if oxigeno < 95: 
		telegram = EnvioTelegram.TelegramBot()
		telegram.getInformation()
		telegram.send("Nombre", "oxigeno")

	else:
		paciente.UpDataToThingspeak(pulso, temperatura, oxigeno)



if __name__ == "__main__":
    main()