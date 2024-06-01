# OpenVPN setup on Asus routers
https://www.youtube.com/watch?v=cHaD-Bofj9A

```bash
easyrsa --batch init-pki
easyrsa --batch --silent --silent-ssl --days=3650 --req-cn="OpenVPN Server" --digest=sha512 build-ca nopass
easyrsa --batch --silent --silent-ssl --req-cn="OpenVPN Server Key" gen-req Server nopass
easyrsa --batch --silent --silent-ssl --days=3650 sign-req server Server
easyrsa --batch --silent --silent-ssl --req-cn="user1" gen-req User1 nopass
easyrsa --batch --silent --silent-ssl --days=3650 sign-req client User1
easyrsa --batch --silent --silent-ssl --req-cn="user2" gen-req User2 nopass
easyrsa --batch --silent --silent-ssl --days=3650 sign-req client User2
easyrsa --batch --silent --silent-ssl --req-cn="user3" gen-req User3 nopass
easyrsa --batch --silent --silent-ssl --days=3650 sign-req client User3
easyrsa gen-dh --silent --silent-ssl
```

# YazFi + Yeelight + Home Assisstant
So yeeligh smart lights only work using the LAN Mode if both sorce and destination IPs belong to the same subnet.
I pushed all lights to YazFi guest wifi network.

NAT is needed to keep the LAN control option.
* login to your ASUS router
* ```nano /jffs/addons/YazFi.d/userscripts.d/nat_to_guest.sh```
```bash
#!/bin/sh
#iptables -I YazFiFORWARD -i wl0.1 -o br0 -d 10.0.1.5 -j ACCEPT
iptables -t nat -A POSTROUTING -o wl0.1 -s 10.0.1.5 -j MASQUERADE
```
where ```10.0.1.5``` is Home Assistant IP

# Asus Router Home assistant addon
Install "ASUS Router" custom integration via HACS to your Home Assistant. 
You can add a parental contol switch to your HA now. 
Block internet access to entertaiment devices via "Parental Controls" tab on the router. Switch on parental controls through the Home Assistant when you need to disable internet for those devices.

# Wifi Jail
put your smart devices who need access to WiFi but not to Internet (e.g., soundbar) into the isolated part of the network.
Add this to ```/jffs/scripts/firewall-start```:
```
iptables -I FORWARD -m iprange --src-range 10.0.1.230-10.0.1.254 -j REJECT
```
Now go to the management portal and assign a static IP from that IP range (do not forget to exclude it from the DHCP scope!).

## Remove the rule
Remove the rule. Get the rule number.
```bash
iptables -L FORWARD --line-numbers
```
Remove the specific rule (e.g., first one)
```bash
iptables -D FORWARD 1
```

# DynDNS for ASUS with Gandi
[Merlin Wiki](https://github.com/RMerl/asuswrt-merlin/wiki/Custom-DDNS/a5d71302d2e976c38d72d23a8ccdc13c1c3b6449)

```bash
#!/bin/bash

APIKEY="<GANDI_API_KEY>" # Your 24-character API key
DOMAIN="example.com" # The domain to be updated
SUBDOMAINS="" # starnge way to create arrays in SH. Append new subdomain to existing array every time
SUBDOMAINS="${SUBDOMAINS} mail"
SUBDOMAINS="${SUBDOMAINS} start"    # The Sub-Domain to update, use {$@,*} to update base domain (*.example.com), or change to "home" for home.example     >

IP=$(ip route get 1 | awk '{print $NF;exit}')
# if external IP is not known to router. More here https://openwrt.org/docs/guide-user/services/ddns/client#detecting_public_ip
#IP="$(curl -fs4 https://myip.dnsomatic.com/)"

for i in ${SUBDOMAINS};
do
   curl -fs -o /dev/null -X PUT -H "Content-Type: application/json" \
        -H "X-Api-Key: ${APIKEY}" \
        -d "{\"rrset_ttl\": 10800, \"rrset_values\": [\"${IP}\"]}" \
        "https://dns.api.gandi.net/api/v5/domains/${DOMAIN}/records/${i}/A"
done

if [ $? -eq 0 ]; then
        /sbin/ddns_custom_updated 1
else
        /sbin/ddns_custom_updated 0
fi
```
DDNS with Gandi and wget instead of curl
```bash
#!/bin/sh

IP=$(ip route get 1 | awk '{print $NF;exit}')
DOMAIN="example.com"
SUBDOMAIN="vpn"

wget --method=PUT --header="Content-Type: application/json" --header="X-Api-Key: <YOUR_TOKEN_HERE>" --body-data='{"rrset_ttl": 10800, "rrset_values": ["'${IP}'"]}' https://dns.api.gandi.net/api/v5/domains/${DOMAIN}/records/${SUBDOMAIN}/A 

# for Burp proxy add this to the end
# -e use_proxy=yes -e http_proxy=127.0.0.1:8080 -e https_proxy=127.0.0.1:8080 --no-check-certificate
```

# clear DNS leases
```killall dnsmasq; rm /var/lib/misc/dnsmasq.leases; service restart_dnsmasq```

# Super Power!
Change router's country to unlock full power and all wifi channels. 
Be carefull. In this mode with auto wifi channel selection router may jump to the wifi channel, not available in your country. Clients will not be able to see or use it. Use manual channel assigment. 

How to pick a good wifi channel.
1. run Inssider (PC) or Wifiman (Android). 
2. find the least used channel
3. check [wiki page](https://en.wikipedia.org/wiki/List_of_WLAN_channels) to make sure that this channel has 160Mhz band

```bash
#!/bin/sh

# Use those commands to get current TX power.
# wl txpwr
# wl txpwr_target_max

# Allow all channels
nvram set location_code=#a
nvram set 0:ccode=#a
nvram set 0:regrev=0
nvram set 1:ccode=#a
nvram set 1:regrev=0
nvram set territory_code=CN/02
nvram set wl_country_code=#a
nvram set wl_reg_mode=h
nvram set wl0_country_code=#a
nvram set wl0_country_rev=0
nvram set wl0_reg_mode=h
nvram set wl1_chlist="36 40 44 48 52 56 60 64 100 104 108 112 116 120 124 128 132 136 140 149 153 157 161 165 169 173 177"
nvram set wl1_country_code=#a
nvram set wl1_country_rev=0
nvram set wl1_reg_mode=h
nvram set acs_unii4=1
nvram set wl_acs_excl_chans_dfs=
nvram set wl_acs_excl_chans=
nvram set wl0_acs_dfs=1
nvram set wl0_acs_excl_chans_base=
nvram set wl0_acs_excl_chans=
nvram set wl1_acs_dfs=1
nvram set wl1_acs_excl_chans_base=
nvram set wl1_acs_excl_chans=

# Channel selection
# nvram set wl0_chanspec=0 # auto select
# nvram set wl1_chanspec=0 # auto select
# nvram set wl_chanspec=0 # auto select
# nvram set wl0_sel_channel=7/80 # force 2.4GHz channel
# nvram set wl1_sel_channel=149/80 # force 5GHz channel

# Commit changes
nvram commit
```
