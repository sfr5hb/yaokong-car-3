/*
 * path.c
 *
 *  Created on: 2026年6月2日
 *      Author: HP
 */


/*
 * 科目一和科目三共用，复现顺序以及对应点数不一样
 */


#include "zf_common_headfile.h"

//float g_nav_out=0.0f;
//位置更新

void post_update(path_t *p,float delta_d)
{
    float yaw_now  = (yaw - *(p->yaw_zero)) * 3.1415926f / 180.0f;
    float yaw_last = *(p->last_yaw)          * 3.1415926f / 180.0f;

    *(p->cur_x)   += delta_d * cosf((yaw_now + yaw_last) / 2.0f);
    *(p->cur_y)   += delta_d * sinf((yaw_now + yaw_last) / 2.0f);
    *(p->last_yaw) = yaw - *(p->yaw_zero);
}

//路径记录
void path_record(path_t *p,float delta_d)
{
    *(p->accum_dist) += fabsf(delta_d);

     if(*(p->accum_dist) >= p->record_interval)
     {
         *(p->accum_dist) = 0.0f;
         if(*(p->num) < (p->direction == PATH_FORWARD ? MAX_PATH_POINTS : FOLLOW_MAX_POINTS))
         {
             p->x[*(p->num)] = *(p->cur_x);
             p->y[*(p->num)] = *(p->cur_y);
             (*(p->num))++;
         }
     }
}

//路径复现

int path_replay(path_t *p)
{
    int16 idx = *(p->target_idx);

    // 终点判定
    if(p->direction == PATH_FORWARD  && idx >= *(p->num)) return 0;
    if(p->direction == PATH_REVERSE  && idx < 0)          return 0;

    double dx   = p->x[idx] - *(p->cur_x);
    double dy   = p->y[idx] - *(p->cur_y);
    float  dist = sqrtf((float)(dx*dx + dy*dy));

     // 到点切换
    if(dist>p->arrive_dist)p->point_flag=0;

    if(dist < p->arrive_dist&&p->point_flag==0)
     {
          p->point_flag=1;
           // 切换点时清导航PID积分，防止过弯过冲
          NavPID.integrator = 0.0f;
          NavPID.last_error = 0.0f;

          if(p->direction == PATH_FORWARD) (*(p->target_idx))++;
          else                             (*(p->target_idx))--;

          idx = *(p->target_idx);

          // 切换后再判终点
          if(p->direction == PATH_FORWARD  && idx >= *(p->num)) return 0;
          if(p->direction == PATH_REVERSE  && idx < 0)          return 0;

          dx = p->x[idx] - *(p->cur_x);
          dy = p->y[idx] - *(p->cur_y);
      }

      // 计算目标航向，控制转向
      float target_yaw   = atan2f((float)dy, (float)dx) * 180.0f / 3.1415926f;
      float yaw_relative = yaw - *(p->yaw_zero);
      if(yaw_relative >  180.0f) yaw_relative -= 360.0f;
      if(yaw_relative < -180.0f) yaw_relative += 360.0f;

      float nav_out = Nav_ctl(target_yaw, yaw_relative);
      p->nav_out_1 = nav_out;
      Motor_Ctrl(SteerAngle_Ctrl(nav_out, angle));

      return 1;



}








