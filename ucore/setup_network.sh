sudo ip a add 192.168.2.1/24 dev eth0 scope global
sudo ip route add 192.168.2.0/24 dev eth0
netcat -l 192.168.2.1 8888
