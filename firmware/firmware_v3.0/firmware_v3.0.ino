#include <Arduino.h>
#include <ArduinoJson.h>
#include <Wire.h>
#include "I2Cdev.h"
#include "MPU6050_6Axis_MotionApps20.h"

// =====================================================================
// PINES DE HARDWARE
// =====================================================================
#define MOTOR_L_IN1 9
#define MOTOR_L_IN2 8
#define MOTOR_R_IN1 11
#define MOTOR_R_IN2 10
#define MOTOR_EEP   12

#define ENC_L_A 4
#define ENC_L_B 5
#define ENC_R_A 6
#define ENC_R_B 7

#define I2C_SDA_PIN 13
#define I2C_SCL_PIN 14

#define PWM_FREQ 20000
#define PWM_RES  8

// =====================================================================
// PARÁMETROS FÍSICOS
// =====================================================================
const float R      = 0.04f;
const float L      = 0.20f;
const float CPR_X4 = 426.67f;

// =====================================================================
// MPU6050
// =====================================================================
MPU6050 mpu;

bool     dmpReady  = false;
uint16_t packetSize;
uint8_t  fifoBuffer[64];

Quaternion  q;
VectorFloat gravity;
float       ypr[3];

// Yaw del DMP en radianes — referencia absoluta para odometría angular
float imu_yaw     = 0.0f;   // yaw actual [rad]
float imu_yaw_ref = 0.0f;   // yaw al momento del reset de odom
bool  imu_ok      = false;

// =====================================================================
// ENCODERS
// =====================================================================
volatile int32_t countL = 0, countR = 0;
uint8_t prevL = 0, prevR = 0;
static const int8_t QDEC_LUT[16] = {0,-1,1,0,1,0,0,-1,-1,0,0,1,0,1,-1,0};

// =====================================================================
// ESTADO DEL ROBOT
// =====================================================================
float odom_x = 0.0f, odom_y = 0.0f, odom_th = 0.0f;
float target_v = 0.0f, target_w = 0.0f;
float target_rpm_L = 0.0f, target_rpm_R = 0.0f;
float current_rpm_L = 0.0f, current_rpm_R = 0.0f;

// =====================================================================
// PID
// =====================================================================
float kpL = 1.35f, kiL = 2.5f,  kdL = 0.13f;
float kpR = 1.35f, kiR = 2.54f, kdR = 0.21f;

float integral_limit = 150.0f;
float d_filter_alpha = 0.6f;

float ffA_L = 0.71f, ffB_L = 30.0f;
float ffA_R = 0.71f, ffB_R = 30.0f;

float rpm_filter_alpha = 0.3f;
float filtered_rpm_L = 0.0f, filtered_rpm_R = 0.0f;

float err_sum_L = 0, last_err_L = 0, d_filtered_L = 0;
float err_sum_R = 0, last_err_R = 0, d_filtered_R = 0;
int   pwm_L = 0, pwm_R = 0;

// =====================================================================
// MODO DE OPERACIÓN
// =====================================================================
enum Mode { MODE_NORMAL, MODE_STEP, MODE_OPEN };
Mode mode = MODE_NORMAL;

float    step_rpm      = 0.0f;
uint32_t step_duration = 3000;
uint32_t step_start    = 0;
int      open_pwm_L    = 0, open_pwm_R = 0;

// =====================================================================
// TEMPORIZACIÓN
// =====================================================================
uint32_t last_time = 0;
const uint32_t LOOP_DT_MS = 50; // 20 Hz

// =====================================================================
// ISRs
// =====================================================================
void IRAM_ATTR isrL() {
  uint8_t curr = (digitalRead(ENC_L_B) << 1) | digitalRead(ENC_L_A);
  countL += QDEC_LUT[(prevL << 2) | curr];
  prevL = curr;
}
void IRAM_ATTR isrR() {
  uint8_t curr = (digitalRead(ENC_R_A) << 1) | digitalRead(ENC_R_B);
  countR += QDEC_LUT[(prevR << 2) | curr];
  prevR = curr;
}

