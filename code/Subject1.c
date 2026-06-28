/*
 * subject1.c
 *  科目一：读取Flash路径（Page9/10/11），自动导航
 *
 *  按键分配：
 *    KEY1：开始记录路径（前一天打点用）
 *    KEY1：停止记录，保存Flash
 *    KEY3：开始自动导航复现
 *    KEY4：退出回菜单
 */

#include "zf_common_headfile.h"

//// ==================== 数据 ====================
//static double nav_x[MAX_PATH_POINTS];
//static double nav_y[MAX_PATH_POINTS];
//static int16  nav_num       = 0;
//static double nav_cur_x     = 0.0;
//static double nav_cur_y     = 0.0;
//static float  nav_yaw_zero  = 0.0f;
//static float  nav_last_yaw  = 0.0f;
//static float  nav_accum     = 0.0f;
//static int16  nav_idx       = 0;
//
//// 路径结构体，绑定上面的数据
//static path_t nav_path = {
//    .x               = nav_x,
//    .y               = nav_y,
//    .num             = &nav_num,
//    .cur_x           = &nav_cur_x,
//    .cur_y           = &nav_cur_y,
//    .yaw_zero        = &nav_yaw_zero,
//    .last_yaw        = &nav_last_yaw,
//    .accum_dist      = &nav_accum,
//    .target_idx      = &nav_idx,
//    .record_interval = 1.20f,//0.80f,
//    .arrive_dist     = 0.80f,//1.20f,
//    .direction       = PATH_FORWARD,
//
//    .point_flag      = 0,
//    .nav_out_1    = 0.0f,
//};
//
////// Flash存取用的外部接口（flash.c里实现）
////extern void nav_flash_save(double *x, double *y, int16 num);
////extern void nav_flash_load(double *x, double *y, int16 *num);
//
//// 状态
//uint8 s1_record_mode = 0;
//uint8 s1_replay_mode = 0;
//
//// 倒车（科目一独有）
//static uint8  daoche_flag    = 0;
//static float  daoche_actdist = 0.0f;
//#define DAOCHE_DIST 1.2f
//
//
//const float mileage_me_l=0.000429f;
//const float mileage_me_r=0.000424f;
//
//
//
//float speed_min=40.0f;//20-1.7
//float speed_max=50.0f;//2.5
//float lo = 8.0f;            // 小于此角度:全速
//float hi = 35.0f;            // 大于此角度:最低速
//
//
//float DELTA_D(void)
//{
//    int32 count_l=0;
//    int32 count_r=0;
//
//    Encoder_GetMileageCount(&count_l, &count_r);
//
//    //左右俩轮取平均
//    // float d_l=(float)mileage_l*mileage_me_l;
//    // float d_r=(float)mileage_r*mileage_me_r;
//
//    float d_l = (float)count_l * mileage_me_l;
//    float d_r = (float)count_r * mileage_me_r;
//
//    float delta_d=(d_l+d_r)*0.5f;
//
//    return delta_d;
//
//}
//
//// ==================== 主任务 ====================
//void subject1_task(void)
//{
//    // KEY1：开始记录
//
//
//
//    if(key1_flag)
//    {
//        key1_flag = 0;
//        if(s1_record_mode == 0 && s1_replay_mode == 0)
//        {
//            s1_record_mode = 1;
//            nav_num        = 0;
//            nav_cur_x      = 0.0;
//            nav_cur_y      = 0.0;
//            nav_yaw_zero   = yaw;
//            nav_last_yaw   = 0.0f;
//            nav_accum      = 0.0f;
//
//            int32 cl = 0, cr = 0;
//            Encoder_GetMileageCount(&cl, &cr);
//
//            ips200_clear();
//            ips200_show_string(0, 0, "Recording...");
//        }
//        else if(s1_record_mode == 1)
//        {
//            // 再按一次停止记录
//            s1_record_mode = 0;
//            Motor_Speed(0.0f, 1);
//            nav_flash_save(nav_x, nav_y, nav_num);
//            //ips200_clear();
//            ips200_show_string(0,  0, "Saved");
//            ips200_show_int(0,    20, nav_num, 3);
//        }
//    }
//
//    // KEY2：开始复现
//    if(key2_flag)
//    {
//        key2_flag = 0;
//        if(s1_record_mode == 0 && s1_replay_mode == 0 && nav_num > 0)
//        {
//            s1_replay_mode = 1;
//            nav_idx        = 0;
//            nav_cur_x      = 0.0;
//            nav_cur_y      = 0.0;
//            nav_yaw_zero   = yaw;
//            nav_last_yaw   = 0.0f;
//            nav_path.direction = PATH_FORWARD;
//
//            int32 cl = 0, cr = 0;
//            Encoder_GetMileageCount(&cl, &cr);
//
//            //ips200_clear();
//            ips200_show_string(0, 0, "Replaying...");
//        }
//    }
//
//    // 核心逻辑
//        // DELTA_D只调用一次，避免重复消费编码器数据
//    float delta_d = 0.0f;
//    if(s1_record_mode || s1_replay_mode || daoche_flag)
//        delta_d = DELTA_D();
//
//    // 位置更新
//    if(s1_record_mode || s1_replay_mode)
//        post_update(&nav_path, delta_d);
//
//    // 记录路径
//    if(s1_record_mode)
//    {
//        Remote_Ctrl();//遥控控制转向加速度
//        path_record(&nav_path, delta_d);
//        ips200_show_int(0, 20, nav_num, 3);
//    }
//
//    // 复现路径
//    if(s1_replay_mode)
//    {
//        int still_going = path_replay(&nav_path);
//        if(still_going)
//        {
//           // Motor_Speed(20.0f, 1);
//            /*
//            float err=fabsf(nav_path.nav_out_1);
//            float speed_c;
//            if(err>30.0f)  speed_c=10.0f;
//            else if(err>20.0f)  speed_c=15.0f;
//            else speed_c=20.0f;//1.7m/s*/
//
//
//             // 丝滑降速（余弦平滑降速）
//
//             //float err=fabsf(nav_path.nav_out_1);
//             float err=fabsf(angle);
//
//             float factor;
//
//
//             if(err <= lo)       factor = 1.0f;//factor=cos((err / lo) * (3.1415926535f / 2));
//             else if(err >= hi)  factor = 0.0f;
//             else                factor =
//                                    cosf((err - lo) / (hi - lo) * (3.1415926535f / 2.0f));
//
//             float speed_c = speed_min + (speed_max - speed_min) * factor;
//
//
//
//
//            Motor_Speed(speed_c, 1);
//
//
//            //复现关键参数
//            ips200_show_string(0, 16*0, "idx");     ips200_show_int  (96, 16*0, nav_idx, 3);//当前目标点序号
//
//            ips200_show_string(0, 16*1, "num");     ips200_show_int  (96, 16*1, nav_num, 3);//总点数
//            ips200_show_string(0, 16*2, "cur_x");   ips200_show_float(96, 16*2, (float)nav_cur_x, 3, 2);//当前位置x
//            ips200_show_string(0, 16*3, "cur_y");   ips200_show_float(96, 16*3, (float)nav_cur_y, 3, 2);//当前位置y
//            ips200_show_string(0, 16*4, "tgt_x");   ips200_show_float(96, 16*4, (float)nav_x[nav_idx], 3, 2);//目标点x
//            ips200_show_string(0, 16*5, "tgt_y");   ips200_show_float(96, 16*5, (float)nav_y[nav_idx], 3, 2);//目标点y
//            ips200_show_string(0, 16*6, "navout");  ips200_show_float(96, 16*6, nav_path.nav_out_1, 3, 2);//导航输出
//            ips200_show_string(0, 16*7, "Stgt");    ips200_show_float(96, 16*7, Steer_target, 3, 2);//转向指令
//            ips200_show_string(0, 16*8, "angle");   ips200_show_float(96, 16*8, angle, 3, 2);//实际转角
//            ips200_show_string(0, 16*9, "yawrel");  ips200_show_float(96, 16*9, yaw - nav_yaw_zero, 3, 2);//相对航向
//            ips200_show_string(0, 16*10,"spd_c");   ips200_show_float(96, 16*10, speed_c, 3, 2);//目标速度
//            ips200_show_string(0, 16*11,"spdL");    ips200_show_float(96, 16*11, speed_left, 3, 2);//左轮速度
//            ips200_show_string(0, 16*12,"spdR");    ips200_show_float(96, 16*12, speed_right, 3, 2);//右轮速度
//            ips200_show_string(0, 16*13,"pflag");   ips200_show_int  (96, 16*13, nav_path.point_flag, 1);//切点标志
//
////
////
////
////            ips200_show_int(0,   20, nav_idx, 3);
////            ips200_show_float(0, 40, (float)nav_cur_x, 2, 3);
////            ips200_show_float(0, 60, (float)nav_cur_y, 2, 3);
////            ips200_show_float(0, 80, nav_path.nav_out_1, 2, 2);
//
//        }
//        else
//        {
//            // 复现完成，触发倒车
//            s1_replay_mode = 0;
//            daoche_flag    = 1;
//            daoche_actdist = 0.0f;
//            Steer_target=0.0f;
//            //Motor_Ctrl(SteerAngle_Ctrl(0.0f, angle));
//            //ips200_clear();
//           // ips200_show_string(0, 100, "Nav Done, Daoche");
//        }
//    }
//
//    // 倒车（科目一独有）
//    if(daoche_flag)
//    {
//        daoche_actdist += fabsf(delta_d);
//        Steer_target=0.0f;
//       // Motor_Ctrl(SteerAngle_Ctrl(0.0f, angle));
//        Motor_Speed(15.0f, 0);
//        if(daoche_actdist >= DAOCHE_DIST)
//        {
//            daoche_flag = 0;
//            Motor_Speed(0.0f, 0);
//            Steer_target = 0.0f;
//           // Motor_Ctrl(0);
//            ips200_clear();
//            ips200_show_string(0, 0, "Done!");
//        }
//    }
//}
//
//void subject1_stop(void)
//{
//    s1_record_mode = 0;
//    s1_replay_mode = 0;
//    daoche_flag    = 0;
//    daoche_actdist = 0.0f;
//    Motor_Speed(0.0f, 1);
//    Steer_target=0.0f;
//    //Motor_Ctrl(SteerAngle_Ctrl(0.0f, angle));
//}
//
//// Flash存取接口，供flash.c使用
//double *subject1_get_x(void)   { return nav_x; }
//double *subject1_get_y(void)   { return nav_y; }
//int16   subject1_get_num(void) { return nav_num; }
//void    subject1_set_num(int16 n) { nav_num = n; }
//
//



































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
static uint8  nav_map_view  = 1;


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
    .record_interval = 1.20f,//0.80f,
    .arrive_dist     = 0.80f,//1.20f,
    .direction       = PATH_FORWARD,

    .point_flag      = 0,
    .nav_out_1    = 0.0f,
};

