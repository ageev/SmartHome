#!/usr/bin/env python3

# qbittorrent webAPI https://github.com/qbittorrent/qBittorrent/wiki/WebUI-API-Documentation
# get group chat id https://stackoverflow.com/questions/32423837/telegram-bot-how-to-get-a-group-chat-id#32572159
# allow bot to join groups - @botfather send /setjoingroups then Enable 

import requests, json, os

BOT_TOKEN = ''
BOT_CHATID = ''

dir_path = os.path.dirname(os.path.realpath(__file__)) # get current script dir
FILENAME = os.path.join(dir_path,'torrents.lst')

def main():
    # Step 1: get torrents from qBT and saved torrents
    torrents = get_torrents()

    # dirty fix to fix constant updates from stalledUP to uploading and viseversa
    for _ in torrents:
        if _[1] == 'stalledUP' or _[1] == 'pausedUP' or _[1] == 'uploading':
            _[1] = 'completed'

    saved_torrents = get_saved_torrents()

    # Step 2: find new ones or completed ones
    changes = [x for x in torrents if x not in saved_torrents]
    if changes:
        for _ in changes:
#            sendtext("Torrent <" + _[0] + "> was added/edited. Current status: " + _[1] + ". Tag:" + _[2])
            sendtext("Торрент [" + _[0] + "](http://<qbittorentIP>). Статус: *" + _[1] + "*")
        save_torrents(torrents)

# get list of torrents from qbittorent
def get_torrents():
    torrents = []

    t = requests.get('http://<qBittorentIP>/query/torrents')
    
    for _ in t.json():
        torrents.append([_['name'], _['state'], _['category']])

    return torrents

# send text to telegram bot
def sendtext(bot_message):
    
    url = 'https://api.telegram.org/bot' + BOT_TOKEN + '/sendMessage?chat_id=' + BOT_CHATID + '&parse_mode=Markdown&text=' + bot_message
    response = requests.get(url)

    return response.json()

def get_saved_torrents():
    saved_torrents = []
    
    with open(FILENAME, 'r', encoding='utf8') as f:
        t = f.read().splitlines()

    for _ in t:
        lines = _.split(';')
        saved_torrents.append(lines)

    return saved_torrents

def save_torrents(torrents):
    torrents.sort()
    with open(FILENAME, 'w', encoding='utf8') as f:
        for _ in torrents:
            f.write(";".join(_) + '\n')
    
if __name__ == "__main__":
    main()   
