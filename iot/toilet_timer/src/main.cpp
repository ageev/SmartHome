#include <M5StickCPlus.h>
#include <Wire.h>
#include <VL53L1X.h>

// ---- pins ----
static const int PIN_SDA   = 32;   // Grove yellow
static const int PIN_SCL   = 33;   // Grove white
static const int PIN_BUZZ  = 2;    // built-in buzzer

// ---- timer settings ----
static const uint32_t TIMER_TOTAL_S    = 4 * 60;     // 4 minutes
static const uint16_t TRIGGER_DELTA_MM = 400;        // |Δdistance| over WINDOW_MS that triggers start/stop
static const uint32_t WINDOW_MS        = 1000;       // observation window for the delta

// ---- state ----
enum State { IDLE, RUNNING, FINISHED, CELEBRATE };
State state = IDLE;
bool showTelemetry = true;                            // toggled by button B
enum Mood { MOOD_HAPPY, MOOD_SAD };
Mood celebrateMood = MOOD_HAPPY;
uint32_t celebrateUntilMs = 0;
int32_t celebrateElapsedS = 0;
static const uint32_t CELEBRATE_DURATION_MS = 6000;

// screen size after rotation: 240 wide, 135 tall (landscape) OR 135x240 (portrait)
int screenW = 240;
int screenH = 135;
uint8_t currentRotation = 1;

VL53L1X tof;
uint16_t lastDistMm = 0;
uint32_t timerStartMs = 0;
uint32_t lastBeepSec = 0xFFFFFFFF;

// redraw state (used by drawing fns and orientation; declared early so sleep helpers can touch it)
static int lastDrawnSec = -1;
static uint16_t lastDrawnColor = 0;
static State lastDrawnState = (State)-1;

// screen sleep
static const uint32_t SLEEP_AFTER_MS = 60UL * 1000UL;
uint32_t lastInteractionMs = 0;
bool backlightOn = true;

static void setBacklight(bool on) {
    if (on == backlightOn) return;
    backlightOn = on;
    M5.Axp.SetLDO2(on);
}

static void touchInteraction() {
    lastInteractionMs = millis();
    if (!backlightOn) {
        setBacklight(true);
        lastDrawnState = (State)-1;
    }
}

// ring buffer of recent (timestamp, distance) samples covering ~WINDOW_MS
struct Sample { uint32_t t; uint16_t d; };
static const int RING_N = 64;
static Sample ring[RING_N];
static int ringHead = 0;       // next write index
static int ringCount = 0;      // valid samples (<= RING_N)

static void ringPush(uint32_t t, uint16_t d) {
    ring[ringHead] = {t, d};
    ringHead = (ringHead + 1) % RING_N;
    if (ringCount < RING_N) ringCount++;
}

static void ringClear() { ringHead = 0; ringCount = 0; }

// returns the distance at approximately (now - WINDOW_MS), or 0 if not enough history
static uint16_t ringDistAtWindowAgo(uint32_t now) {
    if (ringCount == 0) return 0;
    uint32_t target = now - WINDOW_MS;
    uint16_t best = 0;
    uint32_t bestAge = 0;  // we want the sample with t <= target and largest t (closest to target from the past)
    bool found = false;
    for (int i = 0; i < ringCount; ++i) {
        int idx = (ringHead - 1 - i + RING_N) % RING_N;
        if (ring[idx].t <= target) {
            if (!found || ring[idx].t > bestAge) {
                best = ring[idx].d;
                bestAge = ring[idx].t;
                found = true;
            }
            break; // ring is in time order (newest..oldest), first hit is the right one
        }
    }
    return found ? best : 0;
}

// ---- helpers ----

static void beepShort() {
    M5.Beep.tone(4000);
    delay(40);
    M5.Beep.mute();
}

static void beepLong() {
    M5.Beep.tone(3000);
    delay(150);
    M5.Beep.mute();
}

