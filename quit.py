import string
import socket
import sys
import time

addr = "octomap"

# create UDS socket
sock = socket.socket(socket.AF_UNIX, socket.SOCK_STREAM)

try:
	sock.connect(addr)
	print "connected"
except socket.error, msg:
	sock = 0
	print "not connected"
	exit(0)

msg = "QUIT"
sock.sendall(msg)		
		
sock.close()
