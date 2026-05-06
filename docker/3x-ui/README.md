# 3x-ui

**[English](#english)** | **[Русский](#russian)**

---

<a id="english"></a>

## English

3x-ui is a convenient GUI for Xray — an advanced proxy that lets you reach your home network (and the internet) from almost anywhere in the world, even when VPNs are actively blocked.
The VLESS protocol is so good it even slips through corporate SSL decryption proxies. UDP is no problem either — Telegram voice calls work great.

I host 3x-ui on my NAS. I use NekoBox on Windows and Hiddify on phones as clients.

### docker-compose.yml

```yaml
---
version: "3.9"
services:
  3x-ui:
    image: ghcr.io/mhsanaei/3x-ui:latest
    container_name: 3x-ui
    hostname: 3xui
    volumes:
      - /volume2/docker/3x-ui/config:/etc/x-ui/
      - /volume2/docker/3x-ui/log:/var/log/
    environment:
      - TZ=Europe/Zurich
      - PUID=1028
      - PGID=100
      - XRAY_VMESS_AEAD_FORCED=false
      - XUI_LOG_LEVEL=info
    ports:
      - 2053:2053
      - 8082:8082
    restart: unless-stopped
```

### Masking and external access

I don't want to publish 3x-ui directly to the internet — too much hassle with certificates. So the server sits behind nginx-proxy-manager and receives clean HTTP traffic.

1. Create an inconspicuous domain like `video.your-domain.com` and add an A record pointing to your router's external IP.
2. Configure your router to accept connections on port 443 and forward them to the nginx-proxy-manager container (ASUS calls this Virtual Server / Port Forwarding).
3. In the nginx-proxy-manager folder, create a simple website using your favorite AI assistant. Add a `robots.txt` to the root with `User-agent: *` and `Disallow: /` to discourage indexing.

   ![example website folder](https://github.com/ageev/SmartHome/blob/master/Pictures/vless_website.png)

4. Add the proper config to the nginx-proxy-manager container:
   - Add a new host `video.your-domain.com` with a redirect to `http://<NAS_IP>:8082`. Make sure to enable "websocket support"!
   - Go to the Advanced tab and add this config:

```nginx
root /opt/websites/jodeln;
index index.html;
autoindex off;

location / {
    try_files $uri /index.html;
}

location /ws {
    if ($http_upgrade != "websocket") {
        return 404;
    }
    proxy_pass http://10.0.1.5:8082; # NAS IP
    proxy_http_version 1.1;
    proxy_set_header Upgrade $http_upgrade;
    proxy_set_header Connection "upgrade";
    proxy_set_header Host $host;
    proxy_set_header X-Real-IP $remote_addr;
    proxy_set_header X-Forwarded-For $proxy_add_x_forwarded_for;
    proxy_read_timeout 52w;
}

location /<secret_token>/ {
#    allow 10.0.0.0/8;
    proxy_pass http://10.0.1.5:2053; # NAS IP
    proxy_set_header Host $host;
    proxy_set_header X-Real-IP $remote_addr;
    proxy_set_header X-Forwarded-For $proxy_add_x_forwarded_for;
    proxy_set_header X-Forwarded-Proto $scheme;

    proxy_hide_header X-Powered-By;
    proxy_hide_header Server;

    add_header X-Frame-Options DENY;
    add_header X-Content-Type-Options nosniff;
    add_header Referrer-Policy no-referrer;
}
```

Now the admin panel sits at `https://video.your-domain.com/<secret_token>` and clients connect via websocket at `https://video.your-domain.com/ws`. Everyone else just sees a harmless website.

### Inbound configuration

Create a VLESS inbound on port 8082. 8082 is the internal port — clients will connect on 443. So the configurations 3x-ui generates need a small fix:

From this:

```
vless://blablablabla@video.your-website.com:8082?type=ws&encryption=none&path=%2Fws&host=&security=none#User
```

To this:

```
vless://blablablabla@video.your-website.com:443?type=ws&encryption=none&path=%2Fws&host=&security=tls#User
```

Now you can add it to a client and try connecting from outside.

### LAN access from outside

To let some users access not just the internet but also local resources, on the **Xray Configs > Routing Rules** tab add:

1. Inbound = `api`, Outbound = `api`
2. Client = `<comma-separated client names>`, Outbound = `direct`
3. Destination IP = `geoip:private`, Outbound = `blocked`

Then under **Xray Configs > Outbounds**:

1. Tag `proxy`, protocol `freedom`
2. Tag `direct`, protocol `freedom`
3. Tag `blocked`, protocol `blackhole`

---

<a id="russian"></a>

## Русский

3x-ui — это удобный GUI для Xray, продвинутого прокси, который позволяет достучаться до домашней сети (и интернета) практически из любой точки мира, даже если VPN активно блокируется.
Протокол VLESS настолько крут, что проходит даже через корпоративный SSL Decryption proxy. Даже UDP не проблема! Голосовые звонки Telegram работают на ура.

Я хостю сервер 3x-ui у себя на НАСе. В качестве клиентов использую NekoBox на винде и Hiddify на телефонах.

### docker-compose.yml

См. блок выше — конфигурация общая.

### Маскировка и доступ снаружи

Я не хочу публиковать 3x-ui напрямую наружу, чтобы не возиться с сертификатами. Поэтому сервер «сидит» за nginx-proxy-manager и получает сразу «чистый» HTTP трафик.

1. Создайте неприметный домен, что-то типа `video.your-domain.com`, и добавьте для него А-запись на внешний IP вашего роутера.
2. Настройте роутер на приём соединений по 443 порту и пересылку на контейнер nginx-proxy-manager (Asus называет это Virtual Server / Port Forwarding).
3. В папке контейнера nginx-proxy-manager создайте простенький вебсайт используя ИИ. Можно добавить `robots.txt` в корень с `User-agent: *` и `Disallow: /` чтобы отбить ботам желание его индексировать.

   ![пример папки с сайтом](https://github.com/ageev/SmartHome/blob/master/Pictures/vless_website.png)

4. Теперь нужно добавить правильную конфигурацию в контейнер nginx-proxy-manager:
   - Добавьте новый хост `video.your-domain.com` с редиректом на `http://<NAS_IP>:8082`. Обязательно тыкнуть "websocket support"!
   - Идите на вкладку Advanced и добавьте конфигурацию (см. английскую секцию).

Теперь админка у вас сидит по адресу `https://video.your-domain.com/<secret_token>`, а клиенты стучатся через websocket на `https://video.your-domain.com/ws`. Все остальные видят безобидный вебсайт.

### Конфигурация inbound

Создайте VLESS inbound на порту 8082. 8082 — это внутренний порт. Клиенты будут стучаться на 443. Так что конфигурации, которые генерирует 3x-ui, нужно подправлять — заменить `:8082` на `:443` и `security=none` на `security=tls`.

### Доступ к локальной сети извне

Чтобы дать некоторым пользователям доступ не только к интернету, но и к локальным ресурсам, на вкладке **Xray Configs > Routing Rules** добавьте правила:

1. Inbound == `api`, Outbound == `api`
2. Client == `<имена клиентов через запятую>`, Outbound == `direct`
3. Destination IP == `geoip:private`, Outbound == `blocked`

Затем **Xray Configs > Outbounds**:

1. Tag `proxy`, protocol `freedom`
2. Tag `direct`, protocol `freedom`
3. Tag `blocked`, protocol `blackhole`
