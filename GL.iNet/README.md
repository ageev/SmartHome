# Overview
GL.iNet router tunning

## VLAN configuration (for T-Mobile internet)
edit ```/etc/config/network```:
1. replace ```option device 'eth0'``` with ```option device 'eth0.300'``` for WAN interface
2. add switch
```bash
config switch_vlan
        option device 'switch0'
        option vlan '1'
        option description 'WAN_t-mobile'
        option ports '1t 5t'
```