//// Flash存取用的外部接口（flash.c里实现）
//extern void nav_flash_save(double *x, double *y, int16 num);
//extern void nav_flash_load(double *x, double *y, int16 *num);

// 状态
uint8 s1_record_mode = 0;
uint8 s1_replay_mode = 0;

// 倒车（科目一独有）
//static uint8  daoche_flag    = 0;
//static float  daoche_actdist = 0.0f;
//#define DAOCHE_DIST 1.2f

uint8 park_on=0;
park_t park_st=PARK_IDLE;
float park_dist=0.0f;
uint16 park_wait=0;
float park_yaw0=0.0f;


//daoche









const float mileage_me_l=0.000429f;
const float mileage_me_r=0.000424f;



float speed_min=40.0f;//20-1.7
float speed_max=50.0f;//2.5
float lo = 8.0f;            // 小于此角度:全速
float hi = 35.0f;            // 大于此角度:最低速

static int16 s1_last_draw_num = -1;
static uint8 s1_replay_draw_div = 0;


//地图显示函数
static void subject1_render_map(uint8 show_current)
{
    path_display_init(0, 0, 240, 300);//如果显示有问题，就把地图高度调小到280
    path_display_render(&nav_path, show_current);
}
void show_pre(void)
{
    nav_map_view = 1;
    nav_idx = 0;
    subject1_render_map(0);
    ips200_show_string(120, 304, "K1 Rec K2 Go");

}
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

