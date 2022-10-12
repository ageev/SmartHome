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

2. add a task to the Scheduler. Adjust the URL to match the CPU architecture. Run at boot-up as a root
```bash
if [ ! -f /lib/modules/cp210x.ko ]; then
    cd /lib/modules
    wget https://github.com/robertklep/dsm7-usb-serial-drivers/raw/main/modules/apollolake/cp210x.ko
fi

modprobe usbserial
modprobe ftdi_sio
modprobe cdc-acm
insmod /lib/modules/cp210x.ko
chmod 666 /dev/ttyUSB0
```
Some comments here. After DSM upgrade the ```cp210x.io``` file will disappear. The boot-up script will catch this and redownload it.  


## Route to docker
```bash
ip link add macvlan0 link ovs_eth0 type macvlan mode bridge
ip addr add 10.0.1.8/29 dev macvlan0
ip link set macvlan0 up
```
## Sync Radarr every 5 min
Radarr can be nicely integrated with IMDB list. So every time you add anything to your IMDB watchlist - Radarr will catch it and lanch the download. 
BUT because Radarr uses it's own servers for that they've set up a throttling mechanism - you can check all lists minimum every 7 hours...
To bypass this check you can trigger a "list update" task manually in Radarr GUI. I use Burp Suite to catch such requests and export them to curl & set a synology task scheduler to run those every 5 min. 
```bash
curl -i -s -k -X $'POST' \
    -H $'Host: ds.local:7878' -H $'Content-Type: application/json' -H $'X-Api-Key: <PUT_YOUR_API_KEY_HERE>' -H $'Referer: http://ds.local:7878/system/tasks' \
    --data-binary $'{\"name\":\"ImportListSync\"}' \
    $'http://<NAS_IP>:7878/api/v3/command'
```
