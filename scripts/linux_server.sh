#!/bin/sh
ip address add local 10.0.0.1 remote 10.0.0.2 dev tun0
ip link set dev tun0 up
ip link set mtu 1400 tun0
#sudo ifconfig tun0 10.0.0.1 pointopoint 10.0.0.2 mtu 1400 up
# echo 1 > /proc/sys/net/ipv4/ip_forward
# iptables -t nat -A POSTROUTING -s 10.0.0.2/32 -o eth0 -j MASQUERADE
