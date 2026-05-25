#include <Arduino.h>
#include <ArduinoJson.h>

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

#define PWM_FREQ 20000
#define PWM_RES  8

// =====================================================================
// PARÁMETROS FÍSICOS
// =====================================================================
const float R      = 0.04f;
const float L      = 0.20f;
const float CPR_X4 = 426.67f; // Medido: 9 vueltas físicas = 1 vuelta firmware (3840/9)

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
// PID — GANANCIAS INDEPENDIENTES POR RUEDA
// =====================================================================
float kpL = 2.0f, kiL = 0.5f, kdL = 0.1f;
float kpR = 2.0f, kiR = 0.5f, kdR = 0.1f;

float integral_limit = 150.0f;
float d_filter_alpha = 0.15f;

// Feedforward: pwm_ff = ffA * |target_rpm| + ffB  (siempre positivo, signo lo da el target)
// Calibrar con modo open: anota PWM→RPM y manda {"ffAL": x, "ffBL": y} por serial
// Defaults: ajusta ffB_L/R al PWM mínimo donde el motor arranca
float ffA_L = 0.5f, ffB_L = 30.0f;
float ffA_R = 0.5f, ffB_R = 30.0f;

// Filtro EMA para suavizar RPM medido (reduce ruido de cuantización del encoder)
// alpha bajo = más suave pero más lento; 0.3 es buen punto de partida
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

int open_pwm_L = 0, open_pwm_R = 0;

// =====================================================================
// TEMPORIZACIÓN
// =====================================================================
uint32_t last_time = 0;
const uint32_t LOOP_DT_MS = 50; // 20 Hz — más counts por ciclo, menos ruido en RPM

