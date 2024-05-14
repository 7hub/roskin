FROM osrf/ros:kinetic-desktop-full

#RUN mv /etc/apt/sources.list /etc/apt/sources_backup.list && \
#
## 默认注释了源码镜像以提高 apt update 速度，如有需要可自行取消注释
#echo "deb https://mirrors.tuna.tsinghua.edu.cn/ubuntu/ xenial main restricted universe multiverse" >> /etc/apt/sources.list && \
#echo "deb-src https://mirrors.tuna.tsinghua.edu.cn/ubuntu/ xenial main restricted universe multiverse" >> /etc/apt/sources.list && \
#echo "deb https://mirrors.tuna.tsinghua.edu.cn/ubuntu/ xenial-updates main restricted universe multiverse" >> /etc/apt/sources.list && \
## deb-src https://mirrors.tuna.tsinghua.edu.cn/ubuntu/ xenial-updates main restricted universe multiverse
#echo "deb https://mirrors.tuna.tsinghua.edu.cn/ubuntu/ xenial-backports main restricted universe multiverse" >> /etc/apt/sources.list && \
#echo "deb http://security.ubuntu.com/ubuntu/ xenial-security main restricted universe multiverse">> /etc/apt/sources.list 

# Install dependencies
RUN apt-get update && apt-get install -y \
    build-essential \
    #python3-rosdep \
    #python3-colcon-common-extensions \
    && rm -rf /var/lib/apt/lists/*
    

ARG USERNAME=ros_user
ARG USER_UID=1000
ARG USER_GID=$USER_UID

ARG WORKSPACE=/home/$USERNAME


# # Initialize rosdep
# RUN rosdep init && rosdep update

RUN groupadd --gid $USER_GID $USERNAME \
    && useradd -s /bin/bash --uid $USER_UID --gid $USER_GID -m $USERNAME \
    && mkdir /home/$USERNAME/.config && chown $USER_UID:$USER_GID /home/$USERNAME/.config \
    && echo "source /opt/ros/kinetic/setup.bash" >> /home/$USERNAME/.bashrc 

# Add user to sudoers
RUN echo "$USERNAME ALL=(ALL) NOPASSWD: ALL" >> /etc/sudoers



# # Set environment variables
# ENV LANG=C.UTF-8
# ENV LC_ALL=C.UTF-8

# Create a workspace directory
WORKDIR /$WORKSPACE

# create the C++ package
# RUN mkdir src && \
#     . /opt/ros/kinetic/setup.sh && \
#     cd src && \
#     ros2 pkg create --build-type ament_cmake ${PACKAGE_NAME}

# Set environment variables
# ENV ROS_DOMAIN_ID=1
# ENV ROS_MASTER_URI=http://localhost:11311
# ENV ROS_IP=0.0.0.0
# ENV ROS_HOSTNAME=0.0.0.0
# ENV ROS_LOG_DIR=/tmp/ros2_ws/log


#COPY entrypoint.sh /entrypoint.sh

#ENTRYPOINT [ "/bin/bash","/entrypoint.sh" ]

CMD ["bash"]
