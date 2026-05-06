# AdGuard Home

**[English](#english)** | **[Русский](#russian)**

Installing the AdGuard Home docker container on a Synology NAS.

---

<a id="english"></a>

## English

### AdGuard Home vs AdGuard Cloud vs Pi-hole

I've switched fully to AdGuard. Here's why:

**AdGuard Home pros:**
- fast search
- nice GUI
- supports the fancy stuff: DoH, DoT, QUIC (certificate required)

**AdGuard Cloud pros:**
- cheaper than NextDNS, but still a paid subscription
- best option for mobile phones, because AdGuard app management on Android is complex

**Pi-hole pros:**
- open source and community-driven

### Step 1. docker-compose.yml

> I run the AdGuard container inside VirtualDSM. AdGuard also uses certificates provided by the [acme.sh](https://github.com/ageev/SmartHome/tree/master/docker/acme.sh#readme) container.

```yaml
---
version: "3.9"
networks:
  macvlan_network:
    driver: macvlan
    enable_ipv6: false
    driver_opts:
      parent: eth0
    ipam:
      config:
        - subnet: 10.0.1.0/24
          gateway: 10.0.1.1
          ip_range: 10.0.1.8/29
services:
  adguard:
    image: adguard/adguardhome
    container_name: adguard
    hostname: adguard
#    user: 1027:100
    domainname: local
    mac_address: 00:fa:c0:fa:c0:ab
    cap_add:
      - NET_ADMIN
    networks:
      macvlan_network:
        ipv4_address: 10.0.1.9   # this will be your DNS server IP
    dns:
      - 10.0.1.1   # router's IP
      - 1.1.1.1
    environment:
      - TZ=Europe/Zurich
    volumes:
      - /volume1/docker/adguard/work:/opt/adguardhome/work
      - /volume1/docker/adguard/conf:/opt/adguardhome/conf
      - /volume1/docker/acme/config/*.your-domain.com:/opt/adguardhome/cert
    restart: unless-stopped
```

### Step 2. Configuration

DNS blocklists I use:

- AdGuard DNS filter
- HaGeZi's PRO / Samsung tracker / Windows+Office / Xiaomi / Threat Intelligence
- OISD Blocking Big

Custom blocking rules — use these to **allow** traffic. Template: `@@||push.yandex.ru^$important`. Now pushes from Yandex won't be blocked.

### Step 3. Additional configuration

You'll need to figure most of this out yourself, but here's what I did:

- Adjusted the router's DHCP scope to hand out AdGuard's IP as the primary DNS.
- The NAS still uses a static DNS — I don't want things to break if the AdGuard container restarts.
- The router redirects all DNS requests to AdGuard, except requests from the NAS, AdGuard itself, and mobile phones with AdGuard Cloud DNS already set. ASUS calls this *DNS Director*.

  > Yandex speakers are known to fall back to their own DNS servers when they detect that requests to Yandex's ad domains are being blocked. The DNS-redirect feature shuts that escape hatch.

- I added all family devices to AdGuard (Settings → Client Settings). For the kids' tablets I disabled a bunch of unneeded services (Amazon, eBay, ok.ru, …).
- I added some internal network devices to custom blocking rules (Filters → Custom filtering rules). This is needed for internal DNS resolution to work, e.g. for a smart bulb: `10.0.1.155 bulb.local`.
- The [acme.sh](https://github.com/ageev/SmartHome/tree/master/docker/acme.sh) container manages HTTPS certificates. I mount the cert folder from acme.sh into AdGuard (see compose above) and use that cert to protect the AdGuard UI as well.
- Added an A record on my domain pointing to the local AdGuard IP, e.g. `dns 10800 IN A 10.0.1.9`. Now `dns.your-domain.com` works locally.

#### Auto-updating docker

> Do **not** auto-update critical network-infrastructure containers.

### Step 4. Adding a route to docker's macvlan network adapter

Docker's macvlan adapters are available to LAN clients but not to DSM/Docker itself. Usually that's fine, but if you want — for example — to reach AdGuard from a Home Assistant container running on the same docker host (as I do), you need this.

Create a bootup task for the root user ("user-defined script") in Synology Task Scheduler with the following content:

```bash
ip link add macvlan0 link ovs_eth0 type macvlan mode bridge
ip addr add 10.0.1.8/29 dev macvlan0
ip link set macvlan0 up
```

| Token | Meaning |
|---|---|
| `10.0.1.8/29` | IP range dedicated to docker's macvlan. Make sure this range is excluded from the DHCP scope. |
| `macvlan0` | New virtual network interface name. |
| `ovs_eth0` | Synology DSM network interface name. |

---

<a id="russian"></a>

## Русский

### AdGuard Home vs AdGuard Cloud vs Pi-hole

Я полностью переехал на AdGuard. Почему:

**Плюсы AdGuard Home:**
- быстрый поиск
- удобный интерфейс
- поддерживает всё модное: DoH, DoT, QUIC (нужен сертификат)

**Плюсы AdGuard Cloud:**
- дешевле NextDNS, но всё равно платная подписка
- лучший вариант для мобилок, потому что управлять приложением AdGuard на Android не самое тривиальное дело

**Плюсы Pi-hole:**
- open-source и community-driven

### Шаг 1. docker-compose.yml

> Контейнер AdGuard у меня крутится в VirtualDSM. Сертификаты подтягиваются из контейнера [acme.sh](https://github.com/ageev/SmartHome/tree/master/docker/acme.sh#readme).

```yaml
---
version: "3.9"
networks:
  macvlan_network:
    driver: macvlan
    enable_ipv6: false
    driver_opts:
      parent: eth0
    ipam:
      config:
        - subnet: 10.0.1.0/24
          gateway: 10.0.1.1
          ip_range: 10.0.1.8/29
services:
  adguard:
    image: adguard/adguardhome
    container_name: adguard
    hostname: adguard
#    user: 1027:100
    domainname: local
    mac_address: 00:fa:c0:fa:c0:ab
    cap_add:
      - NET_ADMIN
    networks:
      macvlan_network:
        ipv4_address: 10.0.1.9   # это будет IP вашего DNS-сервера
    dns:
      - 10.0.1.1   # IP роутера
      - 1.1.1.1
    environment:
      - TZ=Europe/Zurich
    volumes:
      - /volume1/docker/adguard/work:/opt/adguardhome/work
      - /volume1/docker/adguard/conf:/opt/adguardhome/conf
      - /volume1/docker/acme/config/*.your-domain.com:/opt/adguardhome/cert
    restart: unless-stopped
```

### Шаг 2. Настройка

DNS-блоклисты, которые я использую:

- AdGuard DNS filter
- HaGeZi's PRO / Samsung tracker / Windows+Office / Xiaomi / Threat Intelligence
- OISD Blocking Big

Кастомные правила блокировки — ими, наоборот, можно **разрешать** трафик. Шаблон: `@@||push.yandex.ru^$important`. Теперь пуши Яндекса не блокируются.

### Шаг 3. Дополнительная настройка

Многое придётся допиливать самостоятельно, но вот что сделал я:

- В DHCP роутера выставил AdGuard как первичный DNS.
- На самом НАСе статический DNS — чтобы ничего не отвалилось при перезапуске контейнера AdGuard.
- Роутер перенаправляет все DNS-запросы на AdGuard, кроме запросов от НАСа, самого AdGuard и мобильных телефонов, у которых уже стоит AdGuard Cloud DNS. У ASUS это называется *DNS Director*.

  > Колонки от Яндекса умеют переключаться на свои собственные DNS-серверы, если видят, что запросы к рекламным доменам Яндекса блокируются. Принудительный редирект DNS закрывает эту лазейку.

- Все семейные устройства добавил в AdGuard (Settings → Client Settings). Для детских планшетов отключил ненужные сервисы (Amazon, eBay, ok.ru и т.д.).
- Часть внутренних устройств вписал в кастомные правила (Filters → Custom filtering rules) — это нужно для внутреннего DNS-резолвинга. Например, для умной лампочки: `10.0.1.155 bulb.local`.
- Контейнер [acme.sh](https://github.com/ageev/SmartHome/tree/master/docker/acme.sh) занимается HTTPS-сертификатами. Папку с сертификатами от acme.sh я монтирую в AdGuard (см. compose выше) и тем же сертификатом защищаю интерфейс AdGuard.
- Добавил A-запись на свой домен, указывающую на локальный IP AdGuard, например `dns 10800 IN A 10.0.1.9`. Теперь по `dns.your-domain.com` можно достучаться до AdGuard локально.

#### Автообновление docker

> **Не** автообновляйте контейнеры критичной сетевой инфраструктуры.

### Шаг 4. Добавить маршрут до macvlan-сети docker'а

Macvlan-адаптеры docker'а видны клиентам в LAN, но не видны самому DSM/Docker. Обычно это не проблема, но если, например, нужно достучаться до AdGuard из контейнера Home Assistant, который крутится на том же docker'е (как у меня), без этого не обойтись.

Создаём в планировщике Synology bootup-задачу для root («user-defined script»):

```bash
ip link add macvlan0 link ovs_eth0 type macvlan mode bridge
ip addr add 10.0.1.8/29 dev macvlan0
ip link set macvlan0 up
```

| Что | Зачем |
|---|---|
| `10.0.1.8/29` | Диапазон IP, выделенный под docker macvlan. Не забудьте исключить его из DHCP-пула! |
| `macvlan0` | Имя нового виртуального сетевого интерфейса. |
| `ovs_eth0` | Имя сетевого интерфейса Synology DSM. |
