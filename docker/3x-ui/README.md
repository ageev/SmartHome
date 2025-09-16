3x-ui это удобный ГУИ для Xray - продвинутого прокси, который позволяет достучаться до домашней сети (и Интернета) практически из любой точки мира, даже если VPN активно блокируется. 
VLESS протокол настолько крут, что проходит даже через корпоративный SSL Decryption proxy. Даже UDP не проблема! Голосовые звонки Телеграмма работают на ура.  

Я хостю замаскированный сервер 3x-ui у себя на НАСе. В качестве клиентов использую NekoBox на винде и Hiddify на телефонах.

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
location / {
    try_files $uri /index.html;
}

location /ws {
    if ($http_upgrade != "websocket") {
        return 404;
    }
    proxy_pass http://<NAS_IP>:8082;
    proxy_redirect off;
    proxy_http_version 1.1;
    proxy_set_header Upgrade $http_upgrade;
    proxy_set_header Connection "upgrade";
    proxy_set_header Host $host;
    proxy_set_header X-Real-IP $remote_addr;
    proxy_set_header X-Forwarded-For $proxy_add_x_forwarded_for;
    proxy_read_timeout 52w;
}

location /<secret_token>/ {
    proxy_set_header X-Forwarded-For $proxy_add_x_forwarded_for;
    proxy_set_header X-Forwarded-Proto $scheme;
    proxy_set_header Host $http_host;
    proxy_set_header X-Real-IP $remote_addr;
    proxy_set_header Range $http_range;
    proxy_set_header If-Range $http_if_range; 
    proxy_redirect off;
    proxy_pass http://<NAS_IP>:2053;
}
```
Теперь админка у вас сидит по адресу ```https://video.your-domain.com/<secret_token>```, а клиенты стучатся через websocket на ```https://video.your-domain.com/ws```.
Все остальные видят безобидный вебсайт. 

# конфигурация inbound
Создайте vless inbound на порту 8082. 8082 - это внутренний порт. Клиенты будут стучаться на 443. Так что конфигурации, которые генерирует 3x-ui, нужно будет подправлять. 
Из такой ```vless://blablablabla@video.your-website.com:8082?type=ws&encryption=none&path=%2Fws&host=&security=none#User``` в такую:
vless://blablablabla@video.your-website.com:<span style="color:red">443</span>?type=ws&encryption=none&path=%2Fws&host=&security=<span style="color:red">tls</span>#User

Теперь её можно добавлять в клиента и пробовать достучаться снаружи. 
