/*
 * PID.c
 *
 *  Created on: 2026年4月4日
 *      Author: HP
 */

#include "zf_common_headfile.h"
//转向电机

Steer_PID_t SteerPID;
//左右轮PID
Speed_PID_t LeftPID;
Speed_PID_t RightPID;


Nav_PID_t NavPID;
//int32 current_pos=0;

void SteerAngle_Init(void)
{
    SteerPID.kp = 3.44f;      // 初始值，//昨天是2   3.15
    SteerPID.ki = 0.28f;      // 转向环通常不需要 I   0.43
    SteerPID.kd = 0.69f;      // D 非常重要，抑制震荡  0.15

    SteerPID.imax = 20.0f;   // 积分限幅
    SteerPID.out_limit = 80.0f; // 强制输出限幅在 60% 占空比以内，保护电机

    SteerPID.integrator = 0;
    SteerPID.last_error = 0;

}

float SteerAngle_Ctrl(float target,float current)
{
     if(target > 42)  target = 42;
     if(target < -40) target = -40;//绝对位置编码器左右极限

       // 1. 更新内部变量
     SteerPID.target = (float)target;
     SteerPID.current = (float)current;
     SteerPID.error = SteerPID.target - SteerPID.current;


       // 2. 比例项 P
     float out_p = SteerPID.kp * SteerPID.error;

       // 3. 积分项 I (带抗饱和限幅)
     SteerPID.integrator += SteerPID.error;
     if (SteerPID.integrator > SteerPID.imax)  SteerPID.integrator = SteerPID.imax;
     if (SteerPID.integrator < -SteerPID.imax) SteerPID.integrator = -SteerPID.imax;
     float out_i = SteerPID.ki * SteerPID.integrator;

       // 4. 微分项 D (误差的变化率)
     float out_d = SteerPID.kd * (SteerPID.error - SteerPID.last_error);
     SteerPID.last_error = SteerPID.error;

       // 5. 计算总输出
     SteerPID.output = out_p + out_i + out_d;

       // 6. 核心保护：输出限幅 (防止撞击机械限位)
     if (SteerPID.output > SteerPID.out_limit)  SteerPID.output = SteerPID.out_limit;
     if (SteerPID.output < -SteerPID.out_limit) SteerPID.output = -SteerPID.out_limit;

     return SteerPID.output;

}
/*
void Steer_PID_Init(void)
{
    SteerPID.kp = 3.15f;      // 初始值，//昨天是2
    SteerPID.ki = 0.43f;      // 转向环通常不需要 I
    SteerPID.kd = 0.15f;      // D 非常重要，抑制震荡

    SteerPID.imax = 80.0f;   // 积分限幅
    SteerPID.out_limit = 80.0f; // 强制输出限幅在 60% 占空比以内，保护电机

    SteerPID.integrator = 0;
    SteerPID.last_error = 0;
}


 * @brief 转向位置环控制逻辑
 * @param target  目标脉冲位置 (-200 到 200)
 * @param current 当前编码器位置
 * @return float  处理后的 PWM 输出值 (-limit 到 +limit)
 *
float Steer_Position_Control(int32 target, int32 current)
{

    if(target > 260)  target = 260;
    if(target < -260) target = -260;//昨天是260

    // 1. 更新内部变量
    SteerPID.target = (float)target;
    SteerPID.current = (float)current;
    SteerPID.error = SteerPID.target - SteerPID.current;

    // 2. 比例项 P
    float out_p = SteerPID.kp * SteerPID.error;

    // 3. 积分项 I (带抗饱和限幅)
    SteerPID.integrator += SteerPID.error;
    if (SteerPID.integrator > SteerPID.imax)  SteerPID.integrator = SteerPID.imax;
    if (SteerPID.integrator < -SteerPID.imax) SteerPID.integrator = -SteerPID.imax;
    float out_i = SteerPID.ki * SteerPID.integrator;

    // 4. 微分项 D (误差的变化率)
    float out_d = SteerPID.kd * (SteerPID.error - SteerPID.last_error);
    SteerPID.last_error = SteerPID.error;

    // 5. 计算总输出
    SteerPID.output = out_p + out_i + out_d;

    // 6. 核心保护：输出限幅 (防止撞击机械限位)
    if (SteerPID.output > SteerPID.out_limit)  SteerPID.output = SteerPID.out_limit;
    if (SteerPID.output < -SteerPID.out_limit) SteerPID.output = -SteerPID.out_limit;

    return SteerPID.output;
}*/

//导航外环()PD控制

