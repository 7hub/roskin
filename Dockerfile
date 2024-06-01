FROM or_kinetic:openrave
# FROM osrf/ros:kinetic-desktop-full-xenial
# Update ROS keys (https://discourse.ros.org/t/new-gpg-keys-deployed-for-packages-ros-org/9454, https://github.com/osrf/docker_images/issues/697)

RUN mv /etc/apt/sources.list /etc/apt/sources_backup.list && \
    echo "deb http://mirrors.aliyun.com/ubuntu/ xenial main restricted universe multiverse" >> //etc/apt/sources.list && \
    echo "deb-src http://mirrors.aliyun.com/ubuntu/ xenial main restricted universe multiverse" >> //etc/apt/sources.list && \
    echo "deb http://mirrors.aliyun.com/ubuntu/ xenial-security main restricted universe multiverse" >> //etc/apt/sources.list && \
    echo "deb-src http://mirrors.aliyun.com/ubuntu/ xenial-security main restricted universe multiverse" >> //etc/apt/sources.list && \
    echo "deb http://mirrors.aliyun.com/ubuntu/ xenial-updates main restricted universe multiverse" >> //etc/apt/sources.list && \
    echo "deb-src http://mirrors.aliyun.com/ubuntu/ xenial-updates main restricted universe multiverse" >> //etc/apt/sources.list && \
    echo "deb http://mirrors.aliyun.com/ubuntu/ xenial-proposed main restricted universe multiverse" >> //etc/apt/sources.list && \
    echo "deb-src http://mirrors.aliyun.com/ubuntu/ xenial-proposed main restricted universe multiverse" >> //etc/apt/sources.list && \
    echo "deb http://mirrors.aliyun.com/ubuntu/ xenial-backports main restricted universe multiverse" >> //etc/apt/sources.list && \
    echo "deb-src http://mirrors.aliyun.com/ubuntu/ xenial-backports main restricted universe multiverse" >> //etc/apt/sources.list

# RUN apt-key adv --keyserver 'hkp://keyserver.ubuntu.com:80' --recv-key 4B63CF8FDE49746E98FA01DDAD19BAB3CBF125EA && \
#     apt-key del 421C365BD9FF1F717815A3895523BAEEB01FA116 && \
RUN apt-get update && \
    apt-get install -y --no-install-recommends \
    python3-pip \
    build-essential \
    cmake \
    git \
    vim \
    liblapack-dev \
    libboost-all-dev 

RUN apt-get clean && rm -rf /var/lib/apt/lists/*
#
# enforce a specific version of sympy, which is known to work with OpenRave

# RUN source /opt/ros/noetic/setup.bash

RUN mkdir /catkin_ws/src -p
RUN cd /catkin_ws/src
#RUN catkin_init_workspace 

ARG WORKDIR_PATH=/catkin_ws

WORKDIR ${WORKDIR_PATH}



CMD [ "bash" ]

