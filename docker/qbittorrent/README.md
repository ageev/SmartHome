# qbittorent
популярная торрент качалка с неплохим ГУИ.

# docker-compose.yml
```yml
  qbittorent:
    image: linuxserver/qbittorrent:latest
    container_name: qbittorrent
    environment:
      - PUID=1028
      - PGID=100
      - TZ=Europe/Zurich
      - WEBUI_PORT=8081
      - TORRENTING_PORT=51337
    volumes:
      - /volume2/docker/qbittorent:/config
      - /volume2/media/downloads:/media/downloads
    ports:
      - 8081:8081
      - 51337:51337
      - 51337:51337/udp
    restart: unless-stopped
```

# проброс портов на роутере
В настройках своего роутера не забудьте отправлять трафик с TCP/IP 51337 на НАС. 
Проверьте, что всё работает, с помощью [портчеккера](https://portchecker.co/)
