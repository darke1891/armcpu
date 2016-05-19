import socket
HOST = '127.0.0.1'
#HOST = '192.168.2.2'
PORT=8891
PORT2=8892

s=socket.socket(socket.AF_INET,socket.SOCK_STREAM)
s.connect((HOST,PORT))
print "connected"
l = 26
data = ""
for i in range(0,l):
	tdata = ""
	for j in range(0,26):
		tdata += chr(ord('a')+(j+i)%26);
	tdata += '\n'
	data += tdata

l = 6
for i in range(0,l):
	sdata = data[i*702/l:(i+1)*702/l]
	s.send(sdata)
	print "send",
	print str(i)
s.close()

s2=socket.socket(socket.AF_INET,socket.SOCK_STREAM)
s2.connect((HOST,PORT2))
r_data = ""
for i in range(0,l):
	tdata = s2.recv(702/l)
	r_data += tdata
	print "recv",
	print str(i),
	print "len",
	print len(tdata)

print "-----------------"
print r_data
print "-----------------"

s2.close()
