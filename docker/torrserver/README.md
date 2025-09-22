# torrserver
[Торсервер](https://github.com/YouROK/TorrServer) работает в связке с [NUM](https://4pda.to/forum/index.php?showtopic=959756) чтобы стримить фильмы и сериалы через андроид ТВ приставки на телевизоры. 
В принципе, торсервер можно запускать и напрямую на стиках, но выделенный контейнер разгружает память стиков. 

# docker-compose.yml
```yml
---
version: "3.9"
services:
  torrserver:
    container_name: torrserver
    image: ghcr.io/yourok/torrserver:latest
    environment:
      - PUID=1028
      - PGID=100
      - TS_RDB=0
      - TS_HTTPAUTH=0
      - TS_DONTKILL=1
      - TS_CONF_PATH=/opt/ts/config
      - TS_TORR_DIR=/opt/ts/torrents
    ports:
      - 8090:8090
    volumes:
      - /volume2/docker/torrserver/:/opt/ts/
    restart: unless-stopped
```
