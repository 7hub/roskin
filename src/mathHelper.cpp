#include "mathHelper.h"


    // Homogenous Transformation
    void T2rm(double T[4][4], double R[3][3])
    {
        R[0][0] = T[0][0];
        R[0][1] = T[0][1];
        R[0][2] = T[0][2];
        R[1][0] = T[1][0];
        R[1][1] = T[1][1];
        R[1][2] = T[1][2];
        R[2][0] = T[2][0];
        R[2][1] = T[2][1];
        R[2][2] = T[2][2];
    }

    // HomoMatrix to Euler Rzyx
    void T2zyx(double T[4][4], double euler[3])
    {
        // Calculate the rotation matrix from the homogeneous transformation matrix
        double R[3][3];
        T2rm(T, R);

        // Convert the rotation matrix to Euler angles
        rm2zyx(R, euler);
    }

    // Transform to pose euler Rx Ry Rz
    void T2xyz(double T[4][4], double euler[3])
    {
        // Extract the rotation matrix from the homogeneous transformation matrix
        double R[3][3];
        T2rm(T, R);

        // Convert the rotation matrix to Euler angles
        rm2xyz(R, euler);
    }

    // HomoMatrix to pose array with euler notation
    void T2pose(double T[4][4], double pose[6], EULER euler = EULER::XYZ)
    {
        pose[0] = T[0][3];
        pose[1] = T[1][3];
        pose[2] = T[2][3];

        // Extract the rotation matrix from the homogeneous transformation matrix
        double R[3][3];
        T2rm(T, R);

        // Convert the rotation matrix to Euler angles
        switch (euler)
        {
        case EULER::ZYX:
            rm2zyx(R, pose + 3);
            break;
        case EULER::XYZ:
        default:
            /* code */
            rm2xyz(R, pose + 3);
            break;
        }
    }
    // Rotation matrix to Homogenous Transformation
    void rm2T(double R[3][3], double T[4][4])
    {
        T[0][0] = R[0][0];
        T[0][1] = R[0][1];
        T[0][2] = R[0][2];
        T[1][0] = R[1][0];
        T[1][1] = R[1][1];
        T[1][2] = R[1][2];
        T[2][0] = R[2][0];
        T[2][1] = R[2][1];
        T[2][2] = R[2][2];
        T[0][3] = 0;
        T[1][3] = 0;
        T[2][3] = 0;
        T[3][0] = 0;
        T[3][1] = 0;
        T[3][2] = 0;
        T[3][3] = 1;
    }

    // Rotation matrix to Euler Angles Rzyx
    void rm2zyx(double R[3][3], double euler[3])
    {
        // Calculate the angles rz, ry, and rx
        double rz = atan2(R[0][1], R[0][2]);
        double ry = atan2(-R[0][0], sqrt(pow(R[0][1], 2) + pow(R[0][2], 2)));
        double rx = atan2(R[1][2], R[2][2]);

        // Store the angles in the euler array
        euler[0] = rz;
        euler[1] = ry;
        euler[2] = rx;
    }

    // Euler Rzyx to Rotation matrix
    void zyx2rm(double euler[3], double R[3][3])
    {
        // Calculate the cosine and sine of the euler angles
        double cos_rz = cos(euler[0]);
        double sin_rz = sin(euler[0]);
        double cos_ry = cos(euler[1]);
        double sin_ry = sin(euler[1]);
        double cos_rx = cos(euler[2]);
        double sin_rx = sin(euler[2]);

        // Fill in the rotation matrix elements
        R[0][0] = cos_ry * cos_rx;
        R[0][1] = cos_ry * sin_rx;
        R[0][2] = -sin_ry;
        R[1][0] = sin_rz * sin_ry * cos_rx + cos_rz * sin_rx;
        R[1][1] = sin_rz * sin_ry * sin_rx - cos_rz * cos_rx;
        R[1][2] = sin_rz * cos_ry;
        R[2][0] = -cos_rz * sin_ry * cos_rx + sin_rz * sin_rx;
        R[2][1] = -cos_rz * sin_ry * sin_rx - sin_rz * cos_rx;
        R[2][2] = cos_rz * cos_ry;
    }

    // Euler Rzyx to Quaternion
    void zyx2quat(double euler[3], double quat[4])
    {
        // Calculate the cosine and sine of the euler angles
        double cos_rz = cos(euler[0] / 2);
        double sin_rz = sin(euler[0] / 2);
        double cos_ry = cos(euler[1] / 2);
        double sin_ry = sin(euler[1] / 2);
        double cos_rx = cos(euler[2] / 2);
        double sin_rx = sin(euler[2] / 2);

        // Calculate the quaternion elements
        quat[0] = cos_rz * cos_ry * cos_rx + sin_rz * sin_ry * sin_rx;
        quat[1] = sin_rz * cos_ry * cos_rx - cos_rz * sin_ry * sin_rx;
        quat[2] = cos_rz * sin_ry * cos_rx + sin_rz * cos_ry * sin_rx;
        quat[3] = cos_rz * cos_ry * sin_rx - sin_rz * sin_ry * cos_rx;
    }

    // Quaternion to Euler Rzyx
    void quat2zyx(double quat[4], double euler[3])
    {
        // Calculate the rotation matrix from the quaternion
        double R[3][3];
        quat2rm(quat, R);

        // Convert the rotation matrix to Euler angles
        rm2zyx(R, euler);
    }

    // Quaternion to Rotation matrix
    void quat2rm(double quat[4], double R[3][3])
    {
        // Calculate the rotation matrix elements from the quaternion
        double q0 = quat[0];
        double q1 = quat[1];
        double q2 = quat[2];
        double q3 = quat[3];

        R[0][0] = 1 - 2 * (q1 * q1 + q2 * q2);
        R[0][1] = 2 * (q0 * q2 - q1 * q3);
        R[0][2] = 2 * (q0 * q3 + q1 * q2);

        R[1][0] = 2 * (q0 * q1 + q2 * q3);
        R[1][1] = 1 - 2 * (q2 * q2 + q3 * q3);
        R[1][2] = 2 * (q1 * q3 - q0 * q2);

        R[2][0] = 2 * (q0 * q3 - q1 * q2);
        R[2][1] = 2 * (q1 * q2 + q0 * q3);
        R[2][2] = 1 - 2 * (q0 * q0 + q1 * q1);
    }

    // Rotation matrix to Quaternion
    void rm2quat(double R[3][3], double quat[4])
    {
        // Calculate the trace of the rotation matrix
        double trace = R[0][0] + R[1][1] + R[2][2];

        // Calculate the quaternion elements from the rotation matrix
        if (trace > 0)
        {
            double S = sqrt(trace + 1.0) * 2; // S=4*qw
            quat[0] = 0.25 * S;
            quat[1] = (R[2][1] - R[1][2]) / S;
            quat[2] = (R[0][2] - R[2][0]) / S;
            quat[3] = (R[1][0] - R[0][1]) / S;
        }
        else if ((R[0][0] > R[1][1]) & (R[0][0] > R[2][2]))
        {
            double S = sqrt(1.0 + R[0][0] - R[1][1] - R[2][2]) * 2; // S=4*qx
            quat[0] = (R[2][1] - R[1][2]) / S;
            quat[1] = 0.25 * S;
            quat[2] = (R[0][1] + R[1][0]) / S;
            quat[3] = (R[0][2] + R[2][0]) / S;
        }
        else if (R[1][1] > R[2][2])
        {
            double S = sqrt(1.0 + R[1][1] - R[0][0] - R[2][2]) * 2; // S=4*qy
            quat[0] = (R[0][2] - R[2][0]) / S;
            quat[1] = (R[0][1] + R[1][0]) / S;
            quat[2] = 0.25 * S;
            quat[3] = (R[1][2] + R[2][1]) / S;
        }
        else
        {
            double S = sqrt(1.0 + R[2][2] - R[0][0] - R[1][1]) * 2; // S=4*qz
            quat[0] = (R[1][0] - R[0][1]) / S;
            quat[1] = (R[0][2] + R[2][0]) / S;
            quat[2] = (R[1][2] + R[2][1]) / S;
            quat[3] = 0.25 * S;
        }
        return;
    }

    // pose array to HomoMatrix
    void pose2T(double pose[6], double T[4][4])
    {
        // Calculate the rotation matrix from the Euler angles
        double R[3][3];
        zyx2rm(pose + 3, R);

        // Construct the homogeneous transformation matrix from the rotation matrix and translation vector
        T[0][0] = R[0][0];
        T[0][1] = R[0][1];
        T[0][2] = R[0][2];
        T[0][3] = pose[0];
        T[1][0] = R[1][0];
        T[1][1] = R[1][1];
        T[1][2] = R[1][2];
        T[1][3] = pose[1];
        T[2][0] = R[2][0];
        T[2][1] = R[2][1];
        T[2][2] = R[2][2];
        T[2][3] = pose[2];
        T[3][0] = 0.0;
        T[3][1] = 0.0;
        T[3][2] = 0.0;
        T[3][3] = 1.0;
    }

    // pose array to Euler Rzyx
    void pose2zyx(double pose[6], double euler[3])
    {
        // Calculate the rotation matrix from the Euler angles
        double R[3][3];
        zyx2rm(pose + 3, R);

        // Convert the rotation matrix to Euler angles
        rm2zyx(R, euler);
    }

    // Euler Rzyx to pose array
    void zyx2pose(double euler[3], double pose[6])
    {
        // Calculate the rotation matrix from the Euler angles
        double R[3][3];
        zyx2rm(euler, R);

        // Construct the homogeneous transformation matrix from the rotation matrix and translation vector
        pose[0] = 0.0;
        pose[1] = 0.0;
        pose[2] = 0.0;
        pose[3] = R[0][0];
        pose[4] = R[1][0];
        pose[5] = R[2][0];
    }

    // euler Rx Ry Rz to pose euler Rx Ry Rz
    void xyz2pose(double euler[3], double pose[6])
    {
        // Calculate the rotation matrix from the Euler angles
        double R[3][3];
        xyz2rm(euler, R);

        // Construct the homogeneous transformation matrix from the rotation matrix and translation vector
        pose[0] = 0.0;
        pose[1] = 0.0;
        pose[2] = 0.0;
        pose[3] = R[0][0];
        pose[4] = R[1][0];
        pose[5] = R[2][0];
    }

    // euler rx ry rz to rotation matrix
    void xyz2rm(double euler[3], double R[3][3])
    {
        double sx = sin(euler[0]);
        double cx = cos(euler[0]);
        double sy = sin(euler[1]);
        double cy = cos(euler[1]);
        double sz = sin(euler[2]);
        double cz = cos(euler[2]);

        R[0][0] = cx * cz + sx * sy * sz;
        R[0][1] = cx * sz - sx * sy * cz;
        R[0][2] = sx * cy;
        R[1][0] = sx * cz - cx * sy * sz;
        R[1][1] = cx * cz + sx * sy * sz;
        R[1][2] = -sx * cy;
        R[2][0] = cx * sy;
        R[2][1] = sx * cy;
        R[2][2] = cx * cy;
    }

    // // Rotation matrix to euler Rx Ry Rz
    // void rm2xyz(double R[3][3], double euler[3])
    // {
    //     // Calculate the Euler angles from the rotation matrix
    //     euler[0] = atan2(R[2][1], R[2][2]);
    //     euler[1] = atan2(-R[2][0], sqrt(R[2][1] * R[2][1] + R[2][2] * R[2][2]));
    //     euler[2] = atan2(R[1][0], R[0][0]);
    //     return;
    // }
    // rotation matrix to euler rx ry rz
    void rm2xyz(double R[3][3], double euler[3])
    {
        double sy = sqrt(R[0][0] * R[0][0] + R[1][0] * R[1][0]);

        bool singular = sy < 1e-6;

        if (!singular)
        {
            euler[0] = atan2(R[2][1], R[2][2]);
            euler[1] = atan2(-R[2][0], sy);
            euler[2] = atan2(R[1][0], R[0][0]);
        }
        else
        {
            euler[0] = atan2(-R[1][2], R[1][1]);
            euler[1] = atan2(-R[2][0], sy);
            euler[2] = 0;
        }
    }