void One_Start(void)//按键一科目一开始
{
    s1_record_mode = 1;
    nav_num        = 0;
    nav_cur_x      = 0.0;
    nav_cur_y      = 0.0;
    nav_yaw_zero   = yaw;
    nav_last_yaw   = 0.0f;
    nav_accum      = 0.0f;
    nav_map_view   = 1;
    s1_last_draw_num=-1;

    subject1_render_map(1);
    ips200_show_string(120, 304, "Recording");

}

void Record_Stop(void)//按键一停止记录
{
    s1_record_mode = 0;

    Motor_Speed(0.0f, 1);
    nav_flash_save(nav_x, nav_y, nav_num);

    subject1_render_map(0);
    ips200_show_string(120, 304, "Saved");
    ips200_show_int(176, 304, nav_num, 3);

}

void Replay_Start(void)//按键二富先开始
{
    s1_replay_mode = 1;
    nav_idx        = 0;
    nav_cur_x      = 0.0;
    nav_cur_y      = 0.0;
    nav_yaw_zero   = yaw;
    nav_last_yaw   = 0.0f;
    nav_path.direction = PATH_FORWARD;
    s1_replay_draw_div = 0;

    if(nav_map_view)
        subject1_render_map(1);
    else
        ips200_show_string(0, 0, "Replaying...");

}

