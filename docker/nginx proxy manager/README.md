#Nginx Proxy Manager
Nginx Proxy Manager (aka, NPM) is a tool which allows you:
1. deploy HTTPS inside your network
2. publish something in a secure way to internet 
3. auto manages your HTTPS certs (via let's encrypt ACME bot)

Disadvantages: you need a custom config for most of the apps you publish. Sometimes it's a serious pain in the *** . Google "nginx proxy %appname%" and be ready to try a lot of configs. Traefik can forward traffic directly from the docker. So I may move to Traefik one day... 

## Setting up NPM with Synology
I want NPM to directly handle HTTP/HTTPS traffic on the standard ports (80/443, TCP). Unfortunately those are not available for docker for Synology NAS. DSM uses those ports for itself (just to redirect the traffic to 5001..). Also docker container can't see macvlan docker containers (no local route), so you need 2 dockers.
Best solution I've found so far - Install Synology VM Manager, setup Virtual DSM (you have 1 free license), setup another Docker instance in the Virtual DSM. Setup your NPM docker there. 
![NPM network scheme](https://github.com/ageev/SmartHome/raw/master/Pictures/npm_network.jpg)

Disadvantages:
- you now have 2 dockers, 2 DSMs to manage

## Docker-compose for NGM (VirtualDSM)
```bash
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
networks:
  macvlan_network:
    driver: macvlan
    enable_ipv6: false
    driver_opts:
      parent: eth0
    ipam:
      config:
        - subnet: 192.168.1.0/24
          gateway: 192.168.1.1
          ip_range: 192.168.1.16/29

services:
  nginx-proxy-manager:
    image: 'jc21/nginx-proxy-manager'
    container_name: nginx_proxy_manager
    hostname: nginx_proxy_manager
    #user: 1027:100   #CHANGE ME!
    domainname: local
    mac_address: 00:fa:c0:fa:c0:af
    cap_add:
      - NET_ADMIN
    dns:
      - 192.168.1.9 # CHANGE ME!
    environment:
     - PUID=1027 # CHANGE ME!
     - PGID=100  # CHANGE ME!
     - TZ=Europe/Amsterdam
     - DB_MYSQL_HOST=192.168.1.5  #CHANGE ME!
     - DB_MYSQL_PORT=3306
     - DB_MYSQL_USER=npm
     - DB_MYSQL_PASSWORD=p@55w0rd #CHANGE ME!
     - DB_MYSQL_NAME=npm
      # Uncomment this if IPv6 is not enabled on your host
     - DISABLE_IPV6=true
    volumes:
      - /volume1/docker/nginx_proxy_manager/config.json:/app/config/production.json
      - /volume1/docker/nginx_proxy_manager/data:/data
      - /volume1/docker/nginx_proxy_manager/letsencrypt:/etc/letsencrypt
    restart: unless-stopped
    networks:
      macvlan_network:
        ipv4_address: 192.168.1.17
```

## docker-compose.yml for MariaDB
```yml

```

# Configs for NGINX proxy manager
add a URL in NPM. Enable everything in SSL tab, enable Web Sockets. Paste those configs to the "Advanced" tab
replace <ip> with the real IP

## Radarr
```
# Radarr. Go to settings in Radarr and set "URL base" to "/radarr"
location /radarr {
    proxy_pass http://<ip>:7878;
    proxy_set_header Host $host;
    proxy_set_header X-Real-IP $remote_addr;
    proxy_set_header X-Forwarded-For $proxy_add_x_forwarded_for;
  }
```
## Prowlarr
```
# Prowlarr. Go to settings in Prowlarr and set "URL base" to "/radarr"
location /prowlarr {
    proxy_pass http://<ip>:9696;
    proxy_set_header Host $host;
    proxy_set_header X-Real-IP $remote_addr;
    proxy_set_header X-Forwarded-For $proxy_add_x_forwarded_for;
  }
```

## Plex
```
# Plex. Go to settings, network, custom server access - set 4a.pm/plex
location /web {
    proxy_pass http://<ip>:32400;
    proxy_set_header Host $host;
    proxy_set_header X-Real-IP $remote_addr;
    proxy_set_header X-Forwarded-For $proxy_add_x_forwarded_for;
  }

location /plex {
    proxy_pass http://<ip>:32400/web;
  }
```
## Transmission
```
location /trans {
    proxy_pass http://<ip>:9091;
    proxy_set_header Host $host;
    proxy_set_header X-Real-IP $remote_addr;
    proxy_set_header X-Forwarded-For $proxy_add_x_forwarded_for;
}
```

## Zigbee2mqqt
```
location /zigbee {
    proxy_pass http://<ip>:8081;
    proxy_set_header Host $host;
    proxy_set_header X-Real-IP $remote_addr;
    proxy_set_header X-Forwarded-For $proxy_add_x_forwarded_for;
}
```

## Code server
add a subdomain (e.g., code-server.domain.com)
```
location / {
    proxy_pass http://<ip>:8443;
    proxy_set_header Host $host;
    proxy_set_header X-Real-IP $remote_addr;
    proxy_set_header X-Forwarded-For $proxy_add_x_forwarded_for;
    proxy_set_header Referer “”;
    proxy_http_version 1.1;
    proxy_set_header Upgrade $http_upgrade;
    proxy_set_header Connection “upgrade”; 
  }

location = /robots.txt {
    add_header Content-Type text/plain;
    return 200 "User-agent: *\nDisallow: /\n";
 }

location ~ ^/(.*)
{
    # Connect to local port
    proxy_pass http://<ip>:8443;
}
```

## home assistant
add a subdomain (e.g., ha.domain.com)\
```
 location / {
    proxy_pass http://<ip>:8123;
    proxy_set_header Host $host;
    proxy_set_header X-Real-IP $remote_addr;
    proxy_set_header X-Forwarded-For $proxy_add_x_forwarded_for;
  }

location /api/websocket {
    proxy_pass http://<ip>:8123/api/websocket;
    proxy_set_header Host $host;
    proxy_set_header X-Real-IP $remote_addr;
    proxy_set_header X-Forwarded-For $proxy_add_x_forwarded_for;
    proxy_http_version 1.1;
    proxy_set_header Upgrade $http_upgrade;
    proxy_set_header Connection "upgrade";
    }
 ```
 add this to ```configuration.yaml``` of your HA
 ```bash
 http:
  use_x_forwarded_for: true
  trusted_proxies: 
    - 192.168.1.17/32 #nginx proxy manager ip
  #ip_ban_enabled: true 	        # use this to enable auto IP ban
  #login_attempts_threshold: 3 
  ```
 
 ## Vaultwarden
 add subdomain (e.g., vw.domain.com)
 ```
 location /admin {
  return 404;
  }

location / {
    proxy_pass http://<ip>:8088;
    proxy_set_header Host $host;
    proxy_set_header X-Real-IP $remote_addr;
    proxy_set_header X-Forwarded-For $proxy_add_x_forwarded_for;
    proxy_set_header X-Forwarded-Proto $scheme;
  }
  
 location /notifications/hub {
    proxy_pass http://<ip>:3012;
    proxy_set_header Upgrade $http_upgrade;
    proxy_set_header Connection "upgrade";
  }
  
 location /notifications/hub/negotiate {
    proxy_set_header Host $host;
    proxy_set_header X-Real-IP $remote_addr;
    proxy_set_header X-Forwarded-For $proxy_add_x_forwarded_for;
    proxy_set_header X-Forwarded-Proto $scheme;
    proxy_pass http://<ip>:8088;
 }
```
