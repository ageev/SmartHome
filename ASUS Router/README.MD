DynDNS for ASUS with Gandi

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
