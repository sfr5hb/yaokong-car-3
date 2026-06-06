/*
 * Motor.c
 *
 *  Created on: 2026年4月6日
 *      Author: HP
 */

#include "zf_common_headfile.h"

float steer_duty=0;

void Motor_Init(void)
{
    //m1接驱动的out-,m2接out+

    pwm_init(PWM_CH1, 17000, 0);                                                // PWM 通道 1 初始化频率 17KHz 占空比初始为 0
    pwm_init(PWM_CH2, 17000, 0);    // PWM 通道 2 初始化频率 17KHz 占空比初始为 0


   //drv电机驱动
    gpio_init(DIR_R1, GPO, GPIO_HIGH, GPO_PUSH_PULL);                           // GPIO 初始化为输出 默认上拉输出高
    pwm_init(PWM_R1, 17000, 0);                                                 // PWM 通道初始化频率 17KHz 占空比初始为 0
    gpio_init(DIR_L1, GPO, GPIO_HIGH, GPO_PUSH_PULL);                           // GPIO 初始化为输出 默认上拉输出高
    pwm_init(PWM_L1, 17000, 0);                                            // PWM 通道初始化频率 17KHz 占空比初始为 0


}
//后路编码器
//后轮编码器数据

void Motor_Ctrl(float steer_duty)
{
    // 3. 驱动输出 (执行)
            if(steer_duty >= 0) {
                pwm_set_duty(PWM_CH2, (uint32)(steer_duty * (PWM_DUTY_MAX / 100)));
                pwm_set_duty(PWM_CH1, 0);
            } else {
                pwm_set_duty(PWM_CH2, 0);
                pwm_set_duty(PWM_CH1, (uint32)((-steer_duty) * (PWM_DUTY_MAX / 100)));
            }
}

float speed_left;
float speed_right;
//float target_pulse;

float speed;
uint8 dirt_flag;

void Motor_Speed(float target_speed,uint8 direction)
{


    if(target_speed<0.0f)target_speed=0.0f;
    if(target_speed>40.0f)target_speed=40.0f;

    if((target_speed!=speed) || (direction!=dirt_flag))
    {
        Speed_Reset(&LeftPID);
        Speed_Reset(&RightPID);
    }


    speed=target_speed;
    dirt_flag=direction;
}

void Motor_OUT(float duty_l,float duty_r,uint8 direction)
{
    if(duty_l < 0.0f) duty_l = 0.0f;
    if(duty_r < 0.0f) duty_r = 0.0f;
    if(duty_l > 100.0f) duty_l = 100.0f;
    if(duty_r > 100.0f) duty_r = 100.0f;

    gpio_set_level(DIR_L1, direction);
    gpio_set_level(DIR_R1, direction);
    pwm_set_duty(PWM_L1, (uint32)(duty_l * (PWM_DUTY_MAX / 100.0f)));
    pwm_set_duty(PWM_R1, (uint32)(duty_r * (PWM_DUTY_MAX / 100.0f)));


}

  // target_pulse=target_speed*0.75f;

//   float alpha=0.75f;
//
//   speed_left=alpha*speed_left+(1-alpha)*(float)encoder_data_2;
//   speed_right=alpha*speed_right+(1-alpha)*(float)encoder_data_3;

//   speed_left=(float)encoder_data_2;
//
//   speed_right=(float)encoder_data_3;


//
//    //z左右轮分别PID
//    float duty_left=SPEED_POS(&LeftPID,target_pulse,speed_left);
//    float duty_right=SPEED_POS(&RightPID,target_pulse,speed_right);
//    //输出PWMjia方向
//
//    if(duty_left < 0.0f) duty_left = 0.0f;
//    if(duty_right < 0.0f) duty_right = 0.0f;
//
//    uint32 pwm_l=(uint32)(duty_left*(PWM_DUTY_MAX / 100.0f));
//    uint32 pwm_r=(uint32)(duty_right*(PWM_DUTY_MAX / 100.0f));
//
//    gpio_set_level(DIR_L1, direction);
//    gpio_set_level(DIR_R1, direction);
//
//    pwm_set_duty(PWM_L1, pwm_l);
//    pwm_set_duty(PWM_R1, pwm_r);
//
//}

/*
 * void Motor_Drive_Ctrl(float duty_percent, uint8 reverse)
{
    if(duty_percent > 100.0f) duty_percent = 100.0f;
    if(duty_percent < 0.0f) duty_percent = 0.0f;

    uint32 pwm_val = (uint32)(duty_percent * (PWM_DUTY_MAX / 100.0f));

    // 统一方向控制（两电机同向）
    gpio_set_level(DIR_L1, reverse ? 0 : 1);
    gpio_set_level(DIR_R1, reverse ? 0 : 1);

    pwm_set_duty(PWM_L1, pwm_val);
    pwm_set_duty(PWM_R1, pwm_val);
}
 */

/*/后轮电机
 * void Motor_Control(int left_pwm, int right_pwm)
{
    // 限幅处理
    if(left_pwm > 8000) left_pwm = 8000;
    if(right_pwm > 8000) right_pwm = 8000;

    // 左电机方向与速度
    if(left_pwm >= 0) {
        gpio_set_level(DIR_L1, 0);
        pwm_set_duty(PWM_L1, left_pwm);
    } else {
        gpio_set_level(DIR_L1, 1);
        pwm_set_duty(PWM_L1, -left_pwm);
    }

    if(right_pwm >= 0) {
            gpio_set_level(DIR_R1, 0);
            pwm_set_duty(PWM_R1, right_pwm);
        } else {
            gpio_set_level(DIR_R1, 1);
            pwm_set_duty(PWM_R1, -right_pwm);
        }
}
 *
 */


