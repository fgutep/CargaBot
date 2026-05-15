/*
 * esp32_lift_tof.ino
 * VL53L0X (x5) + DC motor lift + limit switches
 * Control manual por Serial: teclea u=up, d=down, s=stop
 * Board: Waveshare ESP32-S3-Zero
 */

#include <Wire.h>
#include <Adafruit_VL53L0X.h>
#include <ArduinoJson.h>

// ============================================================
//  CONFIGURACIÓN
// ============================================================

#define PIN_SDA 7
#define PIN_SCL 6

#define XSHUT_1 5
#define XSHUT_2 13
#define XSHUT_3 12
#define XSHUT_4 11
#define XSHUT_5 10

#define ADDR_1 0x30
#define ADDR_2 0x31
#define ADDR_3 0x32
#define ADDR_4 0x33
#define ADDR_5 0x34

const char* SENSOR_NAMES[5] = {
  "sensor_1", "sensor_2", "sensor_3", "sensor_4", "sensor_5"
};

// --- Motor DC ---
#define PIN_MOTOR_A  2
#define PIN_MOTOR_B  1
#define MOTOR_SPEED  255   // 0-255

// --- Fines de carrera ---
#define PIN_LIMIT_TOP    8
#define PIN_LIMIT_BOTTOM 9

// pull-down: 10k a GND, switch conecta pin a 3.3V → activado = HIGH
// pull-up:   10k a 3.3V, switch conecta pin a GND  → activado = LOW
#define LIMIT_SWITCH_PULL_UP false

#if LIMIT_SWITCH_PULL_UP
  #define LIMIT_TRIGGERED LOW
#else
  #define LIMIT_TRIGGERED HIGH
#endif

#define SENSOR_PUBLISH_INTERVAL_MS 200

// ============================================================
//  VARIABLES GLOBALES
// ============================================================

const uint8_t SENSOR_ADDRS[5] = { ADDR_1, ADDR_2, ADDR_3, ADDR_4, ADDR_5 };
const uint8_t XSHUT_PINS[5]   = { XSHUT_1, XSHUT_2, XSHUT_3, XSHUT_4, XSHUT_5 };

Adafruit_VL53L0X tof[5];

bool lift_running = false;
int  lift_dir     = 0;   // +1 up, -1 down, 0 stop

unsigned long last_publish_ms = 0;

// ============================================================
//  MOTOR
// ============================================================

void motor_up() {
  analogWrite(PIN_MOTOR_A, 0);
  analogWrite(PIN_MOTOR_B, MOTOR_SPEED);
}


void motor_down() {
  analogWrite(PIN_MOTOR_A, MOTOR_SPEED);
  analogWrite(PIN_MOTOR_B, 0);
}

void motor_stop() {
  analogWrite(PIN_MOTOR_A, 0);
  analogWrite(PIN_MOTOR_B, 0);
}

// ============================================================
//  FINES DE CARRERA
// ============================================================

bool limit_triggered(uint8_t pin) {
  return digitalRead(pin) == LIMIT_TRIGGERED;
}

// Llama cada iteración del loop — para el motor si toca un extremo
void check_limits() {
  if (!lift_running) return;

  if (lift_dir == 1 && limit_triggered(PIN_LIMIT_TOP)) {
    motor_stop();
    lift_running = false;
    lift_dir = 0;
    Serial.println(">> TOP alcanzado - usa 'd' o 's'");
  }
  else if (lift_dir == -1 && limit_triggered(PIN_LIMIT_BOTTOM)) {
    motor_stop();
    lift_running = false;
    lift_dir = 0;
    Serial.println(">> BOTTOM alcanzado - usa 'u' o 's'");
  }
}

// ============================================================
//  HELPERS JSON
// ============================================================

void publish_error(const char* code, const char* msg) {
  StaticJsonDocument<128> doc;
  doc["type"] = "error"; doc["code"] = code; doc["message"] = msg;
  serializeJson(doc, Serial); Serial.println();
}

void publish_status(const char* msg) {
  StaticJsonDocument<128> doc;
  doc["type"] = "status"; doc["message"] = msg;
  serializeJson(doc, Serial); Serial.println();
}

// ============================================================
//  SETUP
// ============================================================

