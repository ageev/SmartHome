# Plex

**[English](#english)** | **[Русский](#russian)**

---

<a id="english"></a>

## English

Plex lets you catalogue and stream video with transcoding. There's a profile per user. Generally a comfortable solution when there are lots of devices in the household. Personally I realised I mostly use Kodi.

A lifetime Plex license runs about €50–60 on Black Friday. To work properly, Plex needs a folder with movies.

### Docker or Plex for Synology?

Docker — it's more convenient, updates ship faster, and hardware-accelerated transcoding works inside docker (if your NAS supports it).

### Installation

First install Plex from the Synology package centre. The system will create the required service user (`PlexMediaServer`) and assign the right permissions. Then uninstall Plex and install the docker version.

### docker-compose.yml

```yaml
---
version: "3.9"
services:
  plex:
    image: plexinc/pms-docker
    container_name: plex
    environment:
      - PLEX_UID=297536   # find your PlexMediaServer user ID with "id -u PlexMediaServer"
      - PLEX_GID=297536
      # - PLEX_CLAIM=claim-xxxx   # get yours at https://www.plex.tv/claim/ (4 min lifetime!)
      - TZ=Europe/Zurich
      - VERSION=latest
      - ADVERTISE_IP=http://192.168.1.5:32400/
      - ALLOWED_NETWORKS=192.168.1.0/24,10.0.0.0/8   # add your VPN network here too
    volumes:
      - /volume1/docker/plex:/config
      - /volume1/media:/media
    devices:
      - /dev/dri:/dev/dri
    restart: unless-stopped
    network_mode: host
```

### Nginx Proxy Manager configuration

To expose the container on `your-domain.com/plex`, add this to **Advanced → Custom Nginx Configuration** in the NPM container settings:

```nginx
# Plex. Settings → Network → Custom server access URLs: set your-domain.com/plex
location /web {
    proxy_pass http://10.0.1.5:32400;
    proxy_set_header Host $host;
    proxy_set_header X-Real-IP $remote_addr;
    proxy_set_header X-Forwarded-For $proxy_add_x_forwarded_for;
}

location /plex {
    proxy_pass http://10.0.1.5:32400/web;
}
```

---

<a id="russian"></a>

## Русский

Plex позволяет каталогизировать и стримить видео с транскодингом. Есть профили под каждого пользователя. В целом удобная штука, когда в семье много устройств. Лично я понял, что пользуюсь в основном Kodi.

Пожизненная лицензия Plex стоит примерно €50–60 на Чёрную пятницу. Чтобы нормально работать, Plex нужна папка с фильмами.

### Docker или Plex for Synology?

Docker — удобнее, апдейты выходят чаще, аппаратная поддержка транскодинга в docker работает (если её поддерживает сам НАС).

### Установка

Сначала поставьте Plex из магазина Synology — система создаст нужного служебного пользователя (`PlexMediaServer`) и выставит права. Потом удалите Plex и поставьте docker-версию.

### docker-compose.yml

```yaml
---
version: "3.9"
services:
  plex:
    image: plexinc/pms-docker
    container_name: plex
    environment:
      - PLEX_UID=297536   # ваш UID PlexMediaServer: id -u PlexMediaServer
      - PLEX_GID=297536
      # - PLEX_CLAIM=claim-xxxx   # https://www.plex.tv/claim/ (живёт 4 минуты!)
      - TZ=Europe/Zurich
      - VERSION=latest
      - ADVERTISE_IP=http://192.168.1.5:32400/
      - ALLOWED_NETWORKS=192.168.1.0/24,10.0.0.0/8   # добавьте сюда и VPN-подсеть
    volumes:
      - /volume1/docker/plex:/config
      - /volume1/media:/media
    devices:
      - /dev/dri:/dev/dri
    restart: unless-stopped
    network_mode: host
```

### Конфигурация Nginx Proxy Manager

Чтобы контейнер был доступен по адресу `your-domain.com/plex`, добавьте это в **Advanced → Custom Nginx Configuration** в настройках контейнера NPM:

```nginx
# Plex. Settings → Network → Custom server access URLs: your-domain.com/plex
location /web {
    proxy_pass http://10.0.1.5:32400;
    proxy_set_header Host $host;
    proxy_set_header X-Real-IP $remote_addr;
    proxy_set_header X-Forwarded-For $proxy_add_x_forwarded_for;
}

location /plex {
    proxy_pass http://10.0.1.5:32400/web;
}
```
