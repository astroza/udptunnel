#!/bin/sh

ifconfig tun0 10.0.0.2 10.0.0.1 mtu 1400 up
old_default=$(route get default |grep gateway|awk '{print $2}')
$provider=$1
route add -host $provider $old_default
route delete default
route add default 10.0.0.1
