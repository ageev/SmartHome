# SmartHouse
## Radarr https://habr.com/ru/post/505814/
Радарр 3 не дает обновлять листы IMDB чаще одного раза в 6 часов.
Чтобы обойти это ограничение можно добавить вот это в планировщик
```bash
curl -X POST "http://localhost:7878/api/v3/command?apikey=dadada" -H "accept: application/json" -d '{"name":"ImportListSync"}'
```

# USB ZigBee donlge on DSM 7 (Synology)

(as a root) create /etc/modules-load.d/user.conf (the directory modules-load.d does not exist, create it; the file can be named whatever you like, but it must end with .conf). Put just these two lines there:
```
usbserial
cdc-acm
```

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
