# "Живи с умом!" (c) Смешарики
Основа моего умного дома - NAS Synology DS218+, в который я вставил 2 планки старой ноутбучной памяти, тем самым, нарастив её до 16 Гб. В НАСе стоят два Enterpise жестких диска (WD RED, Seagate IronWolf Pro) в режиме зеркала (RAID 0).

![Стартовая страница Homer](Pictures/start_page.jpg)
На НАСе крутятся:
- Софт Synology
  - Synology Photos - отличный каталогизатор семейных фотографий. Я немного причесываю семейный архив [скриптами](https://github.com/ageev/others) время от времени
  - HyperBackup - универсальный бэкапер. Работает практически со всеми облаками и имеет очень гибкие политики бэкапа
  - CloudSync - синхронизирует локальную папку на НАСе с каким-нибудь облаком
  - Virtual Machine Manager - тут у меня крутится VitrualDSM (виртуальный НАС)
  - Surveillance Station - отличный менеджер камер наблюдения
- docker - тут всё самое основное
  - [adguard](https://github.com/ageev/SmartHouse/tree/master/adguard) - продвинутая резалка рекламы. Лучше чем [pihole](https://github.com/ageev/SmartHouse/tree/master/pi-hole)
  - [caddy](https://github.com/ageev/SmartHouse/tree/master/docker/caddy) - вебсервер с поддержкой ACME (автоматическая выдача HTTPS сертификатов) - *заменил на Nginx Proxy Manager + acme.sh*
  - [esphome](https://github.com/ageev/SmartHouse/tree/master/docker/esphome) - генератор прошивок и дэшборд для ESP32 (дешевый чип с поддержкой Wi-Fi и BLE). Позволяет перепрошивать всякие Tuya Smart и прочии девайсы для интеграции с Home Assistant
  - home-assistant (HA, homeass, хомяк)- универсальный интеграционный сервис умного дома. За счёт огромного количества аддонов позволяет связывать в единую систему очень разные компоненты. Например, Яндекс Станцию и Телеграм, датчики IKEA и вентилятор Xiaomi. Примеры автоматизаций внутри
  - [homer](https://github.com/ageev/SmartHome/tree/master/docker/homer) - простой вебсервер, на котором удобно сделать стартовую страницу умного дома. Скриншот из Гомера вы видите выше. Когда мне нужно что-то открыть в домашней сети - я использую стартовую страницу Гомера
  - [transmission](https://github.com/ageev/SmartHome/tree/master/docker/transmission) - качалка торрентов. Выбрал её потому что у нее самая удобная интеграция с HA. Использую с плагином для браузера Torrent Control - позволяет скачивать всё сразу на НАС
  - [acme.sh](https://github.com/ageev/SmartHouse/tree/master/acme.sh) - позволяет автоматизировать получение HTTPS сертификатов, посылает уведомоления в телегу, может пушить обновления сразу на НАС
  - [vaultwarden](https://github.com/ageev/SmartHouse/tree/master/vaultwarden) - форк BitWarden - очень хороший менеджер паролей с клиентами под все платформы
  - [nginx proxy manager](https://github.com/ageev/SmartHouse/tree/master/Nginx%20Proxy%20Manager) - единое окно (ну почти) для всех внутренних веб-порталов. Позволяет сделать "зеленый замочек" (HTTP -> HTTPS) там, где его отродясь не было. Один минус NGM - нужно искать уникальный кастомный прокси конфиг под каждое приложение. С Traefik такой проблемы нет. Надо сравнить
  - mariadb - база данных. Нужна для Nginx Proxy Manager
  - [mosquitto](https://github.com/ageev/SmartHome/tree/master/docker/zigbee2mqtt) - MQTT брокер. Нужен для zigbee2mqtt. Работает с очередью запросов к ZigBee устройствам для умного дома
  - [zigbee2mqtt](https://github.com/ageev/SmartHome/tree/master/docker/zigbee2mqtt) - позволяет интегрировать Zigbee устройства умного дома с home-assistant. Работает с USB стиком Sonoff Zigbee 3.0, воткнутым в USB порт НАСа. В целом, можно было использовать и встроенный в Home Assistant zigbee модуль
  - [plex](https://github.com/ageev/SmartHome/tree/master/docker/plex) - каталогизатор видео. Позволяет на лету транскодировать видеопотоки (если железо позволяет. DS218+ позволяет) под различные разрешения и устройства. Ну то есть можно стримить мультики на детские планшеты, и они сами могут выбирать, что смотреть. Пользуюсь редко. В основном, фильмы смотрим через NFS шары и Kodi
  - prowlarr - интегратор торрент треккеров. Даешь ему команду найти торрент - он ищет сразу по всем твоим любимым треккерам. Используется в связке с radarr
  - radarr - менеджер домашней видеобиблиотеки. Работает как-то так:
    - Добавляешь фильм в Watchlist на IMDB.com
    - лист синхронизируется с Radarr
    - Radarr даёт команду Prowlarr найти все раздачи с этим фильмом
    - Radarr фильтрует все раздачи в соответствии с задаными фильтрами (например, качество не хуже HD, английская или русская дорожка. Не менее 3х сидеров)
    - Если после фильтрации осталась нужная раздача, Radarr дает команду Transmission скачать нужный торрент
    - Кладёт раздачу в папочку Movies на НАСе, посылает уведомление через телегу
    - Если вдруг вышла раздача лучшего качества - скачивает её пока максимальное качество не достигнуто
  - torrserver - сервер для NUM. В путешествиях я вожу с собой Amazon Fire TV стик. Воткнул в телевизор в отеле и можно смотреть свой Netflix. NUM в связке с домашним torrserver позволяет смотреть еще и торренты в странах, где за это наказывают

## Radarr https://habr.com/ru/post/505814/
Радарр 3 не дает обновлять листы IMDB чаще одного раза в 6 часов.
Чтобы обойти это ограничение можно добавить вот это в планировщик (убедитесь, что прописали верный хост и токен!)
```bash
curl -i -s -k -X $'POST' \
    -H $'Host: nas.local:7878' -H $'User-Agent: Mozilla/5.0 (Macintosh; Intel Mac OS X 10.15; rv:97.0) Gecko/20100101 Firefox/97.0' -H $'Accept: application/json, text/javascript, */*; q=0.01' -H $'Accept-Language: en-US,en;q=0.5' -H $'Accept-Encoding: gzip, deflate' -H $'Content-Type: application/json' -H $'X-Api-Key: <YOUR_SECRET_TOKEN_HERE!!!!' -H $'X-Requested-With: XMLHttpRequest' -H $'Content-Length: 25' -H $'Origin: http://ds.local:7878' -H $'DNT: 1' -H $'Connection: close' -H $'Referer: http://ds.local:7878/system/tasks' \
    --data-binary $'{\"name\":\"ImportListSync\"}' \
    $'http://nas.local:7878/api/v3/command'
```
(UPDATE) я больше не использую Jackett, заменил на Prowlarr

## torrent watcher
UPDATE! Я теперь мониторю торренты через интеграцию Transmission в Home Assistant. Можно создать правило автоматизации, которое будет посылать нужные вам уведомления в телеграм чат

Используем телеграм бота для мониторинга очереди qBittorent
1. в файл secrets.py нужно внести свои данные:
- qBittorent IP и порт
- Telegram token и chat_id 
Создать бота и достать токен можно через @botfather в Телеграм. После того, как бот создан, нужно отправить "/setjoingroups" -> Enable чтобы бот мог присоединятся к группам.  
Chat_id можно узнать так https://stackoverflow.com/questions/32423837/telegram-bot-how-to-get-a-group-chat-id#32572159

2. скопировать папку telegrambot и secrets.py куда-нибудь себе
3. настроить планировщик чтобы каждую минуту запускал скрипт. Например, так:
(sudo crontab -e)
```
* * * * * /usr/bin/python3 /home/artem/scripts/telegram/torrent_watcher.py >> /var/log/crontab_errors.log 2>&1
```
4. готово!