// blocking melody: pairs of (freq, ms). 0 freq = rest.
static void playMelody(const uint16_t* notes, const uint16_t* durs, int n) {
    for (int i = 0; i < n; i++) {
        if (notes[i] == 0) {
            M5.Beep.mute();
        } else {
            M5.Beep.tone(notes[i]);
        }
        delay(durs[i]);
        M5.Beep.mute();
        delay(20);
    }
}

static void playHappyMelody() {
    // C5 E5 G5 C6 - bright arpeggio
    static const uint16_t n[] = { 523, 659, 784, 1047, 784, 1047 };
    static const uint16_t d[] = { 110, 110, 110, 160,  90,  220 };
    playMelody(n, d, 6);
}

static void playSadMelody() {
    // descending minor: A4 F4 D4 - sad trombone-ish
    static const uint16_t n[] = { 440, 415, 392, 370, 349 };
    static const uint16_t d[] = { 200, 200, 200, 200, 500 };
    playMelody(n, d, 5);
}

static uint16_t readDistanceMm() {
    if (tof.dataReady()) {
        uint16_t d = tof.read(false); // non-blocking read of last result
        if (!tof.timeoutOccurred() && d > 0 && d < 4000) {
            lastDistMm = d;
        }
    }
    return lastDistMm;
}

// ---- orientation ----
// pick rotation from accel: returns 1 or 3 (landscape) / 0 or 2 (portrait)
// uses hysteresis: requires |dominant| > 0.6g to switch.
static uint8_t pickRotation(uint8_t cur) {
    float ax, ay, az;
    M5.Imu.getAccelData(&ax, &ay, &az);
    // dominant horizontal component decides
    float absX = fabsf(ax), absY = fabsf(ay);
    const float TH = 0.6f;
    if (absX < TH && absY < TH) return cur; // flat or noisy -> keep
    if (absX > absY) {
        // X dominates -> landscape
        return (ax > 0) ? 1 : 3;
    } else {
        // Y dominates -> portrait
        return (ay > 0) ? 0 : 2;
    }
}

static void applyRotation(uint8_t r) {
    if (r == currentRotation) return;
    currentRotation = r;
    M5.Lcd.setRotation(r);
    if (r == 1 || r == 3) { screenW = 240; screenH = 135; }
    else                  { screenW = 135; screenH = 240; }
    M5.Lcd.fillScreen(BLACK);
    lastDrawnState = (State)-1;  // force full redraw
    lastDrawnSec = -1;
    lastDrawnColor = 0;
    touchInteraction();
}

// returns color for given remaining seconds
static uint16_t zoneColor(int32_t remainingS) {
    if (remainingS < 30) return RED;
    if (remainingS < 60) return YELLOW;
    return GREEN;
}

// ---- pixel-art sprites (16x16) ----
// chars: '.'=transparent  'b'=dark brown  'B'=mid brown  'h'=highlight
//        'W'=white  'K'=black  'R'=red
static const char* HAPPY_POO[16] = {
    "................",
    "................",
    ".......bbb......",
    "......bBBBb.....",
    ".....bBhhhBb....",
    "....bBBhBhBBb...",
    "...bBBBBBBBBBb..",
    "...bBWKBBBBKWb..",
    "...bBBBBBBBBBb..",
    "..bBBBKBBBBKBBb.",
    "..bBBBBKKKKBBBb.",
    ".bBhBBBBBBBBBhBb",
    ".bBBBBBBBBBBBBBb",
    ".bBBBhBBBBhBBBBb",
    "..bbbbbbbbbbbb..",
    "................",
};
static const char* ANGRY_POO[16] = {
    "................",
    "................",
    ".......bbb......",
    "......bBBBb.....",
    ".....bBhhhBb....",
    "....bRRBBBRRBb..",   // angry red eyebrows
    "...bBBBBBBBBBb..",
    "...bBKWBBBBWKb..",   // angled eyes
    "...bBBBBBBBBBb..",
    "..bBBBKKKKKBBBb.",
    "..bBBKBBBBBKBBBb",   // frown
    ".bBhBBBBBBBBBhBb",
    ".bBBBBBBBBBBBBBb",
    ".bBBBhBBBBhBBBBb",
    "..bbbbbbbbbbbb..",
    "................",
};

