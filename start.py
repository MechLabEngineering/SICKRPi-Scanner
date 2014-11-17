#!/usr/bin/env python
# -*- coding: utf-8 -*-  

from thread import start_new_thread
import subprocess
import time
import socket
import fcntl
import struct
import urllib

HOST = "localhost"
PORT = 4223
UID = "7xwQ9g"
PROG_PATH = "/home/pi/SICKRPi-Scanner/"
run = True
is_scan = False

from tinkerforge.ip_connection import IPConnection
from tinkerforge.bricklet_industrial_digital_in_4 import IndustrialDigitalIn4

#mount stick
#subprocess.call(["sudo mount" , "/dev/sdb1 /media/usbstick -o uid=pi,gid=pi"])

################################################################################

def get_ip_address(ifname):
    s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    return socket.inet_ntoa(fcntl.ioctl(
        s.fileno(),
        0x8915,  # SIOCGIFADDR
        struct.pack('256s', ifname[:15])
    )[20:24])

# thread: start laserscan
def call_laser():
	ret = subprocess.call(PROG_PATH+"main", shell=True)
	
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
			execfile(PROG_PATH+"quit.py")
			is_scan = False
	# check for both interrupts
	elif interrupt_mask & 0x03 and value_mask & 0x03:
		print "stop script"
		if is_scan:
			execfile(PROG_PATH+"quit.py")
			is_scan = False
		global run
		run = False
	# check shutdown interrupt
	elif interrupt_mask & 0x04 and value_mask & 0x04:
		run = False
		subprocess.call(["shutdown" , "-h now"])

################################################################################

# public ip adress
try:
	ipaddr = get_ip_address('wlan0')
	print ipaddr
	print urllib.urlopen("http://htw.fischerm.net/ip/ip.php?ip="+ipaddr)
except:
	print "couldn't public ip address"

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
time.sleep(0.5)	
ipcon.disconnect()
