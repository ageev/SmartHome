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

