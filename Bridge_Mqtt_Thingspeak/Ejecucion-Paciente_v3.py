#!/usr/bin/env python
# coding=utf-8
from Paciente import Paciente
import EnvioTelegram

import paho.mqtt.client as mqtt
import time



	# The callback for when the client receives a CONNACK response from the$
def on_connect(client, userdata, flags, rc):
	print("Connected with result code "+str(rc))
	print ("Waiting for messages")

	# Subscribing in on_connect() means that if we lose the connection and
	# reconnect then subscriptions will be renewed.
	client.subscribe("Sistema/Wemos/#") # Asi se conecta a todos los mensajes
	client.subscribe("Sistema/Movil/DNI")

# The callback for when a PUBLISH message is received from the server.
def on_message(client, userdata, msg):
	global flag_msg
	global mensaje_mqtt 
	global tema_mqtt

	print("Recibido, Topic:"+msg.topic+" Mensaje:"+str(msg.payload))
	tema_mqtt = str(msg.topic)
	mensaje_mqtt = str(msg.payload)

	flag_msg = 1


def main(): 
	global flag_msg
	global mensaje_mqtt 
	global tema_mqtt

	client = mqtt.Client()
	client.on_connect = on_connect
	client.on_message = on_message

	client.connect("192.168.4.1", 1883, 60)
	#client.loop() # Linea necesaria para comprobar comunicaciones

	paciente = Paciente()

	flag_msg = 0
	flag_temp,flag_ox,flag_pulso = 0 , 0 ,0
	pm, pv, tm, tv, om, ov = 0,0,0,0,0,0

	while(1):

		if flag_msg == 1:

			if tema_mqtt.startswith("Sistema/Wemos/RFID"):
				print ("RFID recibido")
				if paciente.checkRFID(mensaje_mqtt):
					client.publish("Sistema/Raspy",1)
					print ("RFID correcto, envio respuesta")

			elif tema_mqtt.startswith("Sistema/Movil"):
				print ("Mensaje de movil")

			elif tema_mqtt.startswith("Sistema/Wemos/Pulso"):
				(tm,tv) = paciente.getDataFromSensor(mensaje_mqtt)

				if (tm != 0):
					flag_temp = 1

			elif tema_mqtt.startswith("Sistema/Wemos/Oxigeno"):
				(om, ov) = paciente.getDataFromSensor(mensaje_mqtt)

				if (om != 0):
					flag_ox = 1

					if om < 95: 
						print ("Envio mensaje de aviso por Telegram")
						telegram = EnvioTelegram.TelegramBot()
						telegram.getInformation()
						telegram.send(paciente.getName(), "oxigeno")

			elif tema_mqtt.startswith("Sistema/Wemos/Temperatura"):
				(pm, pv) = paciente.getDataFromSensor(mensaje_mqtt)	
				if (pm != 0 and pv != 0):
					flag_temp = 1

			else:
				print("Mensaje incorrecto de mqtt")

			flag_msg = 0

		if (flag_temp == 1) and (flag_ox == 1) and (flag_pulso == 1):
			paciente.UpDataToThingspeak(pm, pv, tm, tv, om, ov)
		
		client.loop() # Se debe llamara regularmente para no desconectar de broker

if __name__ == "__main__":
	main()
