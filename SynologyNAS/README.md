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


# USB ZigBee donlge on DSM 7 (Synology)
read here https://www.zigbee2mqtt.io/guide/installation/02_docker.html#docker-on-synology-dsm-7-0 OR alternative setup:


(as a root) create /etc/modules-load.d/user.conf (the directory modules-load.d does not exist, create it; the file can be named whatever you like, but it must end with .conf). Put just these two lines there:
```
usbserial
cdc-acm
```
