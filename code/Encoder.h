/*
 * Encoder.h
 *
 *  Created on: 2026年4月14日
 *      Author: HP
 */

#ifndef CODE_ENCODER_H_
#define CODE_ENCODER_H_


////前轮编码器
//#define ENCODER_1                   (TIM2_ENCODER)
//#define ENCODER_1_A                 (TIM2_ENCODER_CH1_P33_7)
//#define ENCODER_1_B                 (TIM2_ENCODER_CH2_P33_6)


//右后轮编码器
#define ENCODER_3                   (TIM2_ENCODER)
#define ENCODER_3_A                 (TIM2_ENCODER_CH1_P33_7)
#define ENCODER_3_B                 (TIM2_ENCODER_CH2_P33_6)

//左后轮
#define ENCODER_2                   (TIM4_ENCODER)
#define ENCODER_2_A                 (TIM4_ENCODER_CH1_P02_8)
#define ENCODER_2_B                 (TIM4_ENCODER_CH2_P00_9)

//绝对式编码器数值定义
#define Encoder_CPR 4096.0f
#define Steer_RATIO 0.5f
#define STEER_CENTER 2043





void Encoder_init(void);
void Encoder_GET(void);

void Encoder_GetMileageCount(int32 *left, int32 *right);

extern int16 encoder_data;

extern float angle;//编码器角度值
//后轮速度值
extern int32 encoder_data_2;
extern int32 encoder_data_3;

#endif /* CODE_ENCODER_H_ */
