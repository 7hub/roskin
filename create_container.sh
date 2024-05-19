#!/bin/bash
# docker run -it --rm -e DISPLAY=host.docker.internal:0.0 --volume="$(PWD):/ros_ws" noetic_openrav
docker run -it --rm -e DISPLAY=host.docker.internal:0.0 --volume="D:/workspace/roskin:/roskin" or_kinetic:openrave

#docker run -it --rm \
#-e DISPLAY=host.docker.internal:0.0 \
#--volume="$(PWD):/ros_ws" \
#noetic_openrave

#docker run -it --rm \
#--net=host \
# --volume="/tmp/.X11-unix:/tmp/.X11-unix:rw" \
#--net=host \
#-p 5000:5001 \
#--gpus all \
#-env="DISPLAY=host.docker.internal:0.0" \
#--env="QT_X11_NO_MITSHM=1" \
#--volume="/tmp/.X11-unix:/tmp/.X11-unix:rw" \
#ros-kinetic \
#/bin/bash

#-v D:\\workspace\\ros-kinetic:/ros_ws \
# --user ros \
# -v "$(pwd)"\\src:/ros_ws \
# -v $PWD/intstall:/home/ros_ws/install \
# --env="DISPLAY" \
# --env="QT_X11_NO_MITSHM=1" \
# --volume="/tmp/.X11-unix:/tmp/.X11-unix:rw" \
# --volume="/etc/localtime:/etc/localtime:ro" \
# --volume="/home/turtlebot3/ros2_ws:/home/turtlebot3/ros2_ws" \
