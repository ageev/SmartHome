# синхронизация с IMDB раз в 5 минут

Вот такой скрипт, добавленный в планировщик Synology, позволяет запускать синхронизацию с IMDB

```bash
curl -i -s -k -X $'POST' \
    -H $'Host: nas.local:7878' -H $'User-Agent: Mozilla/5.0 (Macintosh; Intel Mac OS X 10.15; rv:97.0) Gecko/20100101 Firefox/97.0' -H $'Accept: application/json, text/javascript, */*; q=0.01' -H $'Accept-Language: en-US,en;q=0.5' -H $'Accept-Encoding: gzip, deflate' -H $'Content-Type: application/json' -H $'X-Api-Key: <YOUR_SECRET_TOKEN_HERE!!!!' -H $'X-Requested-With: XMLHttpRequest' -H $'Content-Length: 25' -H $'Origin: http://ds.local:7878' -H $'DNT: 1' -H $'Connection: close' -H $'Referer: http://ds.local:7878/system/tasks' \
    --data-binary $'{\"name\":\"ImportListSync\"}' \
    $'http://nas.local:7878/api/v3/command'
```
