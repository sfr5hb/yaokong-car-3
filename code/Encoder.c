/*
 * Encoder.c
 *
 *  Created on: 2026年4月14日
 *      Author: HP
 */
#include "zf_common_headfile.h"




//houlun
int32 encoder_data_2 = 0;
int32 encoder_data_3 = 0;


void Encoder_init(void)
{

    //前轮编码器实际上是一个角度位置传感器，采用spi进行通信，使用实际位置


    //引脚定位为SPI3_SCLK_P22_0，SPI3_MOSI_P22_3，SPI3_MISO_P22_1，P33_7，

     absolute_encoder_init();//绝对编码器初始化

     encoder_quad_init(ENCODER_3, ENCODER_3_A, ENCODER_3_B);
     encoder_quad_init(ENCODER_2, ENCODER_2_A, ENCODER_2_B);//后轮编码器改为正交





}

float angle=0.0f;
int16 encoder_data=0;

volatile int32 mileage_l=0;
volatile int32 mileage_r=0;

void Encoder_GET(void)
{

/*
    //绝对位置编码器数据
    encoder_data=absolute_encoder_get_location();//编码器原始值
           //int16 encoder_data_t=absolute_encoder_get_offset();//相对上一次的偏移值

           //float actual_angle=(float)encoder_data/4096.0f*360.0f;

     int16 encoder_center=2221;//实际测量的左右打死的极限值，算出中心位置
     int16 half_angle=(3042-1400)/2;

     int16 offset=encoder_data-encoder_center;
     angle=((float)offset/(float)half_angle)*45.0f;//映射到角度，不知道对不对
     */

    //绝对位置编码器数据
    encoder_data=absolute_encoder_get_location();//编码器原始值

    int16 raw_delta=encoder_data-STEER_CENTER;

    if(raw_delta > 2048)  raw_delta -= 4096;
    if(raw_delta < -2048) raw_delta += 4096;

    float encoder_deg = raw_delta * 360.0f / Encoder_CPR;
    angle = encoder_deg * Steer_RATIO;


    /*//后轮编码器测速度


    encoder_data_2 = encoder_get_count(ENCODER_2);
    encoder_clear_count(ENCODER_2);

    encoder_data_3 = -encoder_get_count(ENCODER_3);
    encoder_clear_count(ENCODER_3);*/




    int32 left = encoder_get_count(ENCODER_2);

    encoder_clear_count(ENCODER_2);

    int32 right = -encoder_get_count(ENCODER_3);

    encoder_clear_count(ENCODER_3);

    encoder_data_2=left;
    encoder_data_3=right;

    mileage_l+=left;
    mileage_r+=right;

    float alpha=0.55f;

   speed_left=alpha*speed_left+(1-alpha)*(float)encoder_data_2;
   speed_right=alpha*speed_right+(1-alpha)*(float)encoder_data_3;




}
void Encoder_GetMileageCount(int32 *left, int32 *right)
{
    uint32 interrupt_state = interrupt_global_disable();

    *left = mileage_l;
    *right = mileage_r;

    mileage_l = 0;
    mileage_r = 0;

    interrupt_global_enable(interrupt_state);
}