static void drawSprite(const char* const* sprite, int x, int y, int scale) {
    for (int r = 0; r < 16; r++) {
        const char* row = sprite[r];
        for (int c = 0; c < 16; c++) {
            uint16_t col;
            switch (row[c]) {
                case 'b': col = M5.Lcd.color565(0x4E, 0x34, 0x2E); break;
                case 'B': col = M5.Lcd.color565(0x79, 0x55, 0x48); break;
                case 'h': col = M5.Lcd.color565(0xA1, 0x88, 0x7F); break;
                case 'W': col = WHITE; break;
                case 'K': col = BLACK; break;
                case 'R': col = M5.Lcd.color565(0xFF, 0x40, 0x40); break;
                default: continue;
            }
            M5.Lcd.fillRect(x + c*scale, y + r*scale, scale, scale, col);
        }
    }
}

// ---- drawing ----

static bool isLandscape() { return screenW > screenH; }

static void drawTelemetry(uint16_t dist, int delta) {
    int y = screenH - 12;
    M5.Lcd.fillRect(0, y - 8, screenW, 20, BLACK);
    if (!showTelemetry) return;
    M5.Lcd.setTextDatum(MC_DATUM);
    M5.Lcd.setTextColor(DARKGREY, BLACK);
    M5.Lcd.setTextSize(1);
    char buf[40];
    snprintf(buf, sizeof(buf), "d=%dmm  Dt=%+d", dist, delta);
    M5.Lcd.drawString(buf, screenW/2, y);
}

static void drawIdle(uint16_t dist, int delta) {
    if (lastDrawnState != IDLE) {
        M5.Lcd.fillScreen(BLACK);
        M5.Lcd.setTextDatum(MC_DATUM);
        M5.Lcd.setTextColor(WHITE, BLACK);
        M5.Lcd.setTextSize(2);
        M5.Lcd.drawString("Ready", screenW/2, screenH/2 - 20);
        M5.Lcd.setTextSize(1);
        M5.Lcd.drawString("sit down to start", screenW/2, screenH/2 + 5);
        lastDrawnState = IDLE;
    }
    drawTelemetry(dist, delta);
}

static void drawRunning(int32_t remainingS, uint16_t dist, int delta) {
    uint16_t col = zoneColor(remainingS);
    if (lastDrawnState != RUNNING) {
        M5.Lcd.fillScreen(BLACK);
        lastDrawnState = RUNNING;
        lastDrawnSec = -1;
        lastDrawnColor = 0;
    }
    if ((int)remainingS != lastDrawnSec || col != lastDrawnColor) {
        int tsize = isLandscape() ? 7 : 4;
        int blockH = tsize * 8 + 8;
        int yMid = screenH / 2 - 8;
        M5.Lcd.fillRect(0, yMid - blockH/2, screenW, blockH, BLACK);
        M5.Lcd.setTextDatum(MC_DATUM);
        M5.Lcd.setTextColor(col, BLACK);
        M5.Lcd.setTextSize(tsize);
        char buf[8];
        int m = remainingS / 60;
        int s = remainingS % 60;
        if (m < 0) { m = 0; s = 0; }
        snprintf(buf, sizeof(buf), "%d:%02d", m, s);
        M5.Lcd.drawString(buf, screenW/2, yMid);
        lastDrawnSec = remainingS;
        lastDrawnColor = col;
    }
    drawTelemetry(dist, delta);
}

