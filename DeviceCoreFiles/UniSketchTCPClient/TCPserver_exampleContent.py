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

			# Images
		imageStrings = [
				[	# Button: Stop
					'HWCg#{}=0:AAAAAAAAAAAP////////8DAAAAAAAAAMJ////////+RP////////8l//+AAAf//6X//wAAA///pf//AAAD//+l//8AAAP//6X//wAAA///pf//AAAD8=',
					'HWCg#{}=1://pf//AAAD//+l//8AAAP//6X//wAAA///pf//AAAD//+l//8AAAP//6X//wAAA///pf//AAAD//+l//8AAAP//6X//wAAA///pf//AAAD//+l//8AA=',
					'HWCg#{}=2:AD//+l//8AAAP//6X//wAAA///pf//AAAD//+l//8AAAP//6X//4AAB///pP////////8if////////kMAAAAAAAAAwP////////8AAAAAAAAAAA',
				],
				[	# Button: Pause
					'HWCg#{}=0:AAAAAAAAAAAP////////8DAAAAAAAAAMJ////////+RP////////8l///4/x///6X///B+D///pf//4H4H//+l///gfgf//6X//+B+B///pf//4H4H8=',
					'HWCg#{}=1://pf//4H4H//+l///gfgf//6X//+B+B///pf//4H4H//+l///gfgf//6X//+B+B///pf//4H4H//+l///gfgf//6X//+B+B///pf//4H4H//+l///gc=',
					'HWCg#{}=2:4H//+l///gfgf//6X//+B+B///pf//4H4H//+l///wfg///6X///j/H///pP////////8if////////kMAAAAAAAAAwP////////8AAAAAAAAAAA',
				],
				[	# Button: Repeat
					'HWCg#{}=0:AAAAAAAAAAAP////////8DAAAAAAAAAMJ////////+RP////////8l/////////6X////h////pf///wA///+l///+AB///6X///weH///pf//+H+/8=',
					'HWCg#{}=1://pf//8P////+l///x//f//6X//+H/8///pf//4//h//+l///j/8H//6X//+P/wP//pf//4/+A//+l///h/4B//6X///H/4///pf//8P/H//+l///4c=',
					'HWCg#{}=2:+H//+l///8Hg///6X///4AH///pf///wA///+l////4f///6X/////////pP////////8if////////kMAAAAAAAAAwP////////8AAAAAAAAAAA',
				],
				[	# Button: Play
					'HWCg#{}=0:AAAAAAAAAAAP////////8DAAAAAAAAAMJ////////+RP////////8l///5/////6X///B/////pf//8D////+l///wD////6X///AD////pf//8AH/8=',
					'HWCg#{}=1://pf//8AB///+l///wAB///6X///AAB///pf//8AAD//+l///wAAD//6X///AAAP//pf//8AAD//+l///wAAf//6X///AAH///pf//8AB///+l///wA=',
					'HWCg#{}=2:H///+l///wA////6X///AP////pf//8D////+l///wf////6X///n/////pP////////8if////////kMAAAAAAAAAwP////////8AAAAAAAAAAA',
				],
				[	# Camera: 1 - Audience
					'HWCg#{}=0:AAAAAAAAAAAAAAAAAAAAAAAAAAAAHgAAAAAfAAA/AAAAAD+DwD8+AAAAP4fjP38AB+A/B+fffwAH8D4H789/AAf4PgPfz/8AB/j+B9///gAH//8f//8=',
					'HWCg#{}=1:/gAH////////gA/////////wH/////////wf/////////D/////////8P/////////w//////////j/////////+P/////////4//////////j////8=',
					'HWCg#{}=2://///x//////////H/////////4f/////////B/////////4/////////////zN0Rz8////+7SXa37////7tVFrfv////uF1xh+/////LXRa37//',
				],
				[	# Camera: 2 - Close up
					'HWCg#{}=0:AAAAAAAAAAAAAAAAAAAAAAAAAAP4AAAAAAAAP/8AAAAAAAB//4AAAAAAAf//wAAAAAAB///AAAAAAAP//+AAAAAAA///4AAAAAAD///gAAAAAAP//+A=',
					'HWCg#{}=1:AAAAAAP//+AAAAAAA///4AAAAAAB///AAAAAAAD//8AAAAAAAP//gAAAAAAAf/8AAAAAAAAf/gAAAAAAAA/8AAAAAAAAD/wAAAAAAAA//gAAAAAAA/8=',
					'HWCg#{}=2://AAAAAAf////4AAAAP/////+AAAP///////AAD////////A/////////////mbojn4////92ku1v9////3aqLW/P////cLrjD7////+Wui1vh//',
				],
				[	# Camera: 3 - Half total
					'HWCg#{}=0:AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAB////8AAHAAH////wAA8AAf////AADwAB////8AAHACH////wBh/AYf////AGf/HB//8=',
					'HWCg#{}=1://AD//+B////8AP//gH////wA//gAf////AB/+AB////8AA/4AH////wAD/wAf////AAf/AB////8AB/8AH////wAH/wAAAAAAAAf8AAAAAAAAA/wAA=',
					'HWCg#{}=2:AAAAAAA/wAAAAAAAAD/AAAAAAAAAO8AAAAAAAAA7wAAAAAAA/////////////M3RHPx////7tJdrf7////u1UWt+f///+4XXGH+////8tdFrfH//',
				],
				[	# Camera: Computer
					'HWCg#{}=0:AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAIAAAAAAAGABwAAAAAAA4AHAMA4AAAHgEcBwDgAAAeAxwHAM=',
					'HWCg#{}=1:gAAAeYxwHAOAAAB7jnAcY4QAAHuOcZxjhAAAe45znHOEAAB7jnOcc4QAAHuOc5xzhAAAe55znHOEAAB///////wAAH//////+AAAf//////wAAB///8=',
					'HWCg#{}=2:///gAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA/////////////M3RtER////7tJa23b////u1VrbFv///+7XRttx////8zdfOxb//',
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

						# 1-4:
					self.request.sendall("HWC#1=5\n".encode('ascii'))
					self.request.sendall("HWCc#1={}\n".format(7|128).encode('ascii'))
					self.request.sendall("HWCt#1=|||Bank A|0|Run Logo||0".encode('ascii')+b"\n")

					self.request.sendall("HWC#2=5\n".encode('ascii'))
					self.request.sendall("HWCc#2={}\n".format(7|128).encode('ascii'))
					self.request.sendall("HWCt#2=|||Bank A|0|Stinger|+Logo|0".encode('ascii')+b"\n")

					self.request.sendall("HWC#3=5\n".encode('ascii'))
					self.request.sendall("HWCc#3={}\n".format(7|128).encode('ascii'))
					self.request.sendall("HWCt#3=|||Bank A|0|Stinger|No logo|0".encode('ascii')+b"\n")

					self.request.sendall("HWC#4=5\n".encode('ascii'))
					self.request.sendall("HWCc#4={}\n".format(7|128).encode('ascii'))
					self.request.sendall("HWCt#4=|||Bank A|0|Chroma|Keyer In|0".encode('ascii')+b"\n")

						# 13-16:
					self.request.sendall("HWC#13=5\n".encode('ascii'))
					self.request.sendall("HWCc#13={}\n".format(7|128).encode('ascii'))
					self.request.sendall("HWCt#13=|||Bank B|0|Set Mix|Transition|0".encode('ascii')+b"\n")

					self.request.sendall("HWC#14=4\n".encode('ascii'))
					self.request.sendall("HWCc#14={}\n".format(7|128).encode('ascii'))
					self.request.sendall("HWCt#14=|||Bank B|0|Host Mic|Toggle|0".encode('ascii')+b"\n")

					self.request.sendall("HWC#15=5\n".encode('ascii'))
					self.request.sendall("HWCc#15={}\n".format(7|128).encode('ascii'))
					self.request.sendall("HWCt#15=|||Bank B|0|Music|Toggle|0".encode('ascii')+b"\n")

					self.request.sendall("HWC#16=5\n".encode('ascii'))
					self.request.sendall("HWCc#16={}\n".format(7|128).encode('ascii'))
					self.request.sendall("HWCt#16=|||Bank B|0|Music|Fade out|0".encode('ascii')+b"\n")

						# 25-28:
					self.request.sendall("HWC#25=5\n".encode('ascii'))
					self.request.sendall("HWCc#25={}\n".format(7|128).encode('ascii'))
					self.request.sendall("HWCt#25=|||Bank C|0|MIC 1".encode('ascii')+b"\n")

					self.request.sendall("HWC#26=5\n".encode('ascii'))
					self.request.sendall("HWCc#26={}\n".format(7|128).encode('ascii'))
					self.request.sendall("HWCt#26=|||Bank C|0|MIC 2".encode('ascii')+b"\n")

					self.request.sendall("HWC#27=5\n".encode('ascii'))
					self.request.sendall("HWCc#27={}\n".format(7|128).encode('ascii'))
					self.request.sendall("HWCt#27=|||Bank C|0|MIC 3".encode('ascii')+b"\n")

					self.request.sendall("HWC#28=5\n".encode('ascii'))
					self.request.sendall("HWCc#28={}\n".format(7|128).encode('ascii'))
					self.request.sendall("HWCt#28=|||Bank C|0|MIC 4".encode('ascii')+b"\n")


						# 5-8:
					self.request.sendall("HWC#5=5\n".encode('ascii'))
					self.request.sendall("HWCc#5={}\n".format(14|128).encode('ascii'))
					self.request.sendall(imageStrings[4][0].format(5).encode('ascii')+b"\n")
					self.request.sendall(imageStrings[4][1].format(5).encode('ascii')+b"\n")
					self.request.sendall(imageStrings[4][2].format(5).encode('ascii')+b"\n")

					self.request.sendall("HWC#6=5\n".encode('ascii'))
					self.request.sendall("HWCc#6={}\n".format(14|128).encode('ascii'))
					self.request.sendall(imageStrings[5][0].format(6).encode('ascii')+b"\n")
					self.request.sendall(imageStrings[5][1].format(6).encode('ascii')+b"\n")
					self.request.sendall(imageStrings[5][2].format(6).encode('ascii')+b"\n")

					self.request.sendall("HWC#7=5\n".encode('ascii'))
					self.request.sendall("HWCc#7={}\n".format(14|128).encode('ascii'))
					self.request.sendall(imageStrings[6][0].format(7).encode('ascii')+b"\n")
					self.request.sendall(imageStrings[6][1].format(7).encode('ascii')+b"\n")
					self.request.sendall(imageStrings[6][2].format(7).encode('ascii')+b"\n")

					self.request.sendall("HWC#8=5\n".encode('ascii'))
					self.request.sendall("HWCc#8={}\n".format(14|128).encode('ascii'))
					self.request.sendall(imageStrings[7][0].format(8).encode('ascii')+b"\n")
					self.request.sendall(imageStrings[7][1].format(8).encode('ascii')+b"\n")
					self.request.sendall(imageStrings[7][2].format(8).encode('ascii')+b"\n")

						# 17-20:
					self.request.sendall("HWC#17=5\n".encode('ascii'))
					self.request.sendall("HWCc#17={}\n".format(14|128).encode('ascii'))
					self.request.sendall("HWCt#17=|||Source|0|Media".encode('ascii')+b"\n")

					self.request.sendall("HWC#18=5\n".encode('ascii'))
					self.request.sendall("HWCc#18={}\n".format(14|128).encode('ascii'))
					self.request.sendall("HWCt#18=|||Source|0|Sat1".encode('ascii')+b"\n")

					self.request.sendall("HWC#19=5\n".encode('ascii'))
					self.request.sendall("HWCc#19={}\n".format(14|128).encode('ascii'))
					self.request.sendall("HWCt#19=|||Source|0|Feed".encode('ascii')+b"\n")

					self.request.sendall("HWC#20=5\n".encode('ascii'))
					self.request.sendall("HWCc#20={}\n".format(14|128).encode('ascii'))
					self.request.sendall("HWCt#20=|||Source|0|Computer|Mac|0".encode('ascii')+b"\n")


						# 9-12:
					self.request.sendall("HWC#9=5\n".encode('ascii'))
					self.request.sendall("HWCc#9={}\n".format(8|128).encode('ascii'))
					self.request.sendall(imageStrings[0][0].format(9).encode('ascii')+b"\n")
					self.request.sendall(imageStrings[0][1].format(9).encode('ascii')+b"\n")
					self.request.sendall(imageStrings[0][2].format(9).encode('ascii')+b"\n")

					self.request.sendall("HWC#10=4\n".encode('ascii'))
					self.request.sendall("HWCc#10={}\n".format(8|128).encode('ascii'))
					self.request.sendall(imageStrings[1][0].format(10).encode('ascii')+b"\n")
					self.request.sendall(imageStrings[1][1].format(10).encode('ascii')+b"\n")
					self.request.sendall(imageStrings[1][2].format(10).encode('ascii')+b"\n")

					self.request.sendall("HWC#11=5\n".encode('ascii'))
					self.request.sendall("HWCc#11={}\n".format(8|128).encode('ascii'))
					self.request.sendall(imageStrings[2][0].format(11).encode('ascii')+b"\n")
					self.request.sendall(imageStrings[2][1].format(11).encode('ascii')+b"\n")
					self.request.sendall(imageStrings[2][2].format(11).encode('ascii')+b"\n")

					self.request.sendall("HWC#12=5\n".encode('ascii'))
					self.request.sendall("HWCc#12={}\n".format(8|128).encode('ascii'))
					self.request.sendall(imageStrings[3][0].format(12).encode('ascii')+b"\n")
					self.request.sendall(imageStrings[3][1].format(12).encode('ascii')+b"\n")
					self.request.sendall(imageStrings[3][2].format(12).encode('ascii')+b"\n")


						# 17-20:
					self.request.sendall("HWC#21=5\n".encode('ascii'))
					self.request.sendall("HWCc#21={}\n".format(16|128).encode('ascii'))
					self.request.sendall("HWCt#21=|||DSK 1|1|OFF".encode('ascii')+b"\n")

					self.request.sendall("HWC#22=5\n".encode('ascii'))
					self.request.sendall("HWCc#22={}\n".format(16|128).encode('ascii'))
					self.request.sendall("HWCt#22=|||DSK 2|1|OFF".encode('ascii')+b"\n")

					self.request.sendall("HWC#23=4\n".encode('ascii'))
					self.request.sendall("HWCc#23={}\n".format(16|128).encode('ascii'))
					self.request.sendall("HWCt#23=|||DSK 3|1|ON".encode('ascii')+b"\n")

					self.request.sendall("HWC#24=4\n".encode('ascii'))
					self.request.sendall("HWCc#24={}\n".format(16|128).encode('ascii'))
					self.request.sendall("HWCt#24=|||DSK 4|1|ON".encode('ascii')+b"\n")


						# 25-28:
					self.request.sendall("HWC#29=5\n".encode('ascii'))
					self.request.sendall("HWCc#29={}\n".format(12|128).encode('ascii'))
					self.request.sendall("HWCt#29=|||Mode|1|Macros".encode('ascii')+b"\n")

					self.request.sendall("HWC#30=5\n".encode('ascii'))
					self.request.sendall("HWCc#30={}\n".format(12|128).encode('ascii'))
					self.request.sendall("HWCt#30=|||Mode|1|AUX 1+2".encode('ascii')+b"\n")

					self.request.sendall("HWC#31=5\n".encode('ascii'))
					self.request.sendall("HWCc#31={}\n".format(12|128).encode('ascii'))
					self.request.sendall("HWCt#31=|||Mode|1|Mediaplayr".encode('ascii')+b"\n")

					self.request.sendall("HWC#32=5\n".encode('ascii'))
					self.request.sendall("HWCc#32={}\n".format(12|128).encode('ascii'))
					self.request.sendall("HWCt#32=|||Mode|1|Router".encode('ascii')+b"\n")

					self.request.sendall("HWC#33=5\n".encode('ascii'))
					self.request.sendall("HWCc#33={}\n".format(3|128).encode('ascii'))
					self.request.sendall("HWCt#33=|||Navigate|0|Down".encode('ascii')+b"\n")

					self.request.sendall("HWC#34=5\n".encode('ascii'))
					self.request.sendall("HWCc#34={}\n".format(3|128).encode('ascii'))
					self.request.sendall("HWCt#34=|||Navigate|0|UP".encode('ascii')+b"\n")

					self.request.sendall("HWC#35=5\n".encode('ascii'))
					self.request.sendall("HWCc#35={}\n".format(2|128).encode('ascii'))
					self.request.sendall("HWCt#35=|||Active".encode('ascii')+b"\n")

					self.request.sendall("HWC#36=5\n".encode('ascii'))
					self.request.sendall("HWCc#36={}\n".format(12|128).encode('ascii'))
					self.request.sendall("HWCt#36=|||SHIFT|0".encode('ascii')+b"\n")


						# 37-39
					self.request.sendall("HWCt#37=|||Macro Banks|1".encode('ascii')+b"\n")
					self.request.sendall("HWCt#38=|||Auxiliary Bus 7|1".encode('ascii')+b"\n")
					self.request.sendall("HWCt#39=|||Deck Control|0".encode('ascii')+b"\n")

			else:
				if self.data != b'':
					for line in self.data.split(b"\n"):
						print("Client {} sent: '{}<NL>'".format(self.client_address[0], line.decode('ascii')))

						# just send back the same data, but upper-cased
						if line == b"list":
							self.request.sendall(b"ActivePanel=1\n")
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

