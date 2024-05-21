#ifndef _MATHHELPER_H_
#define _MATHHELPER_H_

#pragma once

#include <array>
#include <math.h>
#include <stdio.h>

enum class EULER { XYZ = 0, ZYX = 1, XZY = 2, YXZ = 3, YZX = 4, ZXY = 5 };

// math helper
// HomoMatrix to Rotation matrix
void T2rm(double T[4][4], double R[3][3]);
// Rotation matrix to HomoMatrix
void rm2T(double R[3][3], double T[4][4]);

// Rotation matrix to Euler Rzyx
void rm2zyx(double R[3][3], double euler[3]);

// Euler Rzyx to Rotation matrix
void zyx2rm(double euler[3], double R[3][3]);

// Euler Rzyx to Quaternion
void zyx2quat(double euler[3], double quat[4]);

// Quaternion to Euler Rzyx
void quat2zyx(double quat[4], double euler[3]);

// Quaternion to Rotation matrix
void quat2rm(double quat[4], double R[3][3]);

// Rotation matrix to Quaternion
void rm2quat(double R[3][3], double quat[4]);

// HomoMatrix to Euler Rzyx
void T2zyx(double T[4][4], double euler[3]);

// HomoMatrix to pose array
void T2pose(double T[4][4], double pose[6], EULER euler);
// void T2pose(double T[4][4], double pose[6]);

// pose array to HomoMatrix
void pose2T(double pose[6], double T[4][4]);

// pose array to Euler Rzyx
void pose2zyx(double pose[6], double euler[3]);

// Euler Rzyx to pose array
void zyx2pose(double euler[3], double pose[6]);

// Rotation matrix to euler Rx Ry Rz
void rm2xyz(double R[3][3], double euler[3]);

// Transform to pose euler Rx Ry Rz
void T2xyz(double T[4][4], double euler[3]);

// euler Rx Ry Rz to pose euler Rx Ry Rz
void xyz2pose(double euler[3], double pose[6]);

// euler rx ry rz to rotation matrix
void xyz2rm(double euler[3], double R[3][3]);

// pose euler Rx Ry Rz to Transform
void pose2T(double pose[6], double T[4][4]);

#endif // !_MATHHELPER_H_
