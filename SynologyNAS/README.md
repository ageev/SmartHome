# Send Synology NAS notifications to Telegram group
## Prerequisites
1. Telegram bot token
2. Group ChatID
3. DSM7

## Web hook push notifications in DSM7
Those are broken. I created one manually and then just edited the file directly 
```bash
sudo vi /usr/syno/etc/synowebhook.conf
```

Here is the config, you need to transform it to oneliner
```json
{"Telegram Bot":
  {"needssl":true,
  "port":443,
  "prefix":"A new system event occurred on your %HOSTNAME% on %DATE% at %TIME%.",
  "req_method":"get",
  "req_header":"{}",
  "req_param":"{}",
  "sepchar":" ",
  "template":"https://api.telegram.org/bot<PUT YOUR TELEGRAM TOKEN HERE>/sendMessage?chat_id=<YOUR_CHAT_ID>&parse_mode=Markdown&text=@@FULLTEXT@@",
  "type":"custom",
  "url":""
  }
}
```
