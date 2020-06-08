"""
SANITIZED VERSION. FILL WITH YOUR OWN DATA!!!

File used to set environment variables including tokens and passwords.

"""
ENVIRONMENT = "Production"
#ENVIRONMENT = "Test"

if ENVIRONMENT == "Production":
    # General
    NAS_IP = "192.168.1.1"
    ADDRESS_BOOK = {
        '192.168.1.101' : '', 
        }

    # Telegram
    Telegram = {
        "token" : ":",
        "chat_id" : "",
    }

    # Meteoserver
    Meteoserver = {
        "url" : "",
        "token" : "",
        "town" : "",
    }
    
    # qBittorrent
    qBittorrent = {
        "ip" : NAS_IP,
        "port" : "",
    }

    # Synology surveillance station
    SurvStation = {
        "login" : "",
        "password" : "",
        "ip" : NAS_IP,
        "port" : "5001",

    }

    # Gandi
    Gandi = {
        "token" : "",
    }

def main():
    pass

if __name__ == "__main__":
    main()  
