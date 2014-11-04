import string
import socket
import sys
import time

class Octo:

	def __init__(self, addr):
		self.addr = addr
		
		# var init

		self.r = 0.1
		self.offset = [0,6,0]

	def connectUDS(self):
		# create UDS socket
		self.sock = socket.socket(socket.AF_UNIX, socket.SOCK_STREAM)

		try:
			self.sock.connect(self.addr)
			print "connected"
		except socket.error, msg:
			self.sock = 0
			print "not connected"

	def receiveData(self):
		self.connectUDS()
		
		if self.sock != 0:
			start = time.time();
			# send request
			msg = "GET_OCTOMAP"
			self.sock.sendall(msg)		
			print "Message:" + msg
			response = self.sock.recv(50000)
			print str(len(response))
			
			if len(response) == len("NO_DATA"):
				print "NO_DATA";
				return
			
			#print response
			# extract nodes
			nodes = string.split(response,"#")
			#print nodes
			for n in nodes:
				#print n + "::" + str(len(n))
				if len(n) > 0:
					#print n
					sp = string.split(n,";")
					x = float(sp[0])
					y = float(sp[1])
					z = float(sp[2])
					#o = float(sp[3])
					#print str(x) + "::" + str(y) + "::" + str(z)
					coords = calcGridCoord(self.r, (x,y,z))
					#print str(float(sp[0])) + "::" + str(float(sp[1])) +  "::" + str(float(sp[2]))
					print coords
		
			print "Delay:" + str(time.time()-start)
			
			if self.sock != 0:
				# close socket
				self.sock.close()
			
	def __del__(self): 
		print "FIN"
		#if self.sock != 0:

			# close socket
			#self.sock.close()
			
	def calcGridCoord(res, *values):
		ret = []
		for i, v in enumerate(*values):
			ret.append((int((res * round(v/res)) / res))+self.offset[i])
		return ret

if __name__ == "__main__":
	con = Octo("octomap")
	print "Start..."

	while True:
		con.receiveData()
		time.sleep(3)
	print "End..."
