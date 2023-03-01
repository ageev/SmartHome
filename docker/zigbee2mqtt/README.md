# Zigbee2Mqtt
zigbee2mqtt - это одна из трех популярных zigbee интеграций. Наверное даже самая популярная. У них [отличный сайт](https://www.zigbee2mqtt.io/), где можно проверить, работает ли ваша лампочка с z2m или нет.
Я использую zigbee2mqtt для управления всеми моими ZB устройствами в Home Assistant (но можно, в принципе, использовать и встроенную в HA интеграцию Zigbee)

## Пререквизиты
1. Zigbee донгл. Самым удобным мне показался стик Sonoff Zigbee 3.0 с али. К стику нужен небольшой USB удлинитель
2. Нужно загрузить драйвера для стика на НАС
Т.к. после апгрейда Synology DSM НАС может драйвера потерять, у меня в Task Manager стоит вот такой таск (запускать под рутом при загрузке девайса)

Не забудьте вставить правильный URL под вашу конркетную модель НАСа (CPU)! У меня DS218+ c CPU из семейства apollolake. 

```bash
if [ ! -f /lib/modules/cp210x.ko ]; then
    cd /lib/modules
    wget https://github.com/robertklep/dsm7-usb-serial-drivers/raw/main/modules/apollolake/cp210x.ko # CHANGE ME!!!
fi

modprobe usbserial
modprobe ftdi_sio
modprobe cdc-acm
insmod /lib/modules/cp210x.ko
chmod 666 /dev/ttyUSB0  # you need this to have user-level access to the stick from docker
```

## docker-compose.yml zigbee2mqtt
```yml
---
version: "3.9"
services:
  zigbee2mqtt:
    container_name: zigbee2mqtt
    image: koenkk/zigbee2mqtt
    group_add:
      - dialout # needed so user has access to USB device, but no such group exists in DSM7.0, so you need to do "sudo chmod 0666 /dev/ttyUSB0"!
    user: 1029:100  # CHANGE ME!
    volumes:
      - /volume1/docker/zigbee2mqtt:/app/data
      - /run/udev:/run/udev:ro
    ports:
      - 8081:8081
    environment:
      - TZ=Europe/Zurich
    devices:
#      - /dev/ttyACM0:/dev/ttyACM0 # old SONOFF stick
      - /dev/ttyUSB0:/dev/ttyUSB0 # sonoff zigbee 3.0 dongle plus 
    restart: unless-stopped
    network_mode: "bridge"
```

## docker-compose.yml для mosquitto
```yml
---
version: "3.9"
services:
  mosquitto:
    image: eclipse-mosquitto
    container_name: mosquitto
    hostname: mosquitto
    environment:
     - PUID=1029  #CHANGE ME!
     - PGID=100  #CHANGE ME!
     - TZ=Europe/Zurich
    volumes:
     - /volume1/docker/mosquitto/mosquitto.conf:/mosquitto/config/mosquitto.conf:ro
     - /volume1/docker/mosquitto/mosquitto.passwd:/mosquitto/config/mosquitto.passwd  # do chmod 0666 on that file!
     - /volume1/docker/mosquitto/log/mosquitto.log:/mosquitto/log/mosquitto.log # do chmod 0666!
     - /volume1/docker/mosquitto/data:/mosquitto/data
    ports:
     - "1883:1883"
    restart: unless-stopped
    network_mode: "bridge"
```

Как обычно все папки указанные в конфигах надо вначале вручную создать на НАСе.
