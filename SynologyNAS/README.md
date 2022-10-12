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
## Sonoff Zigbee 3.0 Dongle Plus
1. get your CPU architecture. Run ```uname -a``` and note the CPU codename. Example: ```Linux DS218 4.4.180+ #42218 SMP Mon Oct 18 19:17:56 CST 2021 x86_64 GNU/Linux synology_apollolake_218+``` -> apollolake
2. donwload ```cp210x.ko``` file from http://www.jadahl.com/iperf-arp-scan/DSM_7.0/ or https://github.com/robertklep/dsm7-usb-serial-drivers. Pick the right version for your CPU architecture
```
cd /lib/modules
sudo wget https://github.com/robertklep/dsm7-usb-serial-drivers/raw/main/modules/apollolake/cp210x.ko
```

3. Create a bootup task in Synology Task Scheduler. Choose to run as a root user. Chmod section is needed for zigbee2mqtt container to run with user level permissions
```
modprobe usbserial
modprobe ftdi_sio
modprobe cdc-acm
insmod /lib/modules/cp210x.ko
chmod 666 /dev/ttyUSB0
```

## Route to docker
```bash
ip link add macvlan0 link ovs_eth0 type macvlan mode bridge
ip addr add 10.0.1.8/29 dev macvlan0
ip link set macvlan0 up
```
## Sync Radarr every 5 min
```bash
curl -i -s -k -X $'POST' \
    -H $'Host: ds.local:7878' -H $'User-Agent: Mozilla/5.0 (Macintosh; Intel Mac OS X 10.15; rv:97.0) Gecko/20100101 Firefox/97.0' -H $'Accept: application/json, text/javascript, */*; q=0.01' -H $'Accept-Language: en-US,en;q=0.5' -H $'Accept-Encoding: gzip, deflate' -H $'Content-Type: application/json' -H $'X-Api-Key: <PUT_YOUR_API_KEY_HERE>' -H $'X-Requested-With: XMLHttpRequest' -H $'Content-Length: 25' -H $'Origin: http://ds.local:7878' -H $'DNT: 1' -H $'Connection: close' -H $'Referer: http://ds.local:7878/system/tasks' \
    --data-binary $'{\"name\":\"ImportListSync\"}' \
    $'http://ds.local:7878/api/v3/command'
```
