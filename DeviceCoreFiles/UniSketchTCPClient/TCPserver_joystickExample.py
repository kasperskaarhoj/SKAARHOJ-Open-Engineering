#!/usr/bin/env python3
import socketserver
import socket
import base64
import re


"""
Test with JoyStick on XC7+XC3

- Keeps the connection alive
- 
"""

class MyTCPHandler(socketserver.BaseRequestHandler):
	
	def handle(self):
		self.request.settimeout(1)
		busy = False;	# This keeps track of the busy/ready state of the client. We can use this to make sure we are not spamming it with data
		
		HWCcolor = [2] * 256
		xSpeed = 0
		ySpeed = 0

		while True:
			try:
				# self.request is the TCP socket connected to the client
				self.data = self.request.recv(1024).strip()
			except socket.timeout:
				pass
			else:
				if self.data != b'':
					for line in self.data.split(b"\n"):
						outputline = ""
						print("Client {} sent: '{}<NL>'".format(self.client_address[0], line.decode('ascii')))

						if line == b"list":
							outputline = "ActivePanel=1\n"
							self.panelInitialized = True
							busy = False
						if line == b"BSY":
							busy = True

						if line == b"RDY":
							busy = False

						if line == b"ping":
							outputline = "ack\n"
							busy = False

						# Parse down trigger:
						match = re.search(r"^HWC#([0-9]+)=Speed:([\-0-9]+)$", line.decode('ascii'))
						if match:
							HWc = int(match.group(1));	# Extract the HWc number of the keypress from the match
							if (HWc==44):
								xSpeed = int(match.group(2));	# Speed
							if (HWc==43):
								ySpeed = int(match.group(2));	# Speed

							if (xSpeed!=0 or ySpeed!=0):

								# Highlight the button and turn on binary output:
								outputline = "HWCt#40={}|||Speeds:|1|x:|y:|{}|0\n".format(xSpeed, ySpeed)
							else:
								outputline = "HWCt#40=|||Speeds:|1|Still\n"
						
						# If outputline not empty, send content back to client:
						if outputline:
							self.request.sendall(outputline.encode('ascii'))
							print("- Returns: '{}'".format(outputline.replace("\n","<NL>")))

				else:
					print("{} closed".format(self.client_address[0]))
					break



class ThreadedTCPServer(socketserver.ThreadingMixIn, socketserver.TCPServer):
	pass

HOST, PORT = "0.0.0.0", 9923

# Create the server, binding to localhost on port 9923
server = ThreadedTCPServer((HOST, PORT), MyTCPHandler, bind_and_activate=False)
server.allow_reuse_address = True
server.server_bind()
server.server_activate()

# Activate the server; this will keep running until you
# interrupt the program with Ctrl-C
server.serve_forever()
