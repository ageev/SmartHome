# Synology NAS

**[English](#english)** | **[Русский](#russian)**

---

<a id="english"></a>

## English

### Tuning

- RAM upgraded to 32 GB (for VMs and Docker). I picked compatible modules (per Reddit posts) so Synology wouldn't keep nagging me about non-original memory.
- 10 Gbps network card installed (which raised the NAS temperature a bit).
- Seagate Exos drives with extended 5-year warranty (enterprise line — often on sale, unlike IronWolf).
- Two Seagate SSDs which, with [007revad's scripts](https://github.com/007revad/Synology_HDD_db), are turned into the main storage pool. The `syno_app_mover` script from the same author moves all active apps onto the SSDs.
- Two low-profile copper SSD heatsinks. There's almost no room around the SSDs, so only thin heatsinks fit.
- Velcro mod — I lined the HDD cage with Velcro.
- Replaced the fan with a Noctua.
- Mounted on extended rubber feet.
- Built an extra cooling system from an AliExpress fan controller and two fans to boost airflow.

All of that makes the NAS practically silent when there's no HDD activity. The HDDs are only used for backups and the photo archive — everything else lives on SSDs (with HDD backups of those too).

### Sending Synology NAS notifications to a Telegram group via webhooks (DSM 7)

#### Prerequisites

1. Telegram bot token
2. Group chat ID
3. DSM 7

#### Web hook push notifications in DSM 7

These are broken out of the box. I created one manually and then edited the file directly:

```bash
sudo vi /usr/syno/etc/synowebhook.conf
```

Here's the config (you'll probably need to flatten it to a oneliner):

```json
{"Telegram Bot":
  {"needssl":true,
  "port":443,
  "prefix":"_A new system event occurred on your %HOSTNAME% on_ *%DATE%* _at_ *%TIME%*.",
  "req_method":"get",
  "req_header":"{}",
  "req_param":"{}",
  "sepchar":" ",
  "template":"https://api.telegram.org/bot<PUT YOUR TELEGRAM TOKEN HERE>/sendMessage?chat_id=<YOUR_CHAT_ID>&parse_mode=Markdown&text=@@PREFIX@@%0A@@TEXT@@",
  "type":"custom",
  "url":""
  }
}
```

Save it, test from the GUI.

`@@FULLTEXT@@ == @@PREFIX@@%0A@@TEXT@@`

To fully enjoy this, install Synology Log Center. In the "Notification" menu set up a filter on words like "signed in" (web) and "logged in" (SSH). You'll get Telegram messages whenever someone logs into the NAS.

### Covertly opening Synology Photos for external users

You can expose Synology Photos to the internet in a covert manner.
First, go to **Settings > Login Portal > Applications** and create an alias for Synology Photos.

Then go to your [NPM container](https://github.com/ageev/SmartHome/tree/master/docker/nginx-proxy-manager) and add a host with this Custom Configuration:

```nginx
location / {
    if ($remote_addr !~ "^(10\.|172\.(1[6-9]|2[0-9]|3[0-1])\.|192\.168\.|fc00:|fe80:|fd[0-9a-f]{2}:)") {
        return 302 https://google.com; # redirect all external users to Google
    }
    proxy_set_header X-Forwarded-For $proxy_add_x_forwarded_for;
    proxy_set_header X-Forwarded-Proto $scheme;
    proxy_set_header Host $http_host;
    proxy_set_header X-Real-IP $remote_addr;
    proxy_set_header Range $http_range;
    proxy_set_header If-Range $http_if_range;
    proxy_redirect off;
    proxy_pass https://10.0.1.5/;  # your NAS IP
}

location /secret_alias {
    proxy_set_header X-Forwarded-For $proxy_add_x_forwarded_for;
    proxy_set_header X-Forwarded-Proto $scheme;
    proxy_set_header Host $http_host;
    proxy_set_header X-Real-IP $remote_addr;
    proxy_set_header Range $http_range;
    proxy_set_header If-Range $http_if_range;
    proxy_redirect off;
    proxy_pass https://10.0.1.5/secret_alias; # your secret app alias URL
}
```

Don't forget to add external users with limited access (Photos app only) and MFA enabled!

### Limiting internet access

Create two user groups on your NAS:

1. **local** — add all local users here. Group settings > Applications: limit access to Synology Photos by IP to your local network.
2. **external** — add all external Synology Photos users here. Group settings > Applications: deny everything except Synology Photos.
3. Synology Photos > Settings > Shared Space > Access Permissions: set limited permissions for external users.

### USB ZigBee dongle on DSM 7 (Synology)

> This was never stable for me, so I moved to a Virtual Machine — external USB works flawlessly there.

#### Sonoff Zigbee 3.0 Dongle Plus

1. Get your CPU architecture. Run `uname -a` and note the CPU codename. Example: `Linux DS218 4.4.180+ #42218 SMP Mon Oct 18 19:17:56 CST 2021 x86_64 GNU/Linux synology_apollolake_218+` -> `apollolake`.

2. Add a task to the Scheduler. Adjust the URL to match your CPU architecture. Run at boot, as root:

```bash
if [ ! -f /lib/modules/cp210x.ko ]; then
    cd /lib/modules
    wget https://github.com/robertklep/dsm7-usb-serial-drivers/raw/main/modules/apollolake/dsm-7.2/cp210x.ko
fi

modprobe usbserial
modprobe ftdi_sio
modprobe cdc-acm
insmod /lib/modules/cp210x.ko
chmod 666 /dev/ttyUSB0
```

After a DSM upgrade the `cp210x.ko` file disappears. The boot script catches that and re-downloads it.

> If your ZigBee starts running slow — just re-plug the Sonoff stick.

---

<a id="russian"></a>

## Русский

### Тюнинг

- Память расширена до 32 ГБ (ради виртуальных машин и докера). Искал совместимую (из постов с Реддита), дабы Synology не показывал каждый раз предупреждения о неоригинальной памяти.
- Установлена сетевая карта на 10 Гб/с (за счёт чего немного повышена температура НАСа).
- Стоят диски Seagate Exos с докупленной пятилетней гарантией (корпоративная линейка, часто на распродажах в отличие от IronWolf).
- Стоят два SSD Seagate, которые с помощью [скриптов 007revad](https://github.com/007revad/Synology_HDD_db) превращены в основной раздел. С помощью скрипта `syno_app_mover` от него же все активные приложения перемещены на SSD.
- Установил два низкопрофильных медных SSD-радиатора. Места вокруг SSD очень мало, не развернёшься. Радиаторы влазят только тонкие.
- Сделал себе Velcro mod — обклеил липучками карман для ЖД.
- Заменил кулер на Noctua.
- Поставил на удлинённые резиновые ножки.
- Собрал из контроллера с АлиЭкспресс и двух кулеров систему охлаждения, которая увеличивает обдув.

Всё это позволило сделать НАС практически бесшумным если нет обращения к жестким дискам. При этом диски у меня используются только для бэкапа и фотохранилища. Все остальные файлы — на SSD (и их же бэкап на жестких дисках).

### Уведомления Synology NAS в группу Telegram через webhooks в DSM 7

#### Пререквизиты

1. Telegram bot token
2. Group ChatID
3. DSM 7

#### Web hook push notifications в DSM 7

Они сломаны. Я создал один вручную и потом отредактировал файл напрямую:

```bash
sudo vi /usr/syno/etc/synowebhook.conf
```

Конфиг (его, скорее всего, надо превратить в однострочник) — см. английскую секцию выше.

Сохраните, тестируйте через GUI.

`@@FULLTEXT@@ == @@PREFIX@@%0A@@TEXT@@`

Чтобы полностью насладиться плодами трудов, поставьте Synology Log Center. В меню Notification настройте фильтр по словам «signed in» (web) и «logged in» (SSH). Это будет генерить телеграм-сообщения когда кто-то логинится на НАС.

### Скрытое открытие Synology Photos для внешних пользователей

Synology Photos можно открыть наружу скрытно.
Сначала: **Settings > Login Portal > Applications** — создайте Alias для Synology Photos.

Затем в [контейнере NPM](https://github.com/ageev/SmartHome/tree/master/docker/nginx-proxy-manager) добавьте host с такой Custom Configuration:

```nginx
location / {
    if ($remote_addr !~ "^(10\.|172\.(1[6-9]|2[0-9]|3[0-1])\.|192\.168\.|fc00:|fe80:|fd[0-9a-f]{2}:)") {
        return 302 https://google.com;
    }
    proxy_set_header X-Forwarded-For $proxy_add_x_forwarded_for;
    proxy_set_header X-Forwarded-Proto $scheme;
    proxy_set_header Host $http_host;
    proxy_set_header X-Real-IP $remote_addr;
    proxy_set_header Range $http_range;
    proxy_set_header If-Range $http_if_range;
    proxy_redirect off;
    proxy_pass https://10.0.1.5/;
}

location /secret_alias {
    proxy_set_header X-Forwarded-For $proxy_add_x_forwarded_for;
    proxy_set_header X-Forwarded-Proto $scheme;
    proxy_set_header Host $http_host;
    proxy_set_header X-Real-IP $remote_addr;
    proxy_set_header Range $http_range;
    proxy_set_header If-Range $http_if_range;
    proxy_redirect off;
    proxy_pass https://10.0.1.5/secret_alias;
}
```

Не забудьте добавить внешних пользователей с ограниченным доступом (только Photos) и включенной MFA!

### Ограничение интернет-доступа

Создайте две группы пользователей на НАСе:

1. **local** — все локальные пользователи. Group settings > Applications: ограничить доступ к Synology Photos по IP до локальной сети.
2. **external** — все внешние пользователи Synology Photos. Group settings > Applications: запретить всё, кроме Synology Photos.
3. Synology Photos > Settings > Shared Space > Access Permissions — настройте ограниченные права для внешних пользователей.

### USB ZigBee донгл на DSM 7 (Synology)

> Стабильным это никогда не было, так что я переехал на виртуальную машину — там USB работает идеально.

#### Sonoff Zigbee 3.0 Dongle Plus

1. Узнайте архитектуру CPU. Запустите `uname -a` и запишите codename. Пример: `Linux DS218 ... synology_apollolake_218+` -> `apollolake`.

2. Добавьте таск в Scheduler. URL подправьте под свою архитектуру. Запуск при загрузке, под root:

```bash
if [ ! -f /lib/modules/cp210x.ko ]; then
    cd /lib/modules
    wget https://github.com/robertklep/dsm7-usb-serial-drivers/raw/main/modules/apollolake/dsm-7.2/cp210x.ko
fi

modprobe usbserial
modprobe ftdi_sio
modprobe cdc-acm
insmod /lib/modules/cp210x.ko
chmod 666 /dev/ttyUSB0
```

После апгрейда DSM файл `cp210x.ko` исчезает. Скрипт это ловит и перекачивает заново.

> Если ZigBee начал тормозить — просто пере-воткните стик Sonoff.
