# Pi-hole

**[English](#english)** | **[Русский](#russian)**

> I've moved to AdGuard Home. See [docker/adguard](https://github.com/ageev/SmartHome/tree/master/docker/adguard).
> Я переехал на AdGuard Home, см. [docker/adguard](https://github.com/ageev/SmartHome/tree/master/docker/adguard).

---

<a id="english"></a>

## English

### Setting up Pi-hole on Synology with docker using macvlan

There are several ways to install Pi-hole: native app, virtual machine, docker host network, docker macvlan. I tried them all — docker macvlan is the best (macvlan = a separate virtual network adapter). Only the last option lets you run Pi-hole on ports 80/443.

### Step 0. Preparing the environment

1. Create a `docker` folder on the NAS. It will store container configs.
2. Install docker in DSM.
3. Create a `docker` user. Set permissions accordingly (write access to the docker folder, remove everything else).
4. Get user ID and group ID via `id <docker_username>`. Note the UID and GID.
5. Create a `pihole` folder inside `docker`, and a `dnsmasq` folder inside `pihole`.

### Step 1. docker-compose.yml

Copy this file into your docker folder (probably `/volume1/docker`):

```yaml
version: "2"
services:
  pihole:
    image: pihole/pihole:latest
    container_name: pihole
    hostname: pihole
    domainname: local
    mac_address: 00:fa:c0:fa:c0:aa
    cap_add:
      - NET_ADMIN
    networks:
      pihole_network:
         ipv4_address: 192.168.7.8
    dns:
      - 192.168.7.1
      - 1.1.1.1
    environment:
      - PUID=1029   # put your PUID here
      - PGID=100
      - TZ=Europe/Amsterdam
      - WEBPASSWORD=p@ssw0rd       # your password
      - DNSSEC=true
      - DNS_BOGUS_PRIV=true
      - DNS_FQDN_REQUIRED=true
      - REV_SERVER=true
      - REV_SERVER_DOMAIN=local
      - REV_SERVER_TARGET=192.168.7.1     # router's IP
      - REV_SERVER_CIDR=192.168.0.0/16    # your LAN subnet
      - ServerIP=192.168.7.8               # pi-hole IP (again)
      - DNS1=208.67.222.222   # OpenDNS
      - DNS2=1.1.1.1          # Cloudflare
    volumes:
      - /volume1/docker/pihole:/etc/pihole/
      - /volume1/docker/pihole/dnsmasq:/etc/dnsmasq.d/
    restart: unless-stopped

networks:
  pihole_network:
    driver: macvlan
    driver_opts:
      parent: ovs_eth0
    ipam:
      config:
        - subnet: 192.168.7.0/24
          gateway: 192.168.7.1
          # ip_range: 192.168.0.0/16
```

Then SSH into the NAS:

```bash
cd /volume1/docker
sudo docker-compose up -d
```

Done. To upgrade the container, stop it first, run `sudo docker system prune -a` (this leaves the Pi-hole config/logs folder untouched), and bring it up again.

### Step 2. Importing lists

The default Pi-hole lists are too permissive. My average ad-blocking rate on a typical home network is around 20% with the defaults; 40–80% with additional lists.

Download the Pi-hole `.tar.gz` archive. Go to **Pi-hole admin → Settings → Teleporter → Restore** and import it.

It may block something important — open the dashboard, scroll the logs, and build your own whitelist.

### Step 3. Adding a route to docker's macvlan adapter

Docker's macvlan adapters are visible to LAN users but not to DSM/Docker itself. Usually that's fine, but if (like me) you want a Home Assistant container on the same docker host to reach Pi-hole, you need this.

1. Create a file in `/usr/local/etc/rc.d` with a `.sh` extension (this is a DSM autostart script directory).
2. `chmod 755 <filename>`.
3. Adjust and paste:

```bash
#!/bin/bash

if [ "$1" = "start" ]; then
    sleep 60
    ip link add dlink0 link ovs_eth0 type macvlan mode bridge
    ip addr add 192.168.7.127/32 dev dlink0
    ip link set dlink0 up
    ip route add 192.168.7.8/32 dev dlink0
fi
```

| Token | Meaning |
|---|---|
| `192.168.7.127/32` | A random IP to use for the virtual adapter. |
| `dlink0` | New virtual network interface name. |
| `ovs_eth0` | Synology DSM network interface name. |
| `192.168.7.8/32` | The Pi-hole container's IP. |

The `sleep 60` is needed for the script to work — without it the commands run too early and fail.

### Step 4. Customising docker-compose.yml

The official documentation lists every parameter. In my case the DNS settings weren't preserved across container restarts, which is why I wired them into the container configuration directly.

### Links

- Synology DSM developer guide — <https://www.synology.com/en-us/support/developer#tool>
- <https://github.com/pi-hole/docker-pi-hole>

---

<a id="russian"></a>

## Русский

### Установка Pi-hole на Synology в docker через macvlan

Pi-hole можно поставить несколькими способами: нативное приложение, виртуалка, docker host, docker macvlan. Я попробовал всё — docker macvlan лучший (macvlan = отдельный виртуальный сетевой адаптер). Только этот вариант позволяет запустить Pi-hole на открытых портах 80/443.

### Шаг 0. Подготовка окружения

1. Создайте папку `docker` на НАСе для конфигов контейнеров.
2. Установите docker в DSM.
3. Заведите пользователя `docker`. Выставьте права (запись в папку docker, всё остальное снять).
4. Получите UID и GID командой `id <docker_username>`. Запишите их.
5. Внутри `docker` создайте папку `pihole`, внутри `pihole` — папку `dnsmasq`.

### Шаг 1. docker-compose.yml

Скопируйте файл в папку docker (скорее всего `/volume1/docker`):

```yaml
version: "2"
services:
  pihole:
    image: pihole/pihole:latest
    container_name: pihole
    hostname: pihole
    domainname: local
    mac_address: 00:fa:c0:fa:c0:aa
    cap_add:
      - NET_ADMIN
    networks:
      pihole_network:
         ipv4_address: 192.168.7.8
    dns:
      - 192.168.7.1
      - 1.1.1.1
    environment:
      - PUID=1029   # ваш PUID
      - PGID=100
      - TZ=Europe/Amsterdam
      - WEBPASSWORD=p@ssw0rd       # ваш пароль
      - DNSSEC=true
      - DNS_BOGUS_PRIV=true
      - DNS_FQDN_REQUIRED=true
      - REV_SERVER=true
      - REV_SERVER_DOMAIN=local
      - REV_SERVER_TARGET=192.168.7.1     # IP роутера
      - REV_SERVER_CIDR=192.168.0.0/16    # ваша LAN-подсеть
      - ServerIP=192.168.7.8               # IP Pi-hole (ещё раз)
      - DNS1=208.67.222.222   # OpenDNS
      - DNS2=1.1.1.1          # Cloudflare
    volumes:
      - /volume1/docker/pihole:/etc/pihole/
      - /volume1/docker/pihole/dnsmasq:/etc/dnsmasq.d/
    restart: unless-stopped

networks:
  pihole_network:
    driver: macvlan
    driver_opts:
      parent: ovs_eth0
    ipam:
      config:
        - subnet: 192.168.7.0/24
          gateway: 192.168.7.1
          # ip_range: 192.168.0.0/16
```

Затем по SSH:

```bash
cd /volume1/docker
sudo docker-compose up -d
```

Готово. Для апгрейда контейнера сначала остановите его, выполните `sudo docker system prune -a` (папка с конфигом и логами Pi-hole не пострадает) и поднимите контейнер снова.

### Шаг 2. Импорт списков

Дефолтные списки Pi-hole слишком мягкие. Средний процент блокировки рекламы на типичной домашней сети — около 20% на дефолте и 40–80% с дополнительными списками.

Скачайте архив `pi-hole....tar.gz`. В админке: **Settings → Teleporter → Restore** — и импортируйте.

Может заблокироваться что-то важное — открывайте дашборд, листайте логы и собирайте свой белый список.

### Шаг 3. Маршрут до macvlan-адаптера docker'а

Macvlan-адаптеры docker'а видны клиентам LAN, но не самому DSM/Docker. В большинстве случаев это не проблема, но если, например, нужно достучаться до Pi-hole из контейнера Home Assistant, который крутится на том же docker'е (как у меня), без этого не обойтись.

1. Создайте в `/usr/local/etc/rc.d` файл с расширением `.sh` (это автостарт-каталог DSM).
2. `chmod 755 <filename>`.
3. Поправьте и вставьте:

```bash
#!/bin/bash

if [ "$1" = "start" ]; then
    sleep 60
    ip link add dlink0 link ovs_eth0 type macvlan mode bridge
    ip addr add 192.168.7.127/32 dev dlink0
    ip link set dlink0 up
    ip route add 192.168.7.8/32 dev dlink0
fi
```

| Что | Зачем |
|---|---|
| `192.168.7.127/32` | Произвольный IP для виртуального адаптера. |
| `dlink0` | Имя нового виртуального сетевого интерфейса. |
| `ovs_eth0` | Имя сетевого интерфейса Synology DSM. |
| `192.168.7.8/32` | IP контейнера Pi-hole. |

`sleep 60` нужен, чтобы скрипт сработал — без задержки команды выполняются слишком рано и падают.

### Шаг 4. Доработка docker-compose.yml

Все параметры есть в официальной документации. У меня DNS-настройки не сохранялись при перезапуске контейнера — поэтому я зашил их прямо в конфиг контейнера.

### Ссылки

- Synology DSM developer guide — <https://www.synology.com/en-us/support/developer#tool>
- <https://github.com/pi-hole/docker-pi-hole>
