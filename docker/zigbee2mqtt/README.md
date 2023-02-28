# пререквизиты

## docker-compose.yml
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
