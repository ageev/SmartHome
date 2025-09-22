# wyl
удобный и простой контейнер, который с помощью ARP сканирования показывает устройства в локальной сети.

# docker-compose.yml

```yml
  wyl:
    image: aceberg/watchyourlan  # offline image with no connection to internet
    container_name: wyl
    hostname: wyl
    environment:
      - PUID=1028 # not implemented
      - PGID=100 # not implemented
      - TZ=Europe/Zurich
      - IFACES=ovs_eth2  # required: 1 or more interface
#      - ARP_STRS_JOINED=-gNx 10.0.2.0/24 -I ovs_eth2
      - PORT=8840                      # optional, default: 8840
      - TIMEOUT=120                    # optional, time in seconds, default: 120
#      - SHOUTRRR_URL=                  # optional, set url to notify
      - THEME=cerulean                     # optional
      - COLOR=dark                     # optional
    volumes:
      - /volume2/docker/wyl:/data/WatchYourLAN
    restart: unless-stopped
    network_mode: "host"
```

# Nginx Proxy Manager Configuration
Чтобы wyl был доступен по адрессу ```wyl.your-domain.com```, эта конфигурация должна быть добавлена в [NPM](https://github.com/ageev/SmartHome/tree/master/docker/nginx-proxy-manager):

```nginx
location / {
    allow 10.0.0.0/8;
    deny all;

    proxy_set_header Host $host;
    proxy_set_header X-Real-IP $remote_addr;
    proxy_set_header X-Forwarded-For $proxy_add_x_forwarded_for;
    proxy_set_header X-Forwarded-Proto $scheme;

    proxy_redirect off;
    proxy_pass http://10.0.1.5:8840; # NAS IP

    # Security hardening
    proxy_hide_header X-Powered-By; # don’t leak backend info
    proxy_hide_header Server;       # hide upstream server header
    proxy_http_version 1.1;         # needed for keep-alive
    proxy_set_header Connection ""; # prevent header injection
}
```
