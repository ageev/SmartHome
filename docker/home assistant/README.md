# HomAss, хомяк - центр умного дома

# Железо
## Sonoff Zigbee 3.0 dongle
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
## HACKS
