#!/usr/bin/env python3
import socketserver
import socket
import base64
import re
import xml.dom.minidom
import json

"""
Server for UniSketch TCP Client aka "Raw Panel" mode for SKAARHOJ panels.

- Keeps the connection alive
- Asks for panel topology and writes it to an SVG file "TCPserver_topology-output.svg"


THIS IS the reference implementation on how to handle the topology data! 
Please read the comments below to learn.

kasper@skaarhoj.com
"""

class MyTCPHandler(socketserver.BaseRequestHandler):
	
	def handle(self):
		self.request.settimeout(1)
		busy = False;	# This keeps track of the busy/ready state of the client. We can use this to make sure we are not spamming it with data

			# Flags used to keep track of reading data
		busyReadingSVGdata = False;
		busyReadingJSONdata = False;
		askedForPanelTopology = False;

			# This is where we store the SVG and JSON data we receive.
		SVGdata = ""
		JSONdata = ""

			# In this array we track the relation between physical HWc numbers and the ID they will report as when triggered / receiving feedback.
		HWCtracker = [0] * 256

		while True:
			try:
				# self.request is the TCP socket connected to the client
				self.data = self.request.recv(1024)
			except socket.timeout:
				pass
			else:
				if self.data != b'':
						# This line will print the full packet received before we split it by \n
					#print("Client {} sent: '{}' len: {}\n\n".format(self.client_address[0], self.data.decode('ascii'), len(self.data.decode('ascii'))))
					for line in self.data.split(b"\n"):
						outputline = ""

							# This line will print the line we are going to process now:
						#print("Client {} sent: '{}<NL>' len: {}".format(self.client_address[0], line.decode('ascii'), len(line.decode('ascii'))))

							# List is received from the panel during the handshake. In response to that we send "ActivePanel=1\n" back
						if line == b"list":
							outputline = "\nActivePanel=1\n"
							self.panelInitialized = True
							busy = False
						if line == b"BSY":
							busy = True

						if line == b"RDY":
							busy = False

						if line == b"ping":
							outputline = "ack\n"
							busy = False

						# Parse map= commands - this also triggers that we will ask for Panel Topology
						# We will use the data from map to track which HWcs are active on the panel, and only draw those on the SVG output
						match = re.search(r"^map=([0-9]+):([0-9]+)$", line.decode('ascii'))
						if match:
							HWcServer = int(match.group(2));	# Extract the HWc number of the keypress from the match
							HWcClient = int(match.group(1));	# Extract the HWc number of the keypress from the match
							HWCtracker[HWcClient] = HWcServer;
								# This is where we use the first parsed map command to ask for topology next:
							if not askedForPanelTopology:	
								outputline = "PanelTopology?\n"
								askedForPanelTopology = True


								# Reading SVG data:
								# So, in response to the "PanelTopology?\n" command, we will recieve the base SVG data
								# That data will arrive as a number of TCP packes (1024 bytes long, except the last)
								# We end reading the SVG data by looking for "</svg>" closing tag:
						if not busyReadingSVGdata:
							match = re.search(r"^_panelTopology_svgbase=(.*)$", line.decode('ascii'))
							if match:
								busyReadingSVGdata = True
								SVGdata = match.group(1);

								match = re.search(r"</svg>$", match.group(1))
								if match:
									busyReadingSVGdata = False
									
									print("Received SVG data...")
									#print("SVG data: {}".format(SVGdata))
						else:
							SVGdata = SVGdata + line.decode('ascii');
							match = re.search(r"</svg>$", line.decode('ascii'))
							if match:
								busyReadingSVGdata = False
								
								print("Received SVG data...")
								#print("SVG data: {}".format(SVGdata))


								# Reading JSON data:
								# So, in response to the "PanelTopology?\n" command, we will ALSO recieve a JSON string with the individual HWcs and their type/layout
								# We read that into a string, then start processing...
						if not busyReadingJSONdata:
							match = re.search(r"^_panelTopology_HWC=(.*)$", line.decode('ascii'))
							if match:
								busyReadingJSONdata = True
								JSONdata = match.group(1);
						else:
							JSONdata = JSONdata + line.decode('ascii');
							if len(line.decode('ascii')) != 1024:		# TODO: There are issues that lines can have spaces stripped - is that something JSON does or... 
								busyReadingJSONdata = False

								print("Received JSON data...")

								#print("JSON data: {}".format(JSONdata))

									# OK, so we got all the SVG and JSON data into a string. Now, lets create a JSON object and and XML dom for the SVG:
								json_object = json.loads(JSONdata)
								dom = xml.dom.minidom.parseString(SVGdata)

									# Next, we want to add SVG elements to the base SVG to draw up the hardware components
									# The JSON object contains a key on root label with hardware components, "HWc" and a key, "typeIndex" with type data
									# Each hardware component will have an "x" and "y" position, then a text, "txt" label and finally a "type" index. That type number is a reference into the "typeIndex" for information about how to draw this item
									# Notice: Stringly, the type index only reveals how to _draw_ the element, not whether it's really a button, encoder, analog slider, display or intensity component (like a joystick) although that could generally be inferred
									# The typeIndex contains a width and height when an item is drawn as a rectangle. If the height is omitted, the width is radius of a circle. In addition each type may contain sub elements.
									# Now, lets iterate over HWC (HardWare Components) in the JSON object:
								i = 0;
								for HWcDef in json_object["HWc"]:
									if (HWCtracker[(i+1)]):	# Only add elements to the SVG if they were found in the "map=" sent from the panel:
										typeDef = json_object["typeIndex"][str(HWcDef['type'])]	# This gets us the type data for the particular HWc we are processing:

											# Now, lets create the basic SVG element for the HWc - either a rectangle or a circle, based on whether "h" exists or not:
											# Notice, how we find the width and height information by looking into the definition of the HWc type as found in "typeDef"
										if 'h' in typeDef:	# If height exists, it's a rectangle...
											newHWc = dom.createElement("rect")
											newHWc.setAttribute("x", str(round(HWcDef['x']-typeDef['w']/2)))
											newHWc.setAttribute("y", str(round(HWcDef['y']-typeDef['h']/2)))
											newHWc.setAttribute("width", str(typeDef['w']))
											newHWc.setAttribute("height", str(typeDef['h']))
											newHWc.setAttribute("rx", str(10))
											newHWc.setAttribute("ry", str(10))
										else:
											newHWc = dom.createElement("circle")
											newHWc.setAttribute("cx", str(HWcDef['x']))
											newHWc.setAttribute("cy", str(HWcDef['y']))
											newHWc.setAttribute("r", str(round(typeDef['w']/2)))

											# There are some common formatting regardless of rectangle / circle: Like fill and stroke color and stroke width.
										newHWc.setAttribute("fill", "#dddddd")
										newHWc.setAttribute("stroke", "#000")
										newHWc.setAttribute("stroke-width", "2")
										newHWc.setAttribute("id", "HWc"+str(i+1))	# Also, lets add an id to the element! This is not mandatory, but you are likely to want this to program some interaction with the SVG
											# Att this point you may want to add more attributes? Most likely your application will want to make the element clickable so you can configure actions for it...

											# Good, the SVG element is ready, lets add it to the base SVG:
										dom.childNodes[0].appendChild(newHWc)	


											# Some types has additional SVG elements associated with them - like the black display above a four-way button, the handle of a T-bar, the leds in an LED bar etc.
											# These elements are found in the "sub" key - if it exists
											# If an attribute for sub is prefixed with "_" it has a special meaning as listed below. 
											# In particular x and y are _offsets_ added to the HWcs x/y coordinates
										if ('sub' in typeDef.keys()):
											for subEl in typeDef['sub']:
												if (subEl['_']=='r'):	# Rectangle, set up required attributes:
													newHWc = dom.createElement("rect")
													newHWc.setAttribute("x", str(HWcDef['x']+subEl['_x']))
													newHWc.setAttribute("y", str(HWcDef['y']+subEl['_y']))
													newHWc.setAttribute("width", str(subEl['_w']))
													newHWc.setAttribute("height", str(subEl['_h']))
												if (subEl['_']=='c'):	# Circle, set up required attributes:
													newHWc = dom.createElement("circle")
													newHWc.setAttribute("cx", str(HWcDef['x']+subEl['_x']))
													newHWc.setAttribute("cy", str(HWcDef['y']+subEl['_y']))

													# Set up all other attributes not prefixed with "_"
												for attrib in subEl:
													if (attrib[0:1]!='_'):
														newHWc.setAttribute(attrib, str(subEl[attrib]))

													# ... and finally paint them with some default styles:
												newHWc.setAttribute("fill", "#cccccc")
												newHWc.setAttribute("stroke", "#666")
												newHWc.setAttribute("stroke-width", "1")

												dom.childNodes[0].appendChild(newHWc)	


											# Rendering text label: 
											# So, on top of the main element, we want to render the text label
											# Text labels can be split into two lines by a vertical pipe, so if that is found, we will take that into account:
										sp = HWcDef["txt"].split("|")
										cnt = len(sp)
										if (cnt > 1 and len(sp[1]) > 0):
											cnt = 2
										else:
											cnt = 1

										for tl in range(0, cnt):
											newHWc = dom.createElement("text")
											newHWc.setAttribute("x", str(HWcDef['x']))
											newHWc.setAttribute("y", str(HWcDef['y'] + 43 + tl * 60 - (cnt * 60 / 2)))
											newHWc.setAttribute("text-anchor", "middle")
											newHWc.setAttribute("fill", "#000")
											newHWc.setAttribute("font-weight", "bold")
											newHWc.setAttribute("font-size", "35")
											newHWc.setAttribute("font-family", "sans-serif")
											newHWc.appendChild(dom.createTextNode(sp[tl]))
											dom.childNodes[0].appendChild(newHWc)

											# Finally, we will add the number label to upper left corner. This is the HWc ID on the panel!
											# In parenthesis we add the HWc ID that the panel will _actually_ send for this HWc in case it differs from the physical HWc id
										newHWc = dom.createElement("text")
										newHWc.setAttribute("x", str(HWcDef['x']-typeDef['w']/2+4))
										newHWc.setAttribute("y", str(HWcDef['y']-(typeDef['h'] if 'h' in typeDef else typeDef['w'])/2 + 20))
										newHWc.setAttribute("fill", "#000")
										newHWc.setAttribute("font-size", "20")
										newHWc.setAttribute("font-family", "sans-serif")
										newHWc.setAttribute("stroke", "#dddddd")
										newHWc.setAttribute("stroke-width", "6px")
										newHWc.setAttribute("paint-order", "stroke")
										newHWc.appendChild(dom.createTextNode(str(i + 1) + str("" if HWCtracker[i + 1] == i+1 else " ({})".format(HWCtracker[i + 1]))))
										dom.childNodes[0].appendChild(newHWc)
									i=i+1

									# Great, we're done creating the SVG, we can now write it to the output, in this case a file:
								print("Writing SVG file to 'TCPserver_topology-output.svg'...")

								text_file = open("TCPserver_topology-output.svg", "w")
								text_file.write(dom.toprettyxml())
								text_file.close()


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
