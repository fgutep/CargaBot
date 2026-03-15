# ROS 2 Robot Setup & Operation Guide

This guide contains the essential commands used for building the ROS 2 workspace, configuring hardware access, running nodes, connecting to the robot over the network, and visualizing data using Foxglove Studio.

---

# 1. ROS 2 Workspace & Building

These commands clean up bad CMake caches, build your packages, and source the environment so ROS 2 knows your nodes exist.

## Navigate to your workspace

```bash
cd ~/Desktop/Robotica_taller1_grupo_6/ros2_ws
```

## Clean old build files  
*(Crucial after cloning from GitHub)*

```bash
rm -rf build/ install/ log/
```

## Source the main ROS 2 Jazzy installation

```bash
source /opt/ros/jazzy/setup.bash
```

## Build the workspace

```bash
colcon build
```

**Tip**

You can build only the Python package and avoid rebuilding everything:

```bash
colcon build --packages-select lab1_py_pkg --symlink-install
```

Using `--symlink-install` prevents rebuilding every time a `.py` file changes.

## Source your local workspace

Run this **every time you open a new terminal**:

```bash
source install/setup.bash
```

---

# 2. Hardware: Finding & Fixing the USB Port

Linux can be strict with USB serial devices. These commands help you find the ESP32 and grant your user permission to communicate with it.

## Find the ESP32 port immediately after plugging it in

```bash
dmesg | tail -n 20
```

Look for something like:

```
/dev/ttyACM0
```

## List all active serial devices

```bash
ls /dev/ttyUSB* /dev/ttyACM*
```

## Permanently give your user serial port access

```bash
sudo usermod -a -G dialout $USER
```

## Apply the new group permissions immediately (without rebooting)

```bash
newgrp dialout
```

## Temporary permission fix (if `newgrp` fails)

```bash
sudo chmod 666 /dev/ttyACM0
```

---

# 3. Running Your ROS 2 Nodes

Commands to start the core functionality of the robot.

## Run the ESP32 Serial Bridge

```bash
ros2 run lab1_py_pkg turtle_bot_serial_bridge --ros-args -p port:=/dev/ttyACM0
```

You will also run additional nodes in **separate terminals**, for example:

```bash
ros2 run lab1_py_pkg turtle_bot_teleop
```

```bash
ros2 run lab1_py_pkg turtle_bot_interface
```

Make sure each terminal has the workspace sourced before running nodes.

---

# 4. Networking (Wi-Fi & SSH)

Commands used to connect the Raspberry Pi to Wi-Fi and access it remotely.

## Scan for available Wi-Fi networks

```bash
nmcli dev wifi list
```

## Connect the robot to Wi-Fi

```bash
sudo nmcli dev wifi connect "YOUR_WIFI_NAME" password "YOUR_PASSWORD"
```

## Check the robot's IP address

```bash
ip a
```

## Connect to the robot from your laptop

```bash
ssh jffg@<ROBOT_IP>
```

This can be done through:

- Standard terminal
- VS Code Remote SSH

---

# 5. Foxglove Studio Integration

These commands allow streaming of live **Odometry** and **JSON data** to Foxglove Studio.

## Install Foxglove Bridge for ROS 2 Jazzy

```bash
sudo apt update
sudo apt install ros-jazzy-foxglove-bridge
```

## Run the Foxglove Bridge

```bash
ros2 run foxglove_bridge foxglove_bridge --ros-args -p port:=8765
```

Foxglove Studio can now connect to the robot and visualize live ROS 2 data streams.

---
