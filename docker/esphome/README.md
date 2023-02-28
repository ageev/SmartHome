esphome.io <- тут много интересного

Я использую [ATOM M5 stack](https://m5stack.com/) (M5 lite сгорели быстро, но Matrix работает уже много лет) и Sonoff с Tuya Smart. 
Sonoff и Tuya нужно перешивать с бубном, но результат часто того стоит. 
Обновлять этот контейнер следует осторожно. Один раз он всё мне поломал. Ну и обновлять прошивки у ESP32 каждый месяц так же дурное занятие. Работает твой ИоТ? Не трож!

## docker-compose.yml

```yml
---
version: "3.9"
services:
  esphome:
    image: esphome/esphome
    container_name: esphome
    hostname: esphome
    user: 1029:100 # !! CHANGE ME!!! Or disable to build the FW !! otherwise you will see permission denied messages
    environment:
      - TZ=Europe/Zurich
    volumes:
      - /volume1/docker/esphome/config:/config
      - /volume1/docker/esphome/cache:/cache
    ports:
      - 6052:6052
      - 6123:6123
    restart: unless-stopped
    network_mode: "bridge"
```
