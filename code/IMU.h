/*
 * IMU.h
 *
 *  Created on: 2026年3月6日
 *      Author: HP
 */

#ifndef CODE_IMU_H_
#define CODE_IMU_H_

//四元数获取姿态角
//宏定义

#define ACC_LPF_ALPHA       0.8f      // 加速度一阶低通 新数据权重
#define GYRO_LPF_ALPHA      0.8f      // 陀螺仪 X/Y 一阶低通 新数据权重
#define GYRO_SCALE          0.061f    // IMU660RA ±2000dps 量程系数
#define DEG_TO_RAD          0.0174532925f
#define INTEGRAL_LIMIT      20.0f     // 积分抗饱和上限（根据 Ki 调整）



#define Kp 0.2f // 加速度权重
#define Ki 0.001f // 误差积分增益
#define DT 0.005f // 采样时间间隔//和中断周期匹配5ms--0.005f

typedef struct{
        float q0,q1,q2,q3;

}Quat;//四元数四个数值，q0，实部为1，其他为0

typedef struct{
        float gx,gy,gz;//陀螺仪测量值
        float ax,ay,az;//加速度计测量值
}SensorData;

extern float roll, pitch, yaw;

extern uint8 imu_flag;

void IMU_Cal(void);

void Angle_seem(float *roll3,float *pitch3,float *yaw3);



#endif /* CODE_IMU_H_ */
