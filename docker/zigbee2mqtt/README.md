# Zigbee2MQTT

**[English](#english)** | **[Русский](#russian)**

---

<a id="english"></a>

## English

Zigbee2MQTT is one of the three popular Zigbee integrations — probably the most popular. They have a [great website](https://www.zigbee2mqtt.io/) where you can check whether your bulb works with z2m or not.

I use Zigbee2MQTT to drive all my Zigbee devices through Home Assistant (you can, in principle, use HA's built-in Zigbee integration instead).

The container needs a companion **mosquitto** container.

### Prerequisites

1. **A Zigbee dongle.** The most convenient one for me was the Sonoff Zigbee 3.0 stick from AliExpress. The stick wants a short USB extender.
2. **Driver for the stick.** Install the driver on the NAS. Synology DSM upgrades sometimes wipe the driver, so I have a Task Scheduler task that runs as root at boot:

   > Use the right URL for your specific NAS CPU family. I have a DS218+ which is `apollolake`.

   ```bash
   if [ ! -f /lib/modules/cp210x.ko ]; then
       cd /lib/modules
       wget https://github.com/robertklep/dsm7-usb-serial-drivers/raw/main/modules/apollolake/cp210x.ko  # CHANGE ME!
   fi

   modprobe usbserial
   modprobe ftdi_sio
   modprobe cdc-acm
   insmod /lib/modules/cp210x.ko
   chmod 666 /dev/ttyUSB0   # needed so docker can reach the stick at user-level
   ```

### docker-compose.yml — zigbee2mqtt

```yaml
---
version: "3.9"
services:
  zigbee2mqtt:
    container_name: zigbee2mqtt
    image: koenkk/zigbee2mqtt
    group_add:
      - dialout   # needed so the user has access to the USB device,
                  # but no such group exists in DSM 7, so do "sudo chmod 0666 /dev/ttyUSB0"
    user: 1029:100   # CHANGE ME!
    volumes:
      - /volume1/docker/zigbee2mqtt:/app/data
      - /run/udev:/run/udev:ro
    ports:
      - 8081:8081
    environment:
      - TZ=Europe/Zurich
    devices:
      # - /dev/ttyACM0:/dev/ttyACM0   # old Sonoff stick
      - /dev/ttyUSB0:/dev/ttyUSB0     # Sonoff Zigbee 3.0 Dongle Plus
    restart: unless-stopped
    network_mode: "bridge"
```

### docker-compose.yml — mosquitto

```yaml
---
version: "3.9"
services:
  mosquitto:
    image: eclipse-mosquitto
    container_name: mosquitto
    hostname: mosquitto
    environment:
      - PUID=1029   # CHANGE ME!
      - PGID=100    # CHANGE ME!
      - TZ=Europe/Zurich
    volumes:
      - /volume1/docker/mosquitto/mosquitto.conf:/mosquitto/config/mosquitto.conf:ro
      - /volume1/docker/mosquitto/mosquitto.passwd:/mosquitto/config/mosquitto.passwd   # chmod 0666!
      - /volume1/docker/mosquitto/log/mosquitto.log:/mosquitto/log/mosquitto.log         # chmod 0666!
      - /volume1/docker/mosquitto/data:/mosquitto/data
    ports:
      - "1883:1883"
    restart: unless-stopped
    network_mode: "bridge"
```

> As usual, every folder listed in the configs has to be created manually on the NAS first, otherwise you'll see permission errors in the logs.

---

<a id="russian"></a>

## Русский

Zigbee2MQTT — одна из трёх популярных Zigbee-интеграций. Наверное, даже самая популярная. У них [отличный сайт](https://www.zigbee2mqtt.io/), где можно проверить, работает ли ваша лампочка с z2m.

Я использую Zigbee2MQTT для управления всеми моими ZB-устройствами через Home Assistant (можно, в принципе, использовать и встроенную в HA интеграцию Zigbee).

К контейнеру нужен ещё контейнер **mosquitto**.

### Пререквизиты

1. **Zigbee-донгл.** Самым удобным мне показался стик Sonoff Zigbee 3.0 с AliExpress. К стику нужен короткий USB-удлинитель.
2. **Драйвер для стика.** После апгрейда Synology DSM НАС иногда теряет драйвер, поэтому у меня в Task Scheduler стоит такой таск (запускается под root при загрузке):

   > Не забудьте подставить правильный URL под вашу модель НАСа (CPU)! У меня DS218+ с CPU семейства `apollolake`.

   ```bash
   if [ ! -f /lib/modules/cp210x.ko ]; then
       cd /lib/modules
       wget https://github.com/robertklep/dsm7-usb-serial-drivers/raw/main/modules/apollolake/cp210x.ko  # CHANGE ME!
   fi

   modprobe usbserial
   modprobe ftdi_sio
   modprobe cdc-acm
   insmod /lib/modules/cp210x.ko
   chmod 666 /dev/ttyUSB0   # чтобы docker имел user-level доступ к стику
   ```

### docker-compose.yml — zigbee2mqtt

```yaml
---
version: "3.9"
services:
  zigbee2mqtt:
    container_name: zigbee2mqtt
    image: koenkk/zigbee2mqtt
    group_add:
      - dialout   # нужно для доступа к USB-устройству,
                  # но в DSM 7 такой группы нет, поэтому "sudo chmod 0666 /dev/ttyUSB0"
    user: 1029:100   # CHANGE ME!
    volumes:
      - /volume1/docker/zigbee2mqtt:/app/data
      - /run/udev:/run/udev:ro
    ports:
      - 8081:8081
    environment:
      - TZ=Europe/Zurich
    devices:
      # - /dev/ttyACM0:/dev/ttyACM0   # старый Sonoff
      - /dev/ttyUSB0:/dev/ttyUSB0     # Sonoff Zigbee 3.0 Dongle Plus
    restart: unless-stopped
    network_mode: "bridge"
```

### docker-compose.yml — mosquitto

```yaml
---
version: "3.9"
services:
  mosquitto:
    image: eclipse-mosquitto
    container_name: mosquitto
    hostname: mosquitto
    environment:
      - PUID=1029   # CHANGE ME!
      - PGID=100    # CHANGE ME!
      - TZ=Europe/Zurich
    volumes:
      - /volume1/docker/mosquitto/mosquitto.conf:/mosquitto/config/mosquitto.conf:ro
      - /volume1/docker/mosquitto/mosquitto.passwd:/mosquitto/config/mosquitto.passwd   # chmod 0666!
      - /volume1/docker/mosquitto/log/mosquitto.log:/mosquitto/log/mosquitto.log         # chmod 0666!
      - /volume1/docker/mosquitto/data:/mosquitto/data
    ports:
      - "1883:1883"
    restart: unless-stopped
    network_mode: "bridge"
```

> Как обычно, все папки, указанные в конфигах, надо вначале вручную создать на НАСе, иначе получите ошибку доступа в логах.
