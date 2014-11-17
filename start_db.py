#!/usr/bin/env python
# -*- coding: utf-8 -*-  

from thread import start_new_thread
import subprocess
import time
import socket
import fcntl
import struct
import urllib
import os
import signal

HOST = "localhost"
PORT = 4223
UID = "7xwQ9g"
PROG_PATH = "/home/pi/SICKRPi-Scanner/"
run = True
is_scan = False
pro = None

led_l_blink = False
led_r_blink = False

from tinkerforge.ip_connection import IPConnection
from tinkerforge.bricklet_dual_button import DualButton

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
	global pro
	# nicht blockierend
	pro = subprocess.Popen(PROG_PATH+"main",  shell=True, preexec_fn=os.setsid) 

# callback function for interrupts
def cb_state_changed(button_l, button_r, led_l, led_r):
	global is_scan
	global pro
	global led_l_blink
	global led_r_blink
	# check for both interrupts
	if button_l == DualButton.BUTTON_STATE_PRESSED and button_r == DualButton.BUTTON_STATE_PRESSED:
		print "stop script"
		if is_scan:
			led_l_blink = False
			led_r_blink = True
			execfile(PROG_PATH+"quit.py")
			if pro != None:
				os.killpg(pro.pid, signal.SIGTERM)
		global run
		run = False
	# check start interrupt
	elif button_l == DualButton.BUTTON_STATE_PRESSED:
		if is_scan == False:
			print "start scan"
			led_l_blink = True
			is_scan = True
			# start laser thred
			start_new_thread(call_laser,())
	# check stop interrupt
	elif button_r == DualButton.BUTTON_STATE_PRESSED:
		if is_scan:
			print "stop scan"
			led_l_blink =  False			
			execfile(PROG_PATH+"quit.py")
			is_scan = False

################################################################################

# public ip adress
try:
	ipaddr = get_ip_address('wlan0')
	print ipaddr
	urllib.urlopen("http://htw.fischerm.net/ip/ip.php?ip="+ipaddr)
except:
	print "couldn't public ip address"

# init tinkerforge

ipcon = None

try:
	ipcon = IPConnection() # create IP connection
	db = DualButton(UID, ipcon) # create device object

	ipcon.connect(HOST, PORT) # Connect to brickd
	# Don't use device before ipcon is connected

	# Register state changed callback to function cb_state_changed
	db.register_callback(db.CALLBACK_STATE_CHANGED, cb_state_changed)

	# set led state
	db.set_led_state(DualButton.LED_STATE_ON, DualButton.LED_STATE_OFF)

except:
	run = False
	print "no connection to tinkerforge devices!"
	exit(0)

################################################################################

i = 0

while run:
	try:
		time.sleep(0.05)
		# blink
		if i == 10:
			# get led state
			ls = db.get_led_state()
			if led_l_blink:				
				if ls[0] == DualButton.LED_STATE_ON:
					ls[0] = DualButton.LED_STATE_OFF
				else:
					ls[0] = DualButton.LED_STATE_ON
				# set led state
				db.set_selected_led_state(DualButton.LED_LEFT,ls[0])
			if led_r_blink:
				if ls[1] == DualButton.LED_STATE_ON:
					ls[1] = DualButton.LED_STATE_OFF
				else:
					ls[1] = DualButton.LED_STATE_ON
				# set led state
				db.set_selected_led_state(DualButton.LED_RIGHT,ls[1])
			i = 0		
		i = i + 1
	except:
		print "stop main loop"
		run = False
db.set_selected_led_state(DualButton.LED_RIGHT,DualButton.LED_STATE_ON)
time.sleep(0.5)	
if ipcon != None:
	
	ipcon.disconnect()
print "Quit"
