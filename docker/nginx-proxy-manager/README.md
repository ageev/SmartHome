# Nginx Proxy Manager

**[English](#english)** | **[Русский](#russian)**

---

<a id="english"></a>

## English

### Contents

- [What it does](#what-it-does)
- [Setting up NPM with Synology](#setting-up-npm-with-synology)
- [docker-compose.yml (for VirtualDSM)](#docker-composeyml-for-virtualdsm)
- [Gandi DNS challenge](#gandi-dns-challenge)
- [Per-app proxy configs](#per-app-proxy-configs)

### What it does

Nginx Proxy Manager (NPM) is a tool that lets you:

1. Deploy HTTPS inside your network.
2. Publish things to the internet in a secure way.
3. Auto-manage HTTPS certificates (via Let's Encrypt ACME).

**Downsides:** most apps need a custom config to publish them properly. Sometimes it's a serious pain — Google "nginx proxy `<appname>`" and be ready to try a lot of configs. Traefik can route traffic directly from the docker socket, so I may move to Traefik one day.

### Setting up NPM with Synology

I want NPM to handle HTTP/HTTPS traffic directly on the standard ports (80/443, TCP). Unfortunately those aren't available to docker on a Synology NAS — DSM grabs them for itself (just to redirect to 5001…). Also, a regular docker container can't see macvlan docker containers (no local route), so you end up needing two docker hosts.

The best workaround I've found: install Synology VM Manager, set up Virtual DSM (you get one free license), set up another Docker instance inside the Virtual DSM, and run NPM there.

> **Update:** MariaDB is no longer required.

![NPM network scheme](https://github.com/ageev/SmartHome/raw/master/Pictures/npm.jpg)

**Downside:** you now have two dockers and two DSMs to manage.

### docker-compose.yml (for VirtualDSM)

```yaml
# Install new versions:
#   cd /volume1/docker
#   sudo docker-compose pull
#   sudo docker-compose up -d
# Clean docker:
#   sudo docker system prune -a
# Rebuild containers:
#   sudo docker-compose up -d --build

---
version: "3.9"
networks:
  macvlan_network:
    driver: macvlan
    enable_ipv6: false
    driver_opts:
      parent: eth0
    ipam:
      config:
        - subnet: 192.168.1.0/24
          gateway: 192.168.1.1
          ip_range: 192.168.1.16/29

services:
  # If Gandi certificates can't be issued, run:
  #   pip install six
  nginx-proxy-manager:
    image: 'jc21/nginx-proxy-manager'
    container_name: nginx-proxy-manager
    hostname: nginx_proxy_manager
    domainname: local
    mac_address: 00:fa:c0:fa:c0:aa
    cap_add: [NET_ADMIN]
    dns: 10.0.1.1
    environment:
      - TZ=Europe/Zurich
      - DISABLE_IPV6=true
    volumes:
      - /volume1/docker/nginx_proxy_manager/config.json:/app/config/production.json
      - /volume1/docker/nginx_proxy_manager/data:/data
      - /volume1/docker/nginx_proxy_manager/letsencrypt:/etc/letsencrypt
      - /volume1/docker/nginx_proxy_manager/websites:/opt/websites
    healthcheck:
      test: ["CMD", "/bin/check-health"]
      interval: 10s
      timeout: 3s
    restart: unless-stopped
    networks:
      macvlan_network:
        ipv4_address: 10.0.1.11
```

### Gandi DNS challenge

> Only relevant if you use Gandi for your domains, like I do.

Gandi tokens always have an expiry, so you need to rotate them at least yearly. Set up a root task in Synology Task Scheduler to rotate the token:

```bash
#!/bin/sh
set -e

# Path to credentials file inside NPM container
CREDENTIALS_FILE="/volume1/docker/nginx_proxy_manager/letsencrypt/credentials/credentials-1"

# Gandi API endpoint for renewing PAT
# Replace <TOKEN_ID> with your actual PAT ID
GANDI_PAT_RENEW_URL="https://api.gandi.net/v5/organization/access-tokens"

# Extract current token from credentials file
CURRENT_PAT=$(awk -F= '/dns_gandi_token/ {print $2}' "$CREDENTIALS_FILE" | tr -d ' ')

# Request new PAT from Gandi
NEW_PAT=$(curl -s -X POST \
  -H "Authorization: Bearer ${CURRENT_PAT}" \
  -H "Content-Type: application/json" \
  "$GANDI_PAT_RENEW_URL" | jq -r '.access_token')

# Overwrite credentials file with new token
echo "dns_gandi_token=${NEW_PAT}" > "$CREDENTIALS_FILE"
```

### Per-app proxy configs

In NPM, add a URL. Enable everything in the SSL tab, enable Web Sockets. Paste the configs below into the **Advanced** tab. Replace `<ip>` with the real IP.

#### Radarr

```nginx
# Radarr. In Radarr settings, set "URL base" to "/radarr".
location /radarr {
    proxy_pass http://<ip>:7878;
    proxy_set_header Host $host;
    proxy_set_header X-Real-IP $remote_addr;
    proxy_set_header X-Forwarded-For $proxy_add_x_forwarded_for;
}
```

#### Prowlarr

```nginx
# Prowlarr. In Prowlarr settings, set "URL base" to "/prowlarr".
location /prowlarr {
    proxy_pass http://<ip>:9696;
    proxy_set_header Host $host;
    proxy_set_header X-Real-IP $remote_addr;
    proxy_set_header X-Forwarded-For $proxy_add_x_forwarded_for;
}
```

#### Plex

```nginx
# Plex. Settings → Network → Custom server access URLs: set 4a.pm/plex
location /web {
    proxy_pass http://<ip>:32400;
    proxy_set_header Host $host;
    proxy_set_header X-Real-IP $remote_addr;
    proxy_set_header X-Forwarded-For $proxy_add_x_forwarded_for;
}

location /plex {
    proxy_pass http://<ip>:32400/web;
}
```

#### Transmission

```nginx
location /trans {
    proxy_pass http://<ip>:9091;
    proxy_set_header Host $host;
    proxy_set_header X-Real-IP $remote_addr;
    proxy_set_header X-Forwarded-For $proxy_add_x_forwarded_for;
}
```

#### Zigbee2MQTT

```nginx
location /zigbee {
    proxy_pass http://<ip>:8081;
    proxy_set_header Host $host;
    proxy_set_header X-Real-IP $remote_addr;
    proxy_set_header X-Forwarded-For $proxy_add_x_forwarded_for;
}
```

#### Code-server

Add a subdomain (e.g. `code-server.domain.com`):

```nginx
location / {
    proxy_pass http://<ip>:8443;
    proxy_set_header Host $host;
    proxy_set_header X-Real-IP $remote_addr;
    proxy_set_header X-Forwarded-For $proxy_add_x_forwarded_for;
    proxy_set_header Referer "";
    proxy_http_version 1.1;
    proxy_set_header Upgrade $http_upgrade;
    proxy_set_header Connection "upgrade";
}

location = /robots.txt {
    add_header Content-Type text/plain;
    return 200 "User-agent: *\nDisallow: /\n";
}

location ~ ^/(.*) {
    # Connect to local port
    proxy_pass http://<ip>:8443;
}
```

#### Home Assistant

Add a subdomain (e.g. `ha.domain.com`):

```nginx
location / {
    proxy_pass http://<ip>:8123;
    proxy_set_header Host $host;
    proxy_set_header X-Real-IP $remote_addr;
    proxy_set_header X-Forwarded-For $proxy_add_x_forwarded_for;
}

location /api/websocket {
    proxy_pass http://<ip>:8123/api/websocket;
    proxy_set_header Host $host;
    proxy_set_header X-Real-IP $remote_addr;
    proxy_set_header X-Forwarded-For $proxy_add_x_forwarded_for;
    proxy_http_version 1.1;
    proxy_set_header Upgrade $http_upgrade;
    proxy_set_header Connection "upgrade";
}
```

Add this to `configuration.yaml` on the Home Assistant side:

```yaml
http:
  use_x_forwarded_for: true
  trusted_proxies:
    - 192.168.1.17/32   # nginx proxy manager IP
  # ip_ban_enabled: true            # enable auto IP ban
  # login_attempts_threshold: 3
```

#### Vaultwarden

Add a subdomain (e.g. `vw.domain.com`):

```nginx
location /admin {
    return 404;
}

location / {
    proxy_pass http://<ip>:8088;
    proxy_set_header Host $host;
    proxy_set_header X-Real-IP $remote_addr;
    proxy_set_header X-Forwarded-For $proxy_add_x_forwarded_for;
    proxy_set_header X-Forwarded-Proto $scheme;
}

location /notifications/hub {
    proxy_pass http://<ip>:3012;
    proxy_set_header Upgrade $http_upgrade;
    proxy_set_header Connection "upgrade";
}

location /notifications/hub/negotiate {
    proxy_set_header Host $host;
    proxy_set_header X-Real-IP $remote_addr;
    proxy_set_header X-Forwarded-For $proxy_add_x_forwarded_for;
    proxy_set_header X-Forwarded-Proto $scheme;
    proxy_pass http://<ip>:8088;
}
```

---

<a id="russian"></a>

## Русский

### Содержание

- [Что это](#что-это)
- [Установка NPM на Synology](#установка-npm-на-synology)
- [docker-compose.yml (для VirtualDSM)](#docker-composeyml-для-virtualdsm)
- [Gandi DNS challenge](#gandi-dns-challenge-1)
- [Конфиги по приложениям](#конфиги-по-приложениям)

### Что это

Nginx Proxy Manager (NPM) — инструмент, который позволяет:

1. Развернуть HTTPS внутри сети.
2. Безопасно опубликовать что-то наружу.
3. Автоматически управлять HTTPS-сертификатами (через ACME-бот Let's Encrypt).

**Минусы:** для большинства приложений нужен кастомный конфиг. Иногда это серьёзная боль — гуглите «nginx proxy `<имя_приложения>`» и будьте готовы перепробовать кучу вариантов. Traefik умеет проксировать прямо по docker-сокету, так что когда-нибудь, может, перееду на него.

### Установка NPM на Synology

Я хочу, чтобы NPM напрямую обрабатывал HTTP/HTTPS-трафик на стандартных портах (80/443, TCP). К сожалению, эти порты в Synology занят DSM (он лишь редиректит на 5001…). Плюс обычный docker-контейнер не видит macvlan-контейнеры (нет локального маршрута) — поэтому в итоге нужно два docker-хоста.

Лучшее решение из найденных: поставить Synology VM Manager, настроить Virtual DSM (одна лицензия идёт бесплатно), внутри Virtual DSM поднять отдельный Docker и крутить NPM там.

> **Обновление:** MariaDB больше не нужна.

![Схема сети NPM](https://github.com/ageev/SmartHome/raw/master/Pictures/npm.jpg)

**Минус:** теперь нужно управлять двумя docker'ами и двумя DSM.

### docker-compose.yml (для VirtualDSM)

```yaml
# Установить новые версии:
#   cd /volume1/docker
#   sudo docker-compose pull
#   sudo docker-compose up -d
# Очистить docker:
#   sudo docker system prune -a
# Пересобрать контейнеры:
#   sudo docker-compose up -d --build

---
version: "3.9"
networks:
  macvlan_network:
    driver: macvlan
    enable_ipv6: false
    driver_opts:
      parent: eth0
    ipam:
      config:
        - subnet: 192.168.1.0/24
          gateway: 192.168.1.1
          ip_range: 192.168.1.16/29

services:
  # Если сертификаты Gandi не выпускаются:
  #   pip install six
  nginx-proxy-manager:
    image: 'jc21/nginx-proxy-manager'
    container_name: nginx-proxy-manager
    hostname: nginx_proxy_manager
    domainname: local
    mac_address: 00:fa:c0:fa:c0:aa
    cap_add: [NET_ADMIN]
    dns: 10.0.1.1
    environment:
      - TZ=Europe/Zurich
      - DISABLE_IPV6=true
    volumes:
      - /volume1/docker/nginx_proxy_manager/config.json:/app/config/production.json
      - /volume1/docker/nginx_proxy_manager/data:/data
      - /volume1/docker/nginx_proxy_manager/letsencrypt:/etc/letsencrypt
      - /volume1/docker/nginx_proxy_manager/websites:/opt/websites
    healthcheck:
      test: ["CMD", "/bin/check-health"]
      interval: 10s
      timeout: 3s
    restart: unless-stopped
    networks:
      macvlan_network:
        ipv4_address: 10.0.1.11
```

### Gandi DNS challenge

> Актуально, только если вы используете Gandi (как я).

Токены Gandi всегда с истечением, так что их нужно ротировать минимум раз в год. Заведите root-задачу в планировщике Synology:

```bash
#!/bin/sh
set -e

# Путь к файлу с credentials внутри контейнера NPM
CREDENTIALS_FILE="/volume1/docker/nginx_proxy_manager/letsencrypt/credentials/credentials-1"

# Эндпоинт API Gandi для ротации PAT.
# Замените <TOKEN_ID> на реальный ID вашего PAT.
GANDI_PAT_RENEW_URL="https://api.gandi.net/v5/organization/access-tokens"

# Текущий токен
CURRENT_PAT=$(awk -F= '/dns_gandi_token/ {print $2}' "$CREDENTIALS_FILE" | tr -d ' ')

# Запрос нового PAT
NEW_PAT=$(curl -s -X POST \
  -H "Authorization: Bearer ${CURRENT_PAT}" \
  -H "Content-Type: application/json" \
  "$GANDI_PAT_RENEW_URL" | jq -r '.access_token')

# Перезаписать файл новым токеном
echo "dns_gandi_token=${NEW_PAT}" > "$CREDENTIALS_FILE"
```

### Конфиги по приложениям

В NPM добавьте URL. Во вкладке SSL включите всё, включите Web Sockets. Конфиги ниже вставляйте во вкладку **Advanced**. `<ip>` замените на реальный IP.

Конфиги для Radarr, Prowlarr, Plex, Transmission, Zigbee2MQTT, Code-server, Home Assistant и Vaultwarden — идентичны англоязычной секции выше; ниже только примечания, специфичные для Home Assistant и Vaultwarden.

**Home Assistant:** не забудьте дописать в `configuration.yaml`:

```yaml
http:
  use_x_forwarded_for: true
  trusted_proxies:
    - 192.168.1.17/32   # IP Nginx Proxy Manager
  # ip_ban_enabled: true            # автоматический бан по IP
  # login_attempts_threshold: 3
```

**Vaultwarden:** в конфиге обязательно блок `location /admin { return 404; }`, чтобы закрыть админку наружу.
