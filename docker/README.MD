## Blocking internet access FROM docker containers
Containers should not go to Internet unless absolutely must
1. Create two separate Docker networks in your ```docker-compose.yml``` file
```bash
networks:
  internet_allowed:
    driver: bridge
    ipam:
      config:
        - subnet: 172.18.0.0/16
  internet_blocked:
    driver: bridge
    ipam:
      config:
        - subnet: 172.19.0.0/16
```
2. For those containers who doesnt need access to internet add this:
```bash
services:
  container_name
    ...
    networks:
      - internet_blocked
```
3. Go to Synology task scheduler and create a root task triggered after boot:
```bash
iptables -F DOCKER-USER
iptables -A DOCKER-USER -s 172.19.0.0/16 -d 192.168.0.0/16 -j RETURN
iptables -A DOCKER-USER -s 172.19.0.0/16 -d 172.16.0.0/12 -j RETURN
iptables -A DOCKER-USER -s 172.19.0.0/16 -d 10.0.0.0/8 -j RETURN
iptables -A DOCKER-USER -s 172.19.0.0/16 -j DROP
iptables -A DOCKER-USER -j RETURN
```
Note: Synology's iptables doesnt support REJECT -> so I used DROP instead. REJECT is almost instant, while DROP cancels connection attempt after few seconds delay. 
