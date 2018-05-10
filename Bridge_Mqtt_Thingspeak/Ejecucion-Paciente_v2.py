#!/usr/bin/env python
# coding=utf-8
import Paciente
import EnvioTelegram

import paho.mqtt.client as mqtt
import time

mensaje_mqtt = ""
flag_msg = 0

	# The callback for when the client receives a CONNACK response from the$
def on_connect(client, userdata, flags, rc):
	print("Connected with result code "+str(rc))

	# Subscribing in on_connect() means that if we lose the connection and
	# reconnect then subscriptions will be renewed.
	client.subscribe("Sistema/Wemos/Temperatura") # Asi se conecta a todos los mensa$

# The callback for when a PUBLISH message is received from the server.
def on_message(client, userdata, msg):
	print("Recibido, Topic:"+msg.topic+" Mensaje:"+str(msg.payload))
	mensaje_mqtt = msg
	flag_msg = 1


def main(): 

	client = mqtt.Client()
	client.on_connect = on_connect
	client.on_message = on_message

	client.connect("192.168.4.1", 1883, 60)

	client.loop() # Linea necesaria para comprobar comunicaciones

	paciente = Paciente.Paciente()

	flag_msg = 0

	while(1):

		if flag_msg == 1:	#ERROR no consigo que tome esta rama...
			(pulso, temperatura, oxigeno) = paciente.getDataFromSensor(mensaje_mqtt)

			if oxigeno < 95: 
				print ("Envio mensaje de aviso por Telegram")
				telegram = EnvioTelegram.TelegramBot()
				telegram.getInformation()
				telegram.send("Nombre", "oxigeno")

			else:
				if temperatura == 0: # Es que no recibimos mensahe correcto
					print("Mensaje incorrecto de mqtt")
				else:
					paciente.UpDataToThingspeak(pulso, temperatura, oxigeno)

			flag_msg = 0

		client.loop() # Se debe llamara regularmente para no desconectar de broker

if __name__ == "__main__":
	main()