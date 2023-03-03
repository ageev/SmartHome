# Plex

Плекс позволяет каталогизировать и стримить видео с транскодингом. Есть профили под каждого пользователя. Это, в целом, удобная штука, когда у вас в семье много устройств. Лично я понял, что пользуюсь, в основном, Kodi.
Пожизненная лицензия на Плекс стоит в районе 50-60 евро на Черную Пятницу. Чтобы нормально работать, Плексу нужна папка с фильмами. 

## Докер или Plex for Synology
Докер, т.к. это удобней, чаще выходят апдейты. Аппаратная поддержка транскодинга в докере работает (если ваш NAS её поддерживает).

## Установка
Вначале установите Plex из магазина Synology. При этом система создаст нужного служебного пользователя (PlexMediaServer) и даст нужные разрешения. 
Теперь удалите Plex и ставьте версию для докера.

## docker-compose.yml
```yml
---
version: "3.9"
services:
  plex:
    image: plexinc/pms-docker
    container_name: plex
    environment:
      - PLEX_UID=297536  # find your PlexMediaServer user ID with "id -u PlexMediaServer"
      - PLEX_GID=297536
#      - PLEX_CLAIM=claim-xxxx  #get yours here https://www.plex.tv/claim/ (4min lifetime!)
      - TZ=Europe/Zurich
      - VERSION=latest
      - ADVERTISE_IP=http://192.168.1.5:32400/
      - ALLOWED_NETWORKS=192.168.1.0/24,10.0.0.0/8 # add your VPN network here too
    volumes:
      - /volume1/docker/plex:/config
      - /volume1/media:/media
    devices:
      - /dev/dri:/dev/dri
    restart: unless-stopped
    network_mode: host
```
