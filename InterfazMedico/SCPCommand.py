#!/usr/bin/env python
# -*- coding: utf-8 -*-
import os
from paramiko import SSHClient
from scp import SCPClient

class SCPCommand():
	"""docstring for SCP"""
	def __init__(self, ip):
		self.IP = ip
		self.user = "pi"
		self.password = "raspberry"

		self.ssh = SSHClient()
		self.ssh.load_host_keys(os.path.expanduser(os.path.join("~", ".ssh", "known_hosts")))
		
		print self.IP
		print self.user
		print self.password

		self.ssh.connect(self.IP, username=self.user, password=self.password)

	def sendFile(self, text): 
		ruta = "/home/pi/Documents/LSEL/Python_Raspy_v3/" + text
		print ruta
		with SCPClient(self.ssh.get_transport()) as scp: 
			scp.put(text, ruta)		
