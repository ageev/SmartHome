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

I use Synology NAS DS218+ with 16Gb of memory to run almost everything at my home. Adguard is one of many docker containers I have. 

So here are the installation steps for Docker on Synology NAS:
- adjust and copy the docker compose content to your docker compose (e.g. to /volume1/docker/docker-compose.yml)
- SSH to NAS
- "cd /volume1/docker"
- "sudo docker-compose up -d"

docker-compose.yml
```yaml
## install new versions
# cd /volume1/docker
# sudo docker-compose pull
# sudo docker-compose up -d
## clean docker
# sudo docker system prune -a  
## rebuild containers
# sudo docker-compose up -d --build

---
version: "2"
services:

  adguard:
    image: adguard/adguardhome:latest
    container_name: adguard
    hostname: adguard
    domainname: local
    mac_address: 00:fa:c0:fa:c0:aa
    cap_add:
      - NET_ADMIN
    networks:
      macvlan_network:
         ipv4_address: 192.168.1.7  #ip of your adguard container
    dns:
      - 192.168.1.1
      - 1.1.1.1
    environment:
      - PUID=<YOUR_PUID> # user and group ID of your docker NAS user. Needed for additional security, can be removed
      - PGID=<YOUR_PGID>
      - TZ=Europe/Amsterdam # your TZ
    volumes:
      - /volume1/docker/adguard/work:/opt/adguardhome/work
      - /volume1/docker/adguard/conf:/opt/adguardhome/conf
    restart: unless-stopped
    
networks:
  macvlan_network:
    driver: macvlan
    enable_ipv6: false
    driver_opts:
      parent: ovs_eth0
    ipam:
      config:
        - subnet: 192.168.1.0/24
          gateway: 192.168.1.1
          #ip_range: 192.168.1.0/24
```


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
||alt2-mtalk.google.com^$important
```
# Step 3. Additional configurations
You need to figure out this by your own, but here is few things I did:
- Adjusted router's DHCP scope to serve Adguard IP as a primary DNS
- NAS still uses static DNS. I don't want things to get broken if Adguard container is restarted
- I still have Pi-hole on Raspberry Pi Zero W. That's my secondary DNS
- My router redirects all DNS requests to Adguard, except requests from NAS, Adguard itself and Pi-hole (this is called DNSFilter in Asus routers)
- I've added all family devices to Adguard (Settings > Client Settings). For kid's tablets I've switched off all unneeded services (e.g. Amazon, ebay, OK.ru ....)
- I've added some internal network devices to custom blocking rules (filters > custom rules). This is needed for the internal DNS resolution to work

```bash
#internal hosts
192.168.1.1 router.local
192.168.1.2 nas.local
```

# Step 4. Adding proper route to docker's macvlan network adapter
Docker's macvlan adapters are available for LAN users, but are not available for DSM/Docker users. Generaly speaking that's not an issue, but if you want, for example, to add adguard to your home-assistant container running on the same docker (like I did) you need this.
1. create a file in /usr/local/etc/rc.d folder with .sh extention (this is a DSM script autostart directory[1])
2. chmod 755 <filename>
3. andjust & copy this into the file

```bash
#!/bin/bash

if [ "$1" = "start" ];
then
    sleep 60
    ip link add dlink0 link ovs_eth0 type macvlan mode bridge
    ip addr add 192.168.1.127/32 dev dlink0
    ip link set dlink0 up
    ip route add 192.168.1.7/32 dev dlink0
fi
```
* 192.168.1.127/32 - just a random IP to use as a virtual adapter's IP
* dlink0 - new virtual network interface name
* ovs_eth0 - Synology DSM network interface name
* 192.168.1.7/32 - pi-hole container's IP

"sleep 60" is needed for script to work properly, otherwise it fails. 