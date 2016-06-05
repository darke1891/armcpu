#!/usr/bin/python
import socket
import struct
import os
import datetime

#HOST = '127.0.0.1'
HOST = '192.168.2.2'
PORT=8900

start = datetime.datetime.now()

s=socket.socket(socket.AF_INET,socket.SOCK_STREAM)
s.connect((HOST,PORT))
print "connected"

while True:
	a = s.recv(256)
	if a == "exit" or a == "":
		break
	if os.path.exists(a):
		f = open(a,'rb')
		a = f.read()
		f.close()
		l = len(a)
		s.send(struct.pack('i', l))
		st = 0
		while st < l:
			if l - st < 500:
				nl = l - st
			else:
				nl = 500
			s.send(a[st:st+nl])
			re = s.recv(2)
			st += nl
			print st, '/', l, re
	else:
		s.send(struct.pack('i', 0))
s.close()

end = datetime.datetime.now()
print (end - start).total_seconds()