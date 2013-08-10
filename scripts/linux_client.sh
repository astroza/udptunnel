#!/bin/sh

ifconfig tun0 10.0.0.2 pointopoint 10.0.0.1 mtu 1400 up
