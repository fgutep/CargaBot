/*
 * esp32_lift_tof.ino
 * Combined VL53L0X (x5) ToF sensor reader + 28BYJ-48 stepper lift controller
 * Serial JSON bridge for ROS2 integration
 *
 * Board: Waveshare ESP32-S3-Zero
 * Protocol: Newline-delimited JSON (NDJSON)
 */

#include <Wire.h>
#include <Adafruit_VL53L0X.h>
#include <Stepper.h>
#include <ArduinoJson.h>

// ============================================================
//  USER CONFIGURATION — edit this section freely
// ============================================================

// --- I2C ---
#define PIN_SDA 7
#define PIN_SCL 6

// --- ToF Sensor XSHUT pins ---
#define XSHUT_1 5
#define XSHUT_2 13
#define XSHUT_3 12
#define XSHUT_4 11
#define XSHUT_5 10

// --- ToF Sensor I2C addresses (must be unique) ---
#define ADDR_1 0x30
#define ADDR_2 0x31
#define ADDR_3 0x32
#define ADDR_4 0x33
#define ADDR_5 0x34

// --- ToF Sensor logical names (ROS2 topic suffix) ---
// Change these to "front", "left", "right", "rear", "top" etc. after placement
const char* SENSOR_NAMES[5] = {
  "sensor_1",
  "sensor_2",
  "sensor_3",
  "sensor_4",
  "sensor_5"
};

// --- Stepper ---
#define STEPS_PER_REV 2048
#define PIN_IN1 4
#define PIN_IN2 3
#define PIN_IN3 2
#define PIN_IN4 1
#define STEPPER_RPM 17

// --- Limit switches (set LIMIT_SWITCHES_ENABLED to false to disable) ---
#define LIMIT_SWITCHES_ENABLED false
#define PIN_LIMIT_TOP    8   // pulled HIGH when depressed
#define PIN_LIMIT_BOTTOM 9   // pulled HIGH when depressed

// --- Publish interval for sensor readings (ms) ---
#define SENSOR_PUBLISH_INTERVAL_MS 200

// ============================================================
//  END USER CONFIGURATION
// ============================================================

// I2C addresses array — parallel to SENSOR_NAMES
const uint8_t SENSOR_ADDRS[5] = { ADDR_1, ADDR_2, ADDR_3, ADDR_4, ADDR_5 };
const uint8_t XSHUT_PINS[5]   = { XSHUT_1, XSHUT_2, XSHUT_3, XSHUT_4, XSHUT_5 };

Adafruit_VL53L0X tof[5];
Stepper stepper(STEPS_PER_REV, PIN_IN1, PIN_IN3, PIN_IN2, PIN_IN4);

bool  lift_running  = false;
int   lift_dir      = 0;      // +1 up, -1 down, 0 stop
unsigned long last_publish_ms = 0;

// ============================================================
//  HELPERS
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
//  SETUP
// ============================================================

void setup() {
  Serial.begin(115200);
  Wire.begin(PIN_SDA, PIN_SCL);

  // --- Init stepper ---
  stepper.setSpeed(STEPPER_RPM);

  // --- Init limit switches ---
  if (LIMIT_SWITCHES_ENABLED) {
    pinMode(PIN_LIMIT_TOP,    INPUT);
    pinMode(PIN_LIMIT_BOTTOM, INPUT);
  }

  // --- Init ToF sensors ---
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
      // Don't halt — keep going so partial systems still work
    } else {
      char msg[48];
      snprintf(msg, sizeof(msg), "%s ready at 0x%02X", SENSOR_NAMES[i], SENSOR_ADDRS[i]);
      publish_status(msg);
    }
  }

  publish_status("esp32_lift_tof ready");
}

// ============================================================
//  SERIAL COMMAND HANDLER
// ============================================================

/*
 * Incoming JSON commands (from ROS2 serial_bridge node):
 *
 * Lift service call:
 *   {"cmd":"lift","action":"up"}
 *   {"cmd":"lift","action":"down"}
 *   {"cmd":"lift","action":"stop"}
 *
 * Config update (runtime rename of a sensor):
 *   {"cmd":"config","sensor_index":0,"name":"front"}
 */