void Map_display(void)
{
    nav_map_view = !nav_map_view;
    ips200_clear();

    if(nav_map_view)
        subject1_render_map((s1_record_mode || s1_replay_mode) ? 1 : 0);

}

//按键判断函数
void Key_handel(void)
{
    if(key1_flag)
    {
        key1_flag=0;
        if(s1_record_mode==0&&s1_replay_mode==0&& park_on == 0)
        {
            One_Start();
        }
        else if(s1_record_mode==1)
        {
            Record_Stop();
        }
    }

    if(key2_flag)
    {
        key2_flag=0;
        if(s1_record_mode==0&&s1_replay_mode==0&& park_on == 0&&nav_num>0)
        {
            Replay_Start();
        }

    }
    if(key3_flag)
    {
        key3_flag=0;
        Map_display();
    }

}

//速度计算

float speed_calc(void)
{
    float err=fabsf(angle);
    float factor;

    if(err<=lo){factor=1.0f;}
    else if(err>=hi){factor=0.0f;}
    else{factor = cosf((err - lo) / (hi - lo) * (3.1415926535f / 2.0f));}

    return speed_min + (speed_max - speed_min) * factor;


}
//记录

void record_step(float delta_d)
{

    Remote_Ctrl();
    path_record(&nav_path,delta_d);

    if(nav_map_view)
    {
        if(s1_last_draw_num != nav_num)
         {
            s1_last_draw_num = nav_num;
            subject1_render_map(1);
            ips200_show_string(120, 304, "Recording");
         }
    }
    else
    {
        ips200_show_int(0, 20, nav_num, 3);
    }

    if(s1_record_mode == 0)
    {
        s1_last_draw_num = -1;
    }


}

void show_can(float speed_c)
{
    ips200_show_string(0, 16*0, "idx");     ips200_show_int  (96, 16*0, nav_idx, 3);
    ips200_show_string(0, 16*1, "num");     ips200_show_int  (96, 16*1, nav_num, 3);
    ips200_show_string(0, 16*2, "cur_x");   ips200_show_float(96, 16*2, (float)nav_cur_x, 3, 2);
    ips200_show_string(0, 16*3, "cur_y");   ips200_show_float(96, 16*3, (float)nav_cur_y, 3, 2);
    ips200_show_string(0, 16*4, "tgt_x");   ips200_show_float(96, 16*4, (float)nav_x[nav_idx], 3, 2);
    ips200_show_string(0, 16*5, "tgt_y");   ips200_show_float(96, 16*5, (float)nav_y[nav_idx], 3, 2);
    ips200_show_string(0, 16*6, "navout");  ips200_show_float(96, 16*6, nav_path.nav_out_1, 3, 2);
    ips200_show_string(0, 16*7, "Stgt");    ips200_show_float(96, 16*7, Steer_target, 3, 2);
    ips200_show_string(0, 16*8, "angle");   ips200_show_float(96, 16*8, angle, 3, 2);
    ips200_show_string(0, 16*9, "yawrel");  ips200_show_float(96, 16*9, yaw - nav_yaw_zero, 3, 2);
    ips200_show_string(0, 16*10,"spd_c");   ips200_show_float(96, 16*10, speed_c, 3, 2);
    ips200_show_string(0, 16*11,"spdL");    ips200_show_float(96, 16*11, speed_left, 3, 2);
    ips200_show_string(0, 16*12,"spdR");    ips200_show_float(96, 16*12, speed_right, 3, 2);
    ips200_show_string(0, 16*13,"pflag");   ips200_show_int  (96, 16*13, nav_path.point_flag, 1);

}

//fuxian

void replay_step(void)
{
    int still_going=path_replay(&nav_path);

    if(still_going)
    {
        float speed_c=speed_calc();

        Motor_Speed(speed_c,1);

        if(nav_map_view)
        {
            if((s1_replay_draw_div++ % 5) == 0)
            subject1_render_map(1);
        }
        else
        {
            show_can(speed_c);
        }

    }

    else
    {
        s1_replay_mode = 0;
       // daoche_flag    = 1;
        //daoche_actdist = 0.0f;
        Steer_target   = 0.0f;
        Motor_Speed(0.0f,1);
        park_start();
    }

}


