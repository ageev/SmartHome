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
    proxy_pass http://ip:8443;
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
    proxy_pass http://ip:8443;
}
```

## home assistant
add a subdomain (e.g., ha.domain.com)\
```
 location / {
    proxy_pass http://ip:8123;
    proxy_set_header Host $host;
    proxy_set_header X-Real-IP $remote_addr;
    proxy_set_header X-Forwarded-For $proxy_add_x_forwarded_for;
  }

location /api/websocket {
    proxy_pass http://ip:8123/api/websocket;
    proxy_set_header Host $host;
    proxy_set_header X-Real-IP $remote_addr;
    proxy_set_header X-Forwarded-For $proxy_add_x_forwarded_for;
    proxy_http_version 1.1;
    proxy_set_header Upgrade $http_upgrade;
    proxy_set_header Connection "upgrade";
    }
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
 ```
 
