# Toilet Timer

**[English](#english)** | **[Русский](#russian)**

A toilet timer on M5StickC Plus 1.1 + VL53L1X laser rangefinder.
Tracks how long someone sits, rewards on-time exits with a cheerful
pixel-art poo and a happy jingle, shames overstayers with a grumpy poo
and a sad melody.

---

<a id="english"></a>

## English

### Idea

Nobody in the family enjoys waiting for an occupied bathroom for half an
hour. The goal was a harmless device that:

1. Knows when someone sits down (no buttons).
2. Counts down 4 minutes — a reasonable comfort window.
3. If the person finishes between 1:00 and 3:59 — congratulates them.
4. If they overstay — shames them with a gloomy animation.
5. Doesn't burn-in on the wall when nobody's looking.

### Hardware

| Component | Purpose | Price |
|---|---|---|
| M5StickC Plus 1.1 | ESP32-PICO + 1.14" TFT 135×240 + buzzer + IMU + battery | ~$25 |
| M5Stack ToF4M Unit (VL53L1X) | Laser rangefinder, up to 4 m | ~$10 |
| Grove 4-pin cable | included with ToF | — |

No RGB module: the M5StickC Grove port carries I²C for the ToF (G32=SDA,
G33=SCL), and the U003 RGB unit also uses G32 for its data line. They
can't share a wire — I²C and the SK6812 protocol would corrupt each
other. We stuck to the built-in TFT and buzzer.

#### Wiring

```
M5StickC Plus 1.1 ── Grove ── ToF4M (VL53L1X)
                                G32 = SDA
                                G33 = SCL
                                5V, GND
```

That's it. No soldering.

### Logic

#### Sit / stand detection

No calibration needed. A ring buffer holds the last ~3 seconds of
samples (timestamp, distance, ~20 Hz). Each tick we compare the current
reading with the one taken exactly 1 second ago:

- **delta ≤ −400 mm** → object got significantly closer → START.
- **delta ≥ +400 mm** → object got significantly farther → STOP.

This is robust to slow background drift (lighting, lid position, cat
walking by) — only an abrupt change within a 1-second window triggers.
The buffer is cleared on each trigger to prevent the same edge firing
twice.

#### State machine

```
                             ┌─────────────┐
        trigger: dist drop   │             │  delta ≥ 400 mm
   ┌────────────────────────▶│   RUNNING   ├────┬──────────────┐
   │                         │  4:00 → 0   │    │ elapsed ≥ 60 │
   │                         └─────┬───────┘    │              ▼
┌──┴────┐                           │ time      │     ┌─────────────────┐
│ IDLE  │                           │ ran out   │     │  CELEBRATE      │
│ Ready │                           ▼            │     │  HAPPY (6 s)    │
└───┬───┘                     ┌──────────┐       │     │  + jingle       │
    │                         │ FINISHED │       │     └────────┬────────┘
    │                         │ TIME!    │       │              │
    │                         │ beeps    │       │       elapsed < 60
    │                         └────┬─────┘       │       (silent)
    │                              │ delta ≥ 400 │              │
    │                              ▼              │              │
    │                       ┌─────────────────┐  │              │
    │                       │  CELEBRATE      │  │              │
    │                       │  SAD (6 s)      │  │              │
    │                       │  + dirge        │  │              │
    │                       └─────┬───────────┘  │              │
    │                             │              │              │
    │                             ▼              ▼              ▼
    └─────────────────────────────────────────────────────────────
                              auto-return to IDLE
```

#### Colors and sound during countdown

| Remaining | Digit color | Sound |
|---|---|---|
| > 60 s | green | — |
| 30–60 s | yellow | — |
| < 30 s | red | beep every 5 s |
| < 5 s | red | beep every second |
| 0 (TIME!) | white on red | long beeps every 700 ms |

#### Stand-up show

- **HAPPY** (got up between 1:00 and 3:59): green background, 64×64
  smiling pixel-art poo, `GOOD JOB!` text, actual sit time, melody —
  ascending C5–E5–G5–C6 arpeggio.
- **SAD** (got up after the timer expired): red background, the same
  pixel art but angry, `TOO LONG` text, sit time, melody — descending
  "sad trombone".
- The show lasts 6 seconds, then auto-returns to IDLE.
- Button **A** dismisses it early.

#### Power saving and burn-in protection

- After **60 s of inactivity** in IDLE, the backlight is killed via the
  AXP192 PMIC (`SetLDO2(false)`). Screen goes black, OLED doesn't burn,
  battery isn't drained.
- Wakes instantly on: timer start (sensor trigger), any button press, or
  device rotation.
- During RUNNING / FINISHED / CELEBRATE the screen stays on.

#### Auto-rotation

The built-in MPU6886 accelerometer is polled ~5 Hz. The dominant gravity
axis selects one of 4 orientations (two landscape + two portrait).
Hysteresis: a new orientation must hold for 2 seconds before the screen
actually rotates. In portrait the timer digits scale down automatically
(size 4 instead of 7) to fit the 135 px width.

#### Buttons

| Button | Action |
|---|---|
| **A** (large, front) | Cancel timer / dismiss CELEBRATE |
| **B** (side) | Toggle telemetry footer (`d=…mm Δt=…`) |

### Build and flash

Requires VS Code + the **PlatformIO IDE** extension.

```bash
git clone https://github.com/ageev/SmartHome.git
cd SmartHome/iot/toilet_timer
# open the folder in VS Code; PlatformIO will pick up platformio.ini
```

Plug the M5StickC Plus 1.1 into USB-C, ToF Unit into the Grove port. Hit
**Upload** in PlatformIO (the → arrow). The first build pulls in
libraries (M5StickCPlus, VL53L1X) — about a minute.

On macOS the CH9102 driver is usually already bundled. If the port
doesn't show up, install `CH9102_VCP_SER_MacOS` from M5Stack's site.

### Tunable settings

Top of `src/main.cpp`, "timer settings" section:

```cpp
static const uint32_t TIMER_TOTAL_S    = 4 * 60;   // total countdown
static const uint16_t TRIGGER_DELTA_MM = 400;      // Δdistance threshold for start/stop
static const uint32_t WINDOW_MS        = 1000;     // observation window for Δ
```

Other useful constants:
- `SLEEP_AFTER_MS` — how long until the screen sleeps in IDLE.
- `CELEBRATE_DURATION_MS` — duration of the HAPPY/SAD show.

### Files

- [`platformio.ini`](platformio.ini) — PlatformIO config (platform, board, libs).
- [`src/main.cpp`](src/main.cpp) — all the logic, ~500 lines.

### Possible upgrades

- MQTT publish "sit duration: 2:30" event for Home Assistant stats.
- Multiple profiles (kid / adult) with different durations.
- Web config via WiFi captive portal.
- Persist "best time of the week" in NVS and show it on idle.

### License

MIT — do whatever you want, just don't blame the authors if your
flatmate gets offended by the `TOO LONG` screen.

---

<a id="russian"></a>

## Русский

### Идея

Никто в семье не любит когда туалет занят полчаса. Хотелось безобидное
устройство, которое:

1. Само понимает что кто-то сел (без кнопок).
2. Отсчитывает 4 минуты — комфортный таймер.
3. Если человек уложился в 1:00–3:59 — встречает его поздравлением.
4. Если просидел дольше — стыдит мрачной анимацией.
5. Не выгорает на стене когда его никто не смотрит.

### Железо

| Компонент | Назначение | Цена |
|---|---|---|
| M5StickC Plus 1.1 | ESP32-PICO + 1.14" TFT 135×240 + buzzer + IMU + батарея | ~$25 |
| M5Stack ToF4M Unit (VL53L1X) | Лазерный дальномер до 4 м | ~$10 |
| Grove-кабель 4-pin | в комплекте с ToF | — |

Без RGB-модуля: на Grove-порту M5StickC сидит I²C для ToF (G32=SDA,
G33=SCL), а у RGB-модуля U003 шина данных тоже на G32 — повесить их на
одну линию нельзя (I²C и протокол SK6812 будут гадить друг другу).
Решили обойтись TFT-экраном и встроенным бипером.

#### Подключение

```
M5StickC Plus 1.1 ── Grove ── ToF4M (VL53L1X)
                                G32 = SDA
                                G33 = SCL
                                5V, GND
```

Всё. Никакой пайки.

### Логика

#### Детекция «сел / встал»

Калибровка не нужна. В RAM крутится кольцевой буфер последних ~3 секунд
замеров (timestamp, distance, ~20 Гц). На каждом такте сравниваем
текущий замер с тем, что был ровно 1 секунду назад:

- **delta ≤ −400 мм** → человек резко приблизился к датчику → СТАРТ.
- **delta ≥ +400 мм** → человек резко удалился → СТОП.

Подход устойчив к плавным изменениям обстановки (свет, открытая крышка,
котик прошёл) — для триггера нужен резкий скачок именно за окно в 1 с.
После срабатывания буфер чистится — чтобы тот же фронт не сработал
второй раз.

#### Состояния

```
                             ┌─────────────┐
        старт по триггеру    │             │  delta ≥ 400 мм
   ┌────────────────────────▶│   RUNNING   ├────┬──────────────┐
   │                         │  4:00 → 0   │    │ elapsed ≥ 60 │
   │                         └─────┬───────┘    │              ▼
┌──┴────┐                           │ время     │     ┌─────────────────┐
│ IDLE  │                           │ вышло     │     │  CELEBRATE      │
│ Ready │                           ▼            │     │  HAPPY (6 c)    │
└───┬───┘                     ┌──────────┐       │     │  + мелодия      │
    │                         │ FINISHED │       │     └────────┬────────┘
    │                         │ TIME!    │       │              │
    │                         │ бипы     │       │       elapsed < 60
    │                         └────┬─────┘       │       (молча)
    │                              │ delta ≥ 400 │              │
    │                              ▼              │              │
    │                       ┌─────────────────┐  │              │
    │                       │  CELEBRATE      │  │              │
    │                       │  SAD (6 c)      │  │              │
    │                       │  + мелодия      │  │              │
    │                       └─────┬───────────┘  │              │
    │                             │              │              │
    │                             ▼              ▼              ▼
    └─────────────────────────────────────────────────────────────
                              авто-возврат в IDLE
```

#### Цвета и звук во время отсчёта

| Осталось | Цвет цифр | Звук |
|---|---|---|
| > 60 с | зелёный | — |
| 30–60 с | жёлтый | — |
| < 30 с | красный | бип каждые 5 с |
| < 5 с | красный | бип каждую секунду |
| 0 (TIME!) | белый на красном | длинные бипы каждые 700 мс |

#### Шоу при вставании

- **HAPPY** (встал в 1:00–3:59): зелёный фон, 64×64 пиксельная какашка с
  улыбкой, надпись `GOOD JOB!`, фактическое время сидения, мелодия —
  восходящее арпеджио C5–E5–G5–C6.
- **SAD** (встал после истечения 4 минут): красный фон, та же пиксель-арт
  но злая, надпись `TOO LONG`, время сидения, мелодия — нисходящая
  «грустная труба».
- Шоу длится 6 секунд, потом автоматический возврат в IDLE.
- Кнопка **A** прерывает шоу досрочно.

#### Энергосбережение и анти-выгорание

- В состоянии IDLE через **60 секунд бездействия** подсветка гасится
  через AXP192 PMIC (`SetLDO2(false)`). Экран чёрный, OLED не выгорает,
  батарея не садится.
- Просыпается мгновенно при: старте таймера (по датчику), нажатии любой
  кнопки, повороте устройства.
- В RUNNING / FINISHED / CELEBRATE экран всегда включён.

#### Авто-поворот экрана

Встроенный MPU6886 (акселерометр) опрашивается ~5 раз в секунду. По
вектору гравитации выбирается одна из 4 ориентаций (две landscape +
две portrait). Антидребезг: новая ориентация должна продержаться
2 секунды прежде чем экран реально повернётся. В portrait цифры таймера
автоматически уменьшаются (size 4 вместо 7), чтобы влезали в 135 px.

#### Кнопки

| Кнопка | Действие |
|---|---|
| **A** (большая, лицевая) | Отмена таймера / прерывание CELEBRATE |
| **B** (сбоку) | Toggle телеметрии (нижняя строка `d=…mm Δt=…`) |

### Сборка и заливка

Требуется VS Code + расширение **PlatformIO IDE**.

```bash
git clone https://github.com/ageev/SmartHome.git
cd SmartHome/iot/toilet_timer
# открыть папку в VS Code, PlatformIO сам подхватит platformio.ini
```

Подключаем M5StickC Plus 1.1 по USB-C, ToF Unit в Grove-порт. В
PlatformIO жмём **Upload** (стрелка →). Первый билд ставит библиотеки
(M5StickCPlus, VL53L1X) — займёт минуту.

На macOS драйвер для CH9102 обычно уже встроен. Если порт не появляется
— поставить `CH9102_VCP_SER_MacOS` с сайта M5Stack.

### Настройки в коде

Топ `src/main.cpp`, секция `timer settings`:

```cpp
static const uint32_t TIMER_TOTAL_S    = 4 * 60;   // длительность таймера
static const uint16_t TRIGGER_DELTA_MM = 400;      // порог Δрасстояния для старт/стоп
static const uint32_t WINDOW_MS        = 1000;     // окно наблюдения для Δ
```

Полезные константы:
- `SLEEP_AFTER_MS` — через сколько гасить экран в IDLE.
- `CELEBRATE_DURATION_MS` — длительность HAPPY/SAD-шоу.

### Файлы

- [`platformio.ini`](platformio.ini) — конфиг PlatformIO (платформа, борд, библиотеки).
- [`src/main.cpp`](src/main.cpp) — вся логика, ~500 строк.

### Возможные доработки

- MQTT-публикация события «сидение завершено: 2:30» — для статистики в
  Home Assistant.
- Несколько профилей (детский / взрослый) с разной длительностью.
- Web-конфигурация через WiFi captive portal.
- Считать «рекорды» в NVS и показывать «лучшее время недели».

### Лицензия

MIT — делайте что хотите, только не вините авторов если ваш сосед по
квартире обиделся на надпись `TOO LONG`.
