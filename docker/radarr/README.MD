# radarr - медиа менеджер

Radarr это очень популярный self-hosted пакет, задача которого управлять загрузкой киношек, которые вы хотели бы посмотреть. Он работает в связке с [prowlarr'ом](https://github.com/ageev/SmartHome/tree/master/docker/prowlarr) и [qBittorrent](https://github.com/ageev/SmartHome/tree/master/docker/qBittorrent).

Работает всё примерно так:
- я добавляю вышедший/будущий фильм в свой Watchlist через приложение IMDB на телефоне.
- радарр синхронизирует свой локальный лист киношек с моим личным IMDB вотчлистом.
- радарр спрашивает prowlarr, есть ли раздачи на треккерах с нужным языком и качеством?
- если есть - передает торрент файл qbittorrent'у. Если нет - начинает мониторить новые релизы.
- уведомляет в приватный телеграм канал о начале/окончании закачки.
- когда файл скачан, радарр по-прежнему мониторит раздачи и апгрейдит скачанные фильмы если вышел релиз лучшего качества.

Это очень удобная схема. Если я что-то хочу посмотреть, просто "лайкаю" фильм на телефоне и через несколько часов на НАСе уже лежит нужный релиз. Если я жду новый фильм, то он скачается спустя пару часов после релиза на треккерах. 

# docker-compose.yml
```yml
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

# правильная структура папок
С Радарром важно иметь правильную структуру папок на НАСе. Все контейнеры получают доступ к папке Media (она, кстати, крутится на SSD, чтобы жесткие диски не "трещали"). Папка Media так же "расшарена" на НАСе через NFS для удобства, то есть не храните в ней ничего конфиденциального. 
```
\volume2\media
-- downloads # сюда скачиваются торренты 
-- movies # cюда радарр копирует фильмы
```

Собственно главный недостаток радарра - он копирует фильмы из папки Downloads в папку Movies. Таким образом, каждый фильм хранится как бы в двух экземплярах - один из торрент качалки, с оригинальным именем и названием папки, второй в папке Movies, переименнованный по шаблону. 

# синхронизация с IMDB раз в 5 минут

Вот такой скрипт, добавленный в планировщик Synology, позволяет запускать синхронизацию с IMDB

```bash
curl -i -s -k -X $'POST' \
    -H $'Host: nas.local:7878' -H $'User-Agent: Mozilla/5.0 (Macintosh; Intel Mac OS X 10.15; rv:97.0) Gecko/20100101 Firefox/97.0' -H $'Accept: application/json, text/javascript, */*; q=0.01' -H $'Accept-Language: en-US,en;q=0.5' -H $'Accept-Encoding: gzip, deflate' -H $'Content-Type: application/json' -H $'X-Api-Key: <YOUR_SECRET_TOKEN_HERE!!!!' -H $'X-Requested-With: XMLHttpRequest' -H $'Content-Length: 25' -H $'Origin: http://ds.local:7878' -H $'DNT: 1' -H $'Connection: close' -H $'Referer: http://ds.local:7878/system/tasks' \
    --data-binary $'{\"name\":\"ImportListSync\"}' \
    $'http://nas.local:7878/api/v3/command'
```

# Nginx Proxy Manager Configuration
Чтобы контейнер был доступен по адресу ```your-domain.com/radarr" добавьте это в Advanced -> Custom Nginx Configuration в настройках контейнера Nginx Proxy Manager
```
# Radarr. Go to settings in Radarr and set "URL base" to "/radarr"
location /radarr {
    proxy_pass http://10.0.1.5:7878; #NAS IP
    proxy_set_header Host $host;
    proxy_set_header X-Real-IP $remote_addr;
    proxy_set_header X-Forwarded-For $proxy_add_x_forwarded_for;
  }
```