void handle_command(const String& raw) {
  StaticJsonDocument<128> doc;
  DeserializationError err = deserializeJson(doc, raw);
  if (err) {
    publish_error("JSON_PARSE", err.c_str());
    return;
  }

  const char* cmd = doc["cmd"];
  if (!cmd) { publish_error("MISSING_CMD", "no 'cmd' key"); return; }

  // --- Lift command ---
  if (strcmp(cmd, "lift") == 0) {
    const char* action = doc["action"];
    if (!action) { publish_error("MISSING_ACTION", "lift cmd needs 'action'"); return; }

    StaticJsonDocument<128> resp;
    resp["type"] = "service_response";
    resp["cmd"]  = "lift";

    if (strcmp(action, "up") == 0) {
      lift_running = true;  lift_dir = 1;
      resp["result"] = "ok"; resp["action"] = "up";
    } else if (strcmp(action, "down") == 0) {
      lift_running = true;  lift_dir = -1;
      resp["result"] = "ok"; resp["action"] = "down";
    } else if (strcmp(action, "stop") == 0) {
      lift_running = false; lift_dir = 0;
      resp["result"] = "ok"; resp["action"] = "stop";
    } else {
      resp["result"] = "error"; resp["reason"] = "unknown action";
    }
    serializeJson(resp, Serial); Serial.println();

  // --- Config command ---
  } else if (strcmp(cmd, "config") == 0) {
    int idx = doc["sensor_index"] | -1;
    const char* name = doc["name"];
    if (idx < 0 || idx > 4 || !name) {
      publish_error("CONFIG_ERR", "need sensor_index (0-4) and name");
      return;
    }
    SENSOR_NAMES[idx] = name;   // NOTE: points to JSON buffer — use carefully in production
    StaticJsonDocument<128> resp;
    resp["type"]         = "service_response";
    resp["cmd"]          = "config";
    resp["result"]       = "ok";
    resp["sensor_index"] = idx;
    resp["new_name"]     = name;
    serializeJson(resp, Serial); Serial.println();

  } else {
    publish_error("UNKNOWN_CMD", cmd);
  }
}

// ============================================================
//  SENSOR PUBLISH
// ============================================================

void publish_sensors() {
  // Read all sensors first into local vars before touching serial
  VL53L0X_RangingMeasurementData_t measurements[5];
  for (int i = 0; i < 5; i++) {
    tof[i].rangingTest(&measurements[i], false);
  }

  StaticJsonDocument<512> doc;
  doc["type"] = "sensor_data";

  JsonObject ranges = doc.createNestedObject("ranges");
  for (int i = 0; i < 5; i++) {
    if (measurements[i].RangeStatus != 4) {
      ranges[SENSOR_NAMES[i]] = measurements[i].RangeMilliMeter;
    } else {
      ranges[SENSOR_NAMES[i]] = nullptr;  // null = OOB
    }
  }

  if (LIMIT_SWITCHES_ENABLED) {
    JsonObject limits = doc.createNestedObject("limit_switches");
    limits["top"]    = (bool)digitalRead(PIN_LIMIT_TOP);
    limits["bottom"] = (bool)digitalRead(PIN_LIMIT_BOTTOM);
  }

  doc["lift_running"] = lift_running;
  doc["lift_dir"]     = lift_dir;

  serializeJson(doc, Serial);
  Serial.println();
  Serial.flush();  // wait for TX buffer to drain before returning
}

// ============================================================
//  LOOP
// ============================================================

void loop() {
  // --- Handle incoming serial commands ---
  if (Serial.available()) {
    String line = Serial.readStringUntil('\n');
    line.trim();
    if (line.length() > 0) handle_command(line);
  }

  // --- Limit switch safety interlock ---
  if (LIMIT_SWITCHES_ENABLED && lift_running) {
    bool hit_top    = digitalRead(PIN_LIMIT_TOP);
    bool hit_bottom = digitalRead(PIN_LIMIT_BOTTOM);
    if ((lift_dir == 1 && hit_top) || (lift_dir == -1 && hit_bottom)) {
      lift_running = false;
      lift_dir     = 0;
      publish_status("limit_switch_triggered: lift stopped");
    }
  }

  // --- Step motor if running ---
  if (lift_running) {
    stepper.step(lift_dir);
  }

  // --- Publish sensor readings at interval ---
  unsigned long now = millis();
  if (now - last_publish_ms >= SENSOR_PUBLISH_INTERVAL_MS) {
    last_publish_ms = now;
    publish_sensors();
  }
}