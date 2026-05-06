# Caddy

**[English](#english)** | **[Русский](#russian)**

> I've replaced Caddy with [Nginx Proxy Manager](https://github.com/ageev/SmartHome/tree/master/docker/nginx-proxy-manager). Kept here for reference.
>
> Я заменил Caddy на [Nginx Proxy Manager](https://github.com/ageev/SmartHome/tree/master/docker/nginx-proxy-manager). Раздел оставлен для истории.

---

<a id="english"></a>

## English

Caddy is a proxy server with built-in ACME support.

### Installation

The official Caddy docker image doesn't include the Gandi DNS module out of the box, so I have to build a custom image.

1. Create `/volume1/docker/Dockerfile`:

   ```dockerfile
   FROM caddy:builder AS builder
   RUN xcaddy build --with github.com/caddy-dns/gandi

   FROM caddy:latest
   COPY --from=builder /usr/bin/caddy /usr/bin/caddy
   ```

2. Create the directories:

   ```
   /volume1/docker/caddy
   /volume1/docker/caddy/data
   /volume1/docker/caddy/config
   /volume1/docker/caddy/log
   ```

3. Create `/volume1/docker/caddy/caddyfile`. The variables (`DOMAIN`, `EMAIL`, …) are defined later in the docker-compose file.

   > The config below is the one I used to front [Vaultwarden](https://github.com/ageev/SmartHome/tree/master/docker/vaultwarden).

   ```caddyfile
   {
     # Default HTTP port has to be changed, otherwise Caddy won't start
     # if 80 is already in use — even if you don't use HTTP yourself.
     http_port 4080
     acme_dns gandi {$GANDI_API_TOKEN}
     # Try uncommenting if Caddy hits ZeroSSL (or another provider) for the cert
     # and errors out — DNS challenge isn't supported on every endpoint.
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

---

<a id="russian"></a>

## Русский

Caddy — это прокси-сервер с встроенной поддержкой ACME.

### Установка

В официальный docker-образ Caddy не входит модуль Gandi DNS, поэтому нужен кастомный образ.

1. Создайте `/volume1/docker/Dockerfile`:

   ```dockerfile
   FROM caddy:builder AS builder
   RUN xcaddy build --with github.com/caddy-dns/gandi

   FROM caddy:latest
   COPY --from=builder /usr/bin/caddy /usr/bin/caddy
   ```

2. Создайте папки:

   ```
   /volume1/docker/caddy
   /volume1/docker/caddy/data
   /volume1/docker/caddy/config
   /volume1/docker/caddy/log
   ```

3. Создайте `/volume1/docker/caddy/caddyfile`. Переменные (`DOMAIN`, `EMAIL`, …) задаются позже в docker-compose.

   > Конфиг ниже — для [Vaultwarden](https://github.com/ageev/SmartHome/tree/master/docker/vaultwarden).

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
