# Staubli experimental

Experimental packages for Staubli manipulators within [ROS-Industrial][].
See the [ROS wiki][] page for more information.

## VAL 3 driver

Note: the `staubli_val3_driver` package has been split off into its own repository (in [staubli_experimental#47](https://github.com/ros-industrial/staubli_experimental/pull/47)). Its new location is: [ros-industrial/staubli_val3_driver](https://github.com/ros-industrial/staubli_val3_driver).

## Contents

This repository contains packages that will be migrated to the [staubli][]
repository after they have received sufficient testing. The contents of
these packages are subject to change, without prior notice. Any available
APIs are to be considered unstable and are not guaranteed to be complete
and / or functional.

[ROS-Industrial]: http://wiki.ros.org/Industrial
[ROS wiki]: http://wiki.ros.org/staubli_experimental
[staubli]: https://github.com/ros-industrial/staubli

################################################################

# Ubuntu xenial （16.04）安装 Openrave

```sh
cd  # go home 
git clone https://github.com/7hub/openrave-installation 
cd openrave-installation 
./install-dependencies.sh 
./install-osg.sh 
./install-fcl.sh 
./install-openrave.sh
```
 
 ## Docker:
 ```docker
docker run -it --rm -e DISPLAY=host.docker.internal:0.0 --volume="D:/workspace/roskin:/roskin" or_kinetic:openrave
 ```

################################################################

# 生成 `IkFast*.cpp`

## 依赖：

> - ros kinetic
> - openrave
> - lapack

* 设置环境变量

```sh
#!/bin/bash
export MYROBOT_NAME="tx2_60l"
export IKFAST_PRECISION="5"
export PLANNING_GROUP="tx2_60l_arm"
export BASE_LINK="0"
export EEF_LINK="9"
export IKFAST_OUTPUT_PATH=`pwd`/ikfast61_"$PLANNING_GROUP".cpp
```

## 1. 编辑xacro文件

## 2. 生成urdf文件

```sh
cd src/staubli_experimental/staubli_tx2_60_support/urdf
rosrun xacro xacro --inorder -o "$MYROBOT_NAME".urdf "$MYROBOT_NAME".xacro
```

## 3. 生成dae文件

```sh
rosrun collada_urdf urdf_to_collada "$MYROBOT_NAME".urdf "$MYROBOT_NAME".dae
```

## 4. 设置计算精确度 

```sh
cp "$MYROBOT_NAME".dae "$MYROBOT_NAME".backup.dae  # create a backup of your full precision dae.
rosrun moveit_kinematics round_collada_numbers.py "$MYROBOT_NAME".dae "$MYROBOT_NAME".dae "$IKFAST_PRECISION"
```

* 测试：

```sh
openrave-robot.py "$MYROBOT_NAME".dae --info links
openrave "$MYROBOT_NAME".dae
```

## 5. 生成`IkFast*.cpp`

```sh
python `openrave-config --python-dir`/openravepy/_openravepy_/ikfast.py --robot="$MYROBOT_NAME".dae --iktype=transform6d --baselink="$BASE_LINK" --eelink="$EEF_LINK" --savefile="$IKFAST_OUTPUT_PATH"
```

当joint大于6时，需要设定自由轴：

```sh
python `openrave-config --python-dir`/openravepy/_openravepy_/ikfast.py --robot="$MYROBOT_NAME".dae --iktype=transform6d --baselink="$BASE_LINK" --eelink="$EEF_LINK" --freeindex="$FREE_INDEX" --savefile="$IKFAST_OUTPUT_PATH"
```

## 6. 编译 cpp 并测试

复制 `ikfask.h` 到同一目录

```sh
g++ ikfast61_"$PLANNING_GROUP".cpp -lstdc++ -llapack -o compute -lrt
```

## 7. 测试

修改 `ikfast*.cpp` :
添加：

```c
int ik(int argc, char** argv);
int fk(int argc,char** argv); 
```

，执行

```sh
./compute fk j0 j1 j2 j3 j4 j5
./compute ik r00 r01 r02 t0 r10 r11 r12 t1 r20 r21 r22 t2 
```
