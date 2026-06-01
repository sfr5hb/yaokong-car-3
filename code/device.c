/*
 * device.c
 *
 *  Created on: 2026锟斤拷4锟斤拷12锟斤拷
 *      Author: HP
 */

#include "zf_common_headfile.h"

uint8 key1_flag=0;
uint8 key2_flag=0;
uint8 key3_flag=0;
uint8 key4_flag=0;
uint8 switch1_flag = 0;
uint8 switch2_flag = 0;

uint8 key1_state_last = 0;            //锟斤拷一锟轿帮拷锟斤拷锟斤拷锟斤拷状态
uint8 key2_state_last = 0;
uint8 key3_state_last = 0;
uint8 key4_state_last = 0;
uint8 switch1_state_last = 0;         //锟斤拷一锟轿诧拷锟诫开锟截讹拷锟斤拷状态
uint8 switch2_state_last = 0;

uint8 key1_state = 1;
uint8 key2_state = 1;
uint8 key3_state = 1;
uint8 key4_state = 1;
uint8 switch1_state = 0;
uint8 switch2_state = 0;



//void ALL_Init(void)
//{
////    scc8660_init();
//    Encoder_init();
//    scc8660_init();
//
//    ips200_init(IPS200_TYPE_SPI);//屏幕初始化
//   // Encoder_init();//编码器初始化
//
//   // Encoder_init();//编码器初始化
//
//    Motor_PID_Init();//后轮电机初始化
//    SteerAngle_Init();//转向电机角度控制初始化
//    _Nav_PID_init(); // 必须初始化外环
//    Motor_Init();//电机初始化
//
//
////        // 初始化编码器模块与引脚 正交解码编码器模式
//   imu660ra_init();//陀螺仪初始化
//
//}



void mykey_Init(void)      //按键与LED的初始化
{
    //gpio_init(LED1, GPO, GPIO_LOW, GPO_PUSH_PULL);          // 初始化 LED1 输出 默认高电平 推挽输出模式
   // gpio_init(LED2, GPO, GPIO_HIGH, GPO_PUSH_PULL);         // 初始化 LED2 输出 默认高电平 推挽输出模式
    //gpio_init(LED3, GPO, GPIO_LOW, GPO_PUSH_PULL);          // 初始化 LED3 输出 默认高电平 推挽输出模式
    //gpio_init(LED4, GPO, GPIO_HIGH, GPO_PUSH_PULL);         // 初始化 LED4 输出 默认高电平 推挽输出模式

    gpio_init(KEY1, GPI, GPIO_HIGH, GPI_PULL_UP);           // 初始化 KEY1 输入 默认高电平 上拉输入
    gpio_init(KEY2, GPI, GPIO_HIGH, GPI_PULL_UP);           // 初始化 KEY2 输入 默认高电平 上拉输入
    gpio_init(KEY3, GPI, GPIO_HIGH, GPI_PULL_UP);           // 初始化 KEY3 输入 默认高电平 上拉输入
    gpio_init(KEY4, GPI, GPIO_HIGH, GPI_PULL_UP);           // 初始化 KEY4 输入 默认高电平 上拉输入

    gpio_init(SWITCH1, GPI, GPIO_HIGH, GPI_PULL_UP);        // 初始化 SWITCH1 输入 默认高电平 上拉输入
    gpio_init(SWITCH2, GPI, GPIO_HIGH, GPI_PULL_UP);        // 初始化 SWITCH2 输入 默认高电平 上拉输入
}

void key_scan(void)        //按键扫描
{
    //保存按键状态
    key1_state_last = key1_state;
    key2_state_last = key2_state;
   key3_state_last = key3_state;
   key4_state_last = key4_state;



    //读取当前按键状态、按键按下为低电平，松开为高电平
    key1_state = gpio_get_level(KEY1);
    key2_state = gpio_get_level(KEY2);
   key3_state = gpio_get_level(KEY3);
   key4_state = gpio_get_level(KEY4);



    //检测到按键按下之后，并放开置标志位
    //放开后当前按键状态key1_state为1；上一次按键状态为按下0即key1_state_last == 0,取反为1。
    if(key1_state && !key1_state_last) {key1_flag = 1;}
    if(key2_state && !key2_state_last) {key2_flag = 1;}
    if(key3_state && !key3_state_last) {key3_flag = 1;}
    if(key4_state && !key4_state_last) {key4_flag = 1;}


  //system_delay_ms(10);
}


