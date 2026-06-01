/*
 * zhufei.c
 *
 *  Created on: 2026年4月6日
 *      Author: HP
 */
#include "zf_common_headfile.h"

#define ANLEYE_OSC_CHANNEL_NUM  5
#define ANLEYE_OSC_SEND_DIV     10
static float anleye_osc_data[ANLEYE_OSC_CHANNEL_NUM];

static seekfree_assistant_oscilloscope_struct anleye_osc;
static seekfree_assistant_debug_param_struct anleye_param;

//float target_yaw=0;//目标航向角

//extern float speed_left;
//extern float speed_right;
//extern float test_speed;
//extern float test_yaw;
//extern float test_angle;//前轮电机期望转向值
//float test_angle=0.0f;


void Anleye_ch(void)
{
    static uint8 init_flag = 0;
    static uint8 send_cnt = 0;

    if(0 == init_flag)
    {
        memset(&anleye_param, 0, sizeof(anleye_param));
        seekfree_assistant_oscilloscope_config(&anleye_osc, ANLEYE_OSC_CHANNEL_NUM, anleye_osc_data);
        init_flag = 1;
    }

    seekfree_assistant_debug_param_analysis(&anleye_param);

    if(anleye_param.update_flag[0])
    {
        NavPID.kp = anleye_param.data[0];
        anleye_param.update_flag[0] = 0;
    }

    if(anleye_param.update_flag[1])
    {
        NavPID.ki = anleye_param.data[1];
        anleye_param.update_flag[1] = 0;
    }

    if(anleye_param.update_flag[2])
    {
        NavPID.kd = anleye_param.data[2];
        anleye_param.update_flag[2] = 0;
    }

    if(anleye_param.update_flag[7])
    {
        //test_yaw = anleye_param.data[7];
        anleye_param.update_flag[7] = 0;
    }
//    if(anleye_param.update_flag[3])
//    {
//        RightPID.kp=anleye_param.data[3];
//        anleye_param.update_flag[3]=0;
//    }
//    if(anleye_param.update_flag[4])
//    {
//        RightPID.ki=anleye_param.data[4];
//        anleye_param.update_flag[4]=0;
//    }
//    if(anleye_param.update_flag[5])
//    {
//        RightPID.kd=anleye_param.data[5];
//        anleye_param.update_flag[5]=0;
//    }

    send_cnt++;
    if(send_cnt < ANLEYE_OSC_SEND_DIV)
    {
        return;
    }
    send_cnt = 0;
//
//    anleye_osc_data[0] = test_angle;
//    anleye_osc_data[1] = angle;
//    anleye_osc_data[2] = SteerPID.output;
    //anleye_osc_data[0] = test_yaw;
    anleye_osc_data[1] = yaw;
    anleye_osc_data[2] = angle;


    anleye_osc_data[3]=SteerPID.output;////返回最终占空比

    anleye_osc_data[4] = NavPID.out_put;//返回转向电机需要转过的角度

    //anleye_osc_data[3] = SteerPID.output;


    seekfree_assistant_oscilloscope_send(&anleye_osc);
}
