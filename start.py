#!/usr/bin/env python
# -*- coding: utf-8 -*-  

from thread import start_new_thread
import subprocess
import time

HOST = "localhost"
PORT = 4223
UID = "xyz" # Change to your UID
run = True

from tinkerforge.ip_connection import IPConnection
from tinkerforge.bricklet_industrial_digital_in_4 import IndustrialDigitalIn4

#mount stick
subprocess.call(["sudo mount" , "/dev/sda1 /media/usb -o uid=pi,gid=pi"])

def call_laser():
	subprocess.call("./main", shell=True)

# Callback function for interrupts
def cb_interrupt(interrupt_mask, value_mask):
	print('Interrupt by: ' + str(bin(interrupt_mask)))
	print('Value: ' + str(bin(value_mask)))
    
	# check start interrupt
	if interrupt_mask & 0x01:
		# start laser thred
		start_new_thread(call_laser,())
	elif interrupt_mask & 0x02:
		execfile("quit.py")
	elif interrupt_mask & 0x03:
		subprocess.call(["shutdown" , "-h now"])

ipcon = IPConnection() # Create IP connection
idi4 = IndustrialDigitalIn4(UID, ipcon) # Create device object

try:
	ipcon.connect(HOST, PORT) # Connect to brickd
	# Don't use device before ipcon is connected

	# Register callback for interrupts
	idi4.register_callback(idi4.CALLBACK_INTERRUPT, cb_interrupt)

	# Enable interrupt on pin 0, 1, 2
	idi4.set_interrupt(1 << 0)
	idi4.set_interrupt(1 << 1)
	idi4.set_interrupt(1 << 2)
except:
	print "Error!"
	exit()

while run:
	time.sleep(0.5)
	
ipcon.disconnect()
