# snowflake-proxy
Часть проекта Tor. Самый безопасный способ помочь проекту (exit node - самый опасный).

# docker-compose.yml
```yml
  snowflake-proxy:
    network_mode: host
    image: thetorproject/snowflake-proxy:latest
    container_name: snowflake-proxy
    restart: unless-stopped
    # For a full list of Snowflake Proxy CLI parameters see
    # https://gitlab.torproject.org/tpo/anti-censorship/pluggable-transports/snowflake/-/tree/main/proxy?ref_type=heads#running-a-standalone-snowflake-proxy
    command: [ "-capacity", "5", "-metrics", "-ephemeral-ports-range", "40000:50000"]
```

# настройка роутера
чтобы достучаться до ноды нужно так же открыть порт-форвардинг портов 40000-50000 на роутере на IP НАСа. 
