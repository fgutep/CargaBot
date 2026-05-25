#include "I2Cdev.h"
#include "MPU6050_6Axis_MotionApps20.h"
#include <Wire.h>

// Define your custom I2C pins for the ESP32-S3
#define I2C_SDA_PIN 15
#define I2C_SCL_PIN 13

// Initialize the MPU object
MPU6050 mpu;

// MPU control/status vars
bool dmpReady = false;  
uint8_t devStatus;      
uint16_t packetSize;    
uint8_t fifoBuffer[64]; 

// Orientation vars
Quaternion q;           // [w, x, y, z] quaternion container
VectorFloat gravity;    // [x, y, z] gravity vector
float ypr[3];           // [yaw, pitch, roll] yaw/pitch/roll container

void setup() {
    Serial.begin(115200);
    while (!Serial);

    // 1. Initialize I2C Bus with your specific S3 pins
    Wire.begin(I2C_SDA_PIN, I2C_SCL_PIN);
    Wire.setClock(400000); // 400kHz I2C clock 

    Serial.println(F("Initializing MPU6050..."));
    mpu.initialize();

    if (!mpu.testConnection()) {
        Serial.println(F("MPU6050 connection failed! Check wiring."));
        while (1); // Halt execution if sensor isn't found
    }
    Serial.println(F("MPU6050 connection successful."));

    // 2. Initialize DMP
    Serial.println(F("Initializing DMP..."));
    devStatus = mpu.dmpInitialize();

    // 3. Auto-Calibration (DO NOT MOVE THE SENSOR DURING THIS)
    if (devStatus == 0) {
        Serial.println(F("Calibrating internal offsets... Keep perfectly still!"));
        
        mpu.CalibrateAccel(6); 
        mpu.CalibrateGyro(6);
        mpu.PrintActiveOffsets();

        Serial.println(F("Enabling DMP..."));
        mpu.setDMPEnabled(true);

        dmpReady = true;
        packetSize = mpu.dmpGetFIFOPacketSize();
        
        Serial.println(F("DMP ready! Tracking Yaw relative to startup position."));
    } else {
        Serial.print(F("DMP Initialization failed (code "));
        Serial.print(devStatus);
        Serial.println(F(")"));
    }
}

void loop() {
    if (!dmpReady) return;

    // Read the FIFO buffer
    if (mpu.dmpGetCurrentFIFOPacket(fifoBuffer)) {
        
        // Calculate Quaternions and derive Yaw
        mpu.dmpGetQuaternion(&q, fifoBuffer);
        mpu.dmpGetGravity(&gravity, &q);
        mpu.dmpGetYawPitchRoll(ypr, &q, &gravity);

        // Convert Radians to Degrees
        float yaw = ypr[0] * 180 / M_PI;

        Serial.print("Yaw: ");
        Serial.print(yaw);
        Serial.println("°");
    }
}