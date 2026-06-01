/*
 * Motor.h
 *
 *  Created on: 2026年4月6日
 *      Author: HP
 */

#ifndef CODE_MOTOR_H_
#define CODE_MOTOR_H_


#define MAX_DUTY            (50             )                                   // 最大 MAX_DUTY% 占空比

////前轮转向电机
//#define PWM_CH1             (ATOM0_CH4_P20_3)//(ATOM0_CH0_P21_2)//21.4
//#define PWM_CH2             (ATOM0_CH6_P20_0)//(ATOM0_CH1_P21_3)//21.5

#define PWM_CH1             (ATOM0_CH0_P21_2)//21.4
#define PWM_CH2             (ATOM0_CH1_P21_3)//21.5



//后轮左右电机
#define DIR_R1              (P02_7)
#define PWM_R1              (ATOM0_CH5_P02_5)
#define DIR_L1              (P02_6)
#define PWM_L1              (ATOM0_CH4_P02_4)


//#define DIR_R1              (P21_5)
//#define PWM_R1              (ATOM0_CH1_P21_3)
//#define DIR_L1              (P21_4)
//#define PWM_L1              (ATOM1_CH0_P21_2)


extern float steer_duty;
extern float speed_left;
extern float speed_right;
extern float target_pulse;

void Motor_Init(void);
void Motor_Ctrl(float duty);

void Motor_Speed(float target_speed,uint8 reverse);


#endif /* CODE_MOTOR_H_ */
