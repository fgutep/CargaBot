# TurtleBot ROS2 Lab 1

Proyecto de control y seguimiento de trayectoria de TurtleBot usando ROS2 Humble.

## Descripción

Este proyecto implementa tres nodos de ROS2:
- **turtle_bot_teleop**: Control manual del robot mediante teclado (WASD)
- **turtle_bot_interface**: Interfaz gráfica para visualizar trayectoria y gestionar grabaciones
- **turtle_bot_player**: Reproducción de trayectorias grabadas

## Requisitos

- ROS2 Humble  (pues a la maldita sea con jazzy tambien)
- Python 3.10+
- CoppeliaSim (para simulación)
- Librerías: matplotlib, tkinter

## Instalación
```bash
# Clonar el repositorio
cd ~/ros2_ws/src
git clone https://github.com/TU_USUARIO/turtlebot-ros2-lab1.git

# Instalar dependencias
pip3 install matplotlib --break-system-packages
sudo apt install python3-tk

# Compilar
cd ~/ros2_ws
colcon build
source install/setup.bash
```

## Uso

### Terminal 1: Interfaz gráfica
```bash
ros2 run lab1_py_pkg turtle_bot_interface
```

### Terminal 2: Control por teclado
```bash
ros2 run lab1_py_pkg turtle_bot_teleop
```

### Terminal 3: Reproductor (opcional)
```bash
ros2 run lab1_py_pkg turtle_bot_player
```

## Estructura del Proyecto
```
turtlebot_lab1_project/
├── lab1_msgs/              # Mensajes y servicios personalizados
│   ├── msg/
│   │   └── RobotAction.msg
│   └── srv/
│       └── PlayRecording.srv
└── lab1_py_pkg/            # Nodos de Python
    ├── turtle_bot_teleop.py
    ├── turtle_bot_interface.py
    └── turtle_bot_player.py
```

## Características

- ✅ Control del robot con WASD
- ✅ Visualización de trayectoria en tiempo real
- ✅ Grabación de secuencia de acciones
- ✅ Reproducción de trayectorias grabadas
- ✅ Exportación de gráficas (PNG, PDF, SVG)
- ✅ Mensajes y servicios personalizados

