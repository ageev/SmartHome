# torrserver

**[English](#english)** | **[Русский](#russian)**

---

<a id="english"></a>

## English

[TorrServer](https://github.com/YouROK/TorrServer) works in tandem with [NUM](https://4pda.to/forum/index.php?showtopic=959756) to stream movies and TV series through Android TV sticks onto televisions.

In principle TorrServer can run directly on the sticks themselves, but a dedicated container offloads memory pressure from the sticks.

### docker-compose.yml

```yaml
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

---

<a id="russian"></a>

## Русский

[TorrServer](https://github.com/YouROK/TorrServer) работает в связке с [NUM](https://4pda.to/forum/index.php?showtopic=959756) — стримит фильмы и сериалы через Android TV-приставки на телевизоры.

В принципе, TorrServer можно запускать и напрямую на стиках, но выделенный контейнер разгружает их память.

### docker-compose.yml

```yaml
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
