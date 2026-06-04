/*
 * subject1.c
 *  科目一：读取Flash路径（Page9/10/11），自动导航
 *
 *  按键分配：
 *    KEY1：开始记录路径（前一天打点用）
 *    KEY2：停止记录，保存Flash
 *    KEY3：开始自动导航复现
 *    KEY4：退出回菜单
 */

#include "zf_common_headfile.h"

// ==================== 数据 ====================
static double nav_x[MAX_PATH_POINTS];
static double nav_y[MAX_PATH_POINTS];
static int16  nav_num       = 0;
static double nav_cur_x     = 0.0;
static double nav_cur_y     = 0.0;
static float  nav_yaw_zero  = 0.0f;
static float  nav_last_yaw  = 0.0f;
static float  nav_accum     = 0.0f;
static int16  nav_idx       = 0;

// 路径结构体，绑定上面的数据
static path_t nav_path = {
    .x               = nav_x,
    .y               = nav_y,
    .num             = &nav_num,
    .cur_x           = &nav_cur_x,
    .cur_y           = &nav_cur_y,
    .yaw_zero        = &nav_yaw_zero,
    .last_yaw        = &nav_last_yaw,
    .accum_dist      = &nav_accum,
    .target_idx      = &nav_idx,
    .record_interval = 0.80f,
    .arrive_dist     = 0.50f,
    .direction       = PATH_FORWARD,
};

//// Flash存取用的外部接口（flash.c里实现）
//extern void nav_flash_save(double *x, double *y, int16 num);
//extern void nav_flash_load(double *x, double *y, int16 *num);

// 状态
static uint8 s1_record_mode = 0;
static uint8 s1_replay_mode = 0;

// 倒车（科目一独有）
static uint8  daoche_flag    = 0;
static float  daoche_actdist = 0.0f;
#define DAOCHE_DIST 0.4f


const float mileage_me_l=0.000429f;
const float mileage_me_r=0.000424f;
float DELTA_D(void)
{
    int32 count_l=0;
    int32 count_r=0;

    Encoder_GetMileageCount(&count_l, &count_r);

    //左右俩轮取平均
    // float d_l=(float)mileage_l*mileage_me_l;
    // float d_r=(float)mileage_r*mileage_me_r;

    float d_l = (float)count_l * mileage_me_l;
    float d_r = (float)count_r * mileage_me_r;

    float delta_d=(d_l+d_r)*0.5f;

    return delta_d;

}

// ==================== 主任务 ====================
void subject1_task(void)
{
    // KEY1：开始记录



    if(key1_flag)
    {
        key1_flag = 0;
        if(s1_record_mode == 0 && s1_replay_mode == 0)
        {
            s1_record_mode = 1;
            nav_num        = 0;
            nav_cur_x      = 0.0;
            nav_cur_y      = 0.0;
            nav_yaw_zero   = yaw;
            nav_last_yaw   = 0.0f;
            nav_accum      = 0.0f;

            int32 cl = 0, cr = 0;
            Encoder_GetMileageCount(&cl, &cr);

            ips200_clear();
            ips200_show_string(0, 0, "Recording...");
        }
        else if(s1_record_mode == 1)
        {
            // 再按一次停止记录
            s1_record_mode = 0;
            Motor_Speed(0.0f, 1);
            nav_flash_save(nav_x, nav_y, nav_num);
            //ips200_clear();
            ips200_show_string(0,  0, "Saved");
            ips200_show_int(0,    20, nav_num, 3);
        }
    }

    // KEY2：开始复现
    if(key2_flag)
    {
        key2_flag = 0;
        if(s1_record_mode == 0 && s1_replay_mode == 0 && nav_num > 0)
        {
            s1_replay_mode = 1;
            nav_idx        = 0;
            nav_cur_x      = 0.0;
            nav_cur_y      = 0.0;
            nav_yaw_zero   = yaw;
            nav_last_yaw   = 0.0f;
            nav_path.direction = PATH_FORWARD;

            int32 cl = 0, cr = 0;
            Encoder_GetMileageCount(&cl, &cr);

            //ips200_clear();
            ips200_show_string(0, 0, "Replaying...");
        }
    }

    // ===== 核心逻辑 =====
        // DELTA_D只调用一次，避免重复消费编码器数据
    float delta_d = 0.0f;
    if(s1_record_mode || s1_replay_mode || daoche_flag)
        delta_d = DELTA_D();

    // 位置更新
    if(s1_record_mode || s1_replay_mode)
        post_update(&nav_path, delta_d);

    // 记录路径
    if(s1_record_mode)
    {
        path_record(&nav_path, delta_d);
        ips200_show_int(0, 20, nav_num, 3);
    }

    // 复现路径
    if(s1_replay_mode)
    {
        int still_going = path_replay(&nav_path);
        if(still_going)
        {
            Motor_Speed(20.0f, 1);
            ips200_show_int(0,   20, nav_idx, 3);
            ips200_show_float(0, 40, (float)nav_cur_x, 2, 3);
            ips200_show_float(0, 60, (float)nav_cur_y, 2, 3);
        }
        else
        {
            // 复现完成，触发倒车
            s1_replay_mode = 0;
            daoche_flag    = 1;
            daoche_actdist = 0.0f;
            Motor_Ctrl(SteerAngle_Ctrl(0.0f, angle));
            //ips200_clear();
            ips200_show_string(0, 100, "Nav Done, Daoche");
        }
    }

    // 倒车（科目一独有）
    if(daoche_flag)
    {
        daoche_actdist += fabsf(delta_d);
        Motor_Ctrl(SteerAngle_Ctrl(0.0f, angle));
        Motor_Speed(5.0f, 0);
        if(daoche_actdist >= DAOCHE_DIST)
        {
            daoche_flag = 0;
            Motor_Speed(0.0f, 0);
            Motor_Ctrl(0);
            ips200_clear();
            ips200_show_string(0, 0, "Done!");
        }
    }
}

void subject1_stop(void)
{
    s1_record_mode = 0;
    s1_replay_mode = 0;
    daoche_flag    = 0;
    daoche_actdist = 0.0f;
    Motor_Speed(0.0f, 1);
    Motor_Ctrl(SteerAngle_Ctrl(0.0f, angle));
}

// Flash存取接口，供flash.c使用
double *subject1_get_x(void)   { return nav_x; }
double *subject1_get_y(void)   { return nav_y; }
int16   subject1_get_num(void) { return nav_num; }
void    subject1_set_num(int16 n) { nav_num = n; }
