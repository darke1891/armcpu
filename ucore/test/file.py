#!/usr/bin/python
import socket
import struct
import os

#HOST = '127.0.0.1'
HOST = '192.168.2.2'
PORT=8900

s=socket.socket(socket.AF_INET,socket.SOCK_STREAM)
s.connect((HOST,PORT))
print "connected"

a = s.recv(256)
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
		print nl, len(a[st:st+nl])
		s.send(a[st:st+nl])
		re = s.recv(2)
		st += nl
		print st, '/', l, re
else:
	s.send(struct.pack('i', 0))
s.close()