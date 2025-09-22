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
