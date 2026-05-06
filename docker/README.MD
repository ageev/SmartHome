# Docker — Smart Home Containers

**[English](#english)** | **[Русский](#russian)**

---

<a id="english"></a>

## English

### Container index

| Container | Status | Purpose |
|---|---|---|
| [3x-ui](./3x-ui) | active | VPN server using VLESS for access to LAN, YouTube, and everything else |
| [acme.sh](./acme.sh) | active | Automates HTTPS certificate issuance, sends Telegram notifications, can push renewed certs straight into DSM |
| [adguard](./adguard) | active | Advanced ad-blocker. Better than [Pi-hole](./pi-hole) |
| [caddy](./caddy) | inactive | Web server with ACME (auto HTTPS) — *replaced with Nginx Proxy Manager + acme.sh* |
| [esphome](./esphome) | active | Firmware builder and dashboard for ESP32 (cheap chip with WiFi + BLE). Lets you re-flash various Tuya Smart and similar devices for Home Assistant integration |
| [home-assistant](./home-assistant) | active | Universal smart home integration hub. Tons of addons let you wire very different components together — e.g. Yandex Station and Telegram, IKEA sensors and a Xiaomi fan. Automation examples inside |
| [homer](./homer) | active | Simple web server, perfect for a smart-home start page. When I need to open something on the local network, I use the Homer start page |
| [nginx-proxy-manager](./nginx-proxy-manager) | active | One-stop (almost) shop for all internal web portals. Adds the "green padlock" (HTTP -> HTTPS) where it never existed. The downside: you need a unique custom proxy config for each app. Alternative: Traefik |
| [plex](./plex) | inactive | Video catalog. Transcodes streams on the fly (if hardware supports it — DS218+ does). Means you can stream cartoons to kids' tablets and they pick what to watch. I rarely use it — mostly we watch movies via NFS shares + Kodi |
| [prowlarr](./prowlarr) | active | Torrent tracker integrator. Tell it to find a torrent and it searches all your favorite trackers at once. Used together with Radarr |
| [mosquitto](./zigbee2mqtt) | inactive | MQTT broker. Needed for zigbee2mqtt. Handles the request queue to ZigBee devices |
| [paperless](./paperless) | active | Home document archive. Integrates nicely with the scanner — keep copies of all your paper documents (including warranty receipts) |
| [qBittorrent](./qbittorrent) | active | Torrent client. I prefer the GUI over Transmission, plus there's Telegram integration |
| [radarr](./radarr) | active | Home video library manager (I wrote about it [here](https://habr.com/ru/post/505814/)) |
| [snowflake-proxy](./snowflake-proxy) | active | I have a fat internet pipe so I donate part of it to Tor |
| [torrserver](./torrserver) | active | Server for NUM. On the road I bring an Amazon Fire TV stick — plug it into a hotel TV and watch your Netflix. NUM + a home torrserver lets you also watch torrents in countries where that gets you in trouble |
| [transmission](./transmission) | inactive | Torrent client. I picked it because it has the best HA integration. Used with the Torrent Control browser plugin to send everything straight to the NAS |
| [vaultwarden](./vaultwarden) | active | BitWarden fork — a great password manager with clients for every platform |
| [wyl](./wyl) | active | Simple container that lets me see all devices on the local network |
| [zerotier](./zerotier) | active | Sort-of-VPN that puts very different systems into one virtual network. I use it for NAS-to-NAS backups |
| [zigbee2mqtt](./zigbee2mqtt) | inactive | Integrates Zigbee smart-home devices with Home Assistant. Works with the Sonoff Zigbee 3.0 USB stick plugged into the NAS. In principle the built-in Home Assistant Zigbee module would also work |

### The `docker` user

I created a dedicated DSM user named `docker`. This user only has access to the `docker`, `scans` (for paperless), and `media` folders. Access to all DSM apps is denied. Containers run as the `docker` user (not all containers support this!) via the `PUID` and `PGID` env vars. Find your user/group IDs with `sudo synouser --get docker`. A more secure approach: create a unique user per container.

### Folder structure for docker and docker-compose

```
/volume2 #ssd
└── docker
    ├── container1
    ├── container2
    ├── ...
    └── docker-compose.yml
```

I don't use the Synology GUI to manage containers at all. Configuration happens either through `docker-compose.yml` or through individual config files in each container's folder.
To update containers, I SSH to the NAS:

```bash
cd /volume2/docker
sudo docker-compose pull          # pull new images
sudo docker-compose up -d          # create or recreate containers
sudo docker system prune -a        # remove cache, old images, unused containers
```

### Blocking internet access FROM docker containers

Containers shouldn't be allowed to reach the internet unless they absolutely must.

1. Create two separate Docker networks in `docker-compose.yml`:

   ```yaml
   networks:
     internet_allowed:
       driver: bridge
       ipam:
         config:
           - subnet: 172.18.0.0/16
     internet_blocked:
       driver: bridge
       ipam:
         config:
           - subnet: 172.19.0.0/16
   ```

2. For containers that don't need internet, attach them to `internet_blocked`:

   ```yaml
   services:
     container_name:
       ...
       networks:
         - internet_blocked
   ```

3. Go to the Synology task scheduler and create a *root task* triggered after boot:

   > Warning: somehow the redirect to the `DOCKER-USER` chain got disabled with a docker upgrade on Synology (bug? feature?). I added it back manually with `sudo iptables -I FORWARD 1 -j DOCKER-USER`.

   ```bash
   # May be needed due to Synology bug/feature
   #iptables -I FORWARD 1 -j DOCKER-USER
   iptables -F DOCKER-USER
   iptables -A DOCKER-USER -s 172.19.0.0/16 -d 192.168.0.0/16 -j RETURN
   iptables -A DOCKER-USER -s 172.19.0.0/16 -d 172.16.0.0/12 -j RETURN
   iptables -A DOCKER-USER -s 172.19.0.0/16 -d 10.0.0.0/8 -j RETURN
   iptables -A DOCKER-USER -s 172.19.0.0/16 -j DROP
   ```

View routes:

```bash
sudo iptables -vL -n --line-numbers
```

You should see something like this:

```
# traffic first hits FORWARD chain, then is redirected to DEFAULT_FORWARD.. looks like it's Synology behaviour
Chain FORWARD (policy ACCEPT 2949 packets, 2455K bytes)
num   pkts bytes target     prot opt in     out     source               destination
1     2949 2455K DEFAULT_FORWARD  all  --  *      *       0.0.0.0/0            0.0.0.0/0
...
# chain Default_forward sends the traffic further to DOCKER-USER
Chain DEFAULT_FORWARD (1 references)
num   pkts bytes target     prot opt in     out     source               destination
1     2949 2455K DOCKER-USER  all  --  *      *       0.0.0.0/0            0.0.0.0/0
...

Chain DOCKER-USER (1 references)
num   pkts bytes target     prot opt in     out     source               destination
1        0     0 RETURN     all  --  *      *       172.19.0.0/16        192.168.0.0/16
2     5925  901K RETURN     all  --  *      *       172.19.0.0/16        172.16.0.0/12
3        3   252 RETURN     all  --  *      *       172.19.0.0/16        10.0.0.0/8
4        9   756 DROP       all  --  *      *       172.19.0.0/16        0.0.0.0/0
# after the last rule traffic returns to FORWARD chain back
...
```

> Note: Synology's iptables doesn't support `REJECT`, so I used `DROP` instead. `REJECT` is almost instant; `DROP` cancels the connection attempt after a few seconds of delay.

---

<a id="russian"></a>

## Русский

### Текущий состав контейнеров

| Контейнер | Статус | Назначение |
|---|---|---|
| [3x-ui](./3x-ui) | активен | ВПН-сервер на протоколе VLESS для доступа к локальной сети, YouTube и всему остальному |
| [acme.sh](./acme.sh) | активен | Автоматическое получение HTTPS сертификатов, уведомления в Telegram, заливка обновлённого сертификата прямо в DSM |
| [adguard](./adguard) | активен | Продвинутая резалка рекламы. Лучше чем [Pi-hole](./pi-hole) |
| [caddy](./caddy) | неактивен | Вебсервер с поддержкой ACME — *заменил на Nginx Proxy Manager + acme.sh* |
| [esphome](./esphome) | активен | Генератор прошивок и дашборд для ESP32 (дешёвый чип с Wi-Fi и BLE). Позволяет перепрошивать всякие Tuya Smart и прочие девайсы для интеграции с Home Assistant |
| [home-assistant](./home-assistant) | активен | Универсальный интеграционный сервис умного дома. За счёт огромного количества аддонов позволяет связывать в единую систему очень разные компоненты. Например, Яндекс Станцию и Telegram, датчики IKEA и вентилятор Xiaomi. Примеры автоматизаций внутри |
| [homer](./homer) | активен | Простой вебсервер, на котором удобно сделать стартовую страницу умного дома |
| [nginx-proxy-manager](./nginx-proxy-manager) | активен | Единое окно (ну почти) для всех внутренних веб-порталов. Позволяет сделать «зелёный замочек» (HTTP -> HTTPS) там, где его отродясь не было. Один минус — нужно искать уникальный кастомный прокси-конфиг под каждое приложение. Альтернатива — Traefik |
| [plex](./plex) | неактивен | Каталогизатор видео. Позволяет на лету транскодировать видеопотоки (если железо позволяет; DS218+ позволяет) под различные разрешения и устройства. Пользуюсь редко. В основном фильмы смотрим через NFS-шары и Kodi |
| [prowlarr](./prowlarr) | активен | Интегратор торрент-трекеров. Даёшь ему команду найти торрент — он ищет сразу по всем твоим любимым трекерам. Используется в связке с Radarr |
| [mosquitto](./zigbee2mqtt) | неактивен | MQTT-брокер. Нужен для zigbee2mqtt. Работает с очередью запросов к ZigBee-устройствам умного дома |
| [paperless](./paperless) | активен | Домашняя система архива документов. Удобно интегрируется со сканером и позволяет хранить копии всех бумажных документов (включая кассовые чеки для гарантий) |
| [qBittorrent](./qbittorrent) | активен | Торрент-клиент. Нравится GUI больше, чем у Transmission + есть интеграция с Telegram |
| [radarr](./radarr) | активен | Менеджер домашней видеобиблиотеки (тут писал про него [подробнее](https://habr.com/ru/post/505814/)) |
| [snowflake-proxy](./snowflake-proxy) | активен | У меня жирный интернет-канал, поэтому часть трафика я даю проекту TOR |
| [torrserver](./torrserver) | активен | Сервер для NUM. В путешествиях я вожу с собой Amazon Fire TV стик. Воткнул в телевизор в отеле — и можно смотреть свой Netflix. NUM в связке с домашним torrserver позволяет смотреть еще и торренты в странах, где за это наказывают |
| [transmission](./transmission) | неактивен | Качалка торрентов. Выбрал её потому что у неё самая удобная интеграция с HA. Использую с плагином для браузера Torrent Control |
| [vaultwarden](./vaultwarden) | активен | Форк BitWarden — очень хороший менеджер паролей с клиентами под все платформы |
| [wyl](./wyl) | активен | Простой контейнер, который позволяет видеть все устройства в локальной сети |
| [zerotier](./zerotier) | активен | Как-бы-ВПН, который позволяет засунуть очень разные системы в одну виртуальную сеть. Использую для бэкапа с НАСа на НАС |
| [zigbee2mqtt](./zigbee2mqtt) | неактивен | Интеграция Zigbee-устройств с Home Assistant. Работает с USB-стиком Sonoff Zigbee 3.0, воткнутым в USB порт НАСа. В целом, можно было использовать и встроенный в Home Assistant zigbee модуль |

### Пользователь docker

У меня создан специальный DSM-пользователь `docker`. Этот пользователь имеет доступ только к папке `docker`, `scans` (для paperless) и `media`. Доступ ко всем приложениям запрещён. Контейнеры запускаются с полномочиями пользователя `docker` (не все это поддерживают!) через переменные `PUID` и `PGID`. Узнать свой UID/GID можно через `sudo synouser --get docker`. Более безопасный подход — создавать уникального пользователя под каждый контейнер.

### Структура папок docker и docker-compose

```
/volume2 #ssd
└── docker
    ├── container1
    ├── container2
    ├── ...
    └── docker-compose.yml
```

Я совсем не использую GUI Synology для управления контейнерами. Соответственно, вся конфигурация контейнеров происходит либо через `docker-compose.yml`, либо через индивидуальные файлы конфигурации в папке контейнера.
Если мне нужно обновить контейнеры, я захожу на НАС через SSH:

```bash
cd /volume2/docker
sudo docker-compose pull           # скачать новые образы
sudo docker-compose up -d          # создать или пересоздать контейнеры
sudo docker system prune -a        # удалить кэш, старые образы, неиспользуемые контейнеры
```

### Блокировка интернет-доступа ИЗ docker-контейнеров

Контейнеры не должны иметь возможность ходить в интернет, если в этом нет крайней необходимости.

1. Создайте две отдельные сети в `docker-compose.yml`:

   ```yaml
   networks:
     internet_allowed:
       driver: bridge
       ipam:
         config:
           - subnet: 172.18.0.0/16
     internet_blocked:
       driver: bridge
       ipam:
         config:
           - subnet: 172.19.0.0/16
   ```

2. Для контейнеров без интернета добавьте:

   ```yaml
   services:
     container_name:
       ...
       networks:
         - internet_blocked
   ```

3. В планировщике Synology создайте *root task* с триггером «после загрузки»:

   > Внимание: после апгрейда docker на Synology редирект в цепочку `DOCKER-USER` иногда отключается (баг? фича?). Я добавляю его вручную: `sudo iptables -I FORWARD 1 -j DOCKER-USER`.

   ```bash
   #iptables -I FORWARD 1 -j DOCKER-USER
   iptables -F DOCKER-USER
   iptables -A DOCKER-USER -s 172.19.0.0/16 -d 192.168.0.0/16 -j RETURN
   iptables -A DOCKER-USER -s 172.19.0.0/16 -d 172.16.0.0/12 -j RETURN
   iptables -A DOCKER-USER -s 172.19.0.0/16 -d 10.0.0.0/8 -j RETURN
   iptables -A DOCKER-USER -s 172.19.0.0/16 -j DROP
   ```

Посмотреть маршруты:

```bash
sudo iptables -vL -n --line-numbers
```

> Примечание: iptables на Synology не поддерживает `REJECT`, поэтому используется `DROP`. `REJECT` почти мгновенный, `DROP` отменяет попытку соединения через несколько секунд задержки.