// =====================================================================
// MOTORES
// =====================================================================
void setMotorPWM(int pwm_l, int pwm_r) {
  pwm_l = constrain(pwm_l, -255, 255);
  pwm_r = constrain(pwm_r, -255, 255);
  ledcWrite(MOTOR_L_IN1, pwm_l > 0 ?  pwm_l : 0);
  ledcWrite(MOTOR_L_IN2, pwm_l < 0 ? -pwm_l : 0);
  ledcWrite(MOTOR_R_IN1, pwm_r < 0 ? -pwm_r : 0);
  ledcWrite(MOTOR_R_IN2, pwm_r > 0 ?  pwm_r : 0);
}

void resetPID() {
  err_sum_L = err_sum_R = 0;
  last_err_L = last_err_R = 0;
  d_filtered_L = d_filtered_R = 0;
  pwm_L = pwm_R = 0;
}

void stopMotors() { setMotorPWM(0, 0); resetPID(); }

// =====================================================================
// PIDs
// =====================================================================
int computePID_L(float target, float measured, float dt) {
  float err    = target - measured;
  err_sum_L   += err * dt;
  err_sum_L    = constrain(err_sum_L, -integral_limit, integral_limit);
  float raw_d  = (err - last_err_L) / dt;
  d_filtered_L = d_filter_alpha * d_filtered_L + (1.0f - d_filter_alpha) * raw_d;
  last_err_L   = err;
  float ff  = (ffA_L * fabsf(target)) + ffB_L;
  float pid = (kpL * err) + (kiL * err_sum_L) + (kdL * d_filtered_L);
  return (int)constrain((target >= 0 ? 1 : -1) * ff + pid, -255, 255);
}

int computePID_R(float target, float measured, float dt) {
  float err    = target - measured;
  err_sum_R   += err * dt;
  err_sum_R    = constrain(err_sum_R, -integral_limit, integral_limit);
  float raw_d  = (err - last_err_R) / dt;
  d_filtered_R = d_filter_alpha * d_filtered_R + (1.0f - d_filter_alpha) * raw_d;
  last_err_R   = err;
  float ff  = (ffA_R * fabsf(target)) + ffB_R;
  float pid = (kpR * err) + (kiR * err_sum_R) + (kdR * d_filtered_R);
  return (int)constrain((target >= 0 ? 1 : -1) * ff + pid, -255, 255);
}

// =====================================================================
// IMU — lee DMP y actualiza imu_yaw
// Llamado cada loop, no bloqueante
// =====================================================================
void updateIMU() {
  if (!dmpReady) return;
  if (!mpu.dmpGetCurrentFIFOPacket(fifoBuffer)) return;

  mpu.dmpGetQuaternion(&q, fifoBuffer);
  mpu.dmpGetGravity(&gravity, &q);
  mpu.dmpGetYawPitchRoll(ypr, &q, &gravity);

  imu_yaw = ypr[0];  // radianes, referencia al norte magnético/gravedad
  imu_ok  = true;
}

// Ángulo de heading relativo al reset de odometría
// Normalizado a [-π, π]
float getOdomTh() {
  if (!imu_ok) return odom_th;  // fallback a encoders si IMU no responde
  float delta = imu_yaw - imu_yaw_ref;
  // Normalizar a [-π, π]
  while (delta >  M_PI) delta -= 2.0f * M_PI;
  while (delta < -M_PI) delta += 2.0f * M_PI;
  return delta;
}

