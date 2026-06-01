/*
 * navgation.c
 *
 *  Created on: 2026年4月16日
 *      Author: HP
 */


#include "zf_common_headfile.h"


//科目一惯导思路
//陀螺仪角度给到转向，后轮保持匀速
//更新：为了适配倒车入库，根据陀螺仪以及距离推算坐标，在倒车入库点将角度180°反向同时后轮
//反向开始倒车

double current_x = 0.0;
double current_y = 0.0;
int32  last_rear_encoder = 0;
float  last_yaw = 0.0f;

uint8 record_mode = 0;
uint8 replay_mode = 0;
uint8 daoche_mode = 0;
int16 target_point_idx = 0;



//需要重新修改了！！！！！！！！！

//encoder_2:两米累计值：4663左后轮
//encoder_3:两米累加值：4716右后轮


const float mileage_me_l=0.000429f;
const float mileage_me_r=0.000424f;


//const float mileage_me = 0.000817f;      //推车走1米后修改这个值
//float   daoche_start = 3.0f;       //记录完后根据总点数修改

uint8 daoche_flag=0;//倒车标志位，导航结束时开启
float daoche_dist=0.4f;//倒车距离
float daoche_actdist=0;//倒车实际距离

//float last_yaw=0.0f;
float yaw_zero=0.0f;

static float record_accum_dist = 0.0f;



//计算步距
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

//更新坐标
void update_post(float delta_d)
{
    float yaw_now=(yaw-yaw_zero)*3.1415926f/180.0f;
    float yaw_last = last_yaw         * 3.1415926f / 180.0f;
    current_x += delta_d * cosf((yaw_now + yaw_last) / 2.0f);
    current_y += delta_d * sinf((yaw_now + yaw_last) / 2.0f);
    last_yaw = (yaw - yaw_zero);

}


float mileage_update(void)
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




//    float delta_d = (float)encoder_data_3 * mileage_me;   // 正=前进，负=倒车
//
//    encoder_data_3=0;
    float yaw_now=yaw-yaw_zero;

    float yaw_rad_now  = yaw_now * 3.1415926f / 180.0f;
    float yaw_rad_last = last_yaw * 3.1415926f / 180.0f;

    current_x += delta_d * cosf((yaw_rad_now + yaw_rad_last) / 2.0f);
    current_y += delta_d * sinf((yaw_rad_now + yaw_rad_last) / 2.0f);

   // last_yaw = yaw_relative;
    last_yaw=yaw_now;
    return delta_d;


}

// 记录路径（每前进/倒退10cm记一个点）
void record_path(void)
{
    if(record_mode == 0) return;

//    float delta_d = mileage_update();
//
//    record_accum_dist += fabsf(delta_d);

    float delta_d=DELTA_D();//步距
    update_post(delta_d);//更新坐标
    record_accum_dist += fabsf(delta_d);


    if(record_accum_dist > 0.80f)                     // 每50cm记录一次
    {
        record_accum_dist = 0.0f;
        if(path_num < MAX_PATH_POINTS)
        {
            path_x[path_num] = current_x;
            path_y[path_num] = current_y;
            path_num++;
        }
        else
        {
            record_mode = 0;                   // 数组满了自动停止
        }
    }



}

