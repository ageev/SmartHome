#!/usr/bin/env python3
# """Python telegram bot. POST API only"""

# Usefull links
# https://stackoverflow.com/questions/32423837/telegram-bot-how-to-get-a-group-chat-id#32572159
# allow bot to join groups - @botfather send /setjoingroups then Enable 
# https://sourceforge.net/p/telegram/wiki/markdown_syntax/

import requests, json

# Locate the directory containing this file and the repository root.
# Temporarily add these directories to the system path so that we can import
# local files.
import sys
from pathlib import Path
here = Path(__file__).parent.absolute()
repository_root = (here / ".." ).resolve() 
sys.path.insert(0, str(repository_root))

# supress SSL errors
#from requests.packages.urllib3.exceptions import InsecureRequestWarning

# use your own secrets!
from secrets import Telegram

def main():
    pass

# send text to telegram bot
def sendtext(bot_message, token=Telegram["token"], chat_id=Telegram["chat_id"]):
    
    url = f"https://api.telegram.org/bot{token}/sendMessage?chat_id={chat_id}&parse_mode=Markdown&text={bot_message}"
    #response = requests.get(url, verify=False)
    response = requests.get(url)

    return response.json()

if __name__ == "__main__":
    main()  
