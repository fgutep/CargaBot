# CargaBot - Open Source ROS2 Forklift Platform

> **This is an active development project** - Originally started as a TurtleBot Burger clone, this project has evolved into a four-level forklift (montacargas) robot designed for object classification and manipulation.

An open-source, differential drive forklift robot designed to identify, pick up, and classify colored cubes (green, red, blue) using computer vision. By transitioning from a standard mobile base to a forklift architecture, CargaBot provides superior mechanical stability and simplified vertical positioning compared to traditional multi-degree-of-freedom robotic arms.

## Project Goals

- **Accessible**: Built using affordable components like the Raspberry Pi 5, ESP32 microcontrollers, and fully 3D-printable parts.
- **Educational**: A hands-on platform for learning ROS2, computer vision (YOLOv8), embedded systems, and robot kinematics.
- **Object Manipulation**: Features a custom-built elevator mechanism specifically designed for picking up and stacking objects in a laboratory environment.
- **Distributed Processing**: Utilizes remote PC processing for heavy computer vision tasks to maintain real-time operational efficiency on the robot itself.

## Bill of Materials

### Electronics
- **Main Computer**: Raspberry Pi 5 (8GB) - Serves as the primary system processor and runs the ROS2 middleware.
- **Motion Controller**: ESP32-S3 (Dual-core, 240 MHz) - Manages the wheel odometry and traction bridge.
- **Lift & Sensor Controller**: Custom Waveshare ESP32-S3 Zero PCB - Handles $I^{2}C$ multiplexing and elevator control signals.
- **Sensors**: 5x VL53L0X Time-of-Flight sensors - Four provide lateral obstacle avoidance, while the fifth measures the elevator's height.
- **Vision**: Raspberry Pi Camera Module (5 MP, CSI interface) - Oriented downward to capture objects for the vision model.
- **Motors (Traction)**: 2x DC motors with integrated encoders for precise odometry.
- **Motor (Lift)**: 5V Stepper motor for driving the vertical elevator mechanism.
- **Drivers**: Two DRV8833 drivers (configured in parallel to handle higher current) for traction, and a dedicated 5V stepper driver for the lift.

### Mechanical
- **Chassis**: Custom 4-level 3D-printed PLA structure connected by structural support rods for dynamic rigidity.
- **Elevator System**: Driven by a T8 4-start lead screw, supported by upper and lower mechanical limit switches (end-stops) for safe positioning.
- **Wheels**: Custom PLA-printed wheels equipped with TPU treads to significantly improve traction and reduce slipping.

## Key Features & Architecture

### Hardware: The 4-Level Design
- **Level 1 (Base)**: Houses the traction motors, the TPU-treaded wheels, and four VL53L0X ToF sensors pointing forward, backward, left, and right for obstacle avoidance.
- **Level 2 (Electronic Control)**: Contains the main ESP32-S3 board, the parallel DRV8833 motor drivers, and decoupling capacitors.
- **Level 3 (Computation)**: Holds the Raspberry Pi 5 and a vertically positioned fifth VL53L0X sensor used to continuously track the elevator's height without signal interference.
- **Level 4 (Elevator System)**: Accommodates the Pi Camera, the 5V stepper motor driver, and the custom Waveshare ESP32-S3 Zero PCB used for the $I^{2}C$ multiplexing.

### Software
- **ROS2 Middleware**: Relies on a full ROS2 communication suite managing synchronized, distributed nodes.
- **Dual Serial Bridges**: Operates two independent communication bridges; one dedicated to wheel odometry/positioning, and a new "Zero" bridge for exposing the five ToF sensors and the lift state.
- **Distributed YOLOv8 Vision**: Local YOLOv8 inference saturated the Raspberry Pi 5 CPU (reaching 97%), making it impossible to run other nodes simultaneously. To solve this, video is streamed continuously via GStreamer (RTSP protocol) to a desktop computer. The PC runs the YOLOv8 model in real-time and publishes the detection results (class, confidence, and coordinates) back to the robot via ROS2 messages.

## Known Issues & Future Improvements

- **Lift Speed**: The current 5V stepper motor provides precision but is very slow, taking approximately 90 seconds to elevate the platform 15 cm. A planned improvement is to replace this with a more powerful DC motor coupled with gears.
- **Low-Speed PID Control**: The traction PID control has a sensitive minimum PWM threshold and behaves abruptly at low speeds. Upgrading to higher-capacity drivers and further tuning is planned for future iterations.
- **Vision Latency**: While running inference on the PC's GPU is highly efficient, the RTSP video transmission introduces around 200 ms of latency. Future optimizations will explore video compression and hardware acceleration on the Pi 5 to reduce this delay.

## Repository Structure
*(Structure is currently being reorganized to accommodate the new dual-firmware and 4-level chassis files)*

## Getting Started
### Prerequisites
- 3D printer (PLA for chassis components, TPU for wheel treads).
- ROS2 installation (Humble recommended).
- A remote PC with a dedicated GPU for YOLOv8 processing.

*Detailed wiring and build instructions for the 4-level chassis coming soon!*

## Contributing
We welcome contributions! Please open an issue or pull request if you'd like to contribute.

## License
This project is open-source and available under the [MIT License](LICENSE).
