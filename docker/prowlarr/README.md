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
# Prowlarr. Go to settings in Prowlarr and set "URL base" to "/radarr"
location /prowlarr {
    proxy_pass http://10.0.1.5:9696;
    proxy_set_header Host $host;
    proxy_set_header X-Real-IP $remote_addr;
    proxy_set_header X-Forwarded-For $proxy_add_x_forwarded_for;
  }
```
