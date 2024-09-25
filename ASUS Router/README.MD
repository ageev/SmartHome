# ASUS router hacks
ASUS builds nice routers. Best hardware and worst software.. If you really want a good router - check gl.inet. 

# List of hacks
- improve power by changing country
- setup site-to-site VPN
- setup IP-network-based routing

## Prerequisite
- ASUS router with Merlin firmware

# Free NVRAM space
Some old ASUS routers with a new firmware may have a "no free space" NVRAM problem. Best way to clean NVRAM is to factory reset the device. If that's not possible - use this. 
Script below will remove empty NVRAM parameters (e.g., "var="). Will clear some memory, looks harmless, but MAY ACTUALLY BREAK THINGS!
So backup NVRAM first with ```nvram dump > backup.txt``` 
```bash
for line in `nvram show | grep ^[^=]*=$ `; do var=${line%*=}; nvram unset $var; done; nvram commit
```
# Super Power!
Change router's country to unlock full power and all wifi channels. 
Be carefull. In this mode with auto wifi channel selection router may jump to the wifi channel, not available in your country. Clients will not be able to see or use it. Use manual channel assigment if needed.

How to pick a good wifi channel.
1. run Inssider (PC) or Wifiman (Android). 
2. find the least used channel
3. check [wiki page](https://en.wikipedia.org/wiki/List_of_WLAN_channels) to make sure that this channel has 160Mhz band
4. Run this once.

```bash
# IMPORTANT! BACKUP NVRAM FIRST!
# nvram dump > dump.txt

# Use those commands to get the current TX power.
# wl txpwr
# wl txpwr_target_max

# Per interface (depends on your router. This one is for GT-AXE16000)
# wl -i eth7 txpwr_adj_est
# wl -i eth8 txpwr_adj_est
# wl -i eth9 txpwr_adj_est
# wl -i eth10 txpwr_adj_est

# I have 4 bands (AXE16000) -> I need to set country 4 times. You may have 2 or 3 bands -> wl3, wl2 may not needed. Check existing NVRAM dump and adjust accordingly
nvram set 0:ccode=#a
nvram set 0:regrev=0
nvram set 1:ccode=#a
nvram set 1:regrev=0
nvram set 2:ccode=#a
nvram set 2:regrev=0
nvram set 3:ccode=#a
nvram set 3:regrev=0
nvram set 4:ccode=#a
nvram set 4:regrev=0
nvram set location_code=#a
nvram set wl_country_code=#a
nvram set wl_country_rev=0
nvram set wl0_country_abbrev_override=US
nvram set wl0_country_code=#a
nvram set wl0_country_rev=0
nvram set wl1_country_abbrev_override=US
nvram set wl1_country_code=#a
nvram set wl1_country_rev=0
nvram set wl2_country_abbrev_override=US
nvram set wl2_country_code=#a
nvram set wl2_country_rev=0
nvram set wl3_country_abbrev_override=US
nvram set wl3_country_code=#a
nvram set wl3_country_rev=0
nvram commit
```

# Get client specific connection details
Run this to get the list of clients per interface ```wl -i eth7 assoclist```. Pick one and run this ```wl -i eth7 sta_info MAC ADDRESS OF CLIENT```

# Site-to-site VPN between 2 Asus routers.
On "Master router" start OpenVPN server. "Master router" must have VPN port available from Internet. "Slave router" may have local-only IP.
Same VPN connection will be used for both site-to-site and regular VPN. 

Master router local IP: A.A.A.0/24 (pls change to your own range)

Slave router local IP: B.B.B.0/24

Master router VPN IP: VPN.VPN.VPN.0/24

1. Generate proper certs for different users - https://github.com/RMerl/asuswrt-merlin.ng/wiki/Generating-OpenVPN-keys-using-Easy-RSA

See the video here:

https://www.youtube.com/watch?v=cHaD-Bofj9A

.. and create few certs for multiple users. User1 will be used solely for Slave router connection. It will have a dedicated routing rule attached to it so all hosts from Master router's LAN can access all systems in Slave router's LAN and vise versa. User2 and User3 will be just normal users.

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
2. In OpenVPN server general settings set "Both" to access both LAN and Internet.
3. In OpenVPN advanced settings:

Manage Client-Specific Options - Yes

Allow Client <-> Client - No

Allow only specified clients - No

Add new entry into "Allowed clients" list:

User1 B.B.B.0 255.255.255.0 Push=Yes

4. Go to Slave router. Add new OpenVPN client and upload the .ovpn file generated for User1.

- Create NAT on tunnel - Yes
- Inbound Firewall - Allow
- Redirect Internet traffic through tunnel - "VPN Director"

5. Slave router, go to "VPN Director" tab. 

Add new rule:

Some_name, Local IP = B.B.B.0/24, Remote IP = A.A.A.0/24, iface=OVPN1

For mass adding VPN director rule see "Mass adding VPN Director rules" section below

6. done! Check Satus page on Master router. 

# Mass adding VPN Director rules
If you need to redirect traffic to YouTube, for example, via your VPN, do the following.

Step 1. Get the list of YouTube IPs from here https://rockblack.su/vpn/dopolnitelno/diapazon-ip-adresov

Step 2. Create a script and run it. 
```bash
#!/bin/sh

#Add existing rules here. Use your LAN IP instead of 192.168.7.0/24!
rulelist='<1>OFFICE_LAN>192.168.7.0/24>10.0.1.0/24>OVPN1'
n=1

# add ip ranges here. Max 199 rules!
list="188.114.0.0/16, 172.64.0.0/16, 104.18.0.0/16, 64.233.0.0/16, 8.8.4.0/24, 8.8.8.0/24, 8.34.0.0/16, 8.35.0.0/16, 23.236.0.0/16, 23.251.0.0/16, 34.0.0.0/10, 35.184.0.0/13, 35.192.0.0/14, 35.196.0.0/15"

# Set IFS to handle both commas and spaces
IFS=', '

# Loop through the list and construct the rulelist. Replace 192.168.7.0/24 with your LAN IP!
for line in $list; do
    rulelist="${rulelist}<1>YouTube$((n++))>192.168.7.0/24>$line>OVPN1"
done

# Output the result
echo "$rulelist"
echo "Number of elements: $n"

# apply the list
echo "$rulelist" > /jffs/openvpn/vpndirector_rulelist
```

# YazFi + Yeelight + Home Assisstant
!UPDATE I re-programmed all my Yeelights to use ESPHOME (soldering required!)

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
iptables -I FORWARD -m iprange --src-range 10.0.1.231-10.0.1.254 -j REJECT
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
```bash
route del -net 192.168.7.0 netmask 255.255.255.0 dev tun21
```

# DynDNS for ASUS with Gandi and Cloudflare
[Merlin Wiki](https://github.com/RMerl/asuswrt-merlin/wiki/Custom-DDNS/a5d71302d2e976c38d72d23a8ccdc13c1c3b6449)

```bash
#!/bin/bash

APIKEY_GANDI="123123123123"
APIKEY_CLOUDFLARE="12312312312312312"

#IP=$(ip route get 1 | awk '{print $NF;exit}')
# if external IP is not known to router. More here https://openwrt.org/docs/guide-user/services/ddns/client#detecting_public_ip
IP="$(curl -fs4 http://checkip.amazonaws.com)"

# CloudFlare
cloudflare_dns_update() {
   for domain in $domains
      do
         ZONE_ID="aaaaaa" # get it from your Cloudflare dashboard
         RECORD_ID="bbbbbbb" # curl ... "https://api.cloudflare.com/client/v4/zones/${ZONE_ID}/dns_records" | json_pp
         status=$(curl -s -X PUT -H "Content-Type: application/json" \
           -H "Authorization: Bearer ${APIKEY_CLOUDFLARE}" \
           --data "{\"type\":\"A\",\"name\":\"${domain}\",\"content\":\"${IP}\",\"ttl\":1,\"proxied\":false}" \
           "https://api.cloudflare.com/client/v4/zones/${ZONE_ID}/dns_records/${RECORD_ID}")
         logger -s -t ddns "Domain $domain reports $status"
      done
}

domains="1.com 3.com"
cloudflare_dns_update

# Gandi
gandi_dns_update() {
   for sub in $subdomains
      do
         status=$(curl -s -X PUT -H "Content-Type: application/json" \
           -H "X-Api-Key: ${APIKEY_GANDI}" \
           -d "{\"rrset_ttl\": 10800, \"rrset_values\": [\"${IP}\"]}" \
           "https://dns.api.gandi.net/api/v5/domains/${domain}/records/${sub}/A")
         logger -s -t ddns "Domain $sub.$domain reports $status"
      done
}

domain="2.com"
subdomains="www mail post web"
gandi_dns_update


if [ $? -eq 0 ]; then
        /sbin/ddns_custom_updated 1
else
        /sbin/ddns_custom_updated 0
fi

# Update NEXT DNS and AdGuard DNS
curl -fs -o /dev/null "https://link-ip.nextdns.io/aaa/bbbb"
curl -fs -o /dev/null "https://linkip.adguard-dns.com/linkip/asdasd/asdasd"
```
DDNS with Gandi and wget instead of curl (may be usefull for old ASUS routers)
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
