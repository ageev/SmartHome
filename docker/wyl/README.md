# wyl
удобный и простой контейнер, который с помощью ARP сканирования показывает устройства в локальной сети.

# docker-compose.yml

```yml
  wyl:
    image: aceberg/watchyourlan  # offline image with no connection to internet
    container_name: wyl
    hostname: wyl
    environment:
      - PUID=1028 # not implemented
      - PGID=100 # not implemented
      - TZ=Europe/Zurich
      - IFACES=ovs_eth2  # required: 1 or more interface
#      - ARP_STRS_JOINED=-gNx 10.0.2.0/24 -I ovs_eth2
      - PORT=8840                      # optional, default: 8840
      - TIMEOUT=120                    # optional, time in seconds, default: 120
#      - SHOUTRRR_URL=                  # optional, set url to notify
      - THEME=cerulean                     # optional
      - COLOR=dark                     # optional
    volumes:
      - /volume2/docker/wyl:/data/WatchYourLAN
    restart: unless-stopped
    network_mode: "host"
```
