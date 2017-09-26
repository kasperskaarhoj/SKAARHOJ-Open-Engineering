#!/usr/bin/env python3
import socketserver
import socket
import base64
import re
import time

"""
An example of serving text and graphics to UniSketch TCP Client

- Keeps the connection alive
- Turns on light in reported HWcs
- Receives button presses and cycles graphics
- Can hold down button to quickly cycle graphics
"""

class MyTCPHandler(socketserver.BaseRequestHandler):
	
	def handle(self):
		self.request.settimeout(0.1)
		busy = False;	# This keeps track of the busy/ready state of the client. We can use this to make sure we are not spamming it with data
	

			# Text strings for display formatting
			# [value]:[format]:[fine]:[Title]:[isLabel]:[label 1 ]:[label 2]:[value2]:[values pair]:[scale]:[scale range low]:[scale range high]:[scale limit low]:[scale limit high]:[img]
		txtStrings = [
			'HWCt#{}=',	# Blank out display with empty string
			'HWCt#{}=32767',	# 16 bit integer
			'HWCt#{}=-9999',	# 16 bit integer, negative.
			'HWCt#{}=32767|1||Float2',	# Float with 2 decimal points. 
			'HWCt#{}=299|2||Percent',	# Integer value in Percent
			'HWCt#{}=999|3||dB',	# Integer value in dB
			'HWCt#{}=1234|4||Frames',	# Integer in frames
			'HWCt#{}=999|5||Reciproc',	# Reciproc value of integer
			'HWCt#{}=9999|6||Kelvin',		# Kelvin
			'HWCt#{}=9999|7||[Empty!]',	# format 7 = empty!
			'HWCt#{}=-3276|8||Float3',	# Float with 3 decimal points, optimized for 5 char wide space. Op to +/-9999
			'HWCt#{}=||1|[Fine]|1',	# Fine marker set, title as "label"
			'HWCt#{}=||1|Title String', # no value, just title string (and with "fine" indicator)
			'HWCt#{}=|||Title String|1',	# Title string as label (no "bar" in title)
			'HWCt#{}=|||Title string|1|Text1Label',	# Text1label - 5 chars in big font
			'HWCt#{}=|||Title string|1|Text1Label||0',	# Adding the zero (value 2) means we will print two lines and the text label will be in smaller printing
			'HWCt#{}=|||Title string|1|Text1Label|Text2Label',	# Printing two labels - automatically the size is reduced
			'HWCt#{}=|||Title string|1||Text2Label',	# Printing only the second line - automatically the size is reduced
			'HWCt#{}=|||||Text1Label',	# Text1label - 5 chars in big font
			'HWCt#{}=|||||Text1Label||0',	# Adding the zero (value 2) means we will print two lines and the text label will be in smaller printing
			'HWCt#{}=|||||Text1Label|Text2Label',	# Printing two labels - automatically the size is reduced
			'HWCt#{}=||||||Text2Label',	# Printing only the second line - automatically the size is reduced
			'HWCt#{}=123|||Title string|1|Val1:|Val2:|456',	# First and second value is printed in small characters with prefix labels Val1 and Val2

			'HWCt#{}=-1234|1||Coords:||x:|y:|4567|2',	# box type 2
			'HWCt#{}=-1234|1||Coords:||x:|y:|4567|3',	# box type 3
			'HWCt#{}=-1234|1||Coords:||x:|y:|4567|4',	# box type 4

			'HWCt#{}=-500|1||Coords:||||||1|-1000|1000|-700|700|1',	# 1=fill scale
			'HWCt#{}=-500|1||Coords:||||||2|-1000|1000|-700|700|2',	# 2=other scale type
			'HWCt#{}=||||||||||||||1',	# 1: First memory image
			]

			# Images
		imageStrings = [
				[	# "D" in "DUMB"
					'HWCg#{}=0:///////gAAD///////+AAP////////AA/////////gD/////////gP/////////A//////////D///gAP///+P//+AAD///4///4AAD///z///gAAH8=',
					'HWCg#{}=1://7///gAAD///v//+AAAP//////4AAAf//////gAAB//////+AAAH//////4AAAf//////gAAB//////+AAAH//////4AAAf//////gAAD///v//+AA=',
					'HWCg#{}=2:AD///v//+AAAf//8///4AAH///z///gAD///+P/////////w/////////8D/////////gP////////4A////////8AD///////8AAP//////gAAA',
				],
				[	# "U" in "DUMB"
					'HWCg#{}=0:AAB//gAH/+AAAH/+AAf/4AAAf/4AB//gAAB//gAH/+AAAH/+AAf/4AAAf/4AB//gAAB//gAH/+AAAH/+AAf/4AAAf/4AB//gAAB//gAH/+AAAH/+AAc=',
					'HWCg#{}=1:/+AAAH/+AAf/4AAAf/4AB//gAAB//gAH/+AAAH/+AAf/4AAAf/4AB//gAAB//gAH/+AAAH/+AAf/4AAAf/4AB//gAAB//gAH/+AAAH/+AAf/4AAAf/4=',
					'HWCg#{}=2:AAf/4AAAP/8AD//AAAA//4Af/8AAAD//4H//wAAAH/////+AAAAP/////wAAAAf////+AAAAA/////wAAAAA////8AAAAAA////AAAAAAAP//AAA',
				],
				[	# "M" in "DUMB"
					'HWCg#{}=0:D/8P/8B//AAP/x//4P//AA//P//x//+AD/9//////4AP////////wA/////////AD////////+AP////////4A//4f//D//gD//A//4H/+AP/8B//gM=',
					'HWCg#{}=1:/+AP/4B//AP/4A//gH/8A//gD/+Af/wD/+AP/4B//AP/4A//gH/8A//gD/+Af/wD/+AP/4B//AP/4A//gH/8A//gD/+Af/wD/+AP/4B//AP/4A//gH8=',
					'HWCg#{}=2:/AP/4A//gH/8A//gD/+Af/wD/+AP/4B//AP/4A//gH/8A//gD/+Af/wD/+AP/4B//AP/4A//gH/8A//gAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA',
				],
				[	# "B" in "DUMB"
					'HWCg#{}=0:AAAAAAf//gAAD/wAH///wAD//4A////gA///4H////gH///4/////B////3////8H/////////4//////////j/////////+f/////////5//////4M=',
					'HWCg#{}=1://9//////gD//3//wP/+AP//f/+Af/wAf/9//wA//AB//3//AD/8AH//f/8AP/wAf/9//wA//AB//3//AD/8AH//f/8AP/wAf/9//wA//AB//3//AD8=',
					'HWCg#{}=2:/AB//3//////////f/////////9//////////3//////////f/////////9//////////3//////////f/////////9//////////3//////////',
				],
				[	# Panels 1
					'HWCg#{}=0:H/////////9wAAAAAAAAAEAAAAAAAAAAx////AAAAACH////gAAAAIf////AAAAAh////+AAAACH////8AAAAIf////wAAAAh/+B//gAAACH/4B/+AA=',
					'HWCg#{}=1://6H/4B/+Af//4f/gH/4H///h/+Af/g///+H/4H/+D///4f////wf/g/h/////AD8B+H////4AAAf4f////AAB//h////wAH//+H///4AD///4f/gAA=',
					'HWCg#{}=2:AH/+H4f/gAAA/+Afh/+AAAD/4H+H/4AAAP///4f/gAAA////h/+AAAB//++H/4AAAD//j8f/gAAAB/wHQAAAAAAAAABwAAAAAAAAAB//////////',
				],
				[	# Panels 2
					'HWCg#{}=0://////////8AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAD/wP8AA=',
					'HWCg#{}=1:AH/AD/x//AAH//AP/P/+AB//+A////8AP//4D////4B///wP////gP/g/A//h/+A/8D8D/8D/4H///wP/gP/gf///A/+A/+B///8D/4D/4H///wP/gM=',
					'HWCg#{}=2:/4H/wPwP/gP/gP/g/A/+A/+A//D8D/4D/4B///wP/gP/gD///A/+A/+AH//8D/4D/4AH//4P/gP/gAD/AAAAAAAAAAAAAAAAAAAAAP//////////',
				],
				[	# Panels 3
					'HWCg#{}=0://////////8AAAAAAAAAAAAAAAAAAAAAAAAP/gAAAAAAAA/+AAAAAAAAD/4AAAAAAAAP/gAAAAAAAA/+AAAAAAAAD/4AAAAAAAAP/gAAAAD4AA/+AB8=',
					'HWCg#{}=1:/4D/AA/+Af//4P/AD/4D///4/+AP/gf///z/8A/+D/4f/j/4D/4P/g/AH/gP/g//AAD//A/+B///AP/8D/4H///w//wP/gH///z//A/+AH///gAAD/4=',
					'HWCg#{}=2:AAD//wAAD/4AAAf/f/gP/gD8B///8A/+H/4P///gD/4P///+/4AP/gf///z/AA/+Af//8PAAD/4AP/8AAAAAAAAAAAAAAAAAAAAAAP//////////',
				],
				[	# Panels 4
					'HWCg#{}=0://////////gAAAAAAB/8DgAAAAAA/44CA/+AAA/gBwMD/4AAHwADAQP/gAAYAAGBA/+AABgAAYED/4AAHAABgQP/gAAPwAOBA/+AAAOAAwED/4AABwA=',
					'HWCg#{}=1:BwED/4AADgAGAQH/AAAcAA4BAf8AADgADAEB/wAAMPAcAQH/AABx/DgBAf8AAHOfcAEB/wAAPwfgAQD+AAAcAYABAP4AAAAAAAEAAAAf4AAAAQAAAH8=',
					'HWCg#{}=2:+AAAAQP/gMAOAAABA/+BgAYAAAED/4GAAgAAAQP/gYAGAAABA/+B4B4AAAED/4O/9wAAAQP/n4AH4AADAAAfgAfgAAIAAD/AD/AADv/////////4',
				],
				[	# INT in Intelligent
					'HWCg#{}=0:AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA////////////////////////////////AAAAAAAAAAAAAAAAAAAAAD+B+APw///4P4H8A/D///g/gf4D8P8=',
					'HWCg#{}=1://g/gf+D8P//+D+B/8PwAfwAP4H/4/AB/AA/gf/z8AH8AD+B//vwAfwAP4H7//AB/AA/gfn/8AH8AD+B+P/wAfwAP4H4f/AB/AA/gfg/8AH8AD+B+B8=',
					'HWCg#{}=2:8AH8AD+B+A/wAfwAP4H4B/AB/AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAD///////////////////////////////8AAAAAAAAAAAAAAAAAAAAA',
				],
				[	# ELL in Intelligent
					'HWCg#{}=0:AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA////////////////////////////////AAAAAAAAAAAAAAAAAAAAAH//8H8AA/gAf//wfwAD+AB///B/AAM=',
					'HWCg#{}=1:+AB///B/AAP4AH8AAH8AA/gAfwAAfwAD+AB//+B/AAP4AH//4H8AA/gAf//gfwAD+AB//+B/AAP4AH8AAH8AA/gAfwAAfwAD+AB///B//8P//n//8H8=',
					'HWCg#{}=2:/8P//n//8H//w//+f//wf//D//4AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAD///////////////////////////////8AAAAAAAAAAAAAAAAAAAAA',
				],
				[	# IGE in Intelligent
					'HWCg#{}=0:AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA////////////////////////////////AAAAAAAAAAAAAAAAAAAAAA/gAf/AD//+D+AP//AP//4P4B///A8=',
					'HWCg#{}=1://4P4D///A///g/gf4H+D+AAD+D/AAAP4AAP4P4AAA///A/g/g/+D//8D+D+D/4P//wP4P4P/g///A/g/w/+D+AAD+B/gP4P4AAP4D///g///g/gH/8=',
					'HWCg#{}=2:/A///g/gD//wD//+D+AB/4AP//4AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAD///////////////////////////////8AAAAAAAAAAAAAAAAAAAAA',
				],
				[	# NT in Intelligent
					'HWCg#{}=0:AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA////////////////////////////////AAAAAAAAAAAAAAAAAAAAAB+APwf//8AAH8A/B///wAAf4D8H//8=',
					'HWCg#{}=1:wAAf+D8H///AAB/8PwAP4AAAH/4/AA/gAAAf/z8AD+AAAB//vwAP4AAAH7//AA/gAAAfn/8AD+AAAB+P/wAP4AAAH4f/AA/gAAAfg/8AD+AAAB+B/wA=',
					'HWCg#{}=2:D+AAAB+A/wAP4AAAH4B/AA/gAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAD///////////////////////////////8AAAAAAAAAAAAAAAAAAAAA',
				],
				[	# "S" in "WAYS"
					'HWCg#{}=0:AAAAAAAAAAAAAAAAAAAAAB8AAAAAAAAHP8AAAMAAAB9/4AAB4AAAP3HwAAP4AAB/8fgAA/wAAP/h/AAB/wAB/+HcAAB/gAH/45wAAD/gAcDjjgAAH/g=',
					'HWCg#{}=1:AAHDjgAAH/wAD8OOf////gAfw45/////AH/Djn////+A/8OOP////4H/w44AD///w//DjgAf///D/8OOAB///8f/w44AH///x//DjgAP///H/+OcAAc=',
					'HWCg#{}=2:///H/+OcAA///8P/4fwAD///w//h+AAP//+A//D4AAf//4A/c/AAAf//AAF/wAAD//4AAD+AAAf//AAAHgAAB//4AAAAAAAD/+AAAAAAAAAAAAAA',
				],
				[	# "Y" in "WAYS"
					'HWCg#{}=0:A//+A///gAD////h///AAf////h///AH/////B//+B/////8B//+f//3//wD/////wP//AD////+P//4AH///wf///AAP///P///4AAf//////+AAAc=',
					'HWCg#{}=1://////wAAAP/////wAAAAf////gAAAAA////gP//AAD///wB//8AAP//+AH//wAA///8D//+AAD///////4AAP///////AAA///////4AAD///////A=',
					'HWCg#{}=2:AAD//////+AAAP//////gAAA//////wAAAD/////gAAAAP///wAAAAAAA/8AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA',
				],
				[	# "A" in "WAYS"
					'HWCg#{}=0:AAAAAAAAAAD/+AAAAAAAAP//wAAAAAAA//+AH//wAAD//gAf//+AD//8AD///8AP//AAf///wAf/4AB////AB/+AAP///+AH/wAB////4Af8AAH///8=',
					'HWCg#{}=1:8AP4AAP////wA+AAB/////gDwAAH////+AGAAA//+f/4AQAAH//5//wBAAAf//H//AAAAD//4P/+AAAAf//g//4AAAB//8B//wAAAP//8H//AAAB//8=',
					'HWCg#{}=2:////gAAB//////+AAAP//////8AAB///////wAAH///////gAA///////+AAH//8P///8AAf//gAf//wAAA/8AAH//gAAAAwAAP/+AAAAAAAA//8',
				],
				[	# "W" in "WAYS"
					'HWCg#{}=0:AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAADAAAAAAAAB/+AAAAAAAD////wA//+AP////gH//8A//7/+Af//wD//v/4D///gf/+//gP//+B//z//A///4E=',
					'HWCg#{}=1://z//B///8P//P/8H///w//4//4f///j//j//j///+f/+P/+P///9//w//9////3//D//3//////4P//f//////gf////////+B////9////wH////0=',
					'HWCg#{}=2:////wD////j////AP///+P///4A////wf///gB////B///+AH///4D///wAP///gP///AA///8Af//4AD///wB///gAH//+AD//+AAf//4AP//wA',
				],
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
				[	# Button: Rew
					'HWCg#{}=0:AAAAAAAAAAAP////////8DAAAAAAAAAMJ////////+RP////////8l///8f/j//6X///h/8P//pf//8H/g//+l///gf8D//6X//8D/gf//pf//gf8D8=',
					'HWCg#{}=1://pf//A/4H//+l//4H/A///6X//A/4H///pf/4H/A///+l//g/8H///6X/+D/wf///pf/4H/A///+l//wP+B///6X//gf8D///pf//A/4H//+l//+B8=',
					'HWCg#{}=2:8D//+l///A/4H//6X//+B/wP//pf//8H/g//+l///4f/D//6X///x/+P//pP////////8if////////kMAAAAAAAAAwP////////8AAAAAAAAAAA',
				],
				[	# Button: Connect
					'HWCg#{}=0:AAAAAAAAAAAP////////8DAAAAAAAAAMJ////////+RP////////8l/////x///6X////8D///pf////wH//+l/////Af//6X////8B///pf////AH8=',
					'HWCg#{}=1://pf///8AH//+l///jBx///6X//8Af////pf//gH////+l//+A/////6X//4D/////pf//gH////+l///AH////6X//+MHH///pf///8AH//+l////8=',
					'HWCg#{}=2:AH//+l/////Af//6X////8B///pf////wH//+l/////A///6X/////H///pP////////8if////////kMAAAAAAAAAwP////////8AAAAAAAAAAA',
				],
				[	# Button: Play
					'HWCg#{}=0:AAAAAAAAAAAP////////8DAAAAAAAAAMJ////////+RP////////8l///5/////6X///B/////pf//8D////+l///wD////6X///AD////pf//8AH/8=',
					'HWCg#{}=1://pf//8AB///+l///wAB///6X///AAB///pf//8AAD//+l///wAAD//6X///AAAP//pf//8AAD//+l///wAAf//6X///AAH///pf//8AB///+l///wA=',
					'HWCg#{}=2:H///+l///wA////6X///AP////pf//8D////+l///wf////6X///n/////pP////////8if////////kMAAAAAAAAAwP////////8AAAAAAAAAAA',
				],
				[	# Button: Delete
					'HWCg#{}=0:AAAAAAAAAAAP////////8DAAAAAAAAAMJ////////+RP////////8l///H/8f//6X//4P/g///pf//gf8D//+l//+A/gP//6X//4B8A///pf//wDgH8=',
					'HWCg#{}=1://pf//4BAP//+l///wAB///6X///gAP///pf///AB///+l///+AP///6X///4A////pf///AB///+l///4AD///6X///AAH///pf//wDgH//+l//+Ac=',
					'HWCg#{}=2:wD//+l//+A/gP//6X//wH/Af//pf//A/+B//+l//8H/8H//6X//4//4///pP////////8if////////kMAAAAAAAAAwP////////8AAAAAAAAAAA',
				],
				[	# Button: List
					'HWCg#{}=0:AAAAAAAAAAAP////////8DAAAAAAAAAMJ////////+RP////////8l/////////6X/////////pf////////+l/////////6X//8cAAP//pf//xwAA8=',
					'HWCg#{}=1://pf//xwAA//+l/////////6X/////////pf////////+l///HAAD//6X//8cAAP//pf//xwAA//+l/////////6X/////////pf////////+l///HA=',
					'HWCg#{}=2:AA//+l///HAAD//6X//8cAAP//pf////////+l/////////6X/////////pP////////8if////////kMAAAAAAAAAwP////////8AAAAAAAAAAA',
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
				[	# Pattern: Vertical Lines
					'HWCg#{}=0:AAAAAAAAAAD//////////wAAAAAAAAAA//////////8AAAAAAAAAAP//////////AAAAAAAAAAD//////////wAAAAAAAAAA//////////8AAAAAAAA=',
					'HWCg#{}=1:AAD//////////wAAAAAAAAAA//////////8AAAAAAAAAAP//////////AAAAAAAAAAD//////////wAAAAAAAAAA//////////8AAAAAAAAAAP////8=',
					'HWCg#{}=2://///wAAAAAAAAAA//////////8AAAAAAAAAAP//////////AAAAAAAAAAD//////////wAAAAAAAAAA//////////8AAAAAAAAAAP//////////',
				],
				[	# Pattern: Horizontal Lines
					'HWCg#{}=0:qqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqo=',
					'HWCg#{}=1:qqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqo=',
					'HWCg#{}=2:qqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqq',
				],
				[	# Pattern: Checkerboard
					'HWCg#{}=0:qqqqqqqqqqpVVVVVVVVVVaqqqqqqqqqqVVVVVVVVVVWqqqqqqqqqqlVVVVVVVVVVqqqqqqqqqqpVVVVVVVVVVaqqqqqqqqqqVVVVVVVVVVWqqqqqqqo=',
					'HWCg#{}=1:qqpVVVVVVVVVVaqqqqqqqqqqVVVVVVVVVVWqqqqqqqqqqlVVVVVVVVVVqqqqqqqqqqpVVVVVVVVVVaqqqqqqqqqqVVVVVVVVVVWqqqqqqqqqqlVVVVU=',
					'HWCg#{}=2:VVVVVaqqqqqqqqqqVVVVVVVVVVWqqqqqqqqqqlVVVVVVVVVVqqqqqqqqqqpVVVVVVVVVVaqqqqqqqqqqVVVVVVVVVVWqqqqqqqqqqlVVVVVVVVVV',
				],
				[	# Pattern: Noise
					'HWCg#{}=0:SJ5G0zkSRAtAGIDuX08PMSCBVPc0IcQBMFCFCsADBMgohF8EAyMBmCtAIJarMIPUU0ZQgIIFU9wS7uAkqjDs4QQ6cONQ2G3iAy0HqBg25uY8tJkMiwg=',
					'HWCg#{}=1:Z7FYp6GwompI0lT3NCHEAYgOYFPQz+CmCSIgDIGASYphGmlTOgnLjPPJ0LCNF4BOE60FEgDIGASYpoFCRPI2mciSeklK3FNEcgXo04FEgDIGAddRQyU=',
					'HWCg#{}=2:R4GklKas6vmChChWVmEHqBH0ASYs7PhpjuNWvDILGBCPHatGEM9iRjPAPcCgat2aZHfGYTMNSgD7rm+rDlBIYYwA6QqwAMEyCxgQj+0UQi+CAZGA',
				],
				[	# Text: 5 lines
					'HWCg#{}=0:AAAAAAAAAAAwJdyUMk7ul0glEpRLUEiSSCnSdErWTmJ4KRwUSlJIkkgR0mcyTk6SAAAAAAAAAAAACOlwMkHS5wAITUBLQJJIAAhLcErAnkYACElASkA=',
					'HWCg#{}=1:kkEADulwMkCS7gAAAAAAAAAAAADujg5xkucAAESICUpSiAAARI4JSlTmAABEiA5yVIEAAE7uCEmI7gAAAAAAAAAAAIuBmSB0mRAAqgIloISlUACrgiU=',
					'HWCg#{}=2:YGelUACqAj0gFKVQAFOBpSDkmKAAAAAAAAAAAPBHS5wM4O6XgEJqIBKASJLgQluYEuBOYhBCSgQSgEiS4HdLuAyATpIAAAAAAAAAAAAAAAAAAAAA',
				],
				[	# Text: Quick Brown on black
					'HWCg#{}=0://///////////////////////////////4AB4AAAP///ul3MlzS///+SUdKSRT///9Pd0pJWf///0lHSkkU////SXcxnNL///gAAQgAAD//+5xkUu5k=',
					'HWCg#{}=1:L//+lKVWuiUv//7kpVW7pM///pclVLolL//+5Jikupkv//4AAAA4AAH//2lFx3Ml3P//KW0odKUS//wpVSZ0qdL//SlFwXSpHP/8xlUOcxHS//AAEEA=',
					'HWCg#{}=2:AEAAH/dLupnS7jHf8ko6pFLpSh/6e7qkjulK3/pKOj0C6Upf+ku7pczuMd/4ADgAAeCEH///////////////////////////////////////////',
				],
				[	# Text: Quick Brown lowercase on checkerboard
					'HWCg#{}=0:qqAqqqooqqpVRIBUAIIVVaqu4ynSGkqqVUSVklKilVWqpJYqUqOKqlVSk1HOmlVVqqgAKEAACqpVVVVVVVVVVaqCqqoKiqqqVVAAABUgBVWqnKZVykY=',
					'HWCg#{}=1:UqpVUslVJOklVaqSiSkqSSKqVVyGKSVGVVWqgCCACgACqlVVVVVVVVVVqgqqqqqqqqpVQAAAUAABVaoS846mSZSqVVKqWElK2VWqUqpGqVMSqlVOq5w=',
					'HWCg#{}=2:RiGVVapAAgCgiAKqVJVSVVVVVVWoCqgqqqgqqlJEFQAAUQQVp3GJO9KnMcpSStVIkklKVapLCUkSqUpKUUmVO85HMdWoACgAAqAASlVVVVVMVVWV',
				],
				[	# Text: QR + text
					'HWCg#{}=0:AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAD+LL+AAAAAAIKnoIB3OZwAugougISiSAC67y6AtLpIALpEroCXI8gAgtsggHS6SAD+qr+AAAAAAABygAHOM=',
					'HWCg#{}=1:GcAPvQVQEpSlIAJCwRAclKUgCyJz2BLkvSAGwMaIHJMlwA92prgAAAAACpQJUAGY7gAKo5vYAiUEAAtXLkgCJMQACeMPoAI8JAAADPjgAaXEAA/qavg=',
					'HWCg#{}=2:AAAAAAgmyNBxkuhwC6q/6EpaiIALrBe4SlboYAurnShz0ogQCCs7yEJS7uAP6wH4AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA',
				],
				[	# Space: Space ship
					'HWCg#{}=0:AAD/AAAAAAAAA/+AAAAAAAAPgcAAAAAAAD4AwAAAAAAAOABgAAAAAAA8AH/+AAAAAB4H///AAAAAD//+H/AAAAAD/wAA/AAAAAHgAAAeAAAAAYAAAA8=',
					'HWCg#{}=1:gAAAAYAAAAPAAAABgAA+AeAAAAGAAH8A8AAAH//gY4BwAAA///DjgDgAAD//4GOAMAAAAYAAdwBwAAABgAA/AOAAAAGAABwDwAAAAYAAAAeAAAABwAA=',
					'HWCg#{}=2:AB4AAAAB/wAAfAAAAAf//g/wAAAADw///8AAAAAcAH/+AAAAADwAYAAAAAAAPgDgAAAAAAAPgMAAAAAAAAPzwAAAAAAAAf+AAAAAAAAAPwAAAAAA',
				],
				[	# Space: Big heart
					'HWCg#{}=0:AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAcAOAAAAAAABwA4AAAAAAAHADgAAAAAAADzwAAAAAAAAPPAAAPPgAAA88AAB8+AAAf/+AAOf+AAB//4AA58=',
					'HWCg#{}=1:+AAB//4AA3/4AA/DD8AD//gAD8MPwAP/+AAPww/AAf/gAH////gA/+AAf///+AB/gAB////4AD8AAHH//jgAHwAAcf/+OAAMAABx//44AAwABHH//jg=',
					'HWCg#{}=2:AAAAAnH//jgAAAAHcf/+OAAAAAcw//wwAAAADQA88AAAAAAfADzwAAAAABcB/P4AAAAAFwH8/gAAAAADAfz+AAAAAAYA+HwAAAAABgAAAAAAAAAA',
				],
				[	# Space: Invasion
					'HWCg#{}=0:AAAAAAAAAAAQgAAAAIQAAAkAAAAASAAAH4BCAAD8CEAfgCQAAPwEgDbAfgABtg/Af+B+AAP/D8BfoNsEIv0bYF+h/4JC/T/wDwF+h+B4L9AZgX6H4Mw=',
					'HWCg#{}=1:L9AZgDwNsMwHgAAAZh/4AAzAAABmF+gQjMABCAAX6AkAAACQEIPAH4AAAfgJBmAfgAAB+B+GYDbAAANsH4AAf+AAB/42wABfoAAl+n/hCF+iEEX6X6A=',
					'HWCg#{}=2:kA8BIODwX6H4GYPw4ZgPAfgZg/CxmBmDbAAG2PgAGYf+AA/86AAABfoAC/ToAAAF+gAL9MAAAADwAAHgYAAAAZgAAzBgAAABmAADMAAAAAAAAAAA',
				],
				[	# Space: U.F.O.
					'HWCg#{}=0:AAAAAP4AAAAAAAADx4AAAAAAAAcBwAAAAAAABDngAAAAAAAM/+AAAAAAABn/8AAAAAAAGf/wAAAAAP+z//AAAAAD/zP/8AAAAAP/N//wAAAAB/9///A=',
					'HWCg#{}=1:AAAAB48///AAAAADjz//8AAAAAP/n//gAAAAA//P/+AAAAAB/+f/wAAAAAD/8f/AAAAAAH/8f5gAAAAAP/8PPAAAAAAf/8B+AAAAAA////8AAAAAA/k=',
					'HWCg#{}=2://8AAAAACfH//4AAAAAOf//3gAAAAA8f/+PAAAAAD8f/98AAAAAH8f//gAAAAAH8P/+AAAAAAH+D/gAAAAAAH/AAAAAAAAAH4AAAAAAAAADAAAAA',
				],
				[	# SKAARHOJ Logo: SK
					'HWCg#{}=0:AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAH///+D+AD/h////4P4A/4f////A/gH/B/wAAAD+A=',
					'HWCg#{}=1:P8D/gAAAD+D/gP+AAAAP4f4A/8AAAA/j/AB////AD8fwAD////wP/+AAD////g/v4AAAAAf/D+f4AAAAAf8P4/wAAAAA/w/h/wAAAAD/D+B/gAAAA/8=',
					'HWCg#{}=2:D+A/wD////4P4B/wP////A/gD/g////wD+AD/gAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA',
				],
				[	# SKAARHOJ Logo: AA
					'HWCg#{}=0:AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAD/gAAB/gAAAf+AAAP/AAAB/8AAA/+AAAP/4AAH8=',
					'HWCg#{}=1:+AAAf/4AAP/8AAB/fwAB/P4AAP4/gAH8fwAB/B/AA/h/AAH8H8AH+D+AA/gP4AfwH8AH+A/wD/AfwAf///Af///gD///+B////Af///8P///+B/AAf4=',
					'HWCg#{}=2:f4AD+D+AAP5/AAH8f4AAf/4AAf5/AAB//gAA/gAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA',
				],
				[	# SKAARHOJ Logo: RH
					'HWCg#{}=0:AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAB////gP8AA/n////gfwAD+f////h/AAP5/AAP+H8A=',
					'HWCg#{}=1:AP5/AAH/H8AA/n8AAf8fwAD+fwAB/x/AAP5/AAf+H////n////wf///+f///+B////5///+AH////n8Af4AfwAD+fwA/4B/AAP5/AB/wH8AA/n8AD/g=',
					'HWCg#{}=2:H8AA/n8AB/wfwAD+fwAB/h/AAP5/AAD/H8AA/gAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA',
				],
				[	# SKAARHOJ Logo: OJ
					'HWCg#{}=0:AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAD8AAAAAAAAP//4AAAB/gH///4AAAH+B////4AAAf4P+AD/wAA=',
					'HWCg#{}=1:Af4f8AA/4AAB/h/gAB/gAAH+P8AAH+AAAf4/wAAP4AAB/j+AAA/wAAH+P4AAD/AAAf4/wAAP4AAB/j/AAB/gAAH+H8AAH+AAA/wf4AA/wAAD/A/8AP8=',
					'HWCg#{}=2:gAAP+Af///8H///wAf///gf//+AAP//wB///AAAA+AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA',

				],
				[	# TEST icon 1: Film camera
					'HWCg#{}=0:AAAAf8AAAAAAAAD/4AAAAAAAAf/wAAAAAAAD//gAAAAAAAP//AAAAAAAB//8A4AAAAAH//wP4AAAAAfx/D/wAAAAB/H8P/gAAAAH+fx/+AAAAAf//H8=',
					'HWCg#{}=1:/AAAAAP//Hx8AAAAA//4fHwAAAAB//h8/AAAAAD/83/8AADwAH/nf/gAAP8AH4+/8AAA/+AAP5/gAAD//B//x8AAAP//3//wAAAA//////8AAAD///8=',
					'HWCg#{}=2://8AAAD//////wAAAP//////AAAA//+f//8AAAD//B///wAAAP/AH///AAAA/gAf//8AAADgAB///wAAAAAAH///AAAAAAAP//4AAAAAAAP/+AAA',
				],
				[	# TEST icon 2: Play film frame
					'HWCg#{}=0:AB//////8AAAP//////4AAB///////wAAGBwODweDAAAYHA4PB4MAABwcHg8HgwAAH///////AAAf//////8AAB///////wAAH///////AAAf/+f//8=',
					'HWCg#{}=1:/AAAf/8D///8AAB//wD///wAAH//AD///AAAf/8AD//8AAB//wAH//wAAH//AAf//AAAf/8AD//8AAB//wA///wAAH//AP///AAAf/8D///8AAB//58=',
					'HWCg#{}=2:///8AAB///////wAAH///////AAAf//////8AAB///////wAAGDweDweDAAAYHB4PBwMAABgcHg8HAwAAH///////AAAP//////4AAAf//////AA',
				],
				[	# TEST icon 3: Play symbol
					'HWCg#{}=0:AAAAD/AAAAAAAAB//gAAAAAAAf//gAAAAAAD4AfAAAAAAAeAAeAAAAAADwAA8AAAAAAeAAB4AAAAADwAADwAAAAAOA8AHAAAAABwD4AOAAAAAHAPwA4=',
					'HWCg#{}=1:AAAAAGAM4AYAAAAA4AzwBwAAAADgDHgHAAAAAOAMPAcAAAAA4AwMBwAAAADgDAwHAAAAAOAMPAcAAAAA4Ax4BwAAAADgDPAHAAAAAGAM4AYAAAAAcA8=',
					'HWCg#{}=2:wA4AAAAAcA+ADgAAAAA4DwAcAAAAADwAADwAAAAAHgAAeAAAAAAPAADwAAAAAAeAAeAAAAAAA+AHwAAAAAAB//+AAAAAAAB//gAAAAAAAA/wAAAA',
				],
				[	# TEST icon 4: Microphone
					'HWCg#{}=0:APgAAAAAAAAB/gAAAAAAAAP/AAAAAAAAA/8AAAAAAAAD/wAAAAAAAAP/A4zz5+HwA/8DnGcmc7gD/wPcZgZzHAP/A/xmBmMMA/8DfGYHwwwD/wNsZgY=',
					'HWCg#{}=1:4xwD/wMMZyZzuAP/Awzz5jnwA/8AAAAAAAAD/wAAAAAAADP/MAAAAAAAM/8wAAAAAAAz/zAAAAAAADv/c/MY+OZ+Gf5j8xnc5n4c/OM7GY72YA4BwzM=',
					'HWCg#{}=2:+YbefgeHg/MZht5gA/8DAxmOzmAAeAMDGdzOfgAwAwMY+MZ+ADAAAAAAAAAAMAAAAAAAAAAwAAAAAAAAADAAAAAAAAAH/8AAAAAAAAf/wAAAAAAA',
				],
			]


		HWCgraphic = -1
		HWCtimer = [0] * 256

		while True:
			try:
				# self.request is the TCP socket connected to the client
				self.data = self.request.recv(1024).strip()
			except socket.timeout:

				for HWc in range (0, 255):
					if (HWCtimer[HWc] > 0) and (int(round(time.time() * 1000))-HWCtimer[HWc] > 700):

						HWCgraphic = HWCgraphic + 1
						if (HWCgraphic % (len(imageStrings)+len(txtStrings)) < len(imageStrings)):
							outputline = imageStrings[HWCgraphic%(len(imageStrings)+len(txtStrings))][0].format(HWc) + "\n";
							self.request.sendall(outputline.encode('ascii'))
							outputline = imageStrings[HWCgraphic%(len(imageStrings)+len(txtStrings))][1].format(HWc) + "\n";
							self.request.sendall(outputline.encode('ascii'))
							outputline = imageStrings[HWCgraphic%(len(imageStrings)+len(txtStrings))][2].format(HWc) + "\n";
							self.request.sendall(outputline.encode('ascii'))
							outputline = "";
						else:
							outputline = txtStrings[HWCgraphic%(len(imageStrings)+len(txtStrings))-len(imageStrings)].format(HWc) + "\n";
							self.request.sendall(outputline.encode('ascii'))
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

							HWCtimer[HWc] = int(round(time.time() * 1000))
							HWCgraphic = HWCgraphic + 1

							if (HWCgraphic % (len(imageStrings)+len(txtStrings)) < len(imageStrings)):
								outputline = imageStrings[HWCgraphic%(len(imageStrings)+len(txtStrings))][0].format(HWc) + "\n";
								self.request.sendall(outputline.encode('ascii'))
								print("- Returns: '{}'".format(outputline.replace("\n","<NL>")))
								outputline = imageStrings[HWCgraphic%(len(imageStrings)+len(txtStrings))][1].format(HWc) + "\n";
								self.request.sendall(outputline.encode('ascii'))
								print("- Returns: '{}'".format(outputline.replace("\n","<NL>")))
								outputline = imageStrings[HWCgraphic%(len(imageStrings)+len(txtStrings))][2].format(HWc) + "\n";
								self.request.sendall(outputline.encode('ascii'))
								print("- Returns: '{}'".format(outputline.replace("\n","<NL>")))
								outputline = "";
							else:
								outputline = txtStrings[HWCgraphic%(len(imageStrings)+len(txtStrings))-len(imageStrings)].format(HWc) + "\n";

						# Parse Up trigger:
						match = re.search(r"^HWC#([0-9]+)=Up$", line.decode('ascii'))
						if match:
							HWc = int(match.group(1));	# Extract the HWc number of the keypress from the match

							HWCtimer[HWc] = 0

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
