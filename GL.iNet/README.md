# GL.iNet

**[English](#english)** | **[Русский](#russian)**

---

<a id="english"></a>

## English

### Overview

Tuning for the GL.iNet [GL-AXT1800](https://www.gl-inet.com/products/gl-axt1800/) router.

### VLAN configuration (for T-Mobile internet)

1. SSH into GL-AXT1800.
2. Edit `/etc/config/network`:
   - Replace `option device 'eth0'` with `option device 'eth0.300'` for the WAN interface (optionally for WAN6 too).
   - Add this switch config at the end:

     ```
     config switch_vlan
             option device 'switch0'
             option vlan '1'
             option description 'WAN_t-mobile'
             option ports '1t 5t'
     ```

3. (Optional) Google how to exit vim... Google again how to save and exit...
4. Restart the network: `/etc/init.d/network restart`.

---

<a id="russian"></a>

## Русский

### Обзор

Тюнинг роутера GL.iNet [GL-AXT1800](https://www.gl-inet.com/products/gl-axt1800/).

### Настройка VLAN (для интернета T-Mobile)

1. SSH в GL-AXT1800.
2. Отредактируйте `/etc/config/network`:
   - Замените `option device 'eth0'` на `option device 'eth0.300'` для WAN-интерфейса (по желанию — и для WAN6).
   - Добавьте в конец конфигурацию свитча (см. блок выше).

3. (По желанию) Загуглите, как выйти из vim... Загуглите снова, как сохранить и выйти...
4. Перезапустите сеть: `/etc/init.d/network restart`.
