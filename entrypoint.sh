#!/bin/bash

set -e

source /opt/ros/kinetic/setup.bash

cd /home/
#colcon build

source install/setup.bash

# ros2 launch turtlesim  multiturtle_launch.py

echo "Provided arguments :$@"
exec "$@"
