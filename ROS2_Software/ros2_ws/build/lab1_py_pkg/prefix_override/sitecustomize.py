import sys
if sys.prefix == '/usr':
    sys.real_prefix = sys.prefix
    sys.prefix = sys.exec_prefix = '/home/jffg/Desktop/CargaBot/ROS2_Software/ros2_ws/install/lab1_py_pkg'
