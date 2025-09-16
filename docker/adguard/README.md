# How to install ADGUARD HOME docker container on Synology NAS
## Adguard Home vs Adguard Cloud vs Pi-hole
I switched fully to Adguard now and here is why:

Pros of Adguard:
- fast search
- nice GUI
- supports all the fancy stuff: DoH, DoT, QUIC (certificate is required)

Pros of Adguard Cloud:
- cheaper than NextDNS, but still you need a paid subscription
- best option for mobile phones because Adguard app management on Android is compex

Pros of Pi-hole:
- opensource & community driven

# Step 1. Docker-compose.yml
> I use adguard container in VirtualDSM! Adguard also uses certificates provided by [acme.sh](https://github.com/ageev/SmartHome/tree/master/docker/acme.sh#readme) container
```yaml
---
version: "3.9"
networks:
  macvlan_network:
    driver: macvlan
    enable_ipv6: false
    driver_opts:
      parent: eth0
    ipam:
      config:
        - subnet: 10.0.1.0/24
          gateway: 10.0.1.1
          ip_range: 10.0.1.8/29
services:
  adguard:
    image: adguard/adguardhome
    container_name: adguard
    hostname: adguard
#    user: 1027:100
    domainname: local
    mac_address: 00:fa:c0:fa:c0:ab
    cap_add:
      - NET_ADMIN
    networks:
      macvlan_network:
        ipv4_address: 10.0.1.9 # <- this will be your DNS server IP
    dns:
      - 10.0.1.1 #router's IP
      - 1.1.1.1
    environment:
      - TZ=Europe/Zurich
    volumes:
      - /volume1/docker/adguard/work:/opt/adguardhome/work
      - /volume1/docker/adguard/conf:/opt/adguardhome/conf
      - /volume1/docker/acme/config/*.your-domain.com:/opt/adguardhome/cert
    restart: unless-stopped
```


# Step 2. Configuration
Those are DNS blocklists I use: 
- AdGuard DNS filter
- HaGeZi's PRO / Samsung tracker / Windows+Office / Xiaomi / Threat Intelligence
- OISD Blocking Big

Custom blocking rules. Use those to ALLOW traffic. Template: ```@@||push.yandex.ru^$important```. Now pushes from Yandex will not be blocked. 

# Step 3. Additional configurations
You need to figure out this by your own, but here are few things I did:
- Adjusted router's DHCP scope to serve Adguard IP as a primary DNS
- NAS still uses static DNS. I don't want things to get broken if Adguard container is restarted
- My router redirects all DNS requests to Adguard, except requests from NAS, Adguard itself and mobile phones with Adguard Cloud DNS set (this is called *DNS Director* in Asus routers)
> Yandex speakers are known to use their own DNS servers if they see that requests to Yandex's ad servers are blocked. So this feature is useful! 
- I've added all family devices to Adguard (Settings > Client Settings). For kid's tablets I've switched off all unneeded services (e.g. Amazon, ebay, OK.ru ....)
- I've added some internal network devices to custom blocking rules (filters > Custom filtering rules). This is needed for the internal DNS resolution to work (e.g., add this to the custom rules for your smart light bulb: ```10.0.1.155 bulb.local```)
- I also have [ACME.SH](https://github.com/ageev/SmartHouse/tree/master/docker/acme.sh) container which handles HTTPS certificates. I have mounted the cert folder from ACME.SH to AdGuard (see docker-compose config above) and now use this cert to protect AdGuard also
- I've added A record to my domain name, pointed to the local AdGuard IP (e.g. dns 10800 IN A 10.0.1.9). Now I can use dns.your-domain.com url to access AdGuard locally

## Autoupdating docker
*Don't autoupdate your critical network infrastructure containers!*

# Step 4. Adding proper route to docker's macvlan network adapter
Docker's macvlan adapters are available for LAN users, but are not available for DSM/Docker users. Generaly speaking that's not an issue, but if you want, for example, to add adguard to your home-assistant container running on the same docker (like I did) you need this.

Create a bootup task for root user (aka "user-defined scrip") in Synology tasks with this content:
```bash
ip link add macvlan0 link ovs_eth0 type macvlan mode bridge
ip addr add 10.0.1.8/29 dev macvlan0
ip link set macvlan0 up
```

* 10.0.1.8/29 - IP range in my network dedicated to docker's macvlan. Make sure this range is excluded from the DHCP scope!
* macvlan0 - new virtual network interface name
* ovs_eth0 - Synology DSM network interface name
