# Home Assistant

**[English](#english)** | **[Русский](#russian)**

The brain of the smart home.

---

<a id="english"></a>

## English

### Hardware

#### Docker

> Because of constant trouble with the dongle dropping out, I **moved from docker to a virtual machine** (Home Assistant OS) and the problems disappeared.

I prefer using docker on a single home server — the Synology NAS. There is, however, a logic to running Home Assistant on a dedicated device: expansion cards open up more capabilities (an AI accelerator, for instance), and that device can sit directly on the smart-device network instead of the "human" network.

#### Sonoff Zigbee 3.0 dongle

> The Sonoff Zigbee 3.0 USB Dongle died after two years of use. I switched to the Sonoff Zigbee 3.0 USB Dongle Plus V2 (model **ZBDongle-E**).

Home Assistant has its own Zigbee integration. Back when I started, it was poor and I jumped to [zigbee2mqtt](https://github.com/ageev/SmartHome/tree/master/docker/zigbee2mqtt). These days I'd probably stay on the built-in one.

For dongle setup details see the [zigbee2mqtt](../zigbee2mqtt) section.

#### APC UPS

(Notes pending.)

### Installation

#### docker-compose.yml

```yaml
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
    # The dongle is handled by zigbee2mqtt:
    # devices:
    #   - /dev/ttyACM0:/dev/ttyACM0   # old Sonoff stick
    #   - /dev/ttyUSB0:/dev/ttyUSB0   # Sonoff Zigbee 3.0 Dongle Plus
```

#### HACS

HACS is the *unofficial* add-on store for Home Assistant. This is the place where Home Assistant turns into a genuinely powerful tool. HACS pulls integrations straight from GitHub.

### Useful automations

#### "Alisa, find the TV remote"

The Yandex speaker hears this command and triggers a search for the TV remote (Nvidia Shield TV remotes can beep on demand).

What you need:

1. A speaker with Alisa.
2. Home Assistant with the [YandexStation](https://github.com/AlexxIT/YandexStation) integration (installed via HACS).
3. An Nvidia Shield TV connected to Home Assistant via the AndroidTV integration with ADB Debug enabled (over Wi-Fi, with developer mode on the Shield).

**Step 1 — add a scenario in the Yandex app.**

Scenario: when I say "Alisa, find the TV remote", Alisa says "Pultik-pultik, where are you?".

**Step 2 — add the automation in Home Assistant:**

```yaml
alias: 'Alisa find TV remote'
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

**Step 3 — create the `script.remote_finder` script.**

Open the Scripts tab in Home Assistant and add:

```yaml
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

### Nginx Proxy Manager configuration

So that Home Assistant is reachable through NPM, add this to NPM:

```nginx
# This configuration can be used for external access to api/webhook only,
# if NPM/router allows it.

location / {
    if ($remote_addr !~ "^(10\.|172\.(1[6-9]|2[0-9]|3[0-1])\.|192\.168\.|fc00:|fe80:|fd[0-9a-f]{2}:)") {
        return 301 https://google.com;   # redirect non-local IPs
    }
    proxy_pass http://10.0.1.10:8123;    # Home Assistant IP
    proxy_set_header Host $host;
    proxy_set_header X-Real-IP $remote_addr;
    proxy_set_header X-Forwarded-For $proxy_add_x_forwarded_for;
}

location /api/websocket {
    if ($remote_addr !~ "^(10\.|172\.(1[6-9]|2[0-9]|3[0-1])\.|192\.168\.|fc00:|fe80:|fd[0-9a-f]{2}:)") {
        return 301 https://google.com;
    }
    proxy_pass http://10.0.1.10:8123/api/websocket;
    proxy_set_header Host $host;
    proxy_set_header X-Real-IP $remote_addr;
    proxy_set_header X-Forwarded-For $proxy_add_x_forwarded_for;
    proxy_http_version 1.1;
    proxy_set_header Upgrade $http_upgrade;
    proxy_set_header Connection "upgrade";
}

location /api/webhook {
    proxy_pass http://10.0.1.10:8123/api/webhook;
    proxy_set_header Host $host;
    proxy_set_header X-Real-IP $remote_addr;
    proxy_set_header X-Forwarded-For $proxy_add_x_forwarded_for;
    proxy_http_version 1.1;
    proxy_set_header Upgrade $http_upgrade;
    proxy_set_header Connection "upgrade";
}
```

---

<a id="russian"></a>

## Русский

«Хомяк» — центр умного дома.

### Железо

#### Docker

> Из-за постоянных напрягов с отваливающимся донглом я **переехал с docker на виртуальную машину** (Home Assistant OS), и проблемы исчезли.

Мне удобней использовать docker и единственный домашний сервер — Synology NAS. Но есть определённая логика в том, чтобы держать Home Assistant на отдельном устройстве: через платы расширения можно получить больше функционала (например, поставить плату-акселератор для ИИ), и такое устройство можно сразу засунуть в сеть умных устройств, не держа его в «человеческой» сети.

#### Sonoff Zigbee 3.0 dongle

> Sonoff Zigbee 3.0 USB Dongle сдох через два года использования. Перешёл на Sonoff Zigbee 3.0 USB Dongle Plus V2 (модель **ZBDongle-E**).

У хомяка есть родная интеграция Zigbee. Раньше она была плохая, и я спрыгнул на [zigbee2mqtt](https://github.com/ageev/SmartHome/tree/master/docker/zigbee2mqtt). Сейчас бы, наверное, остался на родной.

Про настройку донгла читайте в разделе [zigbee2mqtt](../zigbee2mqtt).

#### APC UPS

(Заметки в работе.)

### Установка

#### docker-compose.yml

```yaml
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
    # Донгл обрабатывается контейнером zigbee2mqtt:
    # devices:
    #   - /dev/ttyACM0:/dev/ttyACM0   # старый Sonoff
    #   - /dev/ttyUSB0:/dev/ttyUSB0   # Sonoff Zigbee 3.0 Dongle Plus
```

#### HACS

HACS — это *неофициальный* магазин аддонов к Home Assistant. Именно тут хомяк становится по-настоящему мощной штукой. HACS подтягивает интеграции прямо с GitHub.

### Полезные автоматизации

#### «Алиса, найди пульт от телевизора»

Яндекс-станция, услышав эту команду, запустит поиск пульта от телевизора (пульты от Nvidia Shield TV умеют пищать по запросу приставки).

Что нужно:

1. Колонка с Алисой.
2. Home Assistant с интеграцией [YandexStation](https://github.com/AlexxIT/YandexStation), установленной через HACS.
3. Nvidia Shield TV, подключённый к Home Assistant через интеграцию AndroidTV с включённым ADB Debug (по Wi-Fi и в режиме разработчика).

**Шаг 1 — добавить сценарий в Яндекс.**

Сценарий: «Если я скажу „Алиса, найди пульт от телевизора“, Алиса скажет „Пультик-пультик, ты где?“».

**Шаг 2 — добавить автоматизацию в Home Assistant:**

```yaml
alias: 'Alisa find TV remote'
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

**Шаг 3 — создать скрипт `script.remote_finder`.**

Открываем вкладку Scripts в Home Assistant и добавляем:

```yaml
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

### Конфигурация Nginx Proxy Manager

Чтобы Home Assistant был доступен через NPM, добавьте такой конфиг:

```nginx
# Эту конфигурацию можно использовать только для внешнего доступа
# к api/webhook, если это разрешено на NPM/роутере.

location / {
    if ($remote_addr !~ "^(10\.|172\.(1[6-9]|2[0-9]|3[0-1])\.|192\.168\.|fc00:|fe80:|fd[0-9a-f]{2}:)") {
        return 301 https://google.com;   # внешние IP — на Google
    }
    proxy_pass http://10.0.1.10:8123;    # IP Home Assistant
    proxy_set_header Host $host;
    proxy_set_header X-Real-IP $remote_addr;
    proxy_set_header X-Forwarded-For $proxy_add_x_forwarded_for;
}

location /api/websocket {
    if ($remote_addr !~ "^(10\.|172\.(1[6-9]|2[0-9]|3[0-1])\.|192\.168\.|fc00:|fe80:|fd[0-9a-f]{2}:)") {
        return 301 https://google.com;
    }
    proxy_pass http://10.0.1.10:8123/api/websocket;
    proxy_set_header Host $host;
    proxy_set_header X-Real-IP $remote_addr;
    proxy_set_header X-Forwarded-For $proxy_add_x_forwarded_for;
    proxy_http_version 1.1;
    proxy_set_header Upgrade $http_upgrade;
    proxy_set_header Connection "upgrade";
}

location /api/webhook {
    proxy_pass http://10.0.1.10:8123/api/webhook;
    proxy_set_header Host $host;
    proxy_set_header X-Real-IP $remote_addr;
    proxy_set_header X-Forwarded-For $proxy_add_x_forwarded_for;
    proxy_http_version 1.1;
    proxy_set_header Upgrade $http_upgrade;
    proxy_set_header Connection "upgrade";
}
```
