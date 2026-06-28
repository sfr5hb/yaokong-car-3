/*
 * remote.c
 *
 *  Created on: 2026年6月5日
 *      Author: HP
 *
 *
 *  //通道数值
 *    channel[0]：192-1008-1808
 *    中位1008附近死区内 → 转向归中
 *    >1108 → 右转，值越大角度越大
 *    <908  → 左转，值越小角度越大
 *
 *  channel[1]：192-992-1792
 *    中位992附近死区内 → 停车
 *    <792  → 后退，值越小速度越大（推杆向上）
 *    推杆向下前进
 *
 */

#include "zf_common_headfile.h"


void Remote_test(void)
{
    if(1 == uart_receiver.finsh_flag)                            // 帧完成标志判断
    {
        if(1 == uart_receiver.state)                             // 遥控器失控状态判断
        {
            printf("CH1-CH6 data: ");
            for(int i = 0; i < 6; i++)
            {
                printf("%d ", uart_receiver.channel[i]);         // 串口输出6个通道数据
            }
            printf("\r\n");
        }
        else
        {
            printf("Remote control has been disconnected.\r\n"); // 串口输出失控提示
        }
        uart_receiver.finsh_flag = 0;                            // 帧完成标志复位
    }

}


void Remote_Ctrl(void)
{
    if(uart_receiver.state==0)
    {
        speed=0.0f;
        Steer_target=0.0f;
        ips200_show_string(0, 0, "erroing...");

//        Motor_Speed(0.0f,1);
//        Motor_Ctrl(SteerAngle_Ctrl(0.0f,angle));
        return;
    }

    uint16 ch0=uart_receiver.channel[0];//转向
    uint16 ch1=uart_receiver.channel[1];//油门



    //转向控制

    float steer_target=0.0f;
    if(ch0>RC_STEER_MID + RC_STEER_DEAD)
    {
        ips200_show_string(0, 0, "2...");
       // ips200_show_float(60,60,angle,3,5);

        // 右转：ch0从1008到1832，映射到0~RC_ANGLE_MAX
        float ratio = (float)(ch0 - (RC_STEER_MID + RC_STEER_DEAD)) /
                           (float)(RC_STEER_MAX_VAL - RC_STEER_MID - RC_STEER_DEAD);
        if(ratio > 1.0f) ratio = 1.0f;
        steer_target = ratio * RC_ANGLE_MAX;
     }
     else if(ch0 < RC_STEER_MID - RC_STEER_DEAD)
     {
         // 左转：ch0从1008到192，映射到0~-RC_ANGLE_MAX
         float ratio = (float)((RC_STEER_MID - RC_STEER_DEAD) - ch0) /
                              (float)(RC_STEER_MID - RC_STEER_DEAD - RC_STEER_MIN);
         if(ratio > 1.0f) ratio = 1.0f;
         steer_target = -ratio * RC_ANGLE_MAX;
     }
            // 死区内steer_target保持0

     // Motor_Ctrl(SteerAngle_Ctrl(steer_target, angle));
     Steer_target = steer_target;



      //油门控制


      float speed_t=0.0f;
      uint8_t dir_t=1;//默认方向是1
      if(ch1 < RC_THROTTLE_MID - RC_THROTTLE_DEAD)
      {
        // 前进：ch1从792到192，映射到0~RC_SPEED_MAX
        float ratio = (float)((RC_THROTTLE_MID - RC_THROTTLE_DEAD) - ch1) /
                           (float)(RC_THROTTLE_MID - RC_THROTTLE_DEAD - RC_THROTTLE_MIN);
        if(ratio > 1.0f) ratio = 1.0f;
        speed_t = ratio * RC_SPEED_MAX * 0.7;
        dir_t=0;
      }
//   // 死区内和推杆向下speed保持0
//      speed=speed_t;
//      dirt_flag=1;
      else if(ch1 > RC_THROTTLE_MID + RC_THROTTLE_DEAD)
      {
        // 后退：ch1 从 1192→1792 映射到 0~RC_SPEED_MAX
       float ratio = (float)(ch1 - (RC_THROTTLE_MID + RC_THROTTLE_DEAD)) /
                        (float)(RC_THROTTLE_MAX - RC_THROTTLE_MID - RC_THROTTLE_DEAD);
       if(ratio > 1.0f) ratio = 1.0f;
       speed_t = ratio * RC_SPEED_MAX;
       dir_t=1;
      }


  speed=speed_t;
  dirt_flag=dir_t;

      //Motor_Speed(speed, 1);


}
