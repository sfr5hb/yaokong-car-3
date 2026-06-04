/*
 * IMU.c
 *
 *  Created on: 2026�?�?�?
 *      Author: HP
 */
#include "zf_common_headfile.h"



float norm,vx,vy,vz,ex,ey,ez;
Quat chj={1,0,0,0};//四元数赋值
SensorData uef={0};//传感器初始化

static float ax_prev = 0.0f, ay_prev = 0.0f, az_prev = 0.0f;
static float bx_prev = 0.0f, by_prev = 0.0f;// bz_prev = 0.0f;
//static float exInt_prev = 0.0f, eyInt_prev = 0.0f, ezInt_prev = 0.0f;
static float exInt=0.0f, eyInt=0.0f, ezInt=0.0f;

static float last_raw_yaw = 0.0f;     // 上一次的 raw atan2 yaw（用于计算最小角度差�?
static float yaw_unwrapped = 0.0f;    // 连续累积�?yaw（可以超�?±180°�?


float gx_corr=0.0f,gy_corr=0.0f,gz_corr=0.0f;
float q0_dot=0.0f, q1_dot=0.0f,q2_dot=0.0f,q3_dot=0.0f;
float pp=0.0f,roll = 0.0f,pitch=0.0f,yaw=0.0f;

//零偏数据
static float gx_p=0.0f,gy_p=0.0f,gz_p=0.0f;

uint8 imu_flag=0;//校准标志位

//yaw所需过去状态
//static float last_filtered_yaw = 0.0f;


//零偏校准函数
void IMU_Cal(void)
{
    float gx_sum=0.0f,gy_sum=0.0f,gz_sum=0.0f;
    int sample_sum=500;//采集500组数据求平均

    for(int i=0;i<sample_sum;i++)
        {
        imu660ra_get_gyro();//陀螺仪
        gx_sum+=imu660ra_gyro_x;
        gy_sum+=imu660ra_gyro_y;
        gz_sum+=imu660ra_gyro_z;
        system_delay_ms(6);
        }
    gx_p=gx_sum/sample_sum;
    gy_p=gy_sum/sample_sum;
    gz_p=gz_sum/sample_sum;

    chj.q0 = 1.0f;
    chj.q1 = 0.0f;
    chj.q2 = 0.0f;
    chj.q3 = 0.0f;

    exInt = 0.0f;
    eyInt = 0.0f;
    ezInt = 0.0f;

    ax_prev = 0.0f;
    ay_prev = 0.0f;
    az_prev = 0.0f;
    bx_prev = 0.0f;
    by_prev = 0.0f;

    imu_flag=1;//校准完成
    last_raw_yaw = 0.0f;
    yaw_unwrapped = 0.0f;
    //last_filtered_yaw = 0.0f;

}

