// Primer código:
#include <Arduino.h>

// --- Pin Definitions ---
// Motor Driver (LEDC PWM)
#define MOTOR_A_IN1 11
#define MOTOR_A_IN2 10
#define MOTOR_B_IN1 9
#define MOTOR_B_IN2 8

// Encoder Pins
#define ENC_A_PHASE_A 4
#define ENC_A_PHASE_B 5
#define ENC_B_PHASE_A 6
#define ENC_B_PHASE_B 7

// PWM Config
#define PWM_FREQ 20000
#define PWM_RES 8
#define CH_A1 0
#define CH_A2 1
#define CH_B1 2
#define CH_B2 3

// --- Global Variables ---
volatile int32_t countA = 0;
volatile int32_t countB = 0;
uint8_t prevABA = 0;
uint8_t prevABB = 0;

// Quadrature lookup table
static const int8_t QDEC_LUT[16] = {0,-1,1,0,1,0,0,-1,-1,0,0,1,0,1,-1,0};

// Change this to your actual gear motor's total counts per revolution
const float CPR_X4 = 3840.0f; 

// --- Interrupt Service Routines (ISRs) ---
void IRAM_ATTR isrA() {
  uint8_t curr = (digitalRead(ENC_A_PHASE_A) << 1) | digitalRead(ENC_A_PHASE_B);
  countA += QDEC_LUT[(prevABA << 2) | curr];
  prevABA = curr;
}

void IRAM_ATTR isrB() {
  uint8_t curr = (digitalRead(ENC_B_PHASE_A) << 1) | digitalRead(ENC_B_PHASE_B);
  countB += QDEC_LUT[(prevABB << 2) | curr];
  prevABB = curr;
}

// --- Motor Control Helper ---
void setMotorSpeed(int speedA, int speedB) {
  // Motor A
  ledcWrite(CH_A1, (speedA > 0) ? abs(speedA) : 0);
  ledcWrite(CH_A2, (speedA < 0) ? abs(speedA) : 0);
  // Motor B
  ledcWrite(CH_B1, (speedB > 0) ? abs(speedB) : 0);
  ledcWrite(CH_B2, (speedB < 0) ? abs(speedB) : 0);
}

void setup() {
  Serial.begin(115200);

  // Setup PWM
  ledcSetup(CH_A1, PWM_FREQ, PWM_RES);
  ledcSetup(CH_A2, PWM_FREQ, PWM_RES);
  ledcSetup(CH_B1, PWM_FREQ, PWM_RES);
  ledcSetup(CH_B2, PWM_FREQ, PWM_RES);
  ledcAttachPin(MOTOR_A_IN1, CH_A1);
  ledcAttachPin(MOTOR_A_IN2, CH_A2);
  ledcAttachPin(MOTOR_B_IN1, CH_B1);
  ledcAttachPin(MOTOR_B_IN2, CH_B2);

  // Setup Encoders
  pinMode(ENC_A_PHASE_A, INPUT_PULLUP);
  pinMode(ENC_A_PHASE_B, INPUT_PULLUP);
  pinMode(ENC_B_PHASE_A, INPUT_PULLUP);
  pinMode(ENC_B_PHASE_B, INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt(ENC_A_PHASE_A), isrA, CHANGE);
  attachInterrupt(digitalPinToInterrupt(ENC_A_PHASE_B), isrA, CHANGE);
  attachInterrupt(digitalPinToInterrupt(ENC_B_PHASE_A), isrB, CHANGE);
  attachInterrupt(digitalPinToInterrupt(ENC_B_PHASE_B), isrB, CHANGE);

  Serial.println("System Ready. Commands: 'f' (Forward), 'b' (Back), 's' (Stop)");
}

void loop() {
  static uint32_t lastMs = 0;
  static int32_t lastCountA = 0;
  static int32_t lastCountB = 0;

  // 1. Handle Serial Commands
  if (Serial.available()) {
    char cmd = Serial.read();
    if (cmd == 'f') setMotorSpeed(200, 200);
    if (cmd == 'b') setMotorSpeed(-200, -200);
    if (cmd == 's') setMotorSpeed(0, 0);
  }

  // 2. Calculate and Report RPM every 200ms
  uint32_t now = millis();
  if (now - lastMs >= 200) {
    int32_t curA, curB;
    noInterrupts();
    curA = countA;
    curB = countB;
    interrupts();

    float dt = (now - lastMs) / 1000.0f;
    float rpmA = ((curA - lastCountA) / dt / CPR_X4) * 60.0f;
    float rpmB = ((curB - lastCountB) / dt / CPR_X4) * 60.0f;

    Serial.printf("A: %d | RPM: %.2f  ---  B: %d | RPM: %.2f\n", curA, rpmA, curB, rpmB);

    lastCountA = curA;
    lastCountB = curB;
    lastMs = now;
  }
}