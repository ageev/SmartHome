# Paperless-ngx

**[English](#english)** | **[Русский](#russian)**

---

<a id="english"></a>

## English

Paperless is awesome. At home I have this beauty:

<img src="https://github.com/ageev/SmartHome/raw/master/Pictures/MFC-L8390CDW_L.jpg" alt="Brother MFC-L8390CDW" width="300">

Every time an important piece of paper lands in my hands, I walk over to the MFP, drop it in the scanner and press the **"paperless – 2 sides"** button. The scanner scans into the network folder `/volume1/scans/inbox` on the NAS. Paperless picks up the file, OCRs it, lets the AI guess the correspondent and the subject, and tags the document. From then on I can log in to the Paperless portal and search by keywords or tags. Over the last three years it has swallowed hundreds of documents. This is one of the critical home systems, so I back it up regularly.

### Preparation

1. Buy a scanner (a phone works too!) that can scan to a network folder.
2. Create a folder on the NAS, e.g. `/volume1/scans/inbox`.
3. Create a dedicated user for the printer and revoke every permission except access to that folder.
4. On the printer, set up a "scan to network folder" template using that user.

### docker-compose.yml

```yaml
---
version: "3.9"
services:
  paperless:
    image: ghcr.io/paperless-ngx/paperless-ngx:latest
    container_name: paperless
    hostname: paperless
    environment:
      - USERMAP_UID=1028   # use yours!
      - USERMAP_GID=100    # use yours!
      - PAPERLESS_TIME_ZONE=Europe/Zurich
      - PAPERLESS_REDIS=redis://paperless-redis:6379
      - PAPERLESS_OCR_LANGUAGE=eng+deu+rus  # runtime OCR languages
      - PAPERLESS_OCR_LANGUAGES=rus         # download additional languages
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
      - PUID=1028   # use yours!
      - PGID=100    # use yours!
      - TZ=Europe/Zurich
    volumes:
      - /volume2/docker/paperless-redis:/data
    expose:
      - 6379:6379
    restart: unless-stopped
```

### Configuration

Don't run as admin. Create a regular user account and get into the habit of visiting the portal once in a while to check that documents were classified correctly.

The general workflow:

1. Every new document gets the `inbox` (or `review`) tag.
2. Once a week or two, you log in, verify the auto-classification, and remove the `inbox` tag.
3. When you add new correspondents or tags, choose **"Auto: Learn matching automatically"**. After a few manual passes, the system starts assigning the right tags and correspondents on its own. Over time, manual editing drops to near zero.

### Nginx Proxy Manager configuration

I access Paperless via `scans.your-domain.com`. Add this to the NPM container:

```nginx
location / {
    proxy_pass http://10.0.1.5:8000;  # NAS IP
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

---

<a id="russian"></a>

## Русский

Paperless — офигенная штука! Дома у меня стоит такой красавец:

<img src="https://github.com/ageev/SmartHome/raw/master/Pictures/MFC-L8390CDW_L.jpg" alt="Brother MFC-L8390CDW" width="300">

Каждый раз когда в руки мне попадает важная бумажка, я иду к МФУ, засовываю её в сканер и нажимаю кнопку **«paperless – 2 sides»**. Сканер сканирует в сетевую папку `/volume1/scans/inbox` на НАСе. Paperless подхватывает файл, распознаёт текст, с помощью ИИ подбирает отправителя и тему и вешает теги. Теперь в любой момент я могу зайти на портал Paperless и, используя поиск по ключевым словам или тегам, найти всё что нужно. За последние три года в Paperless у меня скопились сотни документов. Это одна из критически важных домашних систем, поэтому я её регулярно бэкаплю.

### Подготовка

1. Купите сканер (кстати, можно использовать и телефон!) с поддержкой сканирования в сетевую папку.
2. Создайте папку на НАСе, например `/volume1/scans/inbox`.
3. Заведите отдельного пользователя для принтера и уберите все права, кроме доступа к этой папке.
4. На принтере настройте шаблон «сканирование в сетевую папку» от этого пользователя.

### docker-compose.yml

```yaml
---
version: "3.9"
services:
  paperless:
    image: ghcr.io/paperless-ngx/paperless-ngx:latest
    container_name: paperless
    hostname: paperless
    environment:
      - USERMAP_UID=1028   # ваш!
      - USERMAP_GID=100    # ваш!
      - PAPERLESS_TIME_ZONE=Europe/Zurich
      - PAPERLESS_REDIS=redis://paperless-redis:6379
      - PAPERLESS_OCR_LANGUAGE=eng+deu+rus  # языки OCR во время работы
      - PAPERLESS_OCR_LANGUAGES=rus         # скачать дополнительные языки
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
      - PUID=1028   # ваш!
      - PGID=100    # ваш!
      - TZ=Europe/Zurich
    volumes:
      - /volume2/docker/paperless-redis:/data
    expose:
      - 6379:6379
    restart: unless-stopped
```

### Конфигурация

Не сидите под админом. Заведите себе обычного пользователя и привычку периодически заходить на портал и проверять правильность классификации документов.

Общий подход:

1. Всем новым документам назначается тег `inbox` (или `review`).
2. Раз в недельку-другую вы заходите, проверяете автокатегоризацию и снимаете тег `inbox`.
3. Когда добавляете новых корреспондентов или теги, выбирайте опцию **«Auto: Learn matching automatically»**. После нескольких ручных операций система начинает сама ставить правильные теги и корреспондентов. Со временем ручное редактирование сводится к минимуму.

### Конфигурация Nginx Proxy Manager

Я работаю с Paperless через URL вида `scans.your-domain.com`. В контейнер NPM нужно добавить такую конфигурацию:

```nginx
location / {
    proxy_pass http://10.0.1.5:8000;  # IP НАСа
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
