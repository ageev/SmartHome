# snowflake-proxy

**[English](#english)** | **[Русский](#russian)**

---

<a id="english"></a>

## English

Part of the Tor project. The safest way to help the project — exit nodes are the riskiest, snowflake proxies are the easiest and least exposed contribution.

### docker-compose.yml

```yaml
snowflake-proxy:
  network_mode: host
  image: thetorproject/snowflake-proxy:latest
  container_name: snowflake-proxy
  restart: unless-stopped
  # For a full list of Snowflake Proxy CLI parameters see
  # https://gitlab.torproject.org/tpo/anti-censorship/pluggable-transports/snowflake/-/tree/main/proxy?ref_type=heads#running-a-standalone-snowflake-proxy
  command: [ "-capacity", "5", "-metrics", "-ephemeral-ports-range", "40000:50000" ]
```

### Router setup

To make the node reachable, also forward ports `40000-50000` on the router to the NAS IP.

---

<a id="russian"></a>

## Русский

Часть проекта Tor. Самый безопасный способ помочь проекту — exit node, наоборот, самый опасный.

### docker-compose.yml

```yaml
snowflake-proxy:
  network_mode: host
  image: thetorproject/snowflake-proxy:latest
  container_name: snowflake-proxy
  restart: unless-stopped
  # Полный список параметров CLI:
  # https://gitlab.torproject.org/tpo/anti-censorship/pluggable-transports/snowflake/-/tree/main/proxy?ref_type=heads#running-a-standalone-snowflake-proxy
  command: [ "-capacity", "5", "-metrics", "-ephemeral-ports-range", "40000:50000" ]
```

### Настройка роутера

Чтобы достучаться до ноды, нужно так же открыть порт-форвардинг портов `40000-50000` на роутере на IP НАСа.
