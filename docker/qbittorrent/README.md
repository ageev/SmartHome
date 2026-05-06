# qBittorrent

**[English](#english)** | **[Русский](#russian)**

---

<a id="english"></a>

## English

Popular torrent client with a decent web UI.

### docker-compose.yml

```yaml
qbittorrent:
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

### Port forwarding on the router

In your router settings, forward TCP/UDP `51337` to the NAS. Verify with a [port checker](https://portchecker.co/).

### Telegram bot

Don't forget to set up a Telegram bot for notifications.

### Disabling auth on the local network

`/volume2/docker/qbittorent/qBittorrent/qBittorrent.conf`:

```ini
[Preferences]
...
WebUI\AuthSubnetWhitelist=0.0.0.0/0
WebUI\AuthSubnetWhitelistEnabled=true
```

---

<a id="russian"></a>

## Русский

Популярная торрент-качалка с неплохим веб-интерфейсом.

### docker-compose.yml

```yaml
qbittorrent:
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

### Проброс портов на роутере

В настройках роутера не забудьте отправлять трафик с TCP/UDP `51337` на НАС. Проверьте, что всё работает, [портчекером](https://portchecker.co/).

### Бот в Телеграме

Не забудьте настроить бота в Telegram для уведомлений.

### Отключение аутентификации в локальной сети

`/volume2/docker/qbittorent/qBittorrent/qBittorrent.conf`:

```ini
[Preferences]
...
WebUI\AuthSubnetWhitelist=0.0.0.0/0
WebUI\AuthSubnetWhitelistEnabled=true
```
