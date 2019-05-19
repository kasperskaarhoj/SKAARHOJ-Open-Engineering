#!/usr/bin/env python3
import socketserver
import socket
import base64
import time
import re

"""
	This test sends button colors and graphics as fast as possible
	Great burn-in test and flashing panel!

"""





class MyTCPHandler(socketserver.BaseRequestHandler):
	
	def handle(self):
		while True:
			try:
				# self.request is the TCP socket connected to the client
				self.data = self.request.recv(1024)
				print("Client {} sent: {}".format(self.client_address[0], self.data))
			except socket.timeout:	
				pass


class ThreadedTCPServer(socketserver.ThreadingMixIn, socketserver.TCPServer):
	pass



HOST, PORT = "0.0.0.0", 23

# Create the server, binding to localhost on port 9999
server = ThreadedTCPServer((HOST, PORT), MyTCPHandler, bind_and_activate=False)
server.allow_reuse_address = True
server.server_bind()
server.server_activate()

# Activate the server; this will keep running until you
# interrupt the program with Ctrl-C
server.serve_forever()