// =====================================================================
// PARSEO JSON
// =====================================================================
void parseCommand(const char* buf) {
  StaticJsonDocument<384> doc;
  if (deserializeJson(doc, buf) != DeserializationError::Ok) return;

  if (doc.containsKey("v") && doc.containsKey("w")) {
    mode     = MODE_NORMAL;
    target_v = -(float)doc["v"];
    target_w = doc["w"];
    float v_l = target_v - (target_w * L / 2.0f);
    float v_r = target_v + (target_w * L / 2.0f);
    target_rpm_L = (v_l / (2.0f * PI * R)) * 60.0f;
    target_rpm_R = (v_r / (2.0f * PI * R)) * 60.0f;
  }

  if (doc.containsKey("kpL")) kpL = doc["kpL"];
  if (doc.containsKey("kiL")) kiL = doc["kiL"];
  if (doc.containsKey("kdL")) kdL = doc["kdL"];
  if (doc.containsKey("kpR")) kpR = doc["kpR"];
  if (doc.containsKey("kiR")) kiR = doc["kiR"];
  if (doc.containsKey("kdR")) kdR = doc["kdR"];
  if (doc.containsKey("il"))  integral_limit   = doc["il"];
  if (doc.containsKey("df"))  d_filter_alpha   = doc["df"];
  if (doc.containsKey("rf"))  rpm_filter_alpha = doc["rf"];
  if (doc.containsKey("ffAL")) ffA_L = doc["ffAL"];
  if (doc.containsKey("ffBL")) ffB_L = doc["ffBL"];
  if (doc.containsKey("ffAR")) ffA_R = doc["ffAR"];
  if (doc.containsKey("ffBR")) ffB_R = doc["ffBR"];

  if (doc.containsKey("step_rpm")) {
    mode          = MODE_STEP;
    step_rpm      = doc["step_rpm"];
    step_duration = doc.containsKey("step_ms") ? (uint32_t)doc["step_ms"] : 3000;
    step_start    = millis();
    const char* wheel = doc["step_wheel"] | "both";
    if      (strcmp(wheel, "L") == 0) { target_rpm_L = step_rpm; target_rpm_R = 0; }
    else if (strcmp(wheel, "R") == 0) { target_rpm_L = 0; target_rpm_R = step_rpm; }
    else                               { target_rpm_L = target_rpm_R = step_rpm; }
    resetPID();
  }

  if (doc.containsKey("open_l") || doc.containsKey("open_r")) {
    mode = MODE_OPEN;
    if (doc.containsKey("open_l")) open_pwm_L = (int)doc["open_l"];
    if (doc.containsKey("open_r")) open_pwm_R = (int)doc["open_r"];
  }

  if (doc.containsKey("stop")) {
    mode = MODE_NORMAL;
    target_v = target_w = 0;
    target_rpm_L = target_rpm_R = 0;
    stopMotors();
  }

  if (doc.containsKey("reset_odom")) {
    // Resetear posición XY a 0
    odom_x = odom_y = 0.0f;
    // Resetear heading: guardar yaw actual como nueva referencia
    imu_yaw_ref = imu_yaw;
    odom_th     = 0.0f;
  }
}

// =====================================================================
// SETUP
// =====================================================================
void setup() {
  Serial.begin(115200);

  // ── Motores ──
  ledcAttach(MOTOR_L_IN1, PWM_FREQ, PWM_RES);
  ledcAttach(MOTOR_L_IN2, PWM_FREQ, PWM_RES);
  ledcAttach(MOTOR_R_IN1, PWM_FREQ, PWM_RES);
  ledcAttach(MOTOR_R_IN2, PWM_FREQ, PWM_RES);
  pinMode(MOTOR_EEP, OUTPUT);
  digitalWrite(MOTOR_EEP, HIGH);

  // ── Encoders ──
  pinMode(ENC_L_A, INPUT_PULLUP); pinMode(ENC_L_B, INPUT_PULLUP);
  pinMode(ENC_R_A, INPUT_PULLUP); pinMode(ENC_R_B, INPUT_PULLUP);
  attachInterrupt(ENC_L_A, isrL, CHANGE); attachInterrupt(ENC_L_B, isrL, CHANGE);
  attachInterrupt(ENC_R_A, isrR, CHANGE); attachInterrupt(ENC_R_B, isrR, CHANGE);

  // ── IMU ──
  Wire.begin(I2C_SDA_PIN, I2C_SCL_PIN);
  Wire.setClock(400000);
  mpu.initialize();

  if (mpu.testConnection()) {
    uint8_t devStatus = mpu.dmpInitialize();
    if (devStatus == 0) {
      // Calibración automática — mantener el robot quieto durante el setup
      mpu.CalibrateAccel(6);
      mpu.CalibrateGyro(6);
      mpu.setDMPEnabled(true);
      dmpReady   = true;
      packetSize = mpu.dmpGetFIFOPacketSize();
    }
    // Si falla el DMP el robot sigue funcionando con encoders para th
  }

  last_time = millis();
}

