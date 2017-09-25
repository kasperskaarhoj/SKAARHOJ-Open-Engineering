#!/usr/bin/env python3
import socketserver
import socket
import base64
import re

class MyTCPHandler(socketserver.BaseRequestHandler):
	"""
	The RequestHandler class for our server.

	It is instantiated once per connection to the server, and must
	override the handle() method to implement communication to the
	client.
	"""
	
	def handle(self):
		self.request.settimeout(1)
		busy = False;	# This keeps track of the busy/ready state of the client. We can use this to make sure we are not spamming it with data
		
		HWCcolor = [2] * 256

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

						# Parse map= and turn on the button in dimmed mode for each. 
						# We could use the data from map to track which HWcs are active on the panel
						match = re.search(r"^map=([0-9]+):([0-9]+)$", line.decode('ascii'))
						if match:
							HWc = int(match.group(2));	# Extract the HWc number of the keypress from the match
							outputline = "HWC#{}={}\n".format(HWc,5)

						# Parse down trigger:
						match = re.search(r"^HWC#([0-9]+)=Down$", line.decode('ascii'))
						if match:
							HWc = int(match.group(1));	# Extract the HWc number of the keypress from the match

							# Highlight the button and turn on binary output:
							outputline = "HWC#{}={}\n".format(HWc, 4 | 0x20)

							# Rotate color number:
							HWCcolor[HWc] = (HWCcolor[HWc] + 1)%17;	# Rotate the internally stored color number
							outputline = outputline + "HWCc#{}={}\n".format(HWc,HWCcolor[HWc] | 0x80)	# OR'ing 0x80 to identify the color as an externally imposed color. By default the least significant 6 bits will be an index to a color, but you can OR 0x40 and it will instead accept a rrggbb combination.

						# Parse Up trigger:
						match = re.search(r"^HWC#([0-9]+)=Up$", line.decode('ascii'))
						if match:
							HWc = int(match.group(1));	# Extract the HWc number of the keypress from the match

							# Dim the button:
							outputline = "HWC#{}={}\n".format(HWc,5)

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
