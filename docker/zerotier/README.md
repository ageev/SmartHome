# zerotier
Zerotier аналог Tailscale (который блокирует русских пользователей кстати), который позволяет поместить разные устройства в разных сетях в одну виртуальную сеть.
Zerotier можно назвать "настоящим ВПН", потому как шифрование тут вторично. Использовать Zerotier для интернет траффика - это порнография. А вот для бэкапа самое то. 

# docker-compose.yml
```yml
---
version: "3.9"
services:
  zerotier:
  # join network 
  #sudo docker exec -it zerotier zerotier-cli join <net_id>
  # connection issues. Sometimes the container starts (and exists) too early, so
  # create Synology task to run "docker-compose up -d" after few minutes after boot to start the container again
    container_name: zerotier
    image: zerotier/zerotier-synology:latest
    cap_add:
      - NET_ADMIN
      - SYS_ADMIN
    devices:
      - /dev/net/tun:/dev/net/tun
    volumes:
      - /volume2/docker/zerotier:/var/lib/zerotier-one
    network_mode: "host"
    restart: unless-stopped
```
