единственная цель esphome - подключать устройства на базе суперпопулярной микросхемы ESP32 (и вариантов) к экосистеме умного дома Home Assistant. Огромное количество умных устройств с проприетарной прошивкой работает на ESP32. В Интернетах полно мануалов как перепаять и перепрошить умное устройство чтобы отвязать его от облака и заставить работать локально. 
Только без фанатизма тут. Общее правило: если IoT работает - не трожь! Часто память имеет ограниченный ресурс.

## docker-compose.yml

```yml
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
