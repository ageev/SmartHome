# ESPHome

**[English](#english)** | **[Русский](#russian)**

---

<a id="english"></a>

## English

The single purpose of ESPHome is to bring devices based on the wildly popular ESP32 microcontroller (and its variants) into the Home Assistant ecosystem. A huge number of "smart" devices with proprietary firmware run on ESP32 underneath. The internet is full of guides on how to resolder and reflash a smart device to cut it loose from its cloud and make it work locally.

Don't go overboard, though. General rule: **if the IoT device works — don't touch it.** Flash memory has a limited rewrite budget.

### docker-compose.yml

```yaml
---
version: "3.9"
services:
  esphome:
    image: esphome/esphome
    container_name: esphome
    hostname: esphome
    environment:
      - TZ=Europe/Zurich
    volumes:
      - /volume2/docker/esphome/config:/config
      - /volume2/docker/esphome/cache:/cache
    ports:
      - 6052:6052
      - 6123:6123
    restart: unless-stopped
```

---

<a id="russian"></a>

## Русский

Единственная цель ESPHome — подключать устройства на базе суперпопулярной микросхемы ESP32 (и её вариантов) к экосистеме умного дома Home Assistant. Огромное количество умных устройств с проприетарной прошивкой работает на ESP32. В интернете полно мануалов, как перепаять и перепрошить устройство, чтобы отвязать его от облака и заставить работать локально.

Только без фанатизма. Общее правило: **если IoT работает — не трожь!** Память имеет ограниченный ресурс перезаписи.

### docker-compose.yml

```yaml
---
version: "3.9"
services:
  esphome:
    image: esphome/esphome
    container_name: esphome
    hostname: esphome
    environment:
      - TZ=Europe/Zurich
    volumes:
      - /volume2/docker/esphome/config:/config
      - /volume2/docker/esphome/cache:/cache
    ports:
      - 6052:6052
      - 6123:6123
    restart: unless-stopped
```
