#!/bin/bash
export MYROBOT_NAME="tx2_60"
export IKFAST_PRECISION="6"

export PLANNING_GROUP="tx2_60_arm"
export BASE_LINK="0"
export EEF_LINK="9"
export IKFAST_OUTPUT_PATH=$(pwd)/ikfast61_"$PLANNING_GROUP".cpp
