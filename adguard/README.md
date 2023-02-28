# How to install ADGUARD HOME docker container on Synology NAS
## Adguard vs Pi-hole
I switched fully to Adguard now and here is why:

Pros of Adguard
- fast search
- nice GUI
- supports all the fancy stuff: DoH, DoT, QUIC (certificate is required)

Pros of Pi-hole:
- opensource & community driven

*Nice alternative is to use nextdns.io (fully cloud-based solution)*

# Step 1. Installation
- adjust and copy the docker compose content to your docker compose file (e.g. to /volume1/docker/docker-compose.yml)
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
version: "3.9"
services:

  adguard:
    image: adguard/adguardhome:latest
    container_name: adguard
    hostname: adguard
    user: 1029:100 #put your docker used ID (UID) and group ID (GID) here!
    domainname: local
    mac_address: 00:fa:c0:fa:c0:aa
    cap_add:
      - NET_ADMIN
    networks:
      macvlan_network:
         ipv4_address: 192.168.1.9  #ip of your adguard container. I choose 192.168.1.8/29 subnet for my macvlans
    dns:
      - 192.168.1.1
      - 1.1.1.1
    environment:
      - TZ=Europe/Amsterdam # your TZ
    volumes:
      - /volume1/docker/adguard/work:/opt/adguardhome/work
      - /volume1/docker/adguard/conf:/opt/adguardhome/conf
      - /volume1/docker/acme/config/<cert_path>:/opt/adguardhome/cert #this string is needed if you want to use ACME issued HTTPS cert
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
          ip_range: 192.168.1.8/29
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
||xboxlive.com^$important
||dls.di.atlas.samsung.com^$important
||i.67yo.net^$important
||us.urcatwalkstar.com^$important
||client-telemetry.roblox.com^$important
||ephemeralcounters.api.roblox.com^$important
||alt2-mtalk.google.com^$important
```
# Step 3. Additional configurations
You need to figure out this by your own, but here are few things I did:
- Adjusted router's DHCP scope to serve Adguard IP as a primary DNS
- NAS still uses static DNS. I don't want things to get broken if Adguard container is restarted
- I still have Pi-hole on Raspberry Pi Zero W. That's my secondary DNS
- My router redirects all DNS requests to Adguard, except requests from NAS, Adguard itself and Pi-hole (this is called DNSFilter in Asus routers with Merlin's firmware)
- I've added all family devices to Adguard (Settings > Client Settings). For kid's tablets I've switched off all unneeded services (e.g. Amazon, ebay, OK.ru ....)
- I've added some internal network devices to custom blocking rules (filters > custom rules). This is needed for the internal DNS resolution to work
- I also have Caddy container for Vaultwarden which handles Let's Encrypt wildcard cert for my domain. I have mounted the cert folder from Caddy to AdGuard (see docker-compose config above) and now use this cert to protect AdGuard also
- I've added A record to my domain name, pointed to the local AdGuard IP (e.g. dns 10800 IN A 192.168.1.9). Now I can use dns.example.com url to access AdGuard locally

```bash
#internal hosts
192.168.1.1 router.local
192.168.1.2 nas.local
```
## Autoupdating docker
I've switched off my autoupdate after few bugs, but here is how you can do it:
- go to Task Scheduler
- add a task to be run as root
- paste this to "user-defined script" window:
```bash
cd /volume1/docker/
docker-compose pull #get new versions
docker-compose up -d #recreate containers
docker system prune -a -f #clean docker
```

# Step 4. Adding proper route to docker's macvlan network adapter
Docker's macvlan adapters are available for LAN users, but are not available for DSM/Docker users. Generaly speaking that's not an issue, but if you want, for example, to add adguard to your home-assistant container running on the same docker (like I did) you need this.

Create a bootup task for root user (aka "user-defined scrip") in Synology tasks with this content:
```bash
ip link add macvlan0 link ovs_eth0 type macvlan mode bridge
ip addr add 192.168.1.8/29 dev macvlan0
ip link set macvlan0 up
```

* 192.168.1.8/29 - IP range in my network dedicated to docker's macvlan. Make sure this range is excluded from the DHCP scope!
* macvlan0 - new virtual network interface name
* ovs_eth0 - Synology DSM network interface name