static void drawFinished() {
    if (lastDrawnState != FINISHED) {
        M5.Lcd.fillScreen(RED);
        M5.Lcd.setTextDatum(MC_DATUM);
        M5.Lcd.setTextColor(WHITE, RED);
        M5.Lcd.setTextSize(isLandscape() ? 4 : 3);
        M5.Lcd.drawString("TIME!", screenW/2, screenH/2 - 12);
        M5.Lcd.setTextSize(1);
        M5.Lcd.drawString("get up", screenW/2, screenH/2 + 20);
        lastDrawnState = FINISHED;
    }
}

static void drawCelebrate() {
    if (lastDrawnState != CELEBRATE) {
        bool happy = (celebrateMood == MOOD_HAPPY);
        uint16_t bg = happy ? M5.Lcd.color565(0x10, 0x40, 0x10) : M5.Lcd.color565(0x40, 0x10, 0x10);
        M5.Lcd.fillScreen(bg);
        // big sprite, scale 4 -> 64x64
        int sx = (screenW - 64) / 2;
        int sy = 8;
        drawSprite(happy ? HAPPY_POO : ANGRY_POO, sx, sy, 4);

        M5.Lcd.setTextDatum(MC_DATUM);
        M5.Lcd.setTextColor(happy ? GREEN : YELLOW, bg);
        M5.Lcd.setTextSize(2);
        M5.Lcd.drawString(happy ? "GOOD JOB!" : "TOO LONG", screenW/2, sy + 64 + 14);

        // show elapsed M:SS
        M5.Lcd.setTextColor(WHITE, bg);
        M5.Lcd.setTextSize(1);
        char buf[16];
        int m = celebrateElapsedS / 60;
        int s = celebrateElapsedS % 60;
        snprintf(buf, sizeof(buf), "%d:%02d", m, s);
        M5.Lcd.drawString(buf, screenW/2, sy + 64 + 32);
        lastDrawnState = CELEBRATE;
    }
}

// ---- setup / loop ----

void setup() {
    M5.begin();
    M5.Imu.Init();
    M5.Lcd.setRotation(currentRotation);   // start in landscape, will auto-adjust
    M5.Lcd.fillScreen(BLACK);
    M5.Beep.begin();
    M5.Beep.setBeep(4000, 100);

    Wire.begin(PIN_SDA, PIN_SCL);
    Wire.setClock(400000);

    tof.setBus(&Wire);
    tof.setTimeout(500);
    if (!tof.init()) {
        M5.Lcd.setTextDatum(MC_DATUM);
        M5.Lcd.setTextColor(RED, BLACK);
        M5.Lcd.setTextSize(2);
        M5.Lcd.drawString("ToF FAIL", 120, 60);
        M5.Lcd.setTextSize(1);
        M5.Lcd.drawString("check Grove cable", 120, 90);
        while (true) { delay(1000); }
    }
    tof.setDistanceMode(VL53L1X::Long);
    tof.setMeasurementTimingBudget(50000);
    tof.startContinuous(50);

    Serial.begin(115200);
    Serial.println("toilet-timer boot");
    state = IDLE;
    lastDrawnState = (State)-1;
    lastInteractionMs = millis();
}