void setup() {
  Serial.begin(115200);
  Wire.begin(PIN_SDA, PIN_SCL);

  // Motor
  pinMode(PIN_MOTOR_A, OUTPUT);
  pinMode(PIN_MOTOR_B, OUTPUT);
  motor_stop();

  // Fines de carrera
  pinMode(PIN_LIMIT_TOP,    INPUT);
  pinMode(PIN_LIMIT_BOTTOM, INPUT);

  // Aviso si arranca con un límite ya activo
  if (limit_triggered(PIN_LIMIT_TOP))
    Serial.println("AVISO: fin de carrera TOP activo al arrancar");
  if (limit_triggered(PIN_LIMIT_BOTTOM))
    Serial.println("AVISO: fin de carrera BOTTOM activo al arrancar");

  // ToF sensors
  for (int i = 0; i < 5; i++) {
    pinMode(XSHUT_PINS[i], OUTPUT);
    digitalWrite(XSHUT_PINS[i], LOW);
  }
  delay(10);
  for (int i = 0; i < 5; i++) {
    digitalWrite(XSHUT_PINS[i], HIGH);
    delay(10);
    if (!tof[i].begin(SENSOR_ADDRS[i])) {
      char msg[48];
      snprintf(msg, sizeof(msg), "FAIL init %s", SENSOR_NAMES[i]);
      publish_error("TOF_INIT_FAIL", msg);
    } else {
      char msg[48];
      snprintf(msg, sizeof(msg), "%s OK @ 0x%02X", SENSOR_NAMES[i], SENSOR_ADDRS[i]);
      publish_status(msg);
    }
  }

  Serial.println("=== Comandos: u=UP  d=DOWN  s=STOP ===");
  Serial.println("=== Sistema listo ===");
}

// ============================================================
//  CONTROL MANUAL POR SERIAL
// ============================================================

void handle_manual_input(char c) {
  if (c == 'u' || c == 'U') {
    // UP bloqueado si TOP está presionado
    if (limit_triggered(PIN_LIMIT_TOP)) {
      Serial.println(">> BLOQUEADO: TOP activo - solo 'd' o 's'");
      return;
    }
    lift_running = true;
    lift_dir = 1;
    motor_up();
    Serial.println(">> Motor: UP");

  } else if (c == 'd' || c == 'D') {
    // DOWN bloqueado si BOTTOM está presionado
    if (limit_triggered(PIN_LIMIT_BOTTOM)) {
      Serial.println(">> BLOQUEADO: BOTTOM activo - solo 'u' o 's'");
      return;
    }
    lift_running = true;
    lift_dir = -1;
    motor_down();
    Serial.println(">> Motor: DOWN");

  } else if (c == 's' || c == 'S') {
    // STOP siempre funciona
    motor_stop();
    lift_running = false;
    lift_dir = 0;
    Serial.println(">> Motor: STOP");
  }
}

// ============================================================
//  SENSOR PUBLISH
// ============================================================

void publish_sensors() {
  VL53L0X_RangingMeasurementData_t measurements[5];
  for (int i = 0; i < 5; i++) tof[i].rangingTest(&measurements[i], false);

  StaticJsonDocument<512> doc;
  doc["type"] = "sensor_data";

  JsonObject ranges = doc.createNestedObject("ranges");
  for (int i = 0; i < 5; i++) {
    ranges[SENSOR_NAMES[i]] = (measurements[i].RangeStatus != 4)
                              ? measurements[i].RangeMilliMeter
                              : 0;
  }

  JsonObject limits = doc.createNestedObject("limit_switches");
  limits["top"]    = limit_triggered(PIN_LIMIT_TOP);
  limits["bottom"] = limit_triggered(PIN_LIMIT_BOTTOM);

  doc["lift_running"] = lift_running;
  doc["lift_dir"]     = lift_dir;

  serializeJson(doc, Serial);
  Serial.println();
  Serial.flush();
}

// ============================================================
//  LOOP
// ============================================================

void loop() {
  // Leer tecla del Serial Monitor (u / d / s)
  if (Serial.available()) {
    char c = Serial.read();
    handle_manual_input(c);
  }

  // Interlock fines de carrera
  check_limits();

  // Publicar sensores cada 200ms
  unsigned long now = millis();
  if (now - last_publish_ms >= SENSOR_PUBLISH_INTERVAL_MS) {
    last_publish_ms = now;
    publish_sensors();
  }
}