// =====================================================================
// LOOP
// =====================================================================
void loop() {

  // ── Leer IMU (no bloqueante) ──
  updateIMU();

  // ── Leer serial ──
  static char rxBuf[384];
  static uint16_t rxIdx = 0;
  while (Serial.available()) {
    char c = Serial.read();
    if (c == '\n') {
      rxBuf[rxIdx] = 0;
      parseCommand(rxBuf);
      rxIdx = 0;
    } else if (rxIdx < 383) {
      rxBuf[rxIdx++] = c;
    }
  }

  uint32_t now = millis();
  if (now - last_time < LOOP_DT_MS) return;
  float dt = (now - last_time) / 1000.0f;
  last_time = now;

  // ── Encoders → RPM ──
  int32_t cL, cR;
  noInterrupts();
  cL = countL; countL = 0;
  cR = countR; countR = 0;
  interrupts();

  current_rpm_L = ((float(cL) / dt) / CPR_X4) * 60.0f;
  current_rpm_R = ((float(cR) / dt) / CPR_X4) * 60.0f;

  filtered_rpm_L = rpm_filter_alpha * current_rpm_L + (1.0f - rpm_filter_alpha) * filtered_rpm_L;
  filtered_rpm_R = rpm_filter_alpha * current_rpm_R + (1.0f - rpm_filter_alpha) * filtered_rpm_R;

  // ── Odometría ──
  // Distancia lineal: encoders (más precisos para traslación)
  float v_l_meas = (filtered_rpm_L / 60.0f) * (2.0f * PI * R);
  float v_r_meas = (filtered_rpm_R / 60.0f) * (2.0f * PI * R);
  float v_meas   = (v_r_meas + v_l_meas) / 2.0f;

  // Heading: IMU si disponible, encoders como fallback
  odom_th = getOdomTh();

  // Integración de posición XY con heading del IMU
  odom_x += v_meas * cos(odom_th) * dt;
  odom_y += v_meas * sin(odom_th) * dt;

  // ── Fin de escalón ──
  if (mode == MODE_STEP && (now - step_start > step_duration)) {
    mode = MODE_NORMAL;
    target_rpm_L = target_rpm_R = 0;
    stopMotors();
  }

  // ── Control ──
  if (mode == MODE_OPEN) {
    pwm_L = open_pwm_L;
    pwm_R = open_pwm_R;
    setMotorPWM(open_pwm_L, open_pwm_R);
  } else {
    pwm_L = (target_rpm_L == 0) ? 0 : computePID_L(target_rpm_L, filtered_rpm_L, dt);
    pwm_R = (target_rpm_R == 0) ? 0 : computePID_R(target_rpm_R, filtered_rpm_R, dt);

    if (target_rpm_L == 0) { err_sum_L = 0; d_filtered_L = 0; filtered_rpm_L = 0; }
    if (target_rpm_R == 0) { err_sum_R = 0; d_filtered_R = 0; filtered_rpm_R = 0; }

    setMotorPWM(pwm_L, pwm_R);
  }

  // ── Telemetría ──
  StaticJsonDocument<512> out;
  out["t"]    = now;
  out["rpmL"] = filtered_rpm_L;
  out["rpmR"] = filtered_rpm_R;
  out["rawL"] = current_rpm_L;
  out["rawR"] = current_rpm_R;
  out["tgtL"] = target_rpm_L;
  out["tgtR"] = target_rpm_R;
  out["pwmL"] = pwm_L;
  out["pwmR"] = pwm_R;
  out["x"]    = odom_x;
  out["y"]    = odom_y;
  out["th"]   = odom_th;
  out["yaw"]  = imu_yaw;          // yaw absoluto del IMU [rad]
  out["imu"]  = (int)imu_ok;      // 1 = IMU activo, 0 = fallback encoders
  out["mode"] = (int)mode;
  out["kpL"]  = kpL; out["kiL"] = kiL; out["kdL"] = kdL;
  out["kpR"]  = kpR; out["kiR"] = kiR; out["kdR"] = kdR;
  out["ffAL"] = ffA_L; out["ffBL"] = ffB_L;
  out["ffAR"] = ffA_R; out["ffBR"] = ffB_R;
  serializeJson(out, Serial);
  Serial.println();
}