# Radarr — media manager

**[English](#english)** | **[Русский](#russian)**

---

<a id="english"></a>

## English

Radarr is a very popular self-hosted package whose job is to manage downloading movies you want to watch. It runs in tandem with [Prowlarr](https://github.com/ageev/SmartHome/tree/master/docker/prowlarr) and [qBittorrent](https://github.com/ageev/SmartHome/tree/master/docker/qBittorrent).

The flow looks like this:

- I add a released/upcoming movie to my Watchlist via the IMDb app on my phone.
- Radarr syncs its local movie list with my personal IMDb watchlist.
- Radarr asks Prowlarr whether there are releases on the trackers in the right language and quality.
- If yes, it hands the torrent file to qBittorrent. If not, it starts watching for new releases.
- It pings a private Telegram channel about download start and finish.
- After download, Radarr keeps watching the trackers and upgrades the file if a better-quality release appears.

A very comfortable setup. If I want to watch something, I just "like" the movie on my phone and a few hours later the right release is already on the NAS. If I'm waiting for a new movie, it shows up a couple hours after the trackers release it.

### docker-compose.yml

```yaml
---
version: "3.9"
services:
  radarr:
    image: linuxserver/radarr
    container_name: radarr
    environment:
      - PUID=1028
      - PGID=100
      - TZ=Europe/Zurich
    volumes:
      - /volume2/docker/radarr:/config
      - /volume2/media:/media
    ports:
      - 7878:7878
    restart: unless-stopped
```

### Correct folder structure

With Radarr it's important to have a sane folder layout on the NAS. All the containers get access to the `media` folder (which, by the way, lives on SSD so the spinning disks aren't crunching). `media` is also exported over NFS for convenience — so don't store anything sensitive there.

```
/volume2/media
├── downloads/   # torrents land here
└── movies/      # Radarr copies movies here
```

The main downside of Radarr: it **copies** movies from `downloads` into `movies`. So each movie ends up stored twice — once with its original torrent name, once renamed by Radarr's template.

### Syncing with IMDb every 5 minutes

This script, added to Synology Task Scheduler, kicks off a sync with IMDb:

```bash
curl -i -s -k -X 'POST' \
    -H 'Host: nas.local:7878' \
    -H 'User-Agent: Mozilla/5.0' \
    -H 'Accept: application/json, text/javascript, */*; q=0.01' \
    -H 'Content-Type: application/json' \
    -H 'X-Api-Key: <YOUR_SECRET_TOKEN_HERE>' \
    -H 'X-Requested-With: XMLHttpRequest' \
    -H 'Origin: http://ds.local:7878' \
    -H 'Referer: http://ds.local:7878/system/tasks' \
    --data-binary '{"name":"ImportListSync"}' \
    'http://nas.local:7878/api/v3/command'
```

### Nginx Proxy Manager configuration

To expose the container on `your-domain.com/radarr`, add this to **Advanced → Custom Nginx Configuration** in the NPM container settings:

```nginx
# Radarr. In Radarr settings, set "URL base" to "/radarr".
location /radarr {
    allow 10.0.0.0/8;
    deny all;

    proxy_pass http://10.0.1.5:7878;

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

Radarr — это очень популярный self-hosted пакет, задача которого управлять загрузкой фильмов, которые вы хотели бы посмотреть. Работает в связке с [Prowlarr](https://github.com/ageev/SmartHome/tree/master/docker/prowlarr) и [qBittorrent](https://github.com/ageev/SmartHome/tree/master/docker/qBittorrent).

Работает всё примерно так:

- Я добавляю вышедший/будущий фильм в свой Watchlist через приложение IMDb на телефоне.
- Radarr синхронизирует свой локальный лист с моим IMDb-вотчлистом.
- Radarr спрашивает Prowlarr, есть ли раздачи на трекерах с нужным языком и качеством.
- Если есть — передаёт торрент-файл qBittorrent'у. Если нет — начинает мониторить новые релизы.
- Уведомляет в приватный Telegram-канал о начале и окончании закачки.
- Когда файл скачан, Radarr продолжает мониторить раздачи и апгрейдит фильм, если вышел релиз лучшего качества.

Очень удобная схема. Если я что-то хочу посмотреть, просто «лайкаю» фильм на телефоне — и через несколько часов на НАСе уже лежит нужный релиз. Если я жду новый фильм, он скачается через пару часов после релиза на трекерах.

### docker-compose.yml

```yaml
---
version: "3.9"
services:
  radarr:
    image: linuxserver/radarr
    container_name: radarr
    environment:
      - PUID=1028
      - PGID=100
      - TZ=Europe/Zurich
    volumes:
      - /volume2/docker/radarr:/config
      - /volume2/media:/media
    ports:
      - 7878:7878
    restart: unless-stopped
```

### Правильная структура папок

С Radarr важно иметь правильную структуру папок на НАСе. Все контейнеры получают доступ к папке `media` (которая, кстати, крутится на SSD, чтобы жёсткие диски не «трещали»). Папка `media` так же расшарена на НАСе по NFS для удобства, то есть не храните в ней ничего конфиденциального.

```
/volume2/media
├── downloads/   # сюда скачиваются торренты
└── movies/      # сюда Radarr копирует фильмы
```

Главный недостаток Radarr — он **копирует** фильмы из `downloads` в `movies`. Каждый фильм хранится как бы в двух экземплярах — один из торрент-качалки с оригинальным именем, второй в `movies` с именем по шаблону Radarr.

### Синхронизация с IMDb раз в 5 минут

Вот такой скрипт, добавленный в планировщик Synology, запускает синхронизацию с IMDb:

```bash
curl -i -s -k -X 'POST' \
    -H 'Host: nas.local:7878' \
    -H 'User-Agent: Mozilla/5.0' \
    -H 'Accept: application/json, text/javascript, */*; q=0.01' \
    -H 'Content-Type: application/json' \
    -H 'X-Api-Key: <YOUR_SECRET_TOKEN_HERE>' \
    -H 'X-Requested-With: XMLHttpRequest' \
    -H 'Origin: http://ds.local:7878' \
    -H 'Referer: http://ds.local:7878/system/tasks' \
    --data-binary '{"name":"ImportListSync"}' \
    'http://nas.local:7878/api/v3/command'
```

### Конфигурация Nginx Proxy Manager

Чтобы контейнер был доступен по адресу `your-domain.com/radarr`, добавьте это в **Advanced → Custom Nginx Configuration** в настройках контейнера NPM:

```nginx
# Radarr. В настройках Radarr выставите "URL base" в "/radarr".
location /radarr {
    allow 10.0.0.0/8;
    deny all;

    proxy_pass http://10.0.1.5:7878;

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
