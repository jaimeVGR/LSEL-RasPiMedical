import telegram

class TelegramBot(object):
	"""
		Clase creada para manejar el envio de mensajes desde la raspberry del paciente al 
		telegram del medico en caso de error. 
	"""
	def __init__(self):
		TOKEN = "591870117:AAFMHjb1apJvr-_wUQnoeyUcA0F0QFe9IKc"
		self.bot = telegram.Bot(token=TOKEN)


	def getInformation(self):
		print self.bot.get_me()

	def send(self, name, aspecto): 
		self.chat_id = self.bot.get_updates()[-1].message.chat_id
		aviso = "AVISO URGENTE! El paciente %s esta obteniendo medidas preocupantes respecto a %s" % (name, aspecto)
		self.bot.sendMessage(self.chat_id, aviso)