void Angle_seem(float *roll3,float *pitch3,float *yaw3)
{
    //tft180_show_string(0,40,"seem");
    if(imu_flag==0)
    {
        //tft180_show_string(0,17,"33");
        *roll3 = 0.0f;
        *pitch3 = 0.0f;
        *yaw3 = 0.0f;
         return;
    }


    //1、传感器数据获取
    imu660ra_get_acc();//加速度
    imu660ra_get_gyro();//陀螺仪


    //零偏补偿
    float gx_raw=(float)imu660ra_gyro_x-gx_p;
    float gy_raw=(float)imu660ra_gyro_y-gy_p;
    float gz_raw=(float)imu660ra_gyro_z-gz_p;


    //2、传感器数据滤波
    // 加速度计一阶低通滤波（系数0.8为新数据权重0.2为历史数据权重）
    uef.ax = imu660ra_acc_x * ACC_LPF_ALPHA + (1.0f - ACC_LPF_ALPHA) * ax_prev;
    uef.ay = imu660ra_acc_y * ACC_LPF_ALPHA + (1.0f - ACC_LPF_ALPHA) * ay_prev;
    uef.az = imu660ra_acc_z * ACC_LPF_ALPHA + (1.0f - ACC_LPF_ALPHA) * az_prev;
    ax_prev = uef.ax; // 更新加速度历史值
    ay_prev = uef.ay;
    az_prev = uef.az;

    // 陀螺仪数据处理
    // - 0.07f：陀螺仪量程转换系数（假设传感器量程为�?000dps时的LSB/dps值）
    // - 0.0174532925f：度转弧度系数（π/180�?
    // - 一阶低通滤波（系数同上）
     uef.gx = (0.061f * gx_raw) * 0.0174532925f * 0.8 + 0.2 * bx_prev;
     uef.gy = (0.061f * gy_raw) * 0.0174532925f * 0.8 + 0.2 * by_prev;
   //uef.gz = (0.061f * gz_raw) * 0.0174532925f * 0.8 + 0.2 * bz_prev;

     uef.gz=KalmanFilter(&kf_gz,gz_raw * 0.061f * 0.0174532925f);//滤除噪声



     bx_prev = uef.gx; // 更新角速度历史值
     by_prev =uef.gy;
     //bz_prev = uef.gz;

      // 3. 加速度归一化处理（将加速度向量转换为单位向量）
     norm = sqrt(uef.ax * uef.ax + uef.ay * uef.ay + uef.az * uef.az);
                //uef.ax /= norm;
                //uef.ay /= norm;
                //uef.az /= norm;

     if(norm < 1e-6)//异常处理
     {
           uef.ax=0.0f;uef.ay=0.0f;uef.az=0.0f;
     }
     else
     {
           uef.ax /= norm;
           uef.ay /= norm;
           uef.az /= norm;

      }

     // 4. 计算预测的重力方向（通过当前四元数）
     // 重力向量在机体坐标系中的投影
      vx = 2.0f * (chj.q1 * chj.q3 - chj.q0 * chj.q2);  // 2(q1q3 - q0q2)
      vy = 2.0f * (chj.q0 * chj.q1 + chj.q2 * chj.q3);  // 2(q0q1 + q2q3)
      vz = chj.q0 * chj.q0 - chj.q1 * chj.q1 - chj.q2 * chj.q2 + chj.q3 * chj.q3; // q0² - q1² - q2² + q3²

     // 5. 计算加速度计测量值与预测值的误差（叉积）
      ex = uef.ay * vz - uef.az * vy;  // 横滚轴误差
      ey = uef.az * vx - uef.ax * vz;  // 俯仰轴误差
      ez = uef.ax * vy - uef.ay * vx;  // 偏航轴误差

     // 6. 误差积分（用于消除稳态误差）
      exInt += ex * Ki * DT;
      eyInt += ey * Ki * DT;
      ezInt += ez * Ki * DT;
      //积分限幅
      if (exInt > INTEGRAL_LIMIT)  exInt = INTEGRAL_LIMIT;
      if (exInt < -INTEGRAL_LIMIT) exInt = -INTEGRAL_LIMIT;
      if (eyInt > INTEGRAL_LIMIT)  eyInt = INTEGRAL_LIMIT;
      if (eyInt < -INTEGRAL_LIMIT) eyInt = -INTEGRAL_LIMIT;
      if (ezInt > INTEGRAL_LIMIT)  ezInt = INTEGRAL_LIMIT;
      if (ezInt < -INTEGRAL_LIMIT) ezInt = -INTEGRAL_LIMIT;

//      exInt = exInt_prev + ex * Ki * DT;  // Ki为积分系数，DT为采样周期
//      eyInt = eyInt_prev + ey * Ki * DT;//（ki乘以积分项）
//      ezInt = ezInt_prev + ez * Ki * DT;
//      exInt_prev = exInt;  // 保存积分
//      eyInt_prev = eyInt;
//      ezInt_prev = ezInt;

       //7.陀螺仪矫正
        gx_corr = uef.gx + Kp * ex + exInt; // 比例+积分校正陀螺仪X�?kp乘以误差�?
        gy_corr = uef.gy + Kp * ey + eyInt; // 比例+积分校正陀螺仪误差
        gz_corr = uef.gz + Kp * ez + ezInt; // 比例+积分校正陀螺仪误差

       // 8. 四元数更新（微分方程离散化）
                   // 四元数导数计�?
       q0_dot = -chj.q1 * gx_corr - chj.q2 * gy_corr - chj.q3 * gz_corr;
       q1_dot =  chj.q0 * gx_corr + chj.q2 * gz_corr - chj.q3 * gy_corr;
       q2_dot =  chj.q0 * gy_corr - chj.q1 * gz_corr + chj.q3 * gx_corr;
       q3_dot =  chj.q0 * gz_corr + chj.q1 * gy_corr - chj.q2 * gx_corr;

         // 四元数积分更新（0.5*DT为离散化系数）
        chj.q0 += q0_dot * DT * 0.5f;
        chj.q1 += q1_dot * DT * 0.5f;
        chj.q2 += q2_dot * DT * 0.5f;
        chj.q3 += q3_dot * DT * 0.5f;

                   // 9. 四元数归一化（避免数值误差累积）
        pp = sqrt(chj.q0 * chj.q0 + chj.q1 * chj.q1 + chj.q2 * chj.q2 + chj.q3 * chj.q3);
        if(pp < 1e-6f)
        {
            chj.q0 = 1.0f;
            chj.q1 = 0.0f;
            chj.q2 = 0.0f;
            chj.q3 = 0.0f;
            pp = 1.0f;
        }
        chj.q0 /= pp;
        chj.q1 /= pp;
        chj.q2 /= pp;
        chj.q3 /= pp;

       // 10. 四元数转欧拉角（Z-Y-X旋转顺序）
        // 横滚角（绕X轴旋转）
        roll = atan2(2.0f * (chj.q2 * chj.q3 + chj.q0 * chj.q1),
                               chj.q0 * chj.q0 - chj.q1 * chj.q1 - chj.q2 * chj.q2 + chj.q3 * chj.q3) * 180 / 3.1415926;
        // 俯仰角（绕Y轴旋转）
         float pitch_arg = -2.0f * (chj.q1 * chj.q3 - chj.q0 * chj.q2);
         if(pitch_arg > 1.0f) pitch_arg = 1.0f;
         if(pitch_arg < -1.0f) pitch_arg = -1.0f;
         pitch = asin(pitch_arg) * 180 / 3.1415926;
        // 偏航角（绕Z轴旋转）
         float raw_yaw = atan2(2.0f * (chj.q1 * chj.q2 + chj.q0 * chj.q3),
                              chj.q0 * chj.q0 + chj.q1 * chj.q1 - chj.q2 * chj.q2 - chj.q3 * chj.q3) * 180 / 3.1415926;


         //13.yaw unwrap(处理180度跳变)
         float delta = raw_yaw - last_raw_yaw;
         if(delta >  180.0f) delta -= 360.0f;
         if(delta < -180.0f) delta += 360.0f;
         yaw_unwrapped += delta;
         last_raw_yaw = raw_yaw;


         //14.yaw简单低通
         static float yaw_filtered = 0.0f;
         yaw_filtered += (yaw_unwrapped - yaw_filtered) * 0.3f;

         *roll3  = roll;
         *pitch3 = pitch;
         *yaw3   = yaw_filtered;

}






//        //在进行二级滤波前，处�?180 度跳转逻辑
//        float delta = raw_yaw - last_raw_yaw;
//
//        // 如果差值超�?180 度，说明发生了正负跳转，手动平滑�?
//        if (delta > 180.0f)  delta -= 360.0f;
//        if (delta < -180.0f) delta += 360.0f;
//
//        yaw_unwrapped+=delta;
//        last_raw_yaw=raw_yaw;
//
//       // 11. 输出欧拉�?
//        *roll3 = roll;
//        *pitch3 = pitch;
//        //*yaw3 =KalmanFilter(&kf_yaw,yaw_unwrapped);
//
//        float filtered_yaw=KalmanFilter(&kf_yaw,yaw_unwrapped);
//
////        if(filtered_yaw > 360.0f) filtered_yaw -= 360.0f;
////        if(filtered_yaw < -360.0f) filtered_yaw += 360.0f;
//
//        *yaw3 = filtered_yaw;
//        last_filtered_yaw = filtered_yaw;





