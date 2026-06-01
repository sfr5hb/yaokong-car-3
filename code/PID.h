/*
 * PID.h
 *
 *  Created on: 2026年4月4日
 *      Author: HP
 */

#ifndef _PID_H_
#define _PID_H_


// 转向电机专用 PID 参数结构体
typedef struct
{
    float kp;               // 比例系数
    float ki;               // 积分系数
    float kd;               // 微分系数

    float target;           // 目标脉冲位置
    float current;          // 当前脉冲位置
    float error;            // 当前误差
    float last_error;       // 上次误差

    float integrator;       // 积分累加值
    float imax;             // 积分限幅（抗饱和）

    float output;           // PID 总输出
    float out_limit;        // 输出限幅 (例如 0-100 对应占空比)
} Steer_PID_t;//内环转向环



//增量式PPID,后轮电机参数
typedef struct
{
        float kp;
        float ki;
        float kd;
        float out_limit;        // 最大占空比限制
        float last_error;       // e(k-1)
        float last_last_error;  // e(k-2)
        float last_output;      // 上一次输出的占空比
        float integrator;       // 积分累加值
}Speed_PID_t;

typedef struct
{
   float kp,ki,kd;
   float target,current,error,last_error;
   float integrator,imax;
   float out_put,out_limit;

}Nav_PID_t;



// 全局变量声明
extern Steer_PID_t SteerPID;
extern Nav_PID_t NavPID;

extern Speed_PID_t LeftPID;
extern Speed_PID_t RightPID;

/*之前pid//// 函数接口
//void Steer_PID_Init(void);
//float Steer_Position_Control(int32 target, int32 current);
*/

void SteerAngle_Init(void);
float SteerAngle_Ctrl(float target,float current);





void _Nav_PID_init(void);
float Nav_ctl(float target_yaw,float current_yaw);




//houlun
void Motor_PID_Init(void);
float Speed_PID(Speed_PID_t *pid,float target_speed,float current_speed);



float SPEED_POS(Speed_PID_t *pid,float target,float current);

//void NAV_STEER(void);

extern float yaw;


//外部变量声明
extern float target_yaw;//目标航向角
extern int32 current_encoder_pos;//当前编码器位置
extern float steer_duty;//电机占空比


extern uint8 reach_flag;//状态标志位

#endif /* CODE_PID_H_ */




