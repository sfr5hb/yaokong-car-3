/*
 * remote.h
 *
 *  Created on: 2026年6月5日
 *      Author: HP
 *      通道数据：
 *    channel[0]：192-1008-1808，左右转向
 *    channel[1]：192-992-1792，油门

 */

#ifndef CODE_REMOTE_H_
#define CODE_REMOTE_H_


// 通道中位和范围
#define RC_STEER_MID        1008    // 转向中位
#define RC_STEER_MIN        192     // 转向最左
#define RC_STEER_MAX_VAL    1832    // 转向最右
#define RC_STEER_DEAD       100     // 转向死区

#define RC_THROTTLE_MID     992     // 油门中位
#define RC_THROTTLE_MIN     192     // 油门最前（推杆向上）
#define RC_THROTTLE_MAX     1792    //油门最大值
#define RC_THROTTLE_DEAD    200     // 油门死区

// 转向和速度上限（调试后修改）
#define RC_ANGLE_MAX        42.0f   // 最大转向角度（度）
#define RC_SPEED_MAX        25.0f   // 最大遥控速度

void Remote_Ctrl(void);          // 遥控驾驶，主循环调用
void Remote_test(void);


#endif /* CODE_REMOTE_H_ */
