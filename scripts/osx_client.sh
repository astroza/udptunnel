#!/bin/sh

ifconfig tun0 10.0.0.2 10.0.0.1 mtu 1400 up
old_default=$(route get default |grep gateway|awk '{print $2}')
provider=$1
route add -host $provider $old_default
route delete default
route add default 10.0.0.1

# El portal cautivo tiene un servidor DNS limitado, solo responde a 
# direcciones frecuentes, populares, como facebook o google.

# Lo cambio al DNS de Google 8.8.8.8
for state in `echo 'list State:/Network/Service/[^/]+/DNS' | /usr/sbin/scutil | /usr/bin/awk '{print $4}'`; do
        (echo "get $state"; echo "d.add ServerAddresses * 8.8.8.8"; echo "set $state") | /usr/sbin/scutil
done
