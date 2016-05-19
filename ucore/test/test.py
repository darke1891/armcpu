import socket
import commands
HOST='0.0.0.0'
PORT=8891
PORT2=8892
s= socket.socket(socket.AF_INET,socket.SOCK_STREAM)
s.bind((HOST,PORT))
s.listen(1)
conn, addr=s.accept()
s2= socket.socket(socket.AF_INET,socket.SOCK_STREAM)
s2.bind((HOST,PORT2))
s2.listen(1)
conn2, addr2=s2.accept()
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
	conn.send(sdata)
	print "send",
	print str(i)

r_data = ""
for i in range(0,l):
	tdata = conn2.recv(702/l)
	r_data += tdata
	print "recv",
	print str(i),
	print "len",
	print len(tdata)

print "-----------------"
print r_data
print "-----------------"

conn.close()