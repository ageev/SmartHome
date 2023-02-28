# Живи с умом!
Основа моего умного дома - NAS Synology DS218+, в который я вставил 2 планки старой ноутбучной памяти, тем самым, нарастив её до 16 Гб. 

![Стартовая страница Homer](Pictures/start_page.jpg)
На НАСе крутятся:
- Synology Photos - отличный каталогизатор семейных фотографий. Я немного причесываю семейный архив (скриптами)[https://github.com/ageev/others] время от времени
-- HyperBackup - универсальный бэкапер от Synology
- S

# SmartHouse
## "Алиса, найди пульт от телевизора"
Моя самая любимая интеграция. 

Что нужно:
1. колонка с Алисой
2. home assistant с интеграцией YandexStation (https://github.com/AlexxIT/YandexStation) установленной через HACS
3. Nvidia Shield TV, подключенный к Home Assisstant через интеграцию AndroidTV с включенным ADB Debug (через Wifi и режим разработчика на ShieldTV)

### Шаг 1 - добавить сценарий в Яндексе
Добавляем сценарий: Если я скажу "Алиса, найди пульт от телевизора", Алиса скажет "Пультик-пультик, ты где?"

### Шаг 2 - добавляем автоматизацию в Home Assistant
```
alias: 'ALISA find REMOTE 📺 '
description: ''
trigger:
  - platform: event
    event_type: yandex_speaker
    event_data:
      value: Пультик-пультик, ты где?
condition: []
action:
  - service: script.remote_finder
    data: {}
mode: single
```

### Шаг 3 - создаем скрипт script.remote_finder
Открываем вкладку scripts в Home Assisstant и создаем новый скрипт:
```
alias: TV Remote Finder
icon: mdi:target
mode: single
sequence:
  - service: androidtv.adb_command
    data:
      command: >-
        am start -a android.intent.action.VIEW -d -n
        com.nvidia.remotelocator/.ShieldRemoteLocatorActivity
      entity_id: media_player.ShieldADB
```

## Radarr https://habr.com/ru/post/505814/
Радарр 3 не дает обновлять листы IMDB чаще одного раза в 6 часов.
Чтобы обойти это ограничение можно добавить вот это в планировщик (убедитесь, что прописали верный хост и токен!)
```bash
curl -i -s -k -X $'POST' \
    -H $'Host: nas.local:7878' -H $'User-Agent: Mozilla/5.0 (Macintosh; Intel Mac OS X 10.15; rv:97.0) Gecko/20100101 Firefox/97.0' -H $'Accept: application/json, text/javascript, */*; q=0.01' -H $'Accept-Language: en-US,en;q=0.5' -H $'Accept-Encoding: gzip, deflate' -H $'Content-Type: application/json' -H $'X-Api-Key: <YOUR_SECRET_TOKEN_HERE!!!!' -H $'X-Requested-With: XMLHttpRequest' -H $'Content-Length: 25' -H $'Origin: http://ds.local:7878' -H $'DNT: 1' -H $'Connection: close' -H $'Referer: http://ds.local:7878/system/tasks' \
    --data-binary $'{\"name\":\"ImportListSync\"}' \
    $'http://nas.local:7878/api/v3/command'
```
(UPDATE) я больше не использую Jackett, заменил на Prowlarr

## torrent watcher
UPDATE! Я теперь мониторю торренты через интеграцию Transmission в Home Assistant. Можно создать правило автоматизации, которое будет посылать нужные вам уведомления в телеграм чат

Используем телеграм бота для мониторинга очереди qBittorent
1. в файл secrets.py нужно внести свои данные:
- qBittorent IP и порт
- Telegram token и chat_id 
Создать бота и достать токен можно через @botfather в Телеграм. После того, как бот создан, нужно отправить "/setjoingroups" -> Enable чтобы бот мог присоединятся к группам.  
Chat_id можно узнать так https://stackoverflow.com/questions/32423837/telegram-bot-how-to-get-a-group-chat-id#32572159

2. скопировать папку telegrambot и secrets.py куда-нибудь себе
3. настроить планировщик чтобы каждую минуту запускал скрипт. Например, так:
(sudo crontab -e)
```
* * * * * /usr/bin/python3 /home/artem/scripts/telegram/torrent_watcher.py >> /var/log/crontab_errors.log 2>&1
```
4. готово!
