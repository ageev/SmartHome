# Transmission
торрентокачалка. Использую потом что есть хорошая интеграция с Home Assistant. 
В браузерах стоит плагин Torren Control.

## Home Assistant интеграция
В HomAss есть такие вот автоматизации:
```
ha 
```

## Пробросить порт на роутере
Вы не сможете нормально качать, если не будете нормально отдавать. 
Чтобы нормально отдавать, нужно пробросить порт на домашнем роутере (port forwarding). 

## docker-compose.yml
```yml
---
version: "3.9"
services:
  transmission:
    image: linuxserver/transmission
    container_name: transmission
    environment:
      - PUID=1029 #change me!
      - PGID=100 #change me!
      - TZ=Europe/Zurich
    volumes:
      - /volume1/docker/transmission:/config
      - /volume1/media:/media
      - /volume1/media/watch:/watch
    ports:
      - 9091:9091
      - 31337:31337
      - 31337:31337/udp
    restart: unless-stopped
    network_mode: "bridge"
```
