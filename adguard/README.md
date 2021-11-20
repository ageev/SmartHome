# How to install ADGUARD HOME docker container on Synology NAS
## Adguard vs Pi-hole
I switched fully to Adguard now and here is why:
Pros of Adguard
- fast search
- nice GUI
- supports all the fancy stuff: DoH, DoT...

Pros of Pi-hole:
- opensource & community driven

# Step 1. Installation
- copy the docker compose content to your docker compose (e.g. to /volume1/docker/docker-compose.yml)
- SSH to NAS
- "cd /volume1/docker"
- "sudo docker-compose up -d"

# Step 2. Configuration
I use all the default lists and this one: 
- https://abp.oisd.nl/

Custom blocking rules:
```bash
||graph.facebook.com^$important
||notifier-configs.airbrake.io^$important
||www.googleadservices.com^$important
||v10.events.data.microsoft.com^$important
||arcus-uswest.amazon.com^$important
||cdn.meta.ndmdhs.com^$important
||redirector.googlevideo.com^$important
||clck.yandex.net^$important
||in.appcenter.ms^$important
||mtalk.google.com^
||pushserver.mfms.ru^$important
||todo-ta-g7g.amazon.com^$important
||device-messaging-na.amazon.com^$important
||xboxlive.com^$important
||dls.di.atlas.samsung.com^$important
||i.67yo.net^$important
||us.urcatwalkstar.com^$important
||client-telemetry.roblox.com^$important
||ephemeralcounters.api.roblox.com^$important
@@||tracker.zum.bi^$client='192.168.7.127'
||alt2-mtalk.google.com^$important
||acs.m.taobao.com^$important
||ae01.alicdn.com^$important
```

# Step 3. Adding proper route to docker's macvlan network adapter
Docker's macvlan adapters are available for LAN users, but are not available for DSM/Docker users. Generaly speaking that's not an issue, but if you want, for example, to add pi-hole to your home-assistant container running on the same docker (like I did) you need this.
1. create a file in /usr/local/etc/rc.d folder with .sh extention (this is a DSM script autostart directory[1])
2. chmod 755 <filename>
3. andjust & copy this into the file

```bash
#!/bin/bash

if [ "$1" = "start" ];
then
    sleep 60
    ip link add dlink0 link ovs_eth0 type macvlan mode bridge
    ip addr add 192.168.7.127/32 dev dlink0
    ip link set dlink0 up
    ip route add 192.168.7.8/32 dev dlink0
fi
```
* 192.168.7.127/32 - just a random IP to use as a virtual adapter's IP
* dlink0 - new virtual network interface name
* ovs_eth0 - Synology DSM network interface name
* 192.168.7.8/32 - pi-hole container's IP

"sleep 60" is needed for script to work properly, otherwise it fails. 
