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
3. Go to Synology task scheduler and create a *root task* triggered after boot:
> ⚠️ Warning: Somehow redirect to DOCKER-USER chain got disabled with a docker upgrade on Synology (bug? feature?). So I added it manually with ```sudo iptables -I FORWARD 1 -j DOCKER-USER```

```bash
# May be needed due to Synology bug/feature
#iptables -I FORWARD 1 -j DOCKER-USER
iptables -F DOCKER-USER
iptables -A DOCKER-USER -s 172.19.0.0/16 -d 192.168.0.0/16 -j RETURN
iptables -A DOCKER-USER -s 172.19.0.0/16 -d 172.16.0.0/12 -j RETURN
iptables -A DOCKER-USER -s 172.19.0.0/16 -d 10.0.0.0/8 -j RETURN
iptables -A DOCKER-USER -s 172.19.0.0/16 -j DROP
```

View routes

```bash
sudo iptables -vL -n --line-numbers
```
You should see smthng like this:
```
# traffic first hits FORWARD chain, then is redirected to DEFAULT_FORWARD.. looks like it's Synology behaviour
Chain FORWARD (policy ACCEPT 2949 packets, 2455K bytes)
num   pkts bytes target     prot opt in     out     source               destination
1     2949 2455K DEFAULT_FORWARD  all  --  *      *       0.0.0.0/0            0.0.0.0/0
...
# chain Default_forward sends the traffic further to DOCKER-USER
Chain DEFAULT_FORWARD (1 references)
num   pkts bytes target     prot opt in     out     source               destination
1     2949 2455K DOCKER-USER  all  --  *      *       0.0.0.0/0            0.0.0.0/0
...

Chain DOCKER-USER (1 references)
num   pkts bytes target     prot opt in     out     source               destination
1        0     0 RETURN     all  --  *      *       172.19.0.0/16        192.168.0.0/16
2     5925  901K RETURN     all  --  *      *       172.19.0.0/16        172.16.0.0/12
3        3   252 RETURN     all  --  *      *       172.19.0.0/16        10.0.0.0/8
4        9   756 DROP       all  --  *      *       172.19.0.0/16        0.0.0.0/0
# after the last rule traffic returns to FORWARD chain back
...
```
Note: Synology's iptables doesnt support REJECT -> so I used DROP instead. REJECT is almost instant, while DROP cancels connection attempt after few seconds delay. 
