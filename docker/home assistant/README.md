# HomAss, хомяк - центр умного дома

# Железо
## Sonoff Zigbee 3.0 dongle
У хомяка есть родная интеграция Zigbee. Она была раньше плохая, и я спрыгнул на [zigbee2mqtt](https://github.com/ageev/SmartHome/tree/master/docker/zigbee2mqtt). Сейчас бы уже, наверное, остался на родной. 
Про настройку донгла почитайте в разделе zigbee2mqtt
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