//倒车实现1
void park_next(park_t st)
{
    park_st=st;
    park_dist=0.0f;
}

float angle180(float a)
{
    while(a>180.0f)a-=360.0f;
    while(a<-180.0f)a+=360.0f;
    return a;
}
void park_start(void)//倒车开始
{
    park_on=1;
    park_st=PARK_WAIT;
    park_dist=0.0f;
    park_wait=0;
    park_yaw0=yaw-nav_yaw_zero;//倒车时的初始角度

    Steer_target=0.0f;
    Motor_Speed(0.0f,0);
}
void park_stop(void)
{
    park_on = 0;
    park_st = PARK_IDLE;
    park_dist = 0.0f;
    Steer_target = 0.0f;
    Motor_Speed(0.0f, 0);
    ips200_clear();
    ips200_show_string(0, 0, "Park Done");

}


void park_step(float delta_d)
{
    float d=fabsf(delta_d);
    float yaw_now;
    float yaw_err;

    switch(park_st)
    {
        case PARK_WAIT:
            Steer_target=0.0f;
            Motor_Speed(0.0f,0);

            park_wait++;
            if(park_wait>=PARK_WAIT_T)
                park_next(PARK_BACK);
            break;

        case PARK_BACK:
            park_dist += d;
            Steer_target = 0.0f;
            Motor_Speed(PARK_SPEED, 0);

            if(park_dist >= PARK_BACK_D)
                park_next(PARK_IN);
            break;

        case PARK_IN:
            park_dist += d;
            Steer_target = PARK_IN_A;
            Motor_Speed(PARK_SPEED, 0);

            if(park_dist >= PARK_IN_D)
                park_next(PARK_FIX);
            break;

        case PARK_FIX:
            park_dist += d;
            yaw_now = yaw - nav_yaw_zero;
            yaw_err = angle180(yaw_now - park_yaw0);

            Steer_target = PARK_FIX_A;
            Motor_Speed(PARK_SPEED, 0);

            if(park_dist >= PARK_FIX_D || fabsf(yaw_err) < 3.0f)
               park_next(PARK_LAST);
            break;

        case PARK_LAST:
            park_dist += d;
            Steer_target = 0.0f;
            Motor_Speed(PARK_SPEED, 0);

            if(park_dist >= PARK_LAST_D)
                park_stop();
            break;

         default:
            park_stop();
            break;


    }


}





////倒车实现
//void daoche_step(float delta_d)
//{
//    daoche_actdist += fabsf(delta_d);
//    Steer_target = 0.0f;
//
//    Motor_Speed(15.0f, 0);
//
//    if(daoche_actdist >= DAOCHE_DIST)
//    {
//        daoche_flag = 0;
//        Motor_Speed(0.0f, 0);
//        Steer_target = 0.0f;
//
//        ips200_clear();
//        ips200_show_string(0, 0, "Done!");
//    }
//
//
//}
//
//




void subject1_task(void)
{
    float delta_d=0.0f;

    Key_handel();

    if(s1_record_mode || s1_replay_mode || park_on)
    {
       delta_d = DELTA_D();
    }
    if(s1_record_mode || s1_replay_mode)
    {
       post_update(&nav_path, delta_d);
    }
    if(s1_record_mode)
    {
        record_step(delta_d);
    }

    if(s1_replay_mode)
    {
        replay_step();
    }

    if(park_on)
    {
        park_step(delta_d);
    }

}

void subject1_stop(void)
{
    s1_record_mode = 0;
    s1_replay_mode = 0;
//    daoche_flag    = 0;
//    daoche_actdist = 0.0f;


    park_on = 0;
    park_st = PARK_IDLE;
    park_dist = 0.0f;
    park_wait = 0;

    Motor_Speed(0.0f, 1);
    Steer_target=0.0f;
    //Motor_Ctrl(SteerAngle_Ctrl(0.0f, angle));
}



// Flash存取接口，供flash.c使用
double *subject1_get_x(void)   { return nav_x; }
double *subject1_get_y(void)   { return nav_y; }
int16   subject1_get_num(void) { return nav_num; }
void    subject1_set_num(int16 n) { nav_num = n; }
