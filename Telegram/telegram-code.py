#!usr/bin/env python
# -*- coding: utf-8 -*-
import telegram
import sys

bot = telegram.Bot(token='591870117:AAFMHjb1apJvr-_wUQnoeyUcA0F0QFe9IKc')


print(bot.get_me())
#print bot.get_updates()

chat_id = bot.get_updates()[-1].message.chat_id
print chat_id
bot.sendMessage(chat_id, "Bienvenido Raspi-Medical")
bot.sendMessage(chat_id, "El paciente %s está obteniendo medidas preocupantes" % sys.argv[1])
