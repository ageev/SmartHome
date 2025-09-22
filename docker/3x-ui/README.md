3x-ui это удобный ГУИ для Xray - продвинутого прокси, который позволяет достучаться до домашней сети (и Интернета) практически из любой точки мира, даже если VPN активно блокируется. 
VLESS протокол настолько крут, что проходит даже через корпоративный SSL Decryption proxy. Даже UDP не проблема! Голосовые звонки Телеграмма работают на ура.  

Я хостю сервер 3x-ui у себя на НАСе. В качестве клиентов использую NekoBox на винде и Hiddify на телефонах.

# docker-compose.yml

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

# Маскировка и доступ снаружи
Я не хочу публиковать 3x-ui напрямую наружу чтобы не возиться с сертификатами. Поэтому сервер "сидит" за nginx-proxy-manager и получает сразу "чистый" http траффик.

1. Создайте неприметный домен, что-то типа video.your-domain.com, и добавьте для него А запись на внешний IP вашего роутера.
2. Настройте роутер на прием соединений по 443 порту и пересылку на контейнер nginx proxy manager (Асус называет это Virtual Server / Port Forwarding).
3. В папке контейнера nginx-proxy-manager создайте простенький вебсайт используя ИскИна. Можно добавить ```robots.txt``` в корень с ```User-agent: *``` и ```Disallow: /``` чтобы отбить ботам желание его индексировать.
![пример папки с сайтом](https://github.com/ageev/SmartHome/blob/master/Pictures/vless_website.png)
4. Теперь нужно добавить правильную конфигурацию в контейнер nginx-proxy-manager.
   - добавьте новый хост video.your-domain.com с редиректом на http://<NAS_IP>:8082. Обязательно тыкнуть "websocket support"!
   - идите на вкладку Advanced и добавьте такую конфигурацию:
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
Теперь админка у вас сидит по адресу ```https://video.your-domain.com/<secret_token>```, а клиенты стучатся через websocket на ```https://video.your-domain.com/ws```.
Все остальные видят безобидный вебсайт. 

# Конфигурация inbound
Создайте vless inbound на порту 8082. 8082 - это внутренний порт. Клиенты будут стучаться на 443. Так что конфигурации, которые генерирует 3x-ui, нужно будет подправлять. 
Из такой:
```
vless://blablablabla@video.your-website.com:8082?type=ws&encryption=none&path=%2Fws&host=&security=none#User
```
в такую:
```
vless://blablablabla@video.your-website.com:443?type=ws&encryption=none&path=%2Fws&host=&security=tls#User
```

Теперь её можно добавлять в клиента и пробовать достучаться снаружи. 

# Коступ к локальной сети из вне
Чтобы дать некоторым пользователям возможность не только пользоваться интернетом, но и получать доступ к локальным ресурсам на вкладке "Xray Configs" > "Routing Rules" добавьте правила:

1. Inbound == "api", Outbound == "api";
2. Client == <Имя клиентов через запятую>, Outbound == "direct";
3. Destination IP == "geoip:private", Outbound == "blocked".

Теперь "Xray Configs" > "Outbounds":

1. Tag "proxy", protocol "freedom";
2. Tag "direct", protocol "freedom";
3. Tag "blocked", protocol "blackhole".
