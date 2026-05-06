# Transmission

**[English](#english)** | **[Русский](#russian)**

> I currently use qBittorrent. Maybe one day I'll come back to Transmission. /
> Сейчас я использую qBittorrent. Может, когда-нибудь вернусь и на Transmission.

---

<a id="english"></a>

## English

A torrent client. I used it because of its solid Home Assistant integration. In the browser I run the Torrent Control plugin.

### Forwarding the port on the router

You won't get good download speeds without seeding properly. To seed properly, forward a port on your home router.

### docker-compose.yml

```yaml
---
version: "3.9"
services:
  transmission:
    image: linuxserver/transmission
    container_name: transmission
    environment:
      - PUID=1029  # change me!
      - PGID=100   # change me!
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

---

<a id="russian"></a>

## Русский

Торрент-качалка. Использовал из-за хорошей интеграции с Home Assistant. В браузере стоит плагин Torrent Control.

### Проброс порта на роутере

Вы не сможете нормально качать, если не будете нормально отдавать. Чтобы нормально отдавать, нужно пробросить порт на домашнем роутере (port forwarding).

### docker-compose.yml

```yaml
---
version: "3.9"
services:
  transmission:
    image: linuxserver/transmission
    container_name: transmission
    environment:
      - PUID=1029  # change me!
      - PGID=100   # change me!
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
