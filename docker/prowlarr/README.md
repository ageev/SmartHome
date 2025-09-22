# Prowlarr
Торент-трекер-интегратор. Единая консоль, через которую можно искать файлы на всех своих торент-трекерах.

# docker-compose.yml

```yml
---
version: "3.9"
services:
  prowlarr:
    image: linuxserver/prowlarr
    container_name: prowlarr
    environment:
      - PUID=1028
      - PGID=100
      - UMASK=002
      - TZ=Europe/Zurich
    volumes:
      - /volume2/docker/prowlarr:/config
    ports:
      - "9696:9696"
    restart: unless-stopped
```
# Nginx Proxy Manager Configuration
Чтобы контейнер был доступен по адресу ```your-domain.com/prowlarr" добавьте это в Advanced -> Custom Nginx Configuration в настройках контейнера Nginx Proxy Manager
```
# Radarr. Go to settings in Radarr and set "URL base" to "/radarr"
location /radarr {
    allow 10.0.0.0/8;
    deny all;

    proxy_pass http://10.0.1.5:7878; # NAS IP

    proxy_set_header Host $host;
    proxy_set_header X-Real-IP $remote_addr;
    proxy_set_header X-Forwarded-For $proxy_add_x_forwarded_for;
    proxy_set_header X-Forwarded-Proto $scheme;

    proxy_http_version 1.1;
    proxy_set_header Connection "";

    proxy_hide_header X-Powered-By;
    proxy_hide_header Server;

    proxy_redirect off;
}
```
