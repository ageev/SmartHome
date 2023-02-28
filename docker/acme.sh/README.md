# Acme.sh
Это утилита для коммандной строки, позволяющая получать и обновлять HTTPS сертификаты. Её можно использовать и без докера, но с докером безопасней.

Основные плюсы:
- поддерживает DNS челленджи для ACME, то есть может работать без публикации ваших веб ресурсов наружу. DNS челенджи работают только через DNS API вашего регистратора. То есть можно получить сертификат, который будет использоваться исключительно в локальной сети
- может пушить свежие сертификаты в Synology DSM. Таким образом решается проблема получения доверенного сертификата на DSM без публикации НАСа наружу
- работает с телеграммом. Приятный бонус

## Пререквизиты
1. домен. Я использую домен, купленный Gandi.net. *acme.sh поддерживает не всех регистраторов!*
2. API токен. Нужно сгенерировать его в настройках вашего регистратора
3. Нужно создать несколько доменных имён в консоли регистратора, которые вы затем будете использовать уже внутри сети. Ну например ```dsm 10800 IN A 192.168.1.5```. Теперь URL ```dsm.mydomain.com``` указывает на ваш НАС. Естественно работать такой URL будет только внутри локальной сети
4. нужно вручную создать несколько папок на НАСе: ```/volume1/docker/acme/config``` и ```/volume1/docker/acme/key```
5. заведите нового пользователя на НАСе: имя ```acme```, добавьте в группу админов (к сожалению, без этого никак). Можно и нужно лишить этого пользователя всех прав, кроме доступа к DSM
6. когда в этих папках acme.sh создаст ключи - приватный ключ будет лежать с урезанными правами доступа (ну понятно то почему). Однако чтобы работать с этим ключом из других контейнеров вам нужно будет модифицировать эти разрешения (зайти через SSH на НАС. Выполнить ```sudo chmod 777 /path/to/key/file```. ВНИМАНИЕ! эта команда разрешает доступ к ключу любому локальному юзеру на НАСе)
7. я везде получаю и использую wildcard сертификат (т.е. со свездочкой. *.domain.com). Это удобно, так как не раскрывает внутренние имена в certificate transparrency feed'е

## docker-compose.yml
```yml
## install new versions
# cd /volume1/docker
# sudo docker-compose pull
# sudo docker-compose up -d
## clean docker
# sudo docker system prune -a  
## rebuild containers
# sudo docker-compose up -d --build

---
version: "3.9"
services:
  acme.sh:
    image: neilpang/acme.sh
    container_name: acme.sh
    environment:
      - PUID=1029  # CHANGE ME!
      - PGID=100   # CHANGE ME!
      - TZ=Europe/Zurich
      - GANDI_LIVEDNS_KEY=<YOUR_GANDI_TOKEN>
      - TELEGRAM_BOT_APITOKEN=<TELEGA_TOKEN>
      - TELEGRAM_BOT_CHATID=<CHAT_ID>
      #SYNO Deploy hook 
      - SYNO_Scheme=https
      - SYNO_Hostname=dsm.domain.com #NAS's address. CHANGE ME!
      - SYNO_Port="5001"
      - SYNO_Username=acme
      - SYNO_Password=<PASSWORD>  # CHANGE ME!
#      - SYNO_DID= #copy did cookie parameter from browser's DSM admin session
      - SYNO_Certificate=*.domain.com # CHANGE ME!
      - SYNO_Create=1
    network_mode: host
    volumes:
      - /volume1/docker/acme/config:/acme.sh
      - /volume1/docker/acme/key:/key
    command: daemon
    restart: unless-stopped
    network_mode: "bridge"
```

после установки контейнера нужно выполнить ряд команд в консоли НАСа:
```
sudo docker exec acme.sh acme.sh --register-account -m <YOUR EMAIL>
sudo docker exec tool-acme.sh acme.sh --set-notify --notify-hook telegram
sudo docker exec tool-acme.sh acme.sh --issue -d *.domain.com -k 4096 -ak 4096 --dns dns_gandi_livedns --dnssleep 300
sudo docker exec tool-acme.sh acme.sh --deploy -d *.domain.com --deploy-hook synology_dsm
```
