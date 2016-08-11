udptunnel
=========

It allows TCP/UDP/ICMP traffic over UDP tunneling. It's useful to avoid Internet restrictions.

Demo: http://www.youtube.com/watch?v=A0jLF2ipirg
Updates
=========
* BSD & Mac OSX support
* Auto-config scripts for routing

Mobile network prepaid
=========
Requirements
---------
* A prepaid chip
* Edge/3G Cellphone able to share Internet
* A full access Internet connection (home ADSL?)

Usage
---------
So far, I've tested this technique in an OSX machine (connected to cellphone) and a Linux machine (connected to full access Internet connection).
Once you have access to mobile operator's portal cautivo, you can access to limited services, such as DNS, and TCP ports distinct to 80 (HTTP).
I don't use proxy over TCP because TCP connections receive an arbitrary RESET at any time. Also, I figure out that 53 UDP port 
has the best performance, with lower packet loss.

You must:
* identify the public IP address of the machine connected to full access Internet connection ```<public_address>```
* choose a common password ```<your_password>```

On Linux server (with full access Internet connection)
```
./server 53 <your_password>
```
On OSX client (with limited Internet access)

```
./client <public_address> 53 <your_password>
```
Note: You may need a tuntap module for OSX http://tuntaposx.sourceforge.net/download.xhtml if you get an error saying ``open: No such file or directory``

And that's all!!, you'll have a slow (just a bit) but full access Internet connection!. I've measured 0.210 MBPS in download.

How it works
=========

By default mobile provider blocks any packet but UDP packet, unless you pay for the service. My method consists in sending
TCP/UDP/ICMP frames as payload of an UDP packet to a known host (your server). Your server reinjects these frames to Internet.
```
PC to Internet
PC -> Cellphone -> [limited Internet] -> Your server -> [Internet]

Internet to PC
[Internet] -> Your server -> [limited Internet] -> Cellphone -> PC
```
UDP Packet payload
---------
The messages between _your server_ and _PC_ are UDP packets with the next payload:
```
 ----------- ------------ --------------------------------------------
|     1    |        2    |                        3                   |
|PACKET_TYPE| PACKET_CMD |      TCP/UDP/ICMP Packet or control data   |
| (1 byte)  | (1 byte)   |               (variable size)              |
 ----------- ------------ --------------------------------------------
```
Each UDP packet arrives to _your server_ or _PC_ must be disassembled and reinjected to OS network stack.

There are 2 packet types:
* CONTROL: For authentication and notification
* TRAFFIC: For transporting another packet (TCP/UDP/ICMP).

It works transparently
---------
For reinjecting to OS network stack I use tun driver. It helps to create virtual network interfaces that receive traffic
from an userspace application.

Thanks to the routing scripts (in scripts/) you don't need do anything to get the whole system with Internet access. Routing
scripts are called automatically.

Future
=========
* iPhone port (computer connected to cellphone will not be necessary)
* Multi-client server (at this time, the server is one client only)
* Data compressing

F. Astroza
