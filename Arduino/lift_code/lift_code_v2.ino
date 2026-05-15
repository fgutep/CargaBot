/*
 * esp32_lift_tof.ino
 * VL53L0X (x5) + DC motor lift + limit switches
 * Serial JSON bridge for ROS2 integration
 * Board: Waveshare ESP32-S3-Zero
 * Protocol: Newline-delimited JSON (NDJSON)
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
#define MOTOR_SPEED  200   // 0-255

// --- Fines de carrera (pull-down: 10k a GND, switch a 3.3V) ---
#define PIN_LIMIT_TOP    8
#define PIN_LIMIT_BOTTOM 9
#define LIMIT_TRIGGERED  HIGH

// --- Intervalo de publicación sensores ---
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
//  1. HELPERS JSON  (van primero — los usan las demás funciones)
// ============================================================

void publish_error(const char* code, const char* msg) {
  StaticJsonDocument<128> doc;
  doc["type"]    = "error";
  doc["code"]    = code;
  doc["message"] = msg;
  serializeJson(doc, Serial);
  Serial.println();
}

void publish_status(const char* msg) {
  StaticJsonDocument<128> doc;
  doc["type"]    = "status";
  doc["message"] = msg;
  serializeJson(doc, Serial);
  Serial.println();
}

// ============================================================
//  2. MOTOR
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
//  3. FINES DE CARRERA
// ============================================================

bool limit_triggered(uint8_t pin) {
  return digitalRead(pin) == LIMIT_TRIGGERED;
}

void check_limits() {
  if (!lift_running) return;

  if (lift_dir == 1 && limit_triggered(PIN_LIMIT_TOP)) {
    motor_stop();
    lift_running = false;
    lift_dir     = 0;
    publish_status("limit_switch_triggered: TOP - lift stopped");
  }
  else if (lift_dir == -1 && limit_triggered(PIN_LIMIT_BOTTOM)) {
    motor_stop();
    lift_running = false;
    lift_dir     = 0;
    publish_status("limit_switch_triggered: BOTTOM - lift stopped");
  }
}

// ============================================================
//  4. SETUP
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

  if (limit_triggered(PIN_LIMIT_TOP))
    publish_status("WARNING: top limit activo al arrancar");
  if (limit_triggered(PIN_LIMIT_BOTTOM))
    publish_status("WARNING: bottom limit activo al arrancar");

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
      snprintf(msg, sizeof(msg), "Failed to init %s", SENSOR_NAMES[i]);
      publish_error("TOF_INIT_FAIL", msg);
    } else {
      char msg[48];
      snprintf(msg, sizeof(msg), "%s ready at 0x%02X", SENSOR_NAMES[i], SENSOR_ADDRS[i]);
      publish_status(msg);
    }
  }

  publish_status("esp32_lift_tof ready");
}

// ============================================================
//  5. SERIAL COMMAND HANDLER (ROS2)
// ============================================================

/*
 * Comandos JSON desde ROS2:
 *   {"cmd":"lift","action":"up"}
 *   {"cmd":"lift","action":"down"}
 *   {"cmd":"lift","action":"stop"}
 *   {"cmd":"config","sensor_index":0,"name":"front"}
 */

void handle_command(const String& raw) {
  StaticJsonDocument<128> doc;
  DeserializationError err = deserializeJson(doc, raw);
  if (err) { publish_error("JSON_PARSE", err.c_str()); return; }

  const char* cmd = doc["cmd"];
  if (!cmd) { publish_error("MISSING_CMD", "no 'cmd' key"); return; }

  // --- Lift ---
  if (strcmp(cmd, "lift") == 0) {
    const char* action = doc["action"];
    if (!action) { publish_error("MISSING_ACTION", "lift cmd needs 'action'"); return; }

    StaticJsonDocument<128> resp;
    resp["type"] = "service_response";
    resp["cmd"]  = "lift";

    if (strcmp(action, "up") == 0) {
      if (limit_triggered(PIN_LIMIT_TOP)) {
        resp["result"] = "error";
        resp["reason"] = "top limit active";
      } else {
        lift_running = true;
        lift_dir     = 1;
        motor_up();
        resp["result"] = "ok";
        resp["action"] = "up";
      }
    } else if (strcmp(action, "down") == 0) {
      if (limit_triggered(PIN_LIMIT_BOTTOM)) {
        resp["result"] = "error";
        resp["reason"] = "bottom limit active";
      } else {
        lift_running = true;
        lift_dir     = -1;
        motor_down();
        resp["result"] = "ok";
        resp["action"] = "down";
      }
    } else if (strcmp(action, "stop") == 0) {
      motor_stop();
      lift_running = false;
      lift_dir     = 0;
      resp["result"] = "ok";
      resp["action"] = "stop";
    } else {
      resp["result"] = "error";
      resp["reason"] = "unknown action";
    }

    serializeJson(resp, Serial);
    Serial.println();

  // --- Config ---
  } else if (strcmp(cmd, "config") == 0) {
    int idx          = doc["sensor_index"] | -1;
    const char* name = doc["name"];
    if (idx < 0 || idx > 4 || !name) {
      publish_error("CONFIG_ERR", "need sensor_index (0-4) and name");
      return;
    }
    SENSOR_NAMES[idx] = name;
    StaticJsonDocument<128> resp;
    resp["type"]         = "service_response";
    resp["cmd"]          = "config";
    resp["result"]       = "ok";
    resp["sensor_index"] = idx;
    resp["new_name"]     = name;
    serializeJson(resp, Serial);
    Serial.println();

  } else {
    publish_error("UNKNOWN_CMD", cmd);
  }
}

// ============================================================
//  6. SENSOR PUBLISH
// ============================================================

void publish_sensors() {
  VL53L0X_RangingMeasurementData_t measurements[5];
  for (int i = 0; i < 5; i++) tof[i].rangingTest(&measurements[i], false);

  StaticJsonDocument<512> doc;
  doc["type"] = "sensor_data";

  JsonObject ranges = doc.createNestedObject("ranges");
  for (int i = 0; i < 5; i++) {
    if (measurements[i].RangeStatus != 4) {
      ranges[SENSOR_NAMES[i]] = measurements[i].RangeMilliMeter;
    } else {
      ranges[SENSOR_NAMES[i]] = nullptr;  // null = fuera de rango
    }
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
//  7. LOOP
// ============================================================

void loop() {
  // Comandos entrantes desde ROS2
  if (Serial.available()) {
    String line = Serial.readStringUntil('\n');
    line.trim();
    if (line.length() > 0) handle_command(line);
  }

  // Interlock fines de carrera — frena si toca un límite
  check_limits();

  // Publicar sensores cada 200ms
  unsigned long now = millis();
  if (now - last_publish_ms >= SENSOR_PUBLISH_INTERVAL_MS) {
    last_publish_ms = now;
    publish_sensors();
  }
}