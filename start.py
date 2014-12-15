#!/usr/bin/env python
# -*- coding: utf-8 -*-  

from thread import start_new_thread
from threading import Timer
import subprocess
import time
import socket
import fcntl
import struct
import urllib
import os
import signal

from tinkerforge.ip_connection import IPConnection
from tinkerforge.bricklet_dual_button import DualButton

class LRF:
	HOST = "localhost"
	PORT = 4223
	PROG_PATH = "/home/pi/SICKRPi-Scanner/"
	#PROG_PATH = "/home/gus484/Programme/octomap_ibeo/"	
	TIMER_MS = 1.0

	def __init__(self):
		self.db = None
		self.is_scan = False
		self.led_l_blink = False
		self.led_r_blink = False
		self.run = True
		self.ipcon = None
		self.pro = None

		try:
			# create ip connection
			self.ipcon = IPConnection()

			# register ip connection callbacks
			self.ipcon.register_callback(IPConnection.CALLBACK_ENUMERATE,
							self.cb_enumerate)

			self.ipcon.register_callback(IPConnection.CALLBACK_CONNECTED,
							self.cb_connected)

			self.ipcon.connect(LRF.HOST, LRF.PORT)

			#self.ipcon.enumerate()
		except:
			self.run = False
			print "no connection to tinkerforge devices!"
			exit(0)

	def release(self):
		if self.ipcon != None:
			# turn off leds
			self.db.set_led_state(DualButton.LED_STATE_OFF,DualButton.LED_STATE_OFF)
			# disconnect			
			self.ipcon.disconnect()
			print "quit"
		self.run = False

	# thread: start laserscan
	def call_laser(self):
		# nicht blockierend
		self.pro = subprocess.Popen(LRF.PROG_PATH+"bin/main",  shell=False, preexec_fn=os.setsid) 

	# callback handles device connections and configures possibly lost 
	# configuration of lcd and temperature callbacks, backlight etc.
	def cb_enumerate(self, uid, connected_uid, position, hardware_version, firmware_version, device_identifier, enumeration_type):
		if enumeration_type == IPConnection.ENUMERATION_TYPE_CONNECTED or enumeration_type == IPConnection.ENUMERATION_TYPE_AVAILABLE:
            
			# enumeration is for dual button bricklet
			if device_identifier == DualButton.DEVICE_IDENTIFIER:
				if self.db != None:
					return
				# create dual button device object
				self.db = DualButton(uid, self.ipcon) 
				# register button push callback to function cb_state_changed
				self.db.register_callback(self.db.CALLBACK_STATE_CHANGED, self.cb_state_changed)

				# set led state
				self.db.set_led_state(DualButton.LED_STATE_ON, DualButton.LED_STATE_OFF)

				Timer(LRF.TIMER_MS, self.led_blink).start()

	# callback handles reconnection of ip connection
	def cb_connected(self, connected_reason):
		self.ipcon.enumerate()

	# callback function for interrupts
	def cb_state_changed(self, button_l, button_r, led_l, led_r):
		# check for both interrupts
		if button_l == DualButton.BUTTON_STATE_PRESSED and button_r == DualButton.BUTTON_STATE_PRESSED:
			print "stop script"
			#if self.is_scan:
			execfile(LRF.PROG_PATH+"quit.py")
			time.sleep(0.5)
			try:
				print "terminate process"			
				#os.killpg(self.pro.pid, signal.SIGTERM)
				self.pro.terminate()
			except:
				print "..."
			self.run = False
		# check start interrupt
		elif button_l == DualButton.BUTTON_STATE_PRESSED:
			if self.is_scan == False:
				print "start scan"
				self.led_l_blink = False
				self.db.set_selected_led_state(DualButton.LED_RIGHT,DualButton.LED_STATE_ON)
				self.is_scan = True
				# start laser thread
				start_new_thread(self.call_laser,())
		# check stop interrupt
		elif button_r == DualButton.BUTTON_STATE_PRESSED:
			if self.is_scan:
				print "stop scan"
				self.led_l_blink =  False
				self.db.set_selected_led_state(DualButton.LED_RIGHT,DualButton.LED_STATE_OFF)	
				execfile(LRF.PROG_PATH+"quit.py")
				self.is_scan = False

	def led_blink(self):
		if self.db != None and self.run == True:
			# get led state
			ls = self.db.get_led_state()

			if self.led_l_blink:				
				if ls[0] == DualButton.LED_STATE_ON:
					l = DualButton.LED_STATE_OFF
				else:
					l = DualButton.LED_STATE_ON				
				# set led state
				self.db.set_selected_led_state(DualButton.LED_LEFT,l)
			if self.led_r_blink:
				if ls[1] == DualButton.LED_STATE_ON:
					l = DualButton.LED_STATE_OFF
				else:
					l = DualButton.LED_STATE_ON
				# set led state
				self.db.set_selected_led_state(DualButton.LED_RIGHT,l)
		if self.run == True:
			Timer(LRF.TIMER_MS, self.led_blink).start()

################################################################################
# get local ip adress from iface "ifname"
def get_ip_address(ifname):
    s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    return socket.inet_ntoa(fcntl.ioctl(
        s.fileno(),
        0x8915,  # SIOCGIFADDR
        struct.pack('256s', ifname[:15])
    )[20:24])

################################################################################
# push ip address
try:
	ipaddr = get_ip_address('wlan0')
	print ipaddr
	urllib.urlopen("http://htw.fischerm.net/ip/ip.php?ip="+ipaddr)
except:
	print "couldn't public ip address"

################################################################################

if __name__ == "__main__":
	run = True
	lrf = et = LRF()
	while run:
		try:
			time.sleep(0.05)
			run = lrf.run
		except:
			lrf.run = False
time.sleep(0.05)
lrf.release()

