Paperless офигенная штука! Дома у меня стоит такой красавец:

<img src="https://github.com/ageev/SmartHome/raw/master/Pictures/MFC-L8390CDW_L.jpg" alt="Brother MFC-L8390CDW" width="300">

Каждый раз когда в руки мне попадает важная бумажка, я иду к МФУ, засовываю её в сканер и кликают кнопку "paperless - 2 sides". Сканер сканирует в сетевую папку /volume1/scans/inbox на НАСе. Paperless подхватывает фото, распознает текс, используя ИИ, подбирает отправителя и тему и вешает тэги. Теперь в любой момент времени я могу зайти на портал Paperless и, используя поиск по ключевым словам или тэгам, найти то, что мне надо. В paperless у меня скопились уже сотни документов за последние 3 года. Это одна из критически-важных домашних систем, поэтому я бэкаплю её периодически. 

# подготовка
1. купить сканнер (кстати можно использовать и телефон!) с поддержкой сканирования в сетевую папку.
2. создать папку на НАСе. Например, ```\volume1\scans\inbox```.
3. завести пользователя для принтера и убрать все права кроме доступа к созданной папке.
4. настроить шаблон сканирования в сетевую папку с нужным пользователем на принтере. 

# docker-compose.yml
```yaml
---
version: "3.9"
services:
  paperless:
    image: ghcr.io/paperless-ngx/paperless-ngx:latest
    container_name: paperless
    hostname: paperless
    environment:
      - USERMAP_UID=1028 # use yours!
      - USERMAP_GID=100 # use yours!
      - PAPERLESS_TIME_ZONE=Europe/Zurich
      - PAPERLESS_REDIS=redis://paperless-redis:6379
      - PAPERLESS_OCR_LANGUAGE=eng+deu+rus # Runtime OCR languages
      - PAPERLESS_OCR_LANGUAGES=rus # download additional languages
      - PAPERLESS_URL=https://scans.your-domain.com
    depends_on:
      - paperless-redis
    volumes:
      - /volume2/docker/paperless/data:/usr/src/paperless/data
      - /volume2/docker/paperless/media:/usr/src/paperless/media
      - /volume2/scans/export:/usr/src/paperless/export
      - /volume2/scans/inbox:/usr/src/paperless/consume
    restart: unless-stopped

  paperless-redis:
    image: docker.io/library/redis:7
    container_name: paperless-redis
    hostname: paperless-redis
    environment:
      - PUID=1028 # use yours!
      - PGID=100 # use yours!
      - TZ=Europe/Zurich
    volumes:
      - /volume2/docker/paperless-redis:/data
    expose:
      - 6379:6379
    restart: unless-stopped
```
# Конфигурация

Не сидите под админом! Заведите себе пользователя и привычку периодически заходить на портал и проверять правильность классификации документов. 

Общий подход такой:
1. Всем новым документам назначается тэг "inbox", ну или "review".
2. Раз в недельку другую вы заходите, проверяете правильность автоматического категорирования документов. Убираете тэг "inbox".
3. Когда добавляете новых корреспондентов или тэги, выбираете опцию "Auto: Learn matching automatically". После нескольких ручных операций, система начинает ставить правильные тэги и корреспондентов автоматически. Современем ручное редактирование сводится к минимуму.

# Nginx Proxy Manager Configuration
Я работаю с Paperless через URL вида ```scans.your-domain.com```. Для чего в мой NPM контейнер нужно добавить такую конфигурацию:
```nginx
location / {
    proxy_pass http://10.0.1.5:8000; # NAS IP
    proxy_http_version 1.1;
    proxy_set_header Upgrade $http_upgrade;
    proxy_set_header Connection "upgrade";
    proxy_redirect off;
    proxy_set_header Host $host;
    proxy_set_header X-Real-IP $remote_addr;
    proxy_set_header X-Forwarded-For $proxy_add_x_forwarded_for;
    proxy_set_header X-Forwarded-Host $server;
    add_header Referrer-Policy "strict-origin-when-cross-origin";
}
```
