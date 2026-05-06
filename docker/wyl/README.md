# wyl (WatchYourLAN)

**[English](#english)** | **[Русский](#russian)**

---

<a id="english"></a>

## English

A small, convenient container that uses ARP scanning to list devices on the local network.

### docker-compose.yml

```yaml
wyl:
  image: aceberg/watchyourlan  # offline image with no connection to the internet
  container_name: wyl
  hostname: wyl
  environment:
    - PUID=1028                  # not implemented
    - PGID=100                   # not implemented
    - TZ=Europe/Zurich
    - IFACES=ovs_eth2            # required: 1 or more interfaces
    # - ARP_STRS_JOINED=-gNx 10.0.2.0/24 -I ovs_eth2  # if you have a guest network add it here
    - PORT=8840                  # optional, default: 8840
    - TIMEOUT=120                # optional, seconds, default: 120
    # - SHOUTRRR_URL=            # optional, URL for notifications
    - THEME=cerulean             # optional
    - COLOR=dark                 # optional
  volumes:
    - /volume2/docker/wyl:/data/WatchYourLAN
  restart: unless-stopped
  network_mode: "host"
```

### Nginx Proxy Manager configuration

To expose wyl on `wyl.your-domain.com`, add this config in [NPM](https://github.com/ageev/SmartHome/tree/master/docker/nginx-proxy-manager):

```nginx
location / {
    allow 10.0.0.0/8;
    deny all;

    proxy_set_header Host $host;
    proxy_set_header X-Real-IP $remote_addr;
    proxy_set_header X-Forwarded-For $proxy_add_x_forwarded_for;
    proxy_set_header X-Forwarded-Proto $scheme;

    proxy_redirect off;
    proxy_pass http://10.0.1.5:8840;  # NAS IP

    # Security hardening
    proxy_hide_header X-Powered-By;   # don't leak backend info
    proxy_hide_header Server;         # hide upstream server header
    proxy_http_version 1.1;           # needed for keep-alive
    proxy_set_header Connection "";   # prevent header injection
}
```

---

<a id="russian"></a>

## Русский

Удобный и простой контейнер, который с помощью ARP-сканирования показывает устройства в локальной сети.

### docker-compose.yml

```yaml
wyl:
  image: aceberg/watchyourlan  # офлайн-образ без обращений в интернет
  container_name: wyl
  hostname: wyl
  environment:
    - PUID=1028                  # не реализовано
    - PGID=100                   # не реализовано
    - TZ=Europe/Zurich
    - IFACES=ovs_eth2            # обязательно: один или несколько интерфейсов
    # - ARP_STRS_JOINED=-gNx 10.0.2.0/24 -I ovs_eth2  # если есть гостевая сеть
    - PORT=8840                  # опционально, по умолчанию 8840
    - TIMEOUT=120                # опционально, секунды, по умолчанию 120
    # - SHOUTRRR_URL=            # опционально, URL для уведомлений
    - THEME=cerulean             # опционально
    - COLOR=dark                 # опционально
  volumes:
    - /volume2/docker/wyl:/data/WatchYourLAN
  restart: unless-stopped
  network_mode: "host"
```

### Конфигурация Nginx Proxy Manager

Чтобы wyl был доступен по адресу `wyl.your-domain.com`, эту конфигурацию нужно добавить в [NPM](https://github.com/ageev/SmartHome/tree/master/docker/nginx-proxy-manager):

```nginx
location / {
    allow 10.0.0.0/8;
    deny all;

    proxy_set_header Host $host;
    proxy_set_header X-Real-IP $remote_addr;
    proxy_set_header X-Forwarded-For $proxy_add_x_forwarded_for;
    proxy_set_header X-Forwarded-Proto $scheme;

    proxy_redirect off;
    proxy_pass http://10.0.1.5:8840;  # IP НАСа

    # Hardening
    proxy_hide_header X-Powered-By;
    proxy_hide_header Server;
    proxy_http_version 1.1;
    proxy_set_header Connection "";
}
```
