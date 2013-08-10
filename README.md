udptunnel
=========

It allows TCP/UDP/ICMP traffic over UDP tunneling. It's useful to avoid Internet restrictions.

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

And that's all!!, you'll have a slow (just a bit) but full access Internet connection!. I've measured 0.210 MBPS in download.

F. Astroza
