# Тюнинг
- память расширена до 32Гб (ради виртуальных машин и докера). Искал совместимую (из постов с Реддита) дабы Synology не показывал каждый раз предупреждения о неоригинальной памяти.
- установлена сетевая карта на 10Гбс (за счёт чего немного повышена температура НАСа).
- стоят диски Seagate Exos с докупленной пятилетней гарантией (корпоративная линейка, часто на распродажах в отличие от IronWolf).
- стоят два SSD Seagate, которые с помощью [скриптов 007revad](https://github.com/007revad/Synology_HDD_db) превращены в основной раздел. С помощью скрипта syno_app_mover от него же все активные приложения перемещены на SSD.
- установил два низкопрофильных медных SSD радиатора. Места вокруг SSD очень мало, не развернешься. Радиаторы влазят только тонкие. 
- сделал себе Velcro mod - обклеил липучками карман для ЖД.
- заменил кулер на Noctua.
- поставил на удлиненные резиновые ножки.
- собрал из контроллера с АлиЭкспресс и двух куллеров систему охладжения, которая увеличивает обдув 

Всё это позволило сделать НАС практически бесшумным если нет обращения к жестким дискам. При этом диски у меня используются только для бэкапа и фотохранилища. Все остальные файлы на SSD (и их же бэкап на жетских дисках).

# Send Synology NAS notifications to Telegram group using webhooks in DSM7
## Prerequisites
1. Telegram bot token
2. Group ChatID
3. DSM7

## Web hook push notifications in DSM7
Those are broken. I created one manually and then just edited the file directly 
```bash
sudo vi /usr/syno/etc/synowebhook.conf
```

Here is the config, you probably need to transform it to oneliner
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
save it, test it via GUI. 

@@FULLTEXT@@ == @@PREFIX@@%0A@@TEXT@@


to fully enjoy the benefits you need to instal the Synology Log center. In "Notification" menu you can set the fillter using words "signed in" (for web) and "logged in" (for SSH). This will generate telegram messages when someone is logged in on your NAS

# Covertly opening Synology Photos for external users
It is possible to open Synology Photos for Internet access in a covert manner.
First go to Settings > Login Portal > Applications and create an Alias for Synology photos. 

Then go to your [NPM Container](https://github.com/ageev/SmartHome/tree/master/docker/nginx-proxy-manager) and add a host with this Custom Configuration:
```
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

Now dont forget to add external users with a limited access (only to Photos app) and MFA enabled! 

# limiting internet access
Create 2 user groups on your NAS:
1. "local". Add all local users there. Go to group's settings > Applications. Limit access to Synology photos by IP to your local network.
2. "external". Add all external users of Synology Photos there. Go to group's settings > applications. Deny all except Synology photos.
3. Go to Synology Photos > settings > Shared Space > Access Permissions. Set limited permissions for your external users. 

# USB ZigBee donlge on DSM 7 (Synology)
> This was never stable, so I moved to Virtual Machine instead, where external USB works flawlessly
## Sonoff Zigbee 3.0 Dongle Plus
1. get your CPU architecture. Run ```uname -a``` and note the CPU codename. Example: ```Linux DS218 4.4.180+ #42218 SMP Mon Oct 18 19:17:56 CST 2021 x86_64 GNU/Linux synology_apollolake_218+``` -> apollolake

2. add a task to the Scheduler. Adjust the URL to match the CPU architecture. Run at boot-up as a root
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
Some comments here. After DSM upgrade the ```cp210x.io``` file will disappear. The boot-up script will catch this and redownload it.  

If your ZigBee works slow, just re-plug the Sonoff stick!
