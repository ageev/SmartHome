# acme.sh

**[English](#english)** | **[Русский](#russian)**

---

<a id="english"></a>

## English

A command-line utility for issuing and renewing HTTPS certificates. Works without docker too, but inside docker it's safer.

**Main pros:**

- Supports DNS challenges for ACME, meaning it works without exposing your web resources to the public internet. DNS challenges go through your registrar's DNS API, so you can get a certificate that's only ever used inside the LAN.
- Can push fresh certificates straight into Synology DSM. Solves the "trusted cert for DSM without publishing the NAS" problem.
- Speaks Telegram. Nice bonus.

### Prerequisites

1. **A domain.** I use one registered with Gandi.net. *acme.sh doesn't support every registrar!*
2. **API token.** Generate it in your registrar's settings.
3. **Internal DNS records.** Create a few records in the registrar console that you'll then use inside your LAN. For example, `dsm 10800 IN A 192.168.1.5` — now `dsm.mydomain.com` points at your NAS. Naturally that URL only resolves inside the local network.
4. **Folders on the NAS** must exist beforehand: `/volume1/docker/acme/config` and `/volume1/docker/acme/key`.
5. **A new NAS user named `acme`**, added to the admin group (sadly there's no way around it). Strip every permission except DSM access.
6. **File permissions on keys.** When acme.sh writes the keys into these folders, the private key gets restrictive permissions (rightly so). But to use that key from other containers you'll need to widen them — SSH into the NAS and run `sudo chmod 777 /path/to/key/file`. **Warning:** this opens the key to every local user on the NAS.
7. **Wildcard cert.** I always issue wildcard certs (`*.domain.com`). Convenient, and doesn't leak internal hostnames into certificate-transparency feeds.
8. **Telegram bot.** I have a private chat with a bot. acme.sh can post a message there whenever the cert is renewed. Want the same? Have your bot token and chat ID ready.

### docker-compose.yml

```yaml
---
version: "3.9"
services:
  acme.sh:
    image: neilpang/acme.sh
    container_name: acme.sh
    hostname: acme
    environment:
      - PUID=1027   # put yours!
      - PGID=100    # put yours!
      - TZ=Europe/Zurich
      - GANDI_LIVEDNS_KEY=<your_token>          # Confidential!
      - TELEGRAM_BOT_APITOKEN=<somerandomstring># Confidential!
      - TELEGRAM_BOT_CHATID=<chatid>            # Confidential!
      # SYNO Deploy hook
      - SYNO_Scheme=https
      - SYNO_Hostname=dsm.your-domain.com  # IP or hostname reachable on the NAS
      - SYNO_Port="443"
      - SYNO_Username=acme
      - SYNO_Password=<password>           # Confidential!
      - SYNO_Certificate=*.your-domain.com
      - SYNO_Create=1
    volumes:
      - /volume1/docker/acme/config:/acme.sh
      - /volume1/docker/acme/key:/key
    command: daemon
    restart: unless-stopped
    network_mode: "bridge"
```

After the container is up, run these on the NAS (replace `domain.com` with your domain):

```bash
sudo docker exec acme.sh acme.sh --register-account -m <YOUR_EMAIL>
sudo docker exec acme.sh acme.sh --set-notify --notify-hook telegram
sudo docker exec acme.sh acme.sh --issue -d *.domain.com -k 4096 -ak 4096 --dns dns_gandi_livedns --dnssleep 300
sudo docker exec acme.sh acme.sh --deploy -d *.domain.com --deploy-hook synology_dsm --insecure
```

There's a cron job inside the container that periodically walks the config folders. View it with:

```bash
sudo docker exec acme.sh crontab -l
```

### Links

1. <https://github.com/acmesh-official/acme.sh/wiki/deployhooks#20-deploy-the-certificate-to-synology-dsm>

---

<a id="russian"></a>

## Русский

Утилита командной строки для получения и обновления HTTPS-сертификатов. Можно использовать и без docker, но с docker безопаснее.

**Основные плюсы:**

- Поддерживает DNS-челленджи ACME, то есть может работать без публикации ваших веб-ресурсов наружу. DNS-челленджи идут через DNS API регистратора, так что можно получить сертификат, который будет использоваться исключительно в локальной сети.
- Умеет пушить свежие сертификаты прямо в Synology DSM. Так решается проблема доверенного сертификата на DSM без публикации НАСа.
- Работает с Telegram. Приятный бонус.

### Пререквизиты

1. **Домен.** Я использую купленный на Gandi.net. *acme.sh поддерживает не всех регистраторов!*
2. **API-токен.** Сгенерируйте его в настройках регистратора.
3. **Внутренние DNS-записи.** В консоли регистратора создаём несколько имён, которые будут использоваться внутри сети. Например, `dsm 10800 IN A 192.168.1.5` — теперь URL `dsm.mydomain.com` указывает на ваш НАС. Естественно, такой URL работает только внутри локальной сети.
4. **Папки на НАСе** должны существовать заранее: `/volume1/docker/acme/config` и `/volume1/docker/acme/key`.
5. **Новый пользователь на НАСе** с именем `acme`, добавьте его в группу администраторов (к сожалению, без этого никак). Можно и нужно лишить его всех прав, кроме доступа к DSM.
6. **Права на ключи.** Когда acme.sh положит ключи в эти папки, приватный ключ будет с урезанными правами (понятно почему). Чтобы работать с этим ключом из других контейнеров, права придётся ослабить — зайти по SSH на НАС и выполнить `sudo chmod 777 /path/to/key/file`. **Внимание:** эта команда разрешает доступ к ключу любому локальному пользователю на НАСе.
7. **Wildcard.** Я везде получаю и использую wildcard-сертификат (`*.domain.com`). Удобно и не раскрывает внутренние имена в certificate-transparency-фиде.
8. **Telegram-бот.** У меня в Telegram свой чат с ботом. acme.sh умеет присылать туда сообщение, когда сертификат обновился. Хотите так же? Приготовьте токен и chatID.

### docker-compose.yml

```yaml
---
version: "3.9"
services:
  acme.sh:
    image: neilpang/acme.sh
    container_name: acme.sh
    hostname: acme
    environment:
      - PUID=1027   # ваш!
      - PGID=100    # ваш!
      - TZ=Europe/Zurich
      - GANDI_LIVEDNS_KEY=<your_token>          # Confidential!
      - TELEGRAM_BOT_APITOKEN=<somerandomstring># Confidential!
      - TELEGRAM_BOT_CHATID=<chatid>            # Confidential!
      # SYNO Deploy hook
      - SYNO_Scheme=https
      - SYNO_Hostname=dsm.your-domain.com  # IP или hostname НАСа
      - SYNO_Port="443"
      - SYNO_Username=acme
      - SYNO_Password=<password>           # Confidential!
      - SYNO_Certificate=*.your-domain.com
      - SYNO_Create=1
    volumes:
      - /volume1/docker/acme/config:/acme.sh
      - /volume1/docker/acme/key:/key
    command: daemon
    restart: unless-stopped
    network_mode: "bridge"
```

После запуска контейнера выполните на НАСе (замените `domain.com` на свой домен):

```bash
sudo docker exec acme.sh acme.sh --register-account -m <YOUR_EMAIL>
sudo docker exec acme.sh acme.sh --set-notify --notify-hook telegram
sudo docker exec acme.sh acme.sh --issue -d *.domain.com -k 4096 -ak 4096 --dns dns_gandi_livedns --dnssleep 300
sudo docker exec acme.sh acme.sh --deploy -d *.domain.com --deploy-hook synology_dsm --insecure
```

Внутри контейнера есть cron, который периодически пробегается по папкам в конфиге. Посмотреть его можно так:

```bash
sudo docker exec acme.sh crontab -l
```

### Ссылки

1. <https://github.com/acmesh-official/acme.sh/wiki/deployhooks#20-deploy-the-certificate-to-synology-dsm>
