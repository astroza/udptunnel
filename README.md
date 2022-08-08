udptunnel
=========

This is 'udptunnel' with packet security by xor. 

Updates
=========
* Added packet security with XOR
* Added serialization, logging and ini files [1] [2] [3]
* Removed authentication bytes
* Modified linux script to use iproute2 commands

Usage
=========
First start server
```
   sudo ./server -i server.ini
```
   
Start client
```
  sudo ./client -i client.ini
```

client.ini

```
[client]
server_address=192.168.1.1
server_port=9000
auth=abc
outbound_key=out.key
inbound_key=in.key
```

server.ini

```
[server]
server_port=9000
auth=abc
outbound_key=out.key
inbound_key=in.key
```

[1] https://github.com/liteserver/binn
[2] https://github.com/rxi/log.c
[3] https://github.com/rxi/ini
