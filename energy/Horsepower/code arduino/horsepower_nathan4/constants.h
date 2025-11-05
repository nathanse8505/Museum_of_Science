#ifndef CONSTANTS_H
#define CONSTANTS_H

// ---------- Pins ----------
#define LANG_BUTTON_IO  2       // language selection button (INPUT_PULLUP)
#define XSHUT_PIN       3       // VL53L1X shutdown pin

/*
 * Arduino Nano pin mapping (for reference)
 *                       ---------
 *                   TXD-|       |-Vin 
 *                   RXD-|       |-GND (to sensor)
 *                   RST-|       |-RST
 *                   GND-|       |-+5V (to sensor)
 *     LANG_BUTTON_IO D2-|       |-A7
 *          XSHUT_PIN D3-|       |-A6
 *                    D4-|       |-A5 (SCL)
 *                    D5-|       |-A4 (SDA)
 *                    D6-|       |-A3
 *                    D7-|       |-A2
 *                    D8-|       |-A1
 *                    D9-|       |-A0
 *                   D10-|       |-Ref
 *                   D11-|       |-3.3V
 *                   D12-|       |-D13
 *                        --USB--
 */

// ---------- Debounce / Language ----------
const int    BOUNCE_TIME = 50;  // button debounce (ms)
bool         check_lang  = false;
int8_t       lang        = 0;

// ---------- Physics ----------
const float  WEIGHT_KG_BALL          = 7.5f;
const float  WEIGHT_KG_POWER_HORSE   = 75.0f;
const float  GRAVITY                 = 9.81f;
const float  HORSEPOWER_CONVERSION   = 745.7f;
float        P_Watt = 0.0f;

// ---------- Thresholds & filtering ----------
const float  THRESHOLD         = 120.0f;            // mm (entry gap from baseline)
float        smoothedDistance  = 0.0f;              // filtered distance
const int32_t BAUDERATE        = 115200;
float        initialDistance   = 0.0f;

// motion state
bool         Lift_in_motion    = false;
bool         start_try         = true;

unsigned long startTime        = 0;
unsigned long deltaTime        = 0;

// distance constraints
float        minDistance       = 0.0f;
const int32_t DELTA_MAX_DISTANCE = 700;             // mm travel to compute HP
float        horsepower        = 0.0f;
const int32_t INITIAL_DISTANCE = 1300;              // mm initial baseline

// timers
unsigned long bouncingBallTimer = 0;
unsigned long meas_Tmr_rst      = 0;
unsigned long Tmr_rst_com       = 0;
const unsigned long MEAS_RST_MS        = 5000;      // ms: max time to reach travel
const unsigned long RST_BOUNCING_BALL  = 7000;      // ms: cooldown after max distance
const unsigned long SENSOR_FAULTY      = 15000;     // ms: safety reset
const unsigned long RST_WDT            = 5000;      // ms: wait after reset
const unsigned long RST_COMMUNICATION  = 3600000;   // ms: periodic comm reset




/*
 * Robust filtering & debouncing helpers
 * -------------------------------------
 * - Median of a short window to crush outliers
 * - Step limiter to avoid sudden jumps
 * - Hysteresis + consecutive-samples logic to enter/exit "motion"
 */
static const int MEDIAN_WIN = 3;   // was 5 → smaller window = less lag
static uint16_t ring[MEDIAN_WIN];
static int ringIdx = 0;
static bool ringFilled = false;

// consecutive counters for enter/exit conditions
static int belowCnt = 0;
static int aboveCnt = 0;

// adaptive baseline (tracks slow drifts when idle)
static float baseline = INITIAL_DISTANCE;

// ==== FAST PROFILE (reaction speed) ====
// filtering & debouncing

// ---------- New robustness parameters ----------
const int    ENTER_COUNT = 1;     // consecutive samples below threshold to start
const int    EXIT_COUNT  = 1;     // consecutive samples above threshold to exit
const float  HYST_MM     = 15.0f; // hysteresis band (mm) around minDistance

// filter dynamics (used in getFilteredDistance)
const float MAX_STEP        = 300.0f;  // mm per loop max step
const float IIR_ALPHA      = 0.85f;  // was 0.5 → much less smoothing
const float BETA = 0.02f;  // slow update


#endif