// 路径复现主逻辑（含自动倒车）
void path_replay(void)
{
    if(replay_mode == 0) return;

   // mileage_update();                          // 实时更新当前坐标

    float delta_d = DELTA_D();
    update_post(delta_d);               // 更新 current_x/y


    if(target_point_idx < path_num)
    {

        double dx = path_x[target_point_idx] - current_x;
        double dy = path_y[target_point_idx] - current_y;
        float  dist_to_point = sqrtf((float)(dx*dx + dy*dy));

         //到达当前航迹点就切换下一个
        if(dist_to_point < 0.8f)              // 8cm内认为到达
        {
            target_point_idx++;
//            if(target_point_idx >= daoche_start)
//                daoche_mode = 1;
        }


        if(target_point_idx < path_num)
        {
            dx = path_x[target_point_idx] - current_x;
            dy = path_y[target_point_idx] - current_y;

            float target_yaw = atan2f((float)dy, (float)dx) * 180.0f / 3.1415926f;

            // 使用相对角度
            float yaw_relative = yaw - yaw_zero;
            if (yaw_relative > 180.0f) yaw_relative -= 360.0f;
            if (yaw_relative < -180.0f) yaw_relative += 360.0f;

            float nav_out = Nav_ctl(target_yaw, yaw_relative);




//            // 内环位置环
//            float steer_duty = Steer_Position_Control((int32)nav_out, current_encoder_pos);
//            Motor_Ctrl(steer_duty);


            //内环位置环
            float steer_duty=SteerAngle_Ctrl(nav_out,angle);
            Motor_Ctrl(steer_duty);


            Motor_Speed(20.0f,1);
            ips200_show_float(20,  80, nav_out,5, 1);

             ips200_show_int(0,120,target_point_idx,3);
             //ips200_show_int(0, 120, target_point_idx, 3);
             ips200_show_float(0, 140, dist_to_point, 2, 3);
             ips200_show_float(80, 120, current_x, 2, 3);
             ips200_show_float(80, 140, current_y, 2, 3);


             ips200_show_float(120, 180, path_x[0], 2, 3);
             ips200_show_float(140, 180, path_x[1], 2, 3);






        }
    }
    else
    {
        // 全部点走完，停车
        replay_mode = 0;


        daoche_flag=1;//导航结束，倒车标志位开启
        daoche_actdist=0;

        //int32 error=0-current_encoder_pos;

        Motor_Ctrl(SteerAngle_Ctrl(0, angle));                        // 转向归零

        Motor_Speed(0, 0);             // 后轮停止（你自己实现）
        ips200_show_string(0, 80, "ReDone");
        //ips200_show_int(20,60,error,3);
    }
}
//倒车task
void daoche_task(void)
{
    if(daoche_flag==0)return;

    float delta_d=DELTA_D();
    daoche_actdist+=fabsf(delta_d);
    //转向保持中心位置
    float steer_out=SteerAngle_Ctrl(0.0f, angle);
    Motor_Ctrl(steer_out);

    Motor_Speed(5.0f,0);//倒车速度以及方向
    if(daoche_actdist>=daoche_dist)
    {
        daoche_flag=0;
        Motor_Ctrl(0);
        Motor_Speed(0.0f,0);
        daoche_actdist=0.0f;
    }
}
void nav_stop_all(void)
{
    record_mode = 0;
    replay_mode = 0;
    daoche_mode = 0;
    daoche_flag = 0;
    daoche_actdist = 0.0f;
    target_point_idx = 0;


    Motor_Ctrl(SteerAngle_Ctrl(0.0f, angle));
    Motor_Speed(0.0f, 1);
    ips200_show_string(0, 60, "Nav Stop");
}
// 导航总任务（主循环里调用）
void nav_task(void)
{
    // Key1：开始/停止记录路径（全程手动推车，正向绕完+倒车入库）
    if(key1_flag)
    {
        key1_flag = 0;
        if(record_mode == 0)
        {
            record_mode = 1;
            path_num = 0;

            target_point_idx = 0;
            record_accum_dist = 0.0f;

            int32 clear_l = 0;
            int32 clear_r = 0;
            Encoder_GetMileageCount(&clear_l, &clear_r);

            current_x = 0.0;
            current_y = 0.0;

            yaw_zero = yaw;
            last_yaw = 0.0f;

            //last_yaw=yaw;

            ips200_show_string(0, 60, "Reing...");
        }
        else
        {
            record_mode = 0;
            record_accum_dist = 0.0f;
            ips200_show_string(0, 60, "RecEnd");
            ips200_show_int(0, 90, path_num, 3);
          //  ips200_show_int(0,120,target_point_idx,3);
            CUN_FLASH();              // 自动存Flash
        }
    }

    // Key2：开始复现（自动绕锥桶+倒车入库）
    if(key2_flag)
    {
       // QV_FLASH();
        key2_flag = 0;

        if(record_mode == 0)
        {
            QV_FLASH();            // 先读Flash
            if(path_num > 1)
            {
                //target_yaw_valid = 0;

                replay_mode = 1;
                target_point_idx = 0;


                int32 clear_l = 0;
                int32 clear_r = 0;
                Encoder_GetMileageCount(&clear_l, &clear_r);

//                            current_x = 0.0;
//                            current_y = 0.0;
//
//                            yaw_zero = yaw;
//                            last_yaw = 0.0f;

                current_x = 0.0;
                current_y = 0.0;

                yaw_zero = yaw;
                last_yaw = 0.0f;

                //yaw_zero = yaw;        // 归零角度
                //last_yaw = yaw;

                ips200_show_string(0, 60, "Replaying");
            }
            else
            {
                ips200_show_string(0, 60, "No Path");
            }
        }
    }

    record_path();      // 记录时一直跑
    //tft180_show_int(100, 80, target_point_idx, 4);   // 显示当前目标点序号
   // tft180_show_float(0,  120, nav_out,    5, 1);
    path_replay();      // 复现时一直跑
    daoche_task();
}








