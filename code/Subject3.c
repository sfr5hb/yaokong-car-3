/*
 * Subject3.c
 *
 *  Created on: 2026年6月2日
 *      Author: HP
 */

#include "zf_common_headfile.h"


// ==================== 数据 ====================
static double rc_x[FOLLOW_MAX_POINTS];
static double rc_y[FOLLOW_MAX_POINTS];
static int16  rc_num       = 0;
static double rc_cur_x     = 0.0;
static double rc_cur_y     = 0.0;
static float  rc_yaw_zero  = 0.0f;  // 记录时设置，返回时不重置
static float  rc_last_yaw  = 0.0f;
static float  rc_accum     = 0.0f;
static int16  rc_idx       = 0;

static path_t rc_path = {
    .x               = rc_x,
    .y               = rc_y,
    .num             = &rc_num,
    .cur_x           = &rc_cur_x,
    .cur_y           = &rc_cur_y,
    .yaw_zero        = &rc_yaw_zero,
    .last_yaw        = &rc_last_yaw,
    .accum_dist      = &rc_accum,
    .target_idx      = &rc_idx,
    .record_interval = 0.80f,
    .arrive_dist     = 0.80f,
    .direction       = PATH_REVERSE,  // 科目三固定倒序
};


//状态

//static uint8 s3_drive_mode  = 0;  // 遥控驾驶+记录中
static uint8 s3_return_mode = 0;  // 原路返回中

//测试
static uint8 s3_record_mode=0;

////遥控驾驶
//static void s3_drive(void)
//{
//    if(uart_receiver.state == 0)
//    {
//        Motor_Speed(0.0f, 1);
//        Motor_Ctrl(SteerAngle_Ctrl(0.0f, angle));
//        ips200_show_string(0, 80, "RC Lost!");
//        return;
//    }
//
//    // 转向
//    int16 steer_raw = (int16)uart_receiver.channel[RC_CH_STEER] - 1024;
//    if(steer_raw > RC_STEER_DEAD || steer_raw < -RC_STEER_DEAD)
//        Motor_Ctrl(SteerAngle_Ctrl((float)steer_raw / 1024.0f * RC_STEER_MAX, angle));
//    else
//        Motor_Ctrl(SteerAngle_Ctrl(0.0f, angle));
//
//    // 油门，只前进
//    int16 throttle_raw = (int16)uart_receiver.channel[RC_CH_THROTTLE] - 1024;
//    if(throttle_raw > RC_THROTTLE_DEAD)
//        Motor_Speed((float)throttle_raw / 1024.0f * RC_SPEED_MAX, 1);
//    else
//        Motor_Speed(0.0f, 1);
//}


// ==================== 主任务 ====================
void subject3_task(void)
{
    // KEY1：开始遥控驾驶+记录
    if(key1_flag)
    {
        key1_flag = 0;
        if(s3_record_mode == 0 && s3_return_mode == 0)
        {
            s3_record_mode = 1;
            rc_num        = 0;
            rc_cur_x      = 0.0;
            rc_cur_y      = 0.0;
            rc_yaw_zero   = yaw;   // 记录坐标系基准，返回时不再改变
            rc_last_yaw   = 0.0f;
            rc_accum      = 0.0f;

            int32 cl = 0, cr = 0;
            Encoder_GetMileageCount(&cl, &cr);

            ips200_clear();
            ips200_show_string(0,  0, "RC Drive...");
            ips200_show_string(0, 20, "K2:Stop&Save");
        }
    }

    // KEY2：结束记录，保存Flash
    if(key2_flag)
    {
        key2_flag = 0;
        if(s3_record_mode == 1)
        {
            s3_record_mode = 0;
            Motor_Speed(0.0f, 1);
            Motor_Ctrl(SteerAngle_Ctrl(0.0f, angle));
            rc_flash_save(rc_x, rc_y, rc_num);
            ips200_clear();
            ips200_show_string(0,  0, "Saved");
            ips200_show_int(0,    20, rc_num, 3);
            ips200_show_string(0, 40, "Flip car");
            ips200_show_string(0, 60, "K3:Return");
        }
    }

    // KEY3：掉头完成，开始原路返回
    if(key3_flag)
    {
        key3_flag = 0;
        if(s3_record_mode == 0 && s3_return_mode == 0 && rc_num > 0)
        {
            s3_return_mode = 1;
            rc_idx         = rc_num - 1;  // 从最后一个点倒序
            // yaw_zero不重置，cur_x/y不重置，坐标系全程统一

            int32 cl = 0, cr = 0;
            Encoder_GetMileageCount(&cl, &cr);

            ips200_clear();
            ips200_show_string(0, 0, "Returning...");
        }
    }

    // ===== 核心逻辑 =====
    float delta_d = 0.0f;
    if(s3_record_mode || s3_return_mode)
        delta_d = DELTA_D();

    // 位置更新，遥控和返回阶段都需要
    if(s3_record_mode || s3_return_mode)
        post_update(&rc_path, delta_d);

    // 遥控驾驶+记录
    if(s3_record_mode)
    {
        //s3_drive();
        path_record(&rc_path, delta_d);
        ips200_show_int(0, 20, rc_num, 3);
    }

    // 原路返回
    if(s3_return_mode)
    {
        int still_going = path_replay(&rc_path);
        if(still_going)
        {
            Motor_Speed(15.0f, 1);
            ips200_show_int(0,   20, rc_idx, 3);
            ips200_show_float(0, 40, (float)rc_cur_x, 2, 3);
            ips200_show_float(0, 60, (float)rc_cur_y, 2, 3);
        }
        else
        {
            s3_return_mode = 0;
            Motor_Speed(0.0f, 1);
            Motor_Ctrl(SteerAngle_Ctrl(0.0f, angle));
            ips200_clear();
            ips200_show_string(0, 0, "Return Done!");
        }
    }
}

void subject3_stop(void)
{
    s3_record_mode  = 0;
    s3_return_mode = 0;
    Motor_Speed(0.0f, 1);
    Motor_Ctrl(SteerAngle_Ctrl(0.0f, angle));
}

// Flash存取接口
double *subject3_get_x(void)      { return rc_x; }
double *subject3_get_y(void)      { return rc_y; }
int16   subject3_get_num(void)    { return rc_num; }
void    subject3_set_num(int16 n) { rc_num = n; }






















