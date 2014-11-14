#!/usr/bin/env python
# -*- coding: utf-8 -*-  

from thread import start_new_thread
import subprocess
import time

HOST = "localhost"
PORT = 4223
UID = "7xwQ9g"
run = True
is_scan = False

from tinkerforge.ip_connection import IPConnection
from tinkerforge.bricklet_industrial_digital_in_4 import IndustrialDigitalIn4

#mount stick
#subprocess.call(["sudo mount" , "/dev/sdb1 /media/usbstick -o uid=pi,gid=pi"])

################################################################################

# thread: start laserscan
def call_laser():
	ret = subprocess.call("/home/pi/SICKRPi-Scanner/main", shell=True)
	
	if ret == 1:
		global is_scan
		is_scan = False

# callback function for interrupts
def cb_interrupt(interrupt_mask, value_mask):
	global is_scan
    
	# check start interrupt
	if interrupt_mask & 0x01 and value_mask & 0x01:
		if is_scan == False:
			print "start scan"
			is_scan = True
			# start laser thred
			start_new_thread(call_laser,())
	# check stop interrupt
	elif interrupt_mask & 0x02 and value_mask & 0x02:
		if is_scan:
			print "stop scan"
			execfile("/home/pi/SICKRPi-Scanner/quit.py")
			is_scan = False
	# check shutdown interrupt
	elif interrupt_mask & 0x03 and value_mask & 0x03:
		run = False
		subprocess.call(["shutdown" , "-h now"])

################################################################################

# init tinkerforge

ipcon = IPConnection() # Create IP connection
idi4 = IndustrialDigitalIn4(UID, ipcon) # Create device object

try:
	ipcon.connect(HOST, PORT) # Connect to brickd
	# Don't use device before ipcon is connected

	# Register callback for interrupts
	idi4.register_callback(idi4.CALLBACK_INTERRUPT, cb_interrupt)

	# Enable interrupt on pin 0, 1
	idi4.set_interrupt(0x03)	

except:
	print "Error!"
	exit()

while run:
	try:
		time.sleep(0.05)
		#print idi4.get_value()
	except:
		print "Quit"
		run = False
	
ipcon.disconnect()
