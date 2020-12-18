# SmartHouse
## Radarr https://habr.com/ru/post/505814/
Радарр 3 не дает обновлять листы IMDB чаще одного раза в 6 часов.
Чтобы обойти это ограничение можно добавить вот это в планировщик
```bash
curl -X POST "http://localhost:7878/api/v3/command?apikey=dadada" -H "accept: application/json" -d '{"name":"ImportListSync"}'
```

## torrent watcher
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

## secrets.py
очень удобно хранить все пароли/токены в одном файле. Тогда можно добавить его в .gitignore и ваши секреты не утекут в сеть когда запустишь git push.
Структура файла проста. Это обычный питон словарь:
```python
# Description
SoftName = {
  "key" : "value",
  }
```

Чтобы им пользоваться нужно добавить в шапку скрипта:
```python
import sys
from pathlib import Path
here = Path(__file__).parent.absolute()
repository_root = (here / ".." ).resolve()   # перейти на папку выше. Когда файл secrets.py лежит в корне
sys.path.insert(0, str(repository_root))
```

потом импортировать нужный словарик
```python
from secrets import SoftName
```
и в нужном месте просто вызвать..
```python
key = SoftName['key']
```
Всё. Теперь скрипт чист. Все пароли в одном файле.
