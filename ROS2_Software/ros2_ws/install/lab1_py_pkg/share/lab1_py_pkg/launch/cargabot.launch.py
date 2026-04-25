from launch import LaunchDescription
from launch_ros.actions import Node

def generate_launch_description():
    return LaunchDescription([

        Node(
            package='lab1_py_pkg',
            executable='turtle_bot_serial_bridge',
            name='serial_bridge',
            output='screen',
            parameters=[{
                'port': '/dev/ttyACM1',  # CH340
            }]
        ),

        Node(
            package='lab1_py_pkg',
            executable='serial_bridge_zero',
            name='serial_bridge_zero',
            output='screen',
            parameters=[{
                'serial_port': '/dev/ttyACM0',  # ESP32
            }]
        ),

    ])