// =====================================================================
// ISRs
// =====================================================================
void IRAM_ATTR isrL() {
  uint8_t curr = (digitalRead(ENC_L_B) << 1) | digitalRead(ENC_L_A); // A/B swapped to fix sign
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
// PIDs POSICIONALES INDEPENDIENTES
// =====================================================================
int computePID_L(float target, float measured, float dt) {
  float err    = target - measured;
  err_sum_L   += err * dt;
  err_sum_L    = constrain(err_sum_L, -integral_limit, integral_limit);
  float raw_d  = (err - last_err_L) / dt;
  d_filtered_L = d_filter_alpha * d_filtered_L + (1.0f - d_filter_alpha) * raw_d;
  last_err_L   = err;
  float ff     = (ffA_L * fabsf(target)) + ffB_L;          // feedforward base
  float pid    = (kpL * err) + (kiL * err_sum_L) + (kdL * d_filtered_L);
  float out    = (target >= 0 ? 1 : -1) * ff + pid;        // ff sigue signo del target
  return (int)constrain(out, -255, 255);
}

int computePID_R(float target, float measured, float dt) {
  float err    = target - measured;
  err_sum_R   += err * dt;
  err_sum_R    = constrain(err_sum_R, -integral_limit, integral_limit);
  float raw_d  = (err - last_err_R) / dt;
  d_filtered_R = d_filter_alpha * d_filtered_R + (1.0f - d_filter_alpha) * raw_d;
  last_err_R   = err;
  float ff     = (ffA_R * fabsf(target)) + ffB_R;
  float pid    = (kpR * err) + (kiR * err_sum_R) + (kdR * d_filtered_R);
  float out    = (target >= 0 ? 1 : -1) * ff + pid;
  return (int)constrain(out, -255, 255);
}

// =====================================================================
// PARSEO DE COMANDOS JSON
// Protocolo:
//   Velocidad:       {"v": 0.2, "w": 0.0}
//   Ganancias L:     {"kpL": 3.0, "kiL": 0.3, "kdL": 0.05}
//   Ganancias R:     {"kpR": 2.5, "kiR": 0.3, "kdR": 0.05}
//   Escalon ambas:   {"step_rpm": 60, "step_ms": 4000}
//   Escalon solo L:  {"step_rpm": 60, "step_ms": 4000, "step_wheel": "L"}
//   Escalon solo R:  {"step_rpm": 60, "step_ms": 4000, "step_wheel": "R"}
//   Lazo abierto:    {"open_l": 100, "open_r": 100}
//   Stop:            {"stop": 1}
//   Reset odom:      {"reset_odom": 1}
//   Anti-windup:     {"il": 150}
//   Filtro deriv:    {"df": 0.15}
// =====================================================================
void parseCommand(const char* buf) {
  StaticJsonDocument<384> doc;
  if (deserializeJson(doc, buf) != DeserializationError::Ok) return;

  if (doc.containsKey("v") && doc.containsKey("w")) {
    mode     = MODE_NORMAL;
    target_v = doc["v"];
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
  if (doc.containsKey("il"))  integral_limit    = doc["il"];
  if (doc.containsKey("df"))  d_filter_alpha    = doc["df"];
  if (doc.containsKey("rf"))  rpm_filter_alpha  = doc["rf"]; // RPM filter alpha

  // Feedforward: {"ffAL": 0.5, "ffBL": 30, "ffAR": 0.5, "ffBR": 30}
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
    if (strcmp(wheel, "L") == 0) {
      target_rpm_L = step_rpm; target_rpm_R = 0;
    } else if (strcmp(wheel, "R") == 0) {
      target_rpm_L = 0; target_rpm_R = step_rpm;
    } else {
      target_rpm_L = target_rpm_R = step_rpm;
    }
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
    odom_x = odom_y = odom_th = 0;
  }
}

// =====================================================================
// SETUP
// =====================================================================
void setup() {
  Serial.begin(115200);

  ledcAttach(MOTOR_L_IN1, PWM_FREQ, PWM_RES);
  ledcAttach(MOTOR_L_IN2, PWM_FREQ, PWM_RES);
  ledcAttach(MOTOR_R_IN1, PWM_FREQ, PWM_RES);
  ledcAttach(MOTOR_R_IN2, PWM_FREQ, PWM_RES);

  pinMode(MOTOR_EEP, OUTPUT);
  digitalWrite(MOTOR_EEP, HIGH);

  pinMode(ENC_L_A, INPUT_PULLUP); pinMode(ENC_L_B, INPUT_PULLUP);
  pinMode(ENC_R_A, INPUT_PULLUP); pinMode(ENC_R_B, INPUT_PULLUP);

  attachInterrupt(ENC_L_A, isrL, CHANGE); attachInterrupt(ENC_L_B, isrL, CHANGE);
  attachInterrupt(ENC_R_A, isrR, CHANGE); attachInterrupt(ENC_R_B, isrR, CHANGE);

  last_time = millis();
}

// =====================================================================
// LOOP
// =====================================================================
void loop() {

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

  int32_t cL, cR;
  noInterrupts();
  cL = countL; countL = 0;
  cR = countR; countR = 0;
  interrupts();

  // Cast explícito a float antes de dividir para evitar truncamiento entero
  current_rpm_L = ((float(cL) / dt) / CPR_X4) * 60.0f;
  current_rpm_R = ((float(cR) / dt) / CPR_X4) * 60.0f;

  // Filtro EMA sobre RPM — suaviza saltos de cuantización del encoder
  filtered_rpm_L = rpm_filter_alpha * current_rpm_L + (1.0f - rpm_filter_alpha) * filtered_rpm_L;
  filtered_rpm_R = rpm_filter_alpha * current_rpm_R + (1.0f - rpm_filter_alpha) * filtered_rpm_R;

  // Odometría usa RPM filtrado
  float v_l_meas = (filtered_rpm_L / 60.0f) * (2.0f * PI * R);
  float v_r_meas = (filtered_rpm_R / 60.0f) * (2.0f * PI * R);
  float v_meas   = (v_r_meas + v_l_meas) / 2.0f;
  float w_meas   = (v_r_meas - v_l_meas) / L;
  odom_th += w_meas * dt;
  odom_x  += v_meas * cos(odom_th) * dt;
  odom_y  += v_meas * sin(odom_th) * dt;

  if (mode == MODE_STEP && (now - step_start > step_duration)) {
    mode = MODE_NORMAL;
    target_rpm_L = target_rpm_R = 0;
    stopMotors();
  }

  if (mode == MODE_OPEN) {
    pwm_L = open_pwm_L;  // para telemetría correcta
    pwm_R = open_pwm_R;
    setMotorPWM(open_pwm_L, open_pwm_R);
  } else {
    pwm_L = (target_rpm_L == 0) ? 0 : computePID_L(target_rpm_L, filtered_rpm_L, dt);
    pwm_R = (target_rpm_R == 0) ? 0 : computePID_R(target_rpm_R, filtered_rpm_R, dt);

    if (target_rpm_L == 0) { err_sum_L = 0; d_filtered_L = 0; filtered_rpm_L = 0; }
    if (target_rpm_R == 0) { err_sum_R = 0; d_filtered_R = 0; filtered_rpm_R = 0; }

    setMotorPWM(pwm_L, pwm_R);
  }

  StaticJsonDocument<512> out;
  out["t"]    = now;
  out["rpmL"] = filtered_rpm_L;   // lo que ve el PID
  out["rpmR"] = filtered_rpm_R;
  out["rawL"] = current_rpm_L;    // medición cruda del encoder
  out["rawR"] = current_rpm_R;
  out["tgtL"] = target_rpm_L;
  out["tgtR"] = target_rpm_R;
  out["pwmL"] = pwm_L;
  out["pwmR"] = pwm_R;
  out["x"]    = odom_x;
  out["y"]    = odom_y;
  out["th"]   = odom_th;
  out["mode"] = (int)mode;
  out["kpL"]  = kpL; out["kiL"] = kiL; out["kdL"] = kdL;
  out["kpR"]  = kpR; out["kiR"] = kiR; out["kdR"] = kdR;
  out["ffAL"] = ffA_L; out["ffBL"] = ffB_L;
  out["ffAR"] = ffA_R; out["ffBR"] = ffB_R;
  serializeJson(out, Serial);
  Serial.println();
}