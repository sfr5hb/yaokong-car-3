/*
 * kalman.h
 *
 *  Created on: 2026年4月13日
 *      Author: HP
 */

#ifndef CODE_KALMAN_H_
#define CODE_KALMAN_H_


typedef struct {
    float X_last; //上一时刻的最优结果  X(k-|k-1)
    float X_mid;  //当前时刻的预测结果  X(k|k-1)
    float X_now;  //当前时刻的最优结果  X(k|k)
    float P_mid;  //当前时刻预测结果的协方差  P(k|k-1)
    float P_now;  //当前时刻最优结果的协方差  P(k|k)
    float P_last; //上一时刻最优结果的协方差  P(k-1|k-1)
    float kg;     //kalman增益
    float A;      //系统参数
    float B;
    float Q;
    float R;
    float H;
}extKalman_t;

//卡尔曼滤波
void KalmanCreate(extKalman_t *p,float T_Q,float T_R);
float KalmanFilter(extKalman_t* p,float dat);
void filter_Init(void);
//extern extKalman_t Zero_Kalman;

extern extKalman_t kf_gz;   // 过滤 Z 轴角速度
extern extKalman_t kf_yaw;  // 过滤最终偏航角

#endif /* CODE_KALMAN_H_ */