void loop() {
    M5.update();
    M5.Beep.update();

    // orientation check (~5 Hz, with hysteresis: same target for 400ms)
    static uint32_t lastOriCheck = 0;
    static uint8_t pendingRot = 1;
    static uint32_t pendingSince = 0;
    if (millis() - lastOriCheck > 200) {
        lastOriCheck = millis();
        uint8_t target = pickRotation(currentRotation);
        if (target != currentRotation) {
            if (target != pendingRot) {
                pendingRot = target;
                pendingSince = millis();
            } else if (millis() - pendingSince > 2000) {
                applyRotation(target);
            }
        } else {
            pendingRot = currentRotation;
            pendingSince = 0;
        }
    }

    // BtnA: manual cancel (when running/finished). BtnB: toggle telemetry.
    if (M5.BtnB.wasPressed()) {
        touchInteraction();
        showTelemetry = !showTelemetry;
        lastDrawnState = (State)-1;
        beepShort();
    }
    if (M5.BtnA.wasPressed()) {
        touchInteraction();
        if (state == RUNNING || state == FINISHED || state == CELEBRATE) {
            state = IDLE;
            ringClear();
            lastDrawnState = (State)-1;
            beepShort();
        }
    }

    uint16_t dist = readDistanceMm();
    uint32_t now = millis();
    if (dist > 0) ringPush(now, dist);

    uint16_t past = ringDistAtWindowAgo(now);
    int delta = (past > 0 && dist > 0) ? ((int)dist - (int)past) : 0;  // negative = got closer
    bool haveWindow = (past > 0);

    switch (state) {
        case IDLE: {
            if (haveWindow && delta <= -(int)TRIGGER_DELTA_MM) {
                state = RUNNING;
                timerStartMs = now;
                lastBeepSec = 0xFFFFFFFF;
                lastDrawnState = (State)-1;
                touchInteraction();
                beepShort();
                Serial.printf("-> RUNNING (delta=%d)\n", delta);
                ringClear();
                break;
            }
            if (backlightOn && now - lastInteractionMs > SLEEP_AFTER_MS) {
                setBacklight(false);
                M5.Lcd.fillScreen(BLACK);
                lastDrawnState = (State)-1;
            }
            if (backlightOn) drawIdle(dist, delta);
            break;
        }
        case RUNNING: {
            int32_t elapsed = (now - timerStartMs) / 1000;
            int32_t remaining = (int32_t)TIMER_TOTAL_S - elapsed;
            if (remaining <= 0) {
                state = FINISHED;
                lastDrawnState = (State)-1;
                touchInteraction();
                Serial.println("-> FINISHED");
                break;
            }
            if (remaining < 30) {
                uint32_t sec = (uint32_t)remaining;
                bool shouldBeep = (remaining <= 5) ? true : (sec % 5 == 0);
                if (shouldBeep && sec != lastBeepSec) {
                    beepShort();
                    lastBeepSec = sec;
                }
            }
            // user got up: decide HAPPY (>=60s) or silent IDLE (<60s)
            if (haveWindow && delta >= (int)TRIGGER_DELTA_MM) {
                ringClear();
                touchInteraction();
                lastDrawnState = (State)-1;
                if (elapsed >= 60) {
                    celebrateMood = MOOD_HAPPY;
                    celebrateElapsedS = elapsed;
                    celebrateUntilMs = now + CELEBRATE_DURATION_MS;
                    state = CELEBRATE;
                    Serial.printf("-> CELEBRATE HAPPY (elapsed=%d)\n", (int)elapsed);
                    playHappyMelody();
                } else {
                    state = IDLE;
                    Serial.printf("-> IDLE (too short, elapsed=%d)\n", (int)elapsed);
                }
                break;
            }
            drawRunning(remaining, dist, delta);
            break;
        }
        case FINISHED: {
            drawFinished();
            static uint32_t lastFinBeep = 0;
            if (now - lastFinBeep > 700) {
                beepLong();
                lastFinBeep = now;
            }
            // user finally got up after overrun: SAD show
            if (haveWindow && delta >= (int)TRIGGER_DELTA_MM) {
                int32_t elapsed = (now - timerStartMs) / 1000;
                ringClear();
                touchInteraction();
                lastDrawnState = (State)-1;
                celebrateMood = MOOD_SAD;
                celebrateElapsedS = elapsed;
                celebrateUntilMs = now + CELEBRATE_DURATION_MS;
                state = CELEBRATE;
                Serial.printf("-> CELEBRATE SAD (elapsed=%d)\n", (int)elapsed);
                playSadMelody();
            }
            break;
        }
        case CELEBRATE: {
            drawCelebrate();
            if (now >= celebrateUntilMs) {
                state = IDLE;
                lastDrawnState = (State)-1;
                touchInteraction();
            }
            break;
        }
        default: break;
    }

    delay(20);
}
