# Overview
GL.iNet [GL-AXT1800](https://www.gl-inet.com/products/gl-axt1800/) router tunning

## VLAN configuration (for T-Mobile internet)
- SSH to GL-AXT1800
- edit ```vi /etc/config/network```:
1. replace ```option device 'eth0'``` with ```option device 'eth0.300'``` for WAN interface (optionally for WAN6 too)
2. add switch to the end
```bash
config switch_vlan
        option device 'switch0'
        option vlan '1'
        option description 'WAN_t-mobile'
        option ports '1t 5t'
```
3. (optional) google how to exit VIM... google again how to save and exit...
4. restart network: ```/etc/init.d/network restart```
