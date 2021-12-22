# Setting up the Vaultwarden with Caddy and Gandi DNS on Synology NAS
## KeePass vs BitWarden
I've been using KeePass for many years. It's always a challange to synchronize the databases used on multiple devices. Maybe a better solution will be to use the client-server approach here and store all your passwords in a cetralized database. Bitwarden is an opensource solution, but standalone image is a paid service. Vaultwarden is a free alternative. It also works with official Windows / Android clients, developed by Bitwarden, which are actually great.
Vaultwarden can also work for small companies. The Send feature can be quite usefull in the enterprise world.

I only started recently to use Bitwarden/Vaultwarden, so many things are new for me too. 

## Asumptions and requirements
Those are conditions I have. Yours maybe different, keep this in mind
1. Docker on Synology NAS DS218+ with DSM 7.0.1
2. No ports/services published externally. If I need to access the vaultwarden from the internet - I use VPN
3. Because no HTTP(S) ports are available from the internet, I need to use the DNS-1 ACME challenge to get the Let's Encrypt cert
4. I use Gandi to manage my domain names. Caddy has a module for Gandi, so that's not a problem
5. 80/443 ports are already in use on my NAS

You need a real domain name to get HTTPS certificate. You need a proper HTTPS certificate for Bitwarden browser plugins to work.
So I have a DNS record at Gandi (let's say vw.example.com) which points to my local NAS IP.  

## Vaultwarden design
Vaultwarden needs https, so everyone is using Caddy (reverse proxy) + let's encrypt certs. Caddy can automatically get&renew HTTPS certs. 
So to use Vaultwarden you need:
* 2 containers: Caddy, Vaultwarden
* domain/subdomain name for HTTPS cert

## Preparation steps - docker user
I have a separate user for docker with very limited access rights. That's why you see "PUID" and "PGID" parameters in the docker-compose file below. They tell docker to start the container using the special user context. Now if container is malicious it will not be able to harm me much. 

One problem here - if Caddy is launched with user level permissions - you can't use low ports (e.g. <1000); you can't use 80/443 in Caddy's config.

PUID is a unique number, to get yours SSH to your NAS and run ```id <docker_user>```. Docker user should have sufficient rights to read/write to /volume1/docker/<container> dir

## Setting up Caddy
I need to build a custom Caddy docker image with Gandi support

1. create a file ```/volume1/docker/Dockerfile```
```
FROM caddy:builder AS builder
RUN xcaddy build --with github.com/caddy-dns/gandi

FROM caddy:latest
COPY --from=builder /usr/bin/caddy /usr/bin/caddy
```

2. create directories 
```
/volume1/docker/caddy
/volume1/docker/caddy/data
/volume1/docker/caddy/config
/volume1/docker/caddy/log
/volume1/docker/vaultwarden
```
6. create a file ```/volume1/docker/caddy/caddyfile```. This file has some variables like DOMAIN or EMAIL, which are defined later in the docker-compose file

caddyfile
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

4. create/edit ```/volume1/docker/docker-compose.yml``` file

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
      - DOMAIN=*.example.com # i suggest to get the wildcard cert - you can reuse them in other containers!
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
      - DOMAIN=https://vw.example.com # change this to the actual domain you use. It should be real & match the certificate issued by Caddy
#      - SMTP_HOST=[MAIL-SERVER]
#      - SMTP_FROM=[E-MAIL]
#      - SMTP_PORT=587
#      - SMTP_SSL=true
#      - SMTP_USERNAME=[E-MAIL]
#      - SMTP_PASSWORD=[SMTP-PASS]
      - ADMIN_TOKEN=<random_token> # run <openssl rand -base64 48> to get random token
      - ROCKET_PORT=8088
#      - YUBICO_CLIENT_ID=[OPTIONAL]
#      - YUBICO_SECRET_KEY=[OPTIONAL]
      - LOG_FILE=/data/bitwarden.log
      - EXTENDED_LOGGING=true
      - LOG_LEVEL=warn
      - ROCKET_WORKERS=10
      - SHOW_PASSWORD_HINT=false
#      - DISABLE_ICON_DOWNLOAD=true
    volumes:
      - /volume1/docker/vaultwarden:/data
    restart: unless-stopped
    network_mode: "host"
```

5. SSH to NAS & run docker-compose

```bash
cd /volume1/docker
sudo docker-compose up -d --build
```

6. give Caddy few minutes to get the cert. Go to "https://<your_domain>:4443". You should see the BitWarden login page
7. You can set some admin settings here "https://<your_domain>:4443/admin" using the token specified in the docker-compose file. Don't forget to change "Domain URL" in General settings to https://<your_domain>:**4443**. Otherwise attachment download will not work.
If you want to disable the admin panel - remove/comment "ADMIN_TOKEN" string from your docker-compose file AND from ```/volume1/docker/vaultwarden/config.json```
8. I use Caddy's certificate in some other containers. To do this you just need to mount the certificate folder. 
E.g.
```
/volume1/docker/caddy/data/caddy/certificates:/opt/adguardhome/cert
```
