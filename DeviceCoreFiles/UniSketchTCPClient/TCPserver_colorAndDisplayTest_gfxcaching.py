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
		self.panelInitialized = False
		self.request.settimeout(0.5)	# How quick (in seconds) we will try to send new display and color data to the panel - moderated by looking for the BSY command though.
		dimmed = True	# Dim the panel colors
		color = 0

			# Text strings for display formatting
			# [value]:[format]:[fine]:[Title]:[isLabel]:[label 1 ]:[label 2]:[value2]:[values pair]:[scale]:[scale range low]:[scale range high]:[scale limit low]:[scale limit high]:[img]
		txtStrings = [
			'HWCt#{}=75757',	# Blank out display with empty string
			'HWCt#{}={{CS:1}}32567',	# 16 bit integer
			'HWCt#{}={{CS:3}}-2222',	# 16 bit integer, negative.
			'HWCt#{}={{CR:3}}',	# Float with 2 decimal points. 
			'HWCt#{}={{CR:2}}',	# Integer value in Percent
			]

			# Images
		imageStrings = [
				[	# "D" in "DUMB"
					'HWCg#{}={{CS:1}}',
					'HWCg#{}=0:///////gAAD///////+AAP////////AA/////////gD/////////gP/////////A//////////D///gAP///+P//+AAD///4///4AAD///z///gAAH8=',
					'HWCg#{}=1://7///gAAD///v//+AAAP//////4AAAf//////gAAB//////+AAAH//////4AAAf//////gAAB//////+AAAH//////4AAAf//////gAAD///v//+AA=',
					'HWCg#{}=2:AD///v//+AAAf//8///4AAH///z///gAD///+P/////////w/////////8D/////////gP////////4A////////8AD///////8AAP//////gAAA',
				],
				[	# "D" in "DUMB"
					'HWCg#{}={{CR:1}}',
				],
			]


		rotationIndex = 0
		busy = False;
		lastMillis = 0
		imageOrText = False

		#sendValue = 0;
		HWCtracker = [0] * 256

		while True:
			try:
				# self.request is the TCP socket connected to the client
				self.data = self.request.recv(1024).strip()
			except socket.timeout:	
				pass
					# In case 
				if not busy:
					# self.request.settimeout(50)

					millis = int(round(time.time() * 1000))
					print(millis-lastMillis)
					lastMillis = millis
					
					for a in range (0, 128):
						if HWCtracker[a] > 0:
							line = "HWC#{}={}\n".format(HWCtracker[a],(2+HWCtracker[a]%3) if not dimmed else 5)
							self.request.sendall(line.encode('ascii'))
							
							if dimmed:
								line = "HWCc#{}={}\n".format(HWCtracker[a],color | 0x80)
								color = (color+1)%17
								self.request.sendall(line.encode('ascii'))
					#dimmed = not dimmed	# If full on, the controller crashes - too much power consumed! Maybe LED values need be deminished

					if not imageOrText:
						imageOrText = True
						for a in range (0, 128):
							if HWCtracker[a] > 0:
								txtStrIdx = rotationIndex % len(txtStrings);
								self.request.sendall(txtStrings[txtStrIdx].format(HWCtracker[a]).encode('ascii')+b"\n")
								rotationIndex=rotationIndex+1
					else:
						imageOrText = False
						for a in range (0, 128):
							if HWCtracker[a] > 0:
								imgStrIdx = rotationIndex % len(imageStrings);
								for b in range (0, len(imageStrings[imgStrIdx])):
									self.request.sendall(imageStrings[imgStrIdx][b].format(HWCtracker[a]).encode('ascii')+b"\n")
									#print(imageStrings[imgStrIdx][b].format(HWCtracker[a]).encode('ascii')+b"\n")
								rotationIndex=rotationIndex+1
			else:
				if self.data != b'':
					for line in self.data.split(b"\n"):
						print("Client {} sent: '{}<NL>'".format(self.client_address[0], line.decode('ascii')))

						# just send back the same data, but upper-cased
						if line == b"list":
							self.request.sendall(b"\nActivePanel=1\nlist\n")
							self.panelInitialized = True
							busy = False
							print("- Returned state and assumes panel is now ready")

						if line == b"BSY":
							busy = True

						if line == b"RDY":
							busy = False

						if line == b"ping":
							self.request.sendall(b"ack\n")
							busy = False
							print("- Returned 'ack'")

						# Parse map= and turn on the button in dimmed mode for each. 
						# We could use the data from map to track which HWcs are active on the panel
						match = re.search(r"^map=([0-9]+):([0-9]+)$", line.decode('ascii'))
						if match:
							HWcServer = int(match.group(2));	# Extract the HWc number of the keypress from the match
							HWcClient = int(match.group(1));	# Extract the HWc number of the keypress from the match
							HWCtracker[HWcClient] = HWcServer;

						match = re.search(r"^HWC#([0-9]+)=(.+)$", line.decode('ascii'))
						if match:
							HWcActivated = match.group(1)
							Command = match.group(2)
							for a in range (0, 128):
								if HWCtracker[a] > 0:
									self.request.sendall('HWCt#{}=1|11||||{}|{}'.format(HWCtracker[a],HWcActivated,Command).encode('ascii')+b"\n")

				else:
					print("{} closed".format(self.client_address[0]))
					break



class ThreadedTCPServer(socketserver.ThreadingMixIn, socketserver.TCPServer):
	pass


HOST, PORT = "0.0.0.0", 9923

# Create the server, binding to localhost on port 9999
server = ThreadedTCPServer((HOST, PORT), MyTCPHandler, bind_and_activate=False)
server.allow_reuse_address = True
server.server_bind()
server.server_activate()

# Activate the server; this will keep running until you
# interrupt the program with Ctrl-C
server.serve_forever()

