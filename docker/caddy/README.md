# Caddy

Caddy is a proxy server, which works with ACME.
| :exclamation:  Currently I replaced Caddy with NGINX Proxy Manager   |
|-----------------------------------------|

# Installation

Caddy's docker doesnt support Gandi DNS API out-of-the-box, so I need to compile it

1. Create the file ```/volume1/docker/Dockerfile```
```
FROM caddy:builder AS builder
RUN xcaddy build --with github.com/caddy-dns/gandi

FROM caddy:latest
COPY --from=builder /usr/bin/caddy /usr/bin/caddy
```
2. Create directories
```
/volume1/docker/caddy
/volume1/docker/caddy/data
/volume1/docker/caddy/config
/volume1/docker/caddy/log
```

3. create a file ```/volume1/docker/caddy/caddyfile```. This file has some variables like DOMAIN or EMAIL, which are defined later in the docker-compose file

! Config below is relevan for VaultWarden

```
{
  #default http port needs to be changed or Caddy will not start if it's already in use. Even if you don't use HTTP
  http_port 4080
  acme_dns gandi {$GANDI_API_TOKEN}
  #try to uncomment this if caddy goes to ZeroSSL or another servers for the cert and you get errors. DNS challenge is not supported for every endpoint
  #acme_ca https://acme-v02.api.letsencrypt.org/directory
  email {$EMAIL}
}

# start HTTPS on 4443
{$DOMAIN}:4443 {
  tls {
    dns gandi {$GANDI_API_TOKEN}
  }

  log {
    output file {$LOG_FILE}
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
