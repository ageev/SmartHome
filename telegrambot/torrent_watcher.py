#!/usr/bin/env python3
# """script monitors qBittorent torrents and sends Telegram messages"""

# qbittorrent webAPI https://github.com/qbittorrent/qBittorrent/wiki/WebUI-API-Documentation

import requests, json, os, time

# Locate the directory containing this file and the repository root.
# Temporarily add these directories to the system path so that we can import
# local files.
import sys
from pathlib import Path
here = Path(__file__).parent.absolute()
repository_root = (here / ".." ).resolve() 
sys.path.insert(0, str(repository_root))

# use your own secrets!
from secrets import qBittorrent
from mytelegrambot import sendtext

# in seconds, should match scheduler time (e.g. if script runs every minute ==> AGE = 60). i've added +1 second "just in case".
# you can also save torrent state to disk, but time-based approach was chosen in favor to reduce disk IO operations
AGE = 61

def main():
    # Step 1: get torrents from qBT and saved torrents
    torrents = get_torrents(qBittorrent['ip'], qBittorrent['port'])
#    print(json.dumps(torrents, indent=4, sort_keys=True))

    # check for fresh torrents
    current_time = int(time.time()) #get current epoch time in seconds
    for _ in torrents:
        if (_['completion_on'] != 4294967295) and ((current_time - _['completion_on']) <= AGE):
            size = human_readable_size(_['size'])
            message = f"*Завершен* торрент [{_['name']}](http://{qBittorrent['ip']}:{qBittorrent['port']}) Размер: *{size}*."
            sendtext(message)
        elif (current_time - _['added_on']) <= AGE:
            size = human_readable_size(_['size'])
            message = f"*Добавлен* торрент [{_['name']}](http://{qBittorrent['ip']}:{qBittorrent['port']}) Размер: *{size}*."
            sendtext(message)

# get list of torrents from qbittorent
def get_torrents(ip, port):
    r = requests.get(f'http://{ip}:{port}/api/v2/torrents/info')
    return json.loads(r.text)

def human_readable_size(size, decimal_places=1):
    for unit in ['Б','КБ','МБ','ГБ','ТБ']:
        if size < 1024.0:
            break
        size /= 1024.0
    return f"{size:.{decimal_places}f} {unit}"
    
if __name__ == "__main__":
    main()   
