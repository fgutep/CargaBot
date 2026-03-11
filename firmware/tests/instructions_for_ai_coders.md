# AI Coder Guidelines: ESP32 Differential Drive Motor Base

## 1. System Architecture
* **Microcontroller:** ESP32 (S3/S2/C3 variants).
* **Motor Drivers:** TWO DRV8833 drivers.
* **Configuration:** Paralleled channels (IN1 tied to IN3, IN2 tied to IN4) to double current capacity.
* **Encoders:** Quadrature Hall-effect encoders (3840 CPR).
* **Kinematics:** Differential drive (motors are physically mirrored).



## 2. Pin Mapping Reference
Ensure all generated code strictly adheres to these exact GPIO assignments:

| Component | Pin Function | ESP32 GPIO | Notes |
| :--- | :--- | :--- | :--- |
| **Motor A** | IN1 & IN3 | 9 | - |
| **Motor A** | IN2 & IN4 | 8 | - |
| **Motor B** | IN1 & IN3 | 11 | - |
| **Motor B** | IN2 & IN4 | 10 | - |
| **Driver Ctrl** | EEP (Sleep) | 12 | **MUST** be driven `HIGH` in `setup()` to wake drivers. |
| **Encoder A** | Phase A | 4 | Requires `INPUT_PULLUP` and hardware interrupts. |
| **Encoder A** | Phase B | 5 | Requires `INPUT_PULLUP` and hardware interrupts. |
| **Encoder B** | Phase A | 6 | Requires `INPUT_PULLUP` and hardware interrupts. |
| **Encoder B** | Phase B | 7 | Requires `INPUT_PULLUP` and hardware interrupts. |

## 3. Core Logic Rules & "Gotchas"
When generating C++ (Arduino framework) code for this system, adhere to the following rules:

### A. Driver Enable/Sleep (EEP)
* The EEP pin (`GPIO 12`) controls both DRV8833 chips. If not set to `OUTPUT` and written `HIGH`, the motors will not spin regardless of input signals. 

### B. Differential Drive Inversion
* Because the motors face opposite directions on the chassis, applying identical forward logic to both will cause the robot to spin in place.
* **Software Rule:** Motor B's directional logic must always be inverted relative to Motor A in the motor control function.
    * *Forward A:* IN1/3 = HIGH, IN2/4 = LOW
    * *Forward B:* IN1/3 = LOW, IN2/4 = HIGH

### C. Encoder Handling
* Use a lookup table (`QDEC_LUT`) for rapid, noise-resistant quadrature decoding inside `IRAM_ATTR` interrupt service routines (ISRs).
* Disable interrupts briefly (`noInterrupts()`) when copying volatile encoder counts to local variables in the main loop to prevent race conditions.

### D. PWM Implementation (ESP32 API v3.x)
* Use the modern ESP32 API.
* Do **not** use `ledcSetup` or `ledcAttachPin`.
* Use `ledcAttach(pin, frequency, resolution)` in `setup()`.
* Use `ledcWrite(pin, duty_cycle)` in the control loops.

## 4. Output Formatting Directives
* Keep all generated code and explanations strictly concise. 
* Avoid unnecessary comments; only document complex logic or non-standard API usage.
