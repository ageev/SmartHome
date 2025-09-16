Текущий состав моих докер контейнеров для умного дома.
  - [adguard](https://github.com/ageev/SmartHouse/tree/master/docker/adguard) - продвинутая резалка рекламы. Лучше чем [pihole](https://github.com/ageev/SmartHouse/tree/master/pi-hole)
  - (неактивен) [caddy](https://github.com/ageev/SmartHouse/tree/master/docker/caddy) - вебсервер с поддержкой ACME (автоматическая выдача HTTPS сертификатов) - *заменил на Nginx Proxy Manager + acme.sh*
  - [esphome](https://github.com/ageev/SmartHouse/tree/master/docker/esphome) - генератор прошивок и дэшборд для ESP32 (дешевый чип с поддержкой Wi-Fi и BLE). Позволяет перепрошивать всякие Tuya Smart и прочии девайсы для интеграции с Home Assistant
  - [home-assistant](https://github.com/ageev/SmartHome/tree/master/docker/home-assistant) (HA, homeass, хомяк)- универсальный интеграционный сервис умного дома. За счёт огромного количества аддонов позволяет связывать в единую систему очень разные компоненты. Например, Яндекс Станцию и Телеграм, датчики IKEA и вентилятор Xiaomi. Примеры автоматизаций внутри
  - [homer](https://github.com/ageev/SmartHome/tree/master/docker/homer) - простой вебсервер, на котором удобно сделать стартовую страницу умного дома. Когда мне нужно что-то открыть в домашней сети - я использую стартовую страницу Гомера
  - (неактивен) [transmission](https://github.com/ageev/SmartHome/tree/master/docker/transmission) - качалка торрентов. Выбрал её потому что у нее самая удобная интеграция с HA. Использую с плагином для браузера Torrent Control - позволяет скачивать всё сразу на НАС
  - [acme.sh](https://github.com/ageev/SmartHouse/tree/master/docker/acme.sh) - позволяет автоматизировать получение HTTPS сертификатов, посылает уведомоления в телегу, может закидывать обновленный сертификат прямо в DSM
  - [vaultwarden](https://github.com/ageev/SmartHouse/tree/master/docker/vaultwarden) - форк BitWarden - очень хороший менеджер паролей с клиентами под все платформы
  - [nginx proxy manager](https://github.com/ageev/SmartHouse/tree/master/docker/Nginx%20Proxy%20Manager) - единое окно (ну почти) для всех внутренних веб-порталов. Позволяет сделать "зеленый замочек" (HTTP -> HTTPS) там, где его отродясь не было. Один минус NGM - нужно искать уникальный кастомный прокси конфиг под каждое приложение. С Traefik такой проблемы нет. Надо сравнить
  - (неактивен) [mosquitto](https://github.com/ageev/SmartHome/tree/master/docker/zigbee2mqtt) - MQTT брокер. Нужен для zigbee2mqtt. Работает с очередью запросов к ZigBee устройствам для умного дома
  - (неактивен) [zigbee2mqtt](https://github.com/ageev/SmartHome/tree/master/docker/zigbee2mqtt) - позволяет интегрировать Zigbee устройства умного дома с home-assistant. Работает с USB стиком Sonoff Zigbee 3.0, воткнутым в USB порт НАСа. В целом, можно было использовать и встроенный в Home Assistant zigbee модуль
  - (неактивен) [plex](https://github.com/ageev/SmartHome/tree/master/docker/plex) - каталогизатор видео. Позволяет на лету транскодировать видеопотоки (если железо позволяет. DS218+ позволяет) под различные разрешения и устройства. Ну то есть можно стримить мультики на детские планшеты, и они сами могут выбирать, что смотреть. Пользуюсь редко. В основном, фильмы смотрим через NFS шары и Kodi
  - [paperless](https://github.com/ageev/SmartHome/tree/master/docker/zigbee2mqtt) - домашняя система архива документов. Удобно интегрируется со сканером и позволяет хранить копии всех бумажных документов (включая кассовые чеки для гарантий).
  - [3x-ui](https://github.com/ageev/SmartHome/tree/master/docker/3x-ui) - ВПН сервер на протоколе VLESS для доступа к локальной сети, Youtube и всего остального
  - [wyl](https://github.com/ageev/SmartHome/tree/master/docker/wyl) - простой контейнер, который позволяет мне видеть все устройства в локальной сети
  - [qBittorrent](https://github.com/ageev/SmartHome/tree/master/docker/qbittorrent) - торрент клиент. Нравиться GUI больше, чем у Transmission + есть интеграция с Телеграммом. 
  - [snowflake-proxy](https://github.com/ageev/SmartHome/tree/master/docker/snowflake-proxy) - у меня жирный интернет канал, поэтому часть траффика я даю проекту TOR. 
  - [zerotier](https://github.com/ageev/SmartHome/tree/master/docker/zerotier) - как-бы-ВПН, который позволяет засунуть очень разные системы в одну виртуальную сеть. Использую для бэкапа с НАСа на НАС. 
  - [prowlarr](https://github.com/ageev/SmartHome/tree/master/docker/prowlarr) - интегратор торрент треккеров. Даешь ему команду найти торрент - он ищет сразу по всем твоим любимым треккерам. Используется в связке с radarr
  - [radarr](https://github.com/ageev/SmartHome/tree/master/docker/radarr) - менеджер домашней видеобиблиотеки (тут писал про него подробнее https://habr.com/ru/post/505814/). Работает как-то так:
    - Добавляешь фильм в Watchlist на IMDB.com
    - лист синхронизируется с Radarr
    - Radarr даёт команду Prowlarr найти все раздачи с этим фильмом
    - Radarr фильтрует все раздачи в соответствии с задаными фильтрами (например, качество не хуже HD, английская или русская дорожка. Не менее 3х сидеров)
    - Если после фильтрации осталась нужная раздача, Radarr дает команду Transmission скачать нужный торрент
    - Кладёт раздачу в папочку Movies на НАСе, посылает уведомление через телегу
    - Если вдруг вышла раздача лучшего качества - скачивает её пока максимальное качество не достигнуто
  - [torrserver](https://github.com/ageev/SmartHome/tree/master/docker/torrserver) - сервер для NUM. В путешествиях я вожу с собой Amazon Fire TV стик. Воткнул в телевизор в отеле и можно смотреть свой Netflix. NUM в связке с домашним torrserver позволяет смотреть еще и торренты в странах, где за это наказывают

# пользователь docker
У меня создан специальный DSM пользователь docker. Этот пользователь имеет доступ только к папке docker, scans (для paperless) и media. Доступ ко всем приложениям запрещен. Контейнеры запускаются с полномочиями пользователя docker (не все это поддерживают!) через переменные PUID и PGID. Узнать свой идентификатор пользователя (uid) и группы (gid) можно через ```sudo synouser --get docker```. Более безопасный подход - создавать уникального пользователя под каждый контейнер.

# Структура папок docker и docker-compose
Так выглядит структура папок docker:
```
/volume2 #ssd
-/docker
--/container1
--/container2
...
--docker-compose.yml
```

Я совсем не использую GUI Synology для управления контейнерами. Соответственно вся конфигурация контейнеров происходит либо через docker-compose.yml либо через индувидуальные файлы конфигурации в папке контейнера. 
Если мне нужно обновить контейнеры я захожу на НАС через SSH...
```bash
cd /volume2/docker
sudo docker-compose pull #скачать новые образы контейнеров
sudo docker-compose up -d #создать или пересоздать контейнеры
sudo docker system prune -a #удалить кэш, старые образы и неиспользуемые контейнеры
```

## Blocking internet access FROM docker containers
Containers should not go to Internet unless absolutely must
1. Create two separate Docker networks in your ```docker-compose.yml``` file
```bash
networks:
  internet_allowed:
    driver: bridge
    ipam:
      config:
        - subnet: 172.18.0.0/16
  internet_blocked:
    driver: bridge
    ipam:
      config:
        - subnet: 172.19.0.0/16
```
2. For those containers who doesnt need access to internet add this:
```bash
services:
  container_name
    ...
    networks:
      - internet_blocked
```
3. Go to Synology task scheduler and create a *root task* triggered after boot:
> ⚠️ Warning: Somehow redirect to DOCKER-USER chain got disabled with a docker upgrade on Synology (bug? feature?). So I added it manually with ```sudo iptables -I FORWARD 1 -j DOCKER-USER```

```bash
# May be needed due to Synology bug/feature
#iptables -I FORWARD 1 -j DOCKER-USER
iptables -F DOCKER-USER
iptables -A DOCKER-USER -s 172.19.0.0/16 -d 192.168.0.0/16 -j RETURN
iptables -A DOCKER-USER -s 172.19.0.0/16 -d 172.16.0.0/12 -j RETURN
iptables -A DOCKER-USER -s 172.19.0.0/16 -d 10.0.0.0/8 -j RETURN
iptables -A DOCKER-USER -s 172.19.0.0/16 -j DROP
```

View routes

```bash
sudo iptables -vL -n --line-numbers
```
You should see smthng like this:
```
# traffic first hits FORWARD chain, then is redirected to DEFAULT_FORWARD.. looks like it's Synology behaviour
Chain FORWARD (policy ACCEPT 2949 packets, 2455K bytes)
num   pkts bytes target     prot opt in     out     source               destination
1     2949 2455K DEFAULT_FORWARD  all  --  *      *       0.0.0.0/0            0.0.0.0/0
...
# chain Default_forward sends the traffic further to DOCKER-USER
Chain DEFAULT_FORWARD (1 references)
num   pkts bytes target     prot opt in     out     source               destination
1     2949 2455K DOCKER-USER  all  --  *      *       0.0.0.0/0            0.0.0.0/0
...

Chain DOCKER-USER (1 references)
num   pkts bytes target     prot opt in     out     source               destination
1        0     0 RETURN     all  --  *      *       172.19.0.0/16        192.168.0.0/16
2     5925  901K RETURN     all  --  *      *       172.19.0.0/16        172.16.0.0/12
3        3   252 RETURN     all  --  *      *       172.19.0.0/16        10.0.0.0/8
4        9   756 DROP       all  --  *      *       172.19.0.0/16        0.0.0.0/0
# after the last rule traffic returns to FORWARD chain back
...
```
Note: Synology's iptables doesnt support REJECT -> so I used DROP instead. REJECT is almost instant, while DROP cancels connection attempt after few seconds delay. 