uint8 reach_flag=0;//状态标志位
uint16 reach_count=0;

void _Nav_PID_init(void)
{


    NavPID.kp=3.25f;//转向电机算出来的角度值和航向角之间的映射系数
    NavPID.ki=0.23f;
    NavPID.kd=0.06f;

    NavPID.imax = 10.0f;

            // 核心外环的输出限幅，等于内环的输入极限，转向最大输出180
    NavPID.out_limit = 40.0f;

    NavPID.integrator = 0;
    NavPID.last_error = 0;

}

float Nav_ctl(float target_yaw,float current_yaw)
{
    NavPID.target=target_yaw;
    NavPID.current=current_yaw;

    NavPID.error=NavPID.target-NavPID.current;
    if(NavPID.error>180.0f)NavPID.error-=360.0f;
    if(NavPID.error<-180.0f)NavPID.error+=360.0f;

    float out_p=NavPID.kp*NavPID.error;//比例项

    // 3. 积分项 I
    NavPID.integrator += NavPID.error;
    if (NavPID.integrator > NavPID.imax)  NavPID.integrator = NavPID.imax;
    if (NavPID.integrator < -NavPID.imax) NavPID.integrator = -NavPID.imax;
    float out_i = NavPID.ki * NavPID.integrator;

    // 4. 微分项 D
    float out_d = NavPID.kd * (NavPID.error - NavPID.last_error);
    NavPID.last_error = NavPID.error;

    NavPID.out_put = out_p + out_i +out_d;
    //限幅，不超过编码器最大180
    if (NavPID.out_put > NavPID.out_limit)  NavPID.out_put = NavPID.out_limit;
    if (NavPID.out_put < -NavPID.out_limit) NavPID.out_put = -NavPID.out_limit;

    return NavPID.out_put;//float参数函数的返回值
}


//初始化

void Motor_PID_Init(void)
{
//    LeftPID.kp=0.35f;
//    LeftPID.ki=0.05f;
//    LeftPID.kd=0.05f;
//    LeftPID.out_limit=45.0f;
//    LeftPID.last_error=0;
//    LeftPID.last_last_error=0;
//    LeftPID.last_output=0;


        LeftPID.kp=1.35f;
        LeftPID.ki=0.05f;
        LeftPID.kd=0.05f;
        LeftPID.out_limit=45.0f;
        LeftPID.last_error=0;
        LeftPID.last_last_error=0;
        LeftPID.last_output=0;

            RightPID.kp=1.2f;
            RightPID.ki=0.04f;
            RightPID.kd=0.05f;
            RightPID.out_limit=45.0f;
            RightPID.last_error=0;
            RightPID.last_last_error=0;
            RightPID.last_error=0;


//    RightPID.kp=0.4f;
//    RightPID.ki=0.04f;
//    RightPID.kd=0.05f;
//    RightPID.out_limit=45.0f;
//    RightPID.last_error=0;
//    RightPID.last_last_error=0;
//    RightPID.last_output=0;

}

//单轮PID计算
//增量式pid
float Speed_PID(Speed_PID_t *pid,float target_speed,float current_speed)

{

    float error=target_speed-current_speed;

    //增量式计算
    float P_P=pid->kp*(error-pid->last_error);
    float I_I=pid->ki*error;
    float D_D=pid->kd*(error-2*pid->last_error+pid->last_last_error);

    float OUT=P_P+I_I+D_D;
    //累加得到本次绝对占空比
    pid->last_output+=OUT;
    //xianfu
    if(pid->last_output > pid->out_limit)  pid->last_output = pid->out_limit;
   // if(pid->last_output < -pid->out_limit) pid->last_output = -pid->out_limit;

    if(pid->last_output < 0.0f) pid->last_output = 0.0f;
    //更新
    pid->last_last_error=pid->last_error;
    pid->last_error=error;

    return pid->last_output;


}

//位置式pid
float SPEED_POS(Speed_PID_t *pid,float target,float current)
{
    float error = target - current;

    if(fabsf(error) < 2.5f)
    {
        pid->last_error = error;
        pid->last_output *= 0.85f;
        if(pid->last_output < 1.0f) pid->last_output = 0.0f;
        return pid->last_output;
    }

    float P = pid->kp * error;
    float D = pid->kd * (error - pid->last_error);

    pid->last_error = error;
    pid->last_output = P + D;

    if(pid->last_output > pid->out_limit) pid->last_output = pid->out_limit;
    if(pid->last_output < 0.0f) pid->last_output = 0.0f;

    return pid->last_output;
}
