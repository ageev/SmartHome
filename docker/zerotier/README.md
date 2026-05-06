# zerotier

**[English](#english)** | **[Русский](#russian)**

---

<a id="english"></a>

## English

ZeroTier is a Tailscale alternative (Tailscale, by the way, blocks Russian users) that places devices on different networks into a single virtual network.

ZeroTier could be called a "real VPN" because encryption here is secondary. Using ZeroTier for general internet traffic is pointless. For backup traffic between sites, however, it is excellent.

### docker-compose.yml

```yaml
---
version: "3.9"
services:
  zerotier:
    # Join a network:
    #   sudo docker exec -it zerotier zerotier-cli join <net_id>
    # Connection issues: sometimes the container starts (and exits) too early.
    # Create a Synology task to run "docker-compose up -d" a few minutes after
    # boot to bring the container back up.
    container_name: zerotier
    image: zerotier/zerotier-synology:latest
    cap_add:
      - NET_ADMIN
      - SYS_ADMIN
    devices:
      - /dev/net/tun:/dev/net/tun
    volumes:
      - /volume2/docker/zerotier:/var/lib/zerotier-one
    network_mode: "host"
    restart: unless-stopped
```

---

<a id="russian"></a>

## Русский

ZeroTier — аналог Tailscale (последний, кстати, блокирует русских пользователей), который позволяет поместить разные устройства в разных сетях в одну виртуальную сеть.

ZeroTier можно назвать «настоящим VPN», потому что шифрование тут вторично. Использовать ZeroTier для интернет-трафика — это порнография. А вот для бэкапа — самое то.

### docker-compose.yml

```yaml
---
version: "3.9"
services:
  zerotier:
    # Присоединиться к сети:
    #   sudo docker exec -it zerotier zerotier-cli join <net_id>
    # Проблемы со стартом: контейнер иногда поднимается слишком рано
    # и тут же отваливается. Решение — задача в планировщике Synology,
    # которая выполняет "docker-compose up -d" через пару минут после загрузки.
    container_name: zerotier
    image: zerotier/zerotier-synology:latest
    cap_add:
      - NET_ADMIN
      - SYS_ADMIN
    devices:
      - /dev/net/tun:/dev/net/tun
    volumes:
      - /volume2/docker/zerotier:/var/lib/zerotier-one
    network_mode: "host"
    restart: unless-stopped
```
