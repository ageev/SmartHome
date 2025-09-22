# HomAss, хомяк - центр умного дома

# Железо

## Docker
> Из-за постолянных напрягов с отваливающимся донглом я *переехал с докера на виртуальную машину* (Home Assistant OS) и проблемы исчезли!

Мне удобней использовать докер и единственный домашний сервер - Synology NAS. Но есть определенная логика в том, чтобы использовать отдельное устройство для home-assistant: через платы расширения можно добиться большего функционала (вставить, например, плату-акселератор работы с ИИ), такое устройство можно запихнуть сразу в сеть умных устройств, и не держать его в "человеческой" сети. 

## Sonoff Zigbee 3.0 dongle
| :exclamation: Sonoff Zigbee 3.0 USB Dongle сдох через 2 года использования. Перешел на “Sonoff Zigbee 3.0 USB Dongle Plus V2” (model “ZBDongle-E”)|
|-----------|

У хомяка есть родная интеграция Zigbee. Она была раньше плохая, и я спрыгнул на [zigbee2mqtt](https://github.com/ageev/SmartHome/tree/master/docker/zigbee2mqtt). Сейчас бы уже, наверное, остался на родной. 
Про настройку донгла почитайте в разделе [zigbee2mqtt](../zigbee2mqtt)
## APC UPS


# Установка
## docker-compose.yml
```yml
---
version: "3.9"
services:
  home-assistant:
    image: homeassistant/home-assistant
    container_name: home-assistant
    hostname: home-assistant
    environment:
      - PUID=1029
      - PGID=100
      - TZ=Europe/Zurich
    volumes:
      - /volume1/docker/home-assistant:/config
    restart: unless-stopped
    network_mode: "host"
## will be handled by zigbee2mqtt 
#    devices:
#      - /dev/ttyACM0:/dev/ttyACM0 # old SONOFF stick
#      - /dev/ttyUSB0:/dev/ttyUSB0 # sonoff zigbee 3.0 dongle plus 
```
## HACS
HACS - это *неофициальный* магазин аддонов к хомяку. Это то место, где Хомяк становится понастоящему мощной штукой.
HACS подтягивает интеграции с GitHab. 

## Полезные автоматизации

## "Алиса, найди пульт от телевизора"
Яндекс станция, услышав эту команду, начнёт поиск пульта от телевизора (пульты от Nvidia Shield TV умеют писчать по запросу приставки). 

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

# Nginx Proxy Manager Configuration
чтобы Home Assistant был доступен через Nginx Proxy Manager добавьте в последний такую конфигурацию:
```
# this configuration can be used for external access to api/webhook only if NPM/router allows. 

location / {
    if ($remote_addr !~ "^(10\.|172\.(1[6-9]|2[0-9]|3[0-1])\.|192\.168\.|fc00:|fe80:|fd[0-9a-f]{2}:)")  {
        return 301 https://google.com; # redirect non-local IPs to Google.com.
    }
    proxy_pass http://10.0.1.10:8123; # Home Assistant IP
    proxy_set_header Host $host;
    proxy_set_header X-Real-IP $remote_addr;
    proxy_set_header X-Forwarded-For $proxy_add_x_forwarded_for;
  }

location /api/websocket {
    if ($remote_addr !~ "^(10\.|172\.(1[6-9]|2[0-9]|3[0-1])\.|192\.168\.|fc00:|fe80:|fd[0-9a-f]{2}:)")  {
        return 301 https://google.com;
    }
    proxy_pass http://10.0.1.10:8123/api/websocket; # Home Assistant IP
    proxy_set_header Host $host;
    proxy_set_header X-Real-IP $remote_addr;
    proxy_set_header X-Forwarded-For $proxy_add_x_forwarded_for;
    proxy_http_version 1.1;
    proxy_set_header Upgrade $http_upgrade;
    proxy_set_header Connection "upgrade";
    }

location /api/webhook {
    proxy_pass http://10.0.1.10:8123/api/webhook; # Home Assistant IP
    proxy_set_header Host $host;
    proxy_set_header X-Real-IP $remote_addr;
    proxy_set_header X-Forwarded-For $proxy_add_x_forwarded_for;
    proxy_http_version 1.1;
    proxy_set_header Upgrade $http_upgrade;
    proxy_set_header Connection "upgrade";
    }
```
