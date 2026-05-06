# Vaultwarden

**[English](#english)** | **[Русский](#russian)**

Setting up Vaultwarden with Caddy and Gandi DNS on a Synology NAS.

---

<a id="english"></a>

## English

### Contents

- [KeePass vs Bitwarden](#keepass-vs-bitwarden)
- [Assumptions and requirements](#assumptions-and-requirements)
- [Vaultwarden design](#vaultwarden-design)
- [Preparation: docker user](#preparation-docker-user)
- [Setting up Caddy](#setting-up-caddy)
- [docker-compose.yml](#docker-composeyml)
- [Nginx Proxy Manager configuration](#nginx-proxy-manager-configuration)

### KeePass vs Bitwarden

I'd been using KeePass for many years. The big challenge is always synchronising the database across multiple devices. A better solution is the client-server model: a single centralised database. Bitwarden is open source, but the official hosted offering is paid. Vaultwarden is a free alternative — and it works with the official Windows / Android clients (developed by Bitwarden), which are great.

Recommendation: install Bitwarden's browser extensions and the mobile app (enable fingerprint login).

### Assumptions and requirements

These are the conditions on my side. Yours may differ — keep that in mind.

1. Docker on a Synology NAS DS723+ with DSM 7.1.
2. No ports/services published to the internet. To reach Vaultwarden remotely I use VPN.
3. Because no HTTP(S) ports are reachable from the internet, I have to use the DNS-01 ACME challenge to get a Let's Encrypt cert.
4. I use Gandi for domain names. Caddy has a Gandi module — no problem.
5. Ports 80/443 are already in use on the NAS.

You need a real domain name to issue an HTTPS certificate. You need a proper HTTPS certificate for the Bitwarden browser extensions to work. So I have a Gandi DNS record (let's say `vw.example.com`) pointing at my local NAS IP.

### Vaultwarden design

> **Update:** I've moved to Nginx Proxy Manager for all certificate needs. The Caddy section below is kept for historical reasons — I no longer use it.

Vaultwarden requires HTTPS, so everyone uses Caddy (reverse proxy) plus Let's Encrypt. Caddy can issue and renew certs automatically.

To run Vaultwarden you need:

- 2 containers: Caddy and Vaultwarden.
- A domain or subdomain for the HTTPS cert.

### Preparation: docker user

I have a separate, very low-privilege user for docker. That's why you'll see `PUID` / `PGID` in the compose file below — they tell docker to start the container in that user's context, so a malicious container can't do much damage.

One catch: if Caddy runs at user level, it can't bind low ports (<1000). You can't use 80/443 in Caddy's config.

`PUID` is a unique number — to get yours, SSH to the NAS and run `id <docker_user>`. The docker user must have read/write access to `/volume1/docker/<container>`.

### Setting up Caddy

I need a custom Caddy docker image with Gandi support.

1. Create `/volume1/docker/Dockerfile`:

   ```dockerfile
   FROM caddy:builder AS builder
   RUN xcaddy build --with github.com/caddy-dns/gandi

   FROM caddy:latest
   COPY --from=builder /usr/bin/caddy /usr/bin/caddy
   ```

2. Create directories:

   ```
   /volume2/docker/caddy
   /volume2/docker/caddy/data
   /volume2/docker/caddy/config
   /volume2/docker/caddy/log
   /volume2/docker/vaultwarden
   ```

3. Create `/volume1/docker/caddy/caddyfile`. The variables (`DOMAIN`, `EMAIL`, …) are defined later in the compose file:

   ```caddyfile
   {
     # Default HTTP port has to be changed, otherwise Caddy
     # won't start when 80 is already taken — even if you don't use HTTP.
     http_port 4080
     acme_dns gandi {$GANDI_API_TOKEN}
     # Try uncommenting this if Caddy goes to ZeroSSL or another provider for
     # the cert and errors out — DNS challenge isn't supported on every endpoint.
     # acme_ca https://acme-v02.api.letsencrypt.org/directory
     email {$EMAIL}
   }

   # HTTPS on 4443
   {$DOMAIN}:4443 {
     tls {
       dns gandi {$GANDI_API_TOKEN}
     }

     log {
       output file {$LOG_FILE}
       level debug
     }

     encode zstd gzip

     # Vaultwarden on 8088
     reverse_proxy /notifications/hub/negotiate localhost:8088
     reverse_proxy localhost:8088

     # Websocket port (default 3012)
     reverse_proxy /notifications/hub localhost:3012

     header / {
       Strict-Transport-Security "max-age=31536000;"
       X-XSS-Protection "1; mode=block"
       X-Frame-Options "DENY"
       X-Robots-Tag "none"
       -Server
     }
   }
   ```

### docker-compose.yml

Edit `/volume2/docker/docker-compose.yml`:

```yaml
---
version: "3.9"
services:
  vaultwarden:
    container_name: vaultwarden
    image: vaultwarden/server
    user: 1028:100
    environment:
      - TZ=Europe/Zurich
      - WEBSOCKET_ENABLED=true               # required for websockets
      # - SIGNUPS_ALLOWED=false              # disable signups
      - DOMAIN=https://vw.example.com/secret_path
      # Admin panel — only over local IP!
      # - ADMIN_TOKEN=<random_token>         # generate via: openssl rand -base64 48
      - ROCKET_PORT=8088
      - ROCKET_WORKERS=20
      - LOG_FILE=/data/bitwarden.log
      - EXTENDED_LOGGING=true
      - LOG_LEVEL=warn
      - SHOW_PASSWORD_HINT=false
      - DISABLE_ICON_DOWNLOAD=true           # outgoing internet blocked
    volumes:
      - /volume1/docker/vaultwarden:/data
    restart: unless-stopped
    ports:
      - 8088:8088
      - 3012:3012
    network_mode: "bridge"
    networks:
      - internet_blocked   # custom network with no internet access
    dns:
      - 10.0.1.9
```

SSH to the NAS and bring it up:

```bash
cd /volume1/docker
sudo docker-compose up -d --build
```

Give Caddy a few minutes to obtain the cert. Then go to `https://<your_domain>:4443` — you should see the Bitwarden login page.

You can adjust admin settings at `https://<your_domain>:4443/admin` using the token from the compose file. **Don't forget** to change the **Domain URL** in General settings to `https://<your_domain>:4443`, otherwise attachment downloads won't work. To disable the admin panel afterwards, remove/comment `ADMIN_TOKEN` from the compose file **and** from `/volume1/docker/vaultwarden/config.json`.

> Tip: I also use Caddy's certificate to secure the AdGuard container. Convenient — Let's Encrypt certs are valid only for 3 months and Caddy manages renewals. Just mount the certificate folder, e.g.:
>
> ```
> /volume1/docker/caddy/data/caddy/certificates:/opt/adguardhome/cert
> ```

### Nginx Proxy Manager configuration

As mentioned, I no longer use Caddy. Here's the NPM config I use to expose Vaultwarden over HTTPS:

```nginx
# A fake website shown to all external users, except those who know /secret_token
root /opt/websites/fake_website;
index index.html;
location / {
    try_files $uri /index.html;
}

location /secret_token/admin {
    return 404;
}

location /secret_token/ {
    proxy_pass http://10.0.1.5:8088;   # NAS IP
    proxy_set_header Host $host;
    proxy_set_header X-Real-IP $remote_addr;
    proxy_set_header X-Forwarded-For $proxy_add_x_forwarded_for;
    proxy_set_header X-Forwarded-Proto $forward_scheme;
}

location /secret_token/notifications/hub {
    proxy_pass http://10.0.1.5:3012;   # NAS IP
    proxy_set_header Upgrade $http_upgrade;
    proxy_set_header Connection "upgrade";
    proxy_set_header Host $host;
    proxy_set_header X-Real-IP $remote_addr;
    proxy_set_header Forwarded $remote_addr;
    proxy_set_header X-Forwarded-For $proxy_add_x_forwarded_for;
    proxy_set_header X-Forwarded-Proto $forward_scheme;
}

location /secret_token/notifications/hub/negotiate {
    proxy_set_header Host $host;
    proxy_set_header X-Real-IP $remote_addr;
    proxy_set_header X-Forwarded-For $proxy_add_x_forwarded_for;
    proxy_set_header X-Forwarded-Proto $forward_scheme;
    proxy_pass http://10.0.1.5:8088;   # NAS IP
}
```

---

<a id="russian"></a>

## Русский

### Содержание

- [KeePass vs Bitwarden](#keepass-vs-bitwarden-1)
- [Условия и требования](#условия-и-требования)
- [Архитектура Vaultwarden](#архитектура-vaultwarden)
- [Подготовка: docker-пользователь](#подготовка-docker-пользователь)
- [Настройка Caddy](#настройка-caddy)
- [docker-compose.yml](#docker-composeyml-1)
- [Конфигурация Nginx Proxy Manager](#конфигурация-nginx-proxy-manager)

### KeePass vs Bitwarden

Я много лет пользовался KeePass. Главная боль — синхронизация базы между устройствами. Лучше использовать client-server: единая централизованная база. Bitwarden — open source, но официальный хостинг платный. Vaultwarden — бесплатная альтернатива. Работает с официальными клиентами Windows / Android от Bitwarden — а они отличные.

Рекомендация: поставьте плагины Bitwarden в браузер и мобильное приложение (включите вход по отпечатку пальца).

### Условия и требования

Это мои условия. У вас могут быть другие — учитывайте.

1. Docker на Synology NAS DS723+ с DSM 7.1.
2. Никаких портов/сервисов наружу. Если нужно достучаться до Vaultwarden из интернета — через VPN.
3. Поскольку HTTP(S)-порты наружу закрыты, для получения сертификата Let's Encrypt нужен DNS-01 ACME challenge.
4. Домены у меня на Gandi. У Caddy есть модуль для Gandi — без проблем.
5. Порты 80/443 на НАСе уже заняты.

Чтобы получить HTTPS-сертификат, нужен реальный домен. И нормальный HTTPS-сертификат нужен для работы плагинов Bitwarden в браузере. У меня в Gandi настроена DNS-запись (например, `vw.example.com`), которая указывает на локальный IP НАСа.

### Архитектура Vaultwarden

> **Обновление:** я перевёл всё на Nginx Proxy Manager. Секцию Caddy ниже оставил для истории — больше им не пользуюсь.

Vaultwarden требует HTTPS, поэтому все используют Caddy (reverse proxy) + Let's Encrypt. Caddy умеет автоматически получать и обновлять сертификаты.

Для запуска Vaultwarden нужно:

- 2 контейнера: Caddy и Vaultwarden.
- Домен/субдомен под HTTPS-сертификат.

### Подготовка: docker-пользователь

У меня отдельный пользователь для docker с очень урезанными правами. Поэтому в compose-файле ниже есть `PUID` / `PGID` — они говорят docker'у запустить контейнер в контексте этого пользователя. Если контейнер окажется вредоносным, далеко он не уедет.

Подвох: если Caddy запущен под user-level, он не сможет слушать низкие порты (<1000). 80/443 в конфиге Caddy — забудьте.

`PUID` — это число; узнать своё: SSH на НАС и `id <docker_user>`. У этого пользователя должны быть права на чтение/запись в `/volume1/docker/<container>`.

### Настройка Caddy

Нужен кастомный docker-образ Caddy с поддержкой Gandi.

1. Создайте `/volume1/docker/Dockerfile`:

   ```dockerfile
   FROM caddy:builder AS builder
   RUN xcaddy build --with github.com/caddy-dns/gandi

   FROM caddy:latest
   COPY --from=builder /usr/bin/caddy /usr/bin/caddy
   ```

2. Создайте папки:

   ```
   /volume2/docker/caddy
   /volume2/docker/caddy/data
   /volume2/docker/caddy/config
   /volume2/docker/caddy/log
   /volume2/docker/vaultwarden
   ```

3. Создайте `/volume1/docker/caddy/caddyfile`. Переменные (`DOMAIN`, `EMAIL`, …) определяются позже в compose:

   ```caddyfile
   {
     # Дефолтный HTTP-порт надо поменять, иначе Caddy не запустится,
     # если 80-й уже занят — даже если вы HTTP не используете.
     http_port 4080
     acme_dns gandi {$GANDI_API_TOKEN}
     # Если Caddy уходит за сертом на ZeroSSL или другой провайдер
     # и падает — попробуйте раскомментировать.
     # acme_ca https://acme-v02.api.letsencrypt.org/directory
     email {$EMAIL}
   }

   # HTTPS на 4443
   {$DOMAIN}:4443 {
     tls {
       dns gandi {$GANDI_API_TOKEN}
     }

     log {
       output file {$LOG_FILE}
       level debug
     }

     encode zstd gzip

     # Vaultwarden на 8088
     reverse_proxy /notifications/hub/negotiate localhost:8088
     reverse_proxy localhost:8088

     # Websocket-порт (по умолчанию 3012)
     reverse_proxy /notifications/hub localhost:3012

     header / {
       Strict-Transport-Security "max-age=31536000;"
       X-XSS-Protection "1; mode=block"
       X-Frame-Options "DENY"
       X-Robots-Tag "none"
       -Server
     }
   }
   ```

### docker-compose.yml

Правим `/volume2/docker/docker-compose.yml`:

```yaml
---
version: "3.9"
services:
  vaultwarden:
    container_name: vaultwarden
    image: vaultwarden/server
    user: 1028:100
    environment:
      - TZ=Europe/Zurich
      - WEBSOCKET_ENABLED=true               # нужно для websocket'ов
      # - SIGNUPS_ALLOWED=false              # отключить регистрацию
      - DOMAIN=https://vw.example.com/secret_path
      # Админка — только из локалки!
      # - ADMIN_TOKEN=<random_token>         # сгенерировать: openssl rand -base64 48
      - ROCKET_PORT=8088
      - ROCKET_WORKERS=20
      - LOG_FILE=/data/bitwarden.log
      - EXTENDED_LOGGING=true
      - LOG_LEVEL=warn
      - SHOW_PASSWORD_HINT=false
      - DISABLE_ICON_DOWNLOAD=true           # исходящий интернет заблокирован
    volumes:
      - /volume1/docker/vaultwarden:/data
    restart: unless-stopped
    ports:
      - 8088:8088
      - 3012:3012
    network_mode: "bridge"
    networks:
      - internet_blocked   # моя кастомная сеть без интернета
    dns:
      - 10.0.1.9
```

По SSH:

```bash
cd /volume1/docker
sudo docker-compose up -d --build
```

Дайте Caddy пару минут на получение сертификата. Откройте `https://<your_domain>:4443` — должна показаться страница входа Bitwarden.

Админку можно настроить на `https://<your_domain>:4443/admin` с помощью токена из compose. **Важно**: смените **Domain URL** в General settings на `https://<your_domain>:4443`, иначе скачивание вложений не будет работать. Чтобы отключить админку, уберите/закомментируйте `ADMIN_TOKEN` в compose **и** в `/volume1/docker/vaultwarden/config.json`.

> Совет: сертификатом Caddy я заодно защищаю контейнер AdGuard. Удобно — сертификаты Let's Encrypt живут всего 3 месяца, обновлениями занимается сам Caddy. Достаточно примонтировать папку с сертификатами:
>
> ```
> /volume1/docker/caddy/data/caddy/certificates:/opt/adguardhome/cert
> ```

### Конфигурация Nginx Proxy Manager

Caddy я больше не использую. Вот конфиг NPM, через который ходит Vaultwarden по HTTPS:

```nginx
# Фейковый сайт для всех внешних пользователей, кроме тех, кто знает /secret_token
root /opt/websites/fake_website;
index index.html;
location / {
    try_files $uri /index.html;
}

location /secret_token/admin {
    return 404;
}

location /secret_token/ {
    proxy_pass http://10.0.1.5:8088;   # IP НАСа
    proxy_set_header Host $host;
    proxy_set_header X-Real-IP $remote_addr;
    proxy_set_header X-Forwarded-For $proxy_add_x_forwarded_for;
    proxy_set_header X-Forwarded-Proto $forward_scheme;
}

location /secret_token/notifications/hub {
    proxy_pass http://10.0.1.5:3012;   # IP НАСа
    proxy_set_header Upgrade $http_upgrade;
    proxy_set_header Connection "upgrade";
    proxy_set_header Host $host;
    proxy_set_header X-Real-IP $remote_addr;
    proxy_set_header Forwarded $remote_addr;
    proxy_set_header X-Forwarded-For $proxy_add_x_forwarded_for;
    proxy_set_header X-Forwarded-Proto $forward_scheme;
}

location /secret_token/notifications/hub/negotiate {
    proxy_set_header Host $host;
    proxy_set_header X-Real-IP $remote_addr;
    proxy_set_header X-Forwarded-For $proxy_add_x_forwarded_for;
    proxy_set_header X-Forwarded-Proto $forward_scheme;
    proxy_pass http://10.0.1.5:8088;   # IP НАСа
}
```
