# Prowlarr

**[English](#english)** | **[Русский](#russian)**

---

<a id="english"></a>

## English

A torrent-tracker aggregator. One console for searching across all of your torrent trackers at once.

### docker-compose.yml

```yaml
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

### Nginx Proxy Manager configuration

To expose Prowlarr on `your-domain.com/prowlarr`, add this to **Advanced → Custom Nginx Configuration** in the NPM container settings:

```nginx
# Prowlarr. In Prowlarr settings, set "URL base" to "/prowlarr".
location /prowlarr {
    allow 10.0.0.0/8;
    deny all;

    proxy_pass http://10.0.1.5:7878;  # NAS IP

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

---

<a id="russian"></a>

## Русский

Торрент-трекер-интегратор. Единая консоль, через которую можно искать файлы на всех своих торрент-трекерах.

### docker-compose.yml

```yaml
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

### Конфигурация Nginx Proxy Manager

Чтобы контейнер был доступен по адресу `your-domain.com/prowlarr`, добавьте это в **Advanced → Custom Nginx Configuration** в настройках контейнера NPM:

```nginx
# Prowlarr. В настройках Prowlarr выставите "URL base" в "/prowlarr".
location /prowlarr {
    allow 10.0.0.0/8;
    deny all;

    proxy_pass http://10.0.1.5:7878;  # IP НАСа

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
