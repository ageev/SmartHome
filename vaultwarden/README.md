# Setting up the Vaultwarden with Caddy and Gandi DNS on Synology NAS
## Vaultwarden
Vaultwarden is an opensource fork of the BitWarden
caddyfile
```
{
  #default http port needs to be changed or Caddy will not start if it's already in use. Even if you don't use HTTP
  http_port 4080
  acme_dns gandi {env.GANDI_API_TOKEN}
  email {env.EMAIL}
}

# start HTTPS on 4443
{env.DOMAIN}:4443 {
  tls {
    dns gandi {env.GANDI_API_TOKEN}
  }

  log {
    output file {env.LOG_FILE}
    level debug
  }

  encode zstd gzip

  #connect to vaultwarden on 8088
  reverse_proxy /notifications/hub/negotiate localhost:8088
  reverse_proxy localhost:8088

  #websocket port (default==3012)
  reverse_proxy /notifications/hub localhost:3012
  header / {
    # Enable HTTP Strict Transport Security (HSTS)
    Strict-Transport-Security "max-age=31536000;"
    # Enable cross-site filter (XSS) and tell browser to block detected attacks
    X-XSS-Protection "1; mode=block"
    # Disallow the site to be rendered within a frame (clickjacking protection)
    X-Frame-Options "DENY"
    # Prevent search engines from indexing (optional)
    X-Robots-Tag "none"
    # Server name remove
    -Server
  }
}
```
Dockerfile
```
FROM caddy:builder AS builder
RUN xcaddy build --with github.com/caddy-dns/gandi

FROM caddy:latest
COPY --from=builder /usr/bin/caddy /usr/bin/caddy
```
docker-compose.yml
```yaml
---
version: "3.9"
services:
  caddy:
    container_name: caddy
    build: .  #builds caddy + gandi version. see Dockerfile for details
    environment:
      - PUID=<your docker user PID>
      - PGID=<users group PGID>
      - TZ=Europe/Amsterdam
      - ACME_AGREE=true
      - DOMAIN=*.example.com #wildcard domains are convinient - you can reuse them in other containers
      - EMAIL=<email>
      - GANDI_API_TOKEN=<token>
      - LOG_FILE=/var/log/caddy/caddy.log
    volumes:
      - /volume1/docker/caddy/caddyfile:/etc/caddy/Caddyfile
      - /volume1/docker/caddy/data:/data
      - /volume1/docker/caddy/config:/config
      - /volume1/docker/caddy/log:/var/log/caddy
    restart: unless-stopped
    network_mode: "host"
  
vaultwarden:
    container_name: vaultwarden
    image: vaultwarden/server:latest
    environment:
      - PUID=<your_docker_user_PUID>
      - PGID=<users_group_PGID>
      - TZ=Europe/Amsterdam
      - WEBSOCKET_ENABLED=true # Required to use websockets
#      - SIGNUPS_ALLOWED=false   # set to false to disable signups
      - DOMAIN=https://pass.example.com
#      - SMTP_HOST=[MAIL-SERVER]
#      - SMTP_FROM=[E-MAIL]
#      - SMTP_PORT=587
#      - SMTP_SSL=true
#      - SMTP_USERNAME=[E-MAIL]
#      - SMTP_PASSWORD=[SMTP-PASS]
      - ADMIN_TOKEN=<random_token> # openssl rand -base64 48
      - ROCKET_PORT=8088
#      - YUBICO_CLIENT_ID=[OPTIONAL]
#      - YUBICO_SECRET_KEY=[OPTIONAL]
      - LOG_FILE=/data/bitwarden.log
      - EXTENDED_LOGGING=true
      - LOG_LEVEL=warn
      - ROCKET_WORKERS=10
      - SHOW_PASSWORD_HINT=false
      - DISABLE_ICON_DOWNLOAD=true
    volumes:
      - /volume1/docker/vaultwarden:/data
    restart: unless-stopped
    network_mode: "host"
```
