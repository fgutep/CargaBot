from setuptools import find_packages, setup
from glob import glob

package_name = 'lab1_py_pkg'

setup(
    name=package_name,
    version='0.0.0',
    packages=find_packages(exclude=['test']),
    data_files=[
        ('share/ament_index/resource_index/packages',
            ['resource/' + package_name]),
        ('share/' + package_name, ['package.xml']),
        ('share/' + package_name + '/launch', glob('launch/*.launch.py')),

    ],
    install_requires=['setuptools'],
    zip_safe=True,
    maintainer='juan-felipe',
    maintainer_email='juanfelipe.anru@gmail.com',
    description='TODO: Package description',
    license='TODO: License declaration',
    extras_require={
        'test': [
            'pytest',
        ],
    },
    entry_points={
        'console_scripts': [
            "turtle_bot_teleop = lab1_py_pkg.turtle_bot_teleop:main",
            "turtle_bot_interface = lab1_py_pkg.turtle_bot_interface:main",
            "turtle_bot_player = lab1_py_pkg.turtle_bot_player:main",
            'turtle_bot_serial_bridge = lab1_py_pkg.turtle_bot_serial_bridge:main',
            'serial_bridge_zero = lab1_py_pkg.serial_bridge_zero:main',
        ],
    },
)
