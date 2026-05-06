# ASUS Router Hacks (Merlin firmware)

**[English](#english)** | **[Русский](#russian)**

---

<a id="english"></a>

## English

ASUS builds nice routers — best hardware, worst software. It takes a lot of nerve cells to set everything up the way you want it, but usually after that you just never really touch it again, and it all works.

### Contents

- [DPI bypass](#dpi-bypass)
- [WireGuard client restart on server's IP change](#wireguard-client-restart-on-servers-ip-change)
- [Get interface speed](#get-interface-speed)
- [Free NVRAM space](#free-nvram-space)
- [Super Power](#super-power)
- [Get client-specific connection details](#get-client-specific-connection-details)
- [Site-to-site VPN between two ASUS routers](#site-to-site-vpn-between-two-asus-routers)
- [Mass-adding VPN Director rules](#mass-adding-vpn-director-rules)
- [YazFi + Yeelight + Home Assistant](#yazfi--yeelight--home-assistant)
- [ASUS Router Home Assistant addon](#asus-router-home-assistant-addon)
- [WiFi Jail](#wifi-jail)
- [DynDNS for ASUS with Gandi and Cloudflare](#dyndns-for-asus-with-gandi-and-cloudflare)
- [Clear DNS leases](#clear-dns-leases)

### DPI bypass

Copy the `wgclient-start` file to `/jffs/scripts` and `chmod +x` it.
Every time the first WireGuard client connection (e.g. `wgc1` — *adjust the script if you have different or multiple WG clients!*) is established, chunks of random data are sent to fool DPI. This helps mask the WireGuard signature, but the approach won't work if DPI is set to block "unknown" traffic.

### WireGuard client restart on server's IP change

The WG client resolves the server IP once during start. If the server has a dynamic IP, you need to restart the client to pick up the new IP every time the server's IP changes.
Copy `wgc_restart_ip_change` to `/jffs/scripts/` and `chmod +x` it.
Add this to `/jffs/scripts/services-start`:

```bash
#!/bin/sh
cru a wgipupdate "*/5 * * * * /jffs/scripts/wgc_restart_ip_change"
```

### Get interface speed

```bash
ethctl eth0 media-type
```

### Free NVRAM space

Some old ASUS routers running new firmware may hit a "no free space" NVRAM problem. The best way to clean NVRAM is a factory reset. If that's not possible — use this.
The script below removes empty NVRAM parameters (e.g. `var=`). It will free some memory and looks harmless, but *MAY ACTUALLY BREAK THINGS!*

> Back up NVRAM first with `nvram dump > backup.txt`.

```bash
for line in `nvram show | grep ^[^=]*=$ `; do var=${line%*=}; nvram unset $var; done; nvram commit
```

### Super Power

Change the router's country to unlock full power and all WiFi channels.

> Be careful. In this mode with auto WiFi channel selection the router may jump to a channel that's not available in your country. Clients won't be able to see or use it. Use manual channel assignment if needed.

How to pick a good WiFi channel:

1. Run Inssider (PC) or Wifiman (Android).
2. Find the least-used channel.
3. Check the [Wikipedia page](https://en.wikipedia.org/wiki/List_of_WLAN_channels) to make sure that channel supports the 160 MHz band.

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

# I have 4 bands (AXE16000) -> I need to set country 4 times. You may have 2 or 3 bands -> wl3, wl2 may not be needed. Check existing NVRAM dump and adjust accordingly
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

### Get client-specific connection details

Run `wl -i eth7 assoclist` to get the list of clients per interface. Pick one and run `wl -i eth7 sta_info <CLIENT_MAC_ADDRESS>`.

### Site-to-site VPN between two ASUS routers

On the "Master router" start an OpenVPN server. The Master router must have its VPN port reachable from the internet. The "Slave router" can have a local-only IP.
The same VPN connection will be used for both site-to-site and regular VPN access.

| Role | Local subnet |
|---|---|
| Master router LAN | A.A.A.0/24 (replace with yours) |
| Slave router LAN | B.B.B.0/24 |
| Master router VPN subnet | VPN.VPN.VPN.0/24 |

1. Generate proper certs for different users — see the [Merlin wiki on generating OpenVPN keys](https://github.com/RMerl/asuswrt-merlin.ng/wiki/Generating-OpenVPN-keys-using-Easy-RSA), or this [video walkthrough](https://www.youtube.com/watch?v=cHaD-Bofj9A).

   Create a few certs for multiple users. User1 will be used solely for the Slave router connection — it will have a dedicated routing rule attached so all hosts on the Master router's LAN can reach all systems on the Slave router's LAN and vice versa. User2 and User3 will be regular users.

2. In OpenVPN server general settings, set "Both" to allow access to both LAN and internet.
3. In OpenVPN advanced settings:
   - **Manage Client-Specific Options**: Yes
   - **Allow Client <-> Client**: No
   - **Allow only specified clients**: No
   - Add a new entry to the "Allowed clients" list:

     ```
     User1   B.B.B.0   255.255.255.0   Push=Yes
     ```

4. On the Slave router, add a new OpenVPN client and upload the `.ovpn` file generated for User1.
   - **Create NAT on tunnel**: Yes
   - **Inbound Firewall**: Allow
   - **Redirect Internet traffic through tunnel**: VPN Director

5. On the Slave router, go to the "VPN Director" tab and add a new rule:

   ```
   Some_name, Local IP = B.B.B.0/24, Remote IP = A.A.A.0/24, iface=OVPN1
   ```

   For mass-adding VPN Director rules, see the section below.

6. Done! Check the Status page on the Master router.

### Mass-adding VPN Director rules

If you need to redirect traffic to YouTube, for example, via your VPN, do this:

1. Get the list of YouTube IPs [here](https://rockblack.su/vpn/dopolnitelno/diapazon-ip-adresov).
2. Create a script and run it. *OVPN1* is the first OpenVPN client on your router. If you use WireGuard, it should be `WGC1`.

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

### YazFi + Yeelight + Home Assistant

> UPDATE: I re-flashed all my Yeelights with ESPHome (soldering required!).

Yeelight smart bulbs only work in LAN mode if both source and destination IPs are on the same subnet.
I moved all bulbs to the YazFi guest WiFi network.

NAT is needed to keep the LAN control option working.

- SSH to your ASUS router.
- `nano /jffs/addons/YazFi.d/userscripts.d/nat_to_guest.sh`

```bash
#!/bin/sh
#iptables -I YazFiFORWARD -i wl0.1 -o br0 -d 10.0.1.5 -j ACCEPT
iptables -t nat -A POSTROUTING -o wl0.1 -s 10.0.1.5 -j MASQUERADE
```

where `10.0.1.5` is the Home Assistant IP.

### ASUS Router Home Assistant addon

Install the "ASUS Router" custom integration via HACS into your Home Assistant.
You can now add a parental-control switch to HA.
Block internet access to entertainment devices via the "Parental Controls" tab on the router. Toggle parental controls through Home Assistant when you need to disable internet for those devices.

### WiFi Jail

Put smart devices that need WiFi but not internet (e.g. a soundbar) into an isolated part of the network.
Add this to `/jffs/scripts/firewall-start`:

```bash
iptables -I FORWARD -m iprange --src-range 10.0.1.231-10.0.1.254 -j REJECT
```

Now go to the management portal and assign a static IP from that range (don't forget to exclude it from the DHCP scope!).

#### Removing the rule

Get the rule number:

```bash
iptables -L FORWARD --line-numbers
```

Remove the specific rule (e.g. the first one):

```bash
iptables -D FORWARD 1
```

```bash
route del -net 192.168.7.0 netmask 255.255.255.0 dev tun21
```

### DynDNS for ASUS with Gandi and Cloudflare

[Merlin Wiki](https://github.com/RMerl/asuswrt-merlin/wiki/Custom-DDNS/a5d71302d2e976c38d72d23a8ccdc13c1c3b6449)

```bash
#!/bin/bash

APIKEY_GANDI="123123123123"
APIKEY_CLOUDFLARE="12312312312312312"

#IP=$(ip route get 1 | awk '{print $NF;exit}')
# if external IP is not known to router. More here https://openwrt.org/docs/guide-user/services/ddns/client#detecting_public_ip
IP="$(curl -fs4 http://checkip.amazonaws.com)"

# CloudFlare
ZONE_ID="aaaaaa" # get it from your Cloudflare dashboard
RECORD_ID="bbbbbbb" # curl ... "https://api.cloudflare.com/client/v4/zones/${ZONE_ID}/dns_records" | json_pp
DOMAIN="1.com"
status=$(curl -s -X PUT -H "Content-Type: application/json" \
   -H "Authorization: Bearer ${APIKEY_CLOUDFLARE}" \
   --data "{\"type\":\"A\",\"name\":\"${DOMAIN}\",\"content\":\"${IP}\",\"ttl\":1,\"proxied\":false}" \
   "https://api.cloudflare.com/client/v4/zones/${ZONE_ID}/dns_records/${RECORD_ID}")
logger -s -t ddns "Domain $domain reports $status"

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

DDNS with Gandi and `wget` instead of `curl` (may be useful for old ASUS routers):

```bash
#!/bin/sh

IP=$(ip route get 1 | awk '{print $NF;exit}')
DOMAIN="example.com"
SUBDOMAIN="vpn"

wget --method=PUT --header="Content-Type: application/json" --header="X-Api-Key: <YOUR_TOKEN_HERE>" --body-data='{"rrset_ttl": 10800, "rrset_values": ["'${IP}'"]}' https://dns.api.gandi.net/api/v5/domains/${DOMAIN}/records/${SUBDOMAIN}/A

# for Burp proxy add this to the end
# -e use_proxy=yes -e http_proxy=127.0.0.1:8080 -e https_proxy=127.0.0.1:8080 --no-check-certificate
```

### Clear DNS leases

```bash
killall dnsmasq; rm /var/lib/misc/dnsmasq.leases; service restart_dnsmasq
```

---

<a id="russian"></a>

## Русский

ASUS делает классные роутеры — лучшее железо и худший софт. Чтобы настроить всё как надо, уйдёт куча нервных клеток, но потом обычно ты больше к этому не возвращаешься — и всё работает.

### Содержание

- [Обход DPI](#обход-dpi)
- [Перезапуск WireGuard клиента при смене IP сервера](#перезапуск-wireguard-клиента-при-смене-ip-сервера)
- [Скорость интерфейса](#скорость-интерфейса)
- [Освобождение NVRAM](#освобождение-nvram)
- [Super Power](#super-power-1)
- [Детали соединения для конкретного клиента](#детали-соединения-для-конкретного-клиента)
- [Site-to-site VPN между двумя ASUS роутерами](#site-to-site-vpn-между-двумя-asus-роутерами)
- [Массовое добавление правил VPN Director](#массовое-добавление-правил-vpn-director)
- [YazFi + Yeelight + Home Assistant](#yazfi--yeelight--home-assistant-1)
- [ASUS Router в Home Assistant](#asus-router-в-home-assistant)
- [WiFi Jail](#wifi-jail-1)
- [DynDNS для ASUS с Gandi и Cloudflare](#dyndns-для-asus-с-gandi-и-cloudflare)
- [Очистка DNS лизов](#очистка-dns-лизов)

### Обход DPI

Скопируйте `wgclient-start` в `/jffs/scripts` и сделайте `chmod +x`.
Каждый раз при установлении первого WireGuard-соединения (например, `wgc1` — *поправьте скрипт, если у вас другие или несколько WG-клиентов!*) отправляются куски случайных данных, чтобы запутать DPI. Это помогает маскировать сигнатуру WireGuard, но не сработает, если DPI настроен блокировать «неизвестный» трафик.

### Перезапуск WireGuard клиента при смене IP сервера

WG-клиент резолвит IP сервера один раз при старте. Если у сервера динамический IP, клиент нужно перезапускать каждый раз когда меняется адрес сервера.
Скопируйте `wgc_restart_ip_change` в `/jffs/scripts/` и сделайте `chmod +x`.
В `/jffs/scripts/services-start` добавьте:

```bash
#!/bin/sh
cru a wgipupdate "*/5 * * * * /jffs/scripts/wgc_restart_ip_change"
```

### Скорость интерфейса

```bash
ethctl eth0 media-type
```

### Освобождение NVRAM

Некоторые старые ASUS роутеры с новой прошивкой могут получить ошибку «no free space» в NVRAM. Лучший способ — factory reset. Если это не вариант — используйте подход ниже.
Скрипт ниже удалит пустые параметры NVRAM (типа `var=`). Освободит немного памяти, выглядит безобидно, но *МОЖЕТ СЛОМАТЬ ВЕЩИ!*

> Сначала обязательно сделайте бэкап NVRAM: `nvram dump > backup.txt`.

```bash
for line in `nvram show | grep ^[^=]*=$ `; do var=${line%*=}; nvram unset $var; done; nvram commit
```

### Super Power

Поменяйте страну роутера, чтобы открыть полную мощность и все WiFi-каналы.

> Осторожно. В этом режиме при автовыборе WiFi-канала роутер может прыгнуть на канал, недоступный в вашей стране. Клиенты его просто не увидят. При необходимости назначайте канал вручную.

Как выбрать хороший WiFi-канал:

1. Запустите Inssider (PC) или Wifiman (Android).
2. Найдите наименее загруженный канал.
3. Проверьте по [странице Википедии](https://en.wikipedia.org/wiki/List_of_WLAN_channels), что этот канал поддерживает 160 МГц.

См. блок команд в английской секции выше.

### Детали соединения для конкретного клиента

Получить список клиентов на интерфейсе: `wl -i eth7 assoclist`. Выбрать одного и: `wl -i eth7 sta_info <MAC адрес клиента>`.

### Site-to-site VPN между двумя ASUS роутерами

На «Master router» запустите OpenVPN сервер. У Master router VPN порт должен быть доступен из интернета. У «Slave router» может быть только локальный IP.
Один и тот же VPN будет использоваться и для site-to-site, и для обычного VPN.

| Роль | Локальная подсеть |
|---|---|
| Master router LAN | A.A.A.0/24 (замените на свою) |
| Slave router LAN | B.B.B.0/24 |
| Master router VPN | VPN.VPN.VPN.0/24 |

Дальше — см. подробные шаги в английской секции. Логика та же: генерируете сертификаты для разных пользователей, для User1 настраиваете client-specific options с пушем подсети Slave router, на Slave router настраиваете VPN-клиент и правило в VPN Director.

### Массовое добавление правил VPN Director

Если нужно перенаправить, скажем, YouTube через VPN:

1. Возьмите список IP YouTube [здесь](https://rockblack.su/vpn/dopolnitelno/diapazon-ip-adresov).
2. Скрипт см. в английской секции. *OVPN1* — это первый OpenVPN-клиент на роутере. Для WireGuard будет `WGC1`.

### YazFi + Yeelight + Home Assistant

> ОБНОВЛЕНИЕ: Я перепрошил все Yeelight на ESPHome (с пайкой!).

Yeelight в LAN Mode работают только если src и dst IP в одной подсети.
Все лампы я загнал в гостевую сеть YazFi.

Чтобы LAN-контроль работал, нужен NAT.

- SSH на роутер.
- `nano /jffs/addons/YazFi.d/userscripts.d/nat_to_guest.sh`:

```bash
#!/bin/sh
iptables -t nat -A POSTROUTING -o wl0.1 -s 10.0.1.5 -j MASQUERADE
```

где `10.0.1.5` — IP Home Assistant.

### ASUS Router в Home Assistant

Через HACS поставьте в Home Assistant кастомную интеграцию «ASUS Router».
Теперь в HA можно добавить переключатель родительского контроля.
Заблокируйте интернет развлекательным устройствам через «Parental Controls» на роутере. Включайте/выключайте через Home Assistant, когда надо ограничить интернет этим устройствам.

### WiFi Jail

Загнать умные устройства, которым нужен WiFi, но не нужен интернет (например, саундбар), в изолированную часть сети.
В `/jffs/scripts/firewall-start`:

```bash
iptables -I FORWARD -m iprange --src-range 10.0.1.231-10.0.1.254 -j REJECT
```

Теперь зайдите в админку и назначьте этим устройствам статический IP из этого диапазона (не забудьте исключить его из DHCP scope!).

#### Удалить правило

Узнать номер правила:

```bash
iptables -L FORWARD --line-numbers
```

Удалить (например, первое):

```bash
iptables -D FORWARD 1
```

```bash
route del -net 192.168.7.0 netmask 255.255.255.0 dev tun21
```

### DynDNS для ASUS с Gandi и Cloudflare

См. [Merlin Wiki](https://github.com/RMerl/asuswrt-merlin/wiki/Custom-DDNS/a5d71302d2e976c38d72d23a8ccdc13c1c3b6449). Полные скрипты — в английской секции выше (содержимое идентично).

### Очистка DNS лизов

```bash
killall dnsmasq; rm /var/lib/misc/dnsmasq.leases; service restart_dnsmasq
```
