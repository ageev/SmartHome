# Setting up pi-hole on Synology with docker using macvlan
There are multiple ways to have pi-hole installed: native app, Virtual Machine, docker host, docker macvlan. I tried them all, docker macvlan is the best one (macvlan == separate virtual network adapter). Only last option gives you opportunety to run pi-hole on 80/443 opened ports. 

# Step 0. Preparing the environment
1. create a "docker" folder on NAS. In will be used to store container's configs
2. install docker in DSM
3. create a docker user. Set user's persmissions accordingly (write access to docker folder, remove everything else)
4. get user ID and group ID using "id *docker_username*" console command. Note the UID and GID numbers
5. create *pihole* folder inside *docker* folder. Create *dnsmasq* folder inside *pihole* folder
  
# Step 1. Docker-compose
1. copy the "docker-compose.yml" file from this repo to your docker folder (probably /Volume1/docker)
2. customize the docker-compose file (see below)
3. SSH to NAS
```bash
cd /volume1/docker
sudo docker-compose up -d
```
4. all done!

if you need to upgrade the container, stop it 1st. Run ```sudo docker system prune -a```. This will remove everything except the pi-hole config/logs folder and do the (3) again.

# Step 2. Importing lists
Default pi-hole lists are too unrestrictive. My average ad blocking rate for average home network is 20% with default list, 40-80% with additional ones. 
Download the pi-hole....tar.gz file. Go to pi-hole admin -> Settings -> Teleporter -> Restore and import the file content.
It may block smthng important -> goto pihole dashboard, go through your logs and build your own whitelist.

# Step 3. Adding proper route to docker's macvlan network adapter
Docker's macvlan adapters are available for LAN users, but are not available for DSM/Docker users. Generaly speaking that's not an issue, but if you want, for example, to add pi-hole to your home-assistant container running on the same docker (like I did) you need this.
1. create a file in /usr/local/etc/rc.d folder with .sh extention (this is a DSM script autostart directory[1])
2. chmod 755 <filename>
3. andjust & copy this into the file

```bash
#!/bin/bash

if [ "$1" = "start" ];
then
    sleep 60
    ip link add dlink0 link ovs_eth0 type macvlan mode bridge
    ip addr add 192.168.7.127/32 dev dlink0
    ip link set dlink0 up
    ip route add 192.168.7.8/32 dev dlink0
fi
```
* 192.168.7.127/32 - just a random IP to use as a virtual adapter's IP
* dlink0 - new virtual network interface name
* ovs_eth0 - Synology DSM network interface name
* 192.168.7.8/32 - pi-hole container's IP

"sleep 60" is needed for script to work properly, otherwise it fails. 

# Step 4. Customising docker-compose.yml 
Here is the official documentation on all parameters [2]. I found out that my DNS settings were not preserved during container restart. That's why I put them inside the container configuration.
  
# Links
* [1] Synology DSM developer guide https://www.synology.com/en-us/support/developer#tool
* [2] https://github.com/pi-hole/docker-pi-hole
