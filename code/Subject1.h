/*
 * Subject1.h
 *
 *  Created on: 2026年6月2日
 *      Author: HP
 */

//#ifndef CODE_SUBJECT1_H_
//#define CODE_SUBJECT1_H_
//
//void subject1_task(void);
//void subject1_stop(void);   // 退出时调用
//
//float DELTA_D(void);
//extern uint8 s1_record_mode;
//extern uint8 s1_replay_mode;//复现标志位
//
//
//
//double *subject1_get_x(void);
//double *subject1_get_y(void);
//int16   subject1_get_num(void);
//void    subject1_set_num(int16 n);
//
//
//#endif /* CODE_SUBJECT1_H_ */
//
//
//

















#ifndef CODE_SUBJECT1_H_
#define CODE_SUBJECT1_H_

typedef enum {
    PARK_IDLE = 0,
    PARK_WAIT,
    PARK_BACK,
    PARK_IN,
    PARK_FIX,
    PARK_LAST,
} park_t;//倒车部分路径状态切换
#define PARK_SPEED       7.0f

#define PARK_WAIT_T      30      // 等转向归正
#define PARK_BACK_D      0.25f   // 先直退
#define PARK_IN_D        0.45f   // 打角切入
#define PARK_FIX_D       0.35f   // 反打修正
#define PARK_LAST_D      0.30f   // 回正直退

#define PARK_IN_A        28.0f//打脚切入角度
#define PARK_FIX_A      -22.0f//倒车部分参数定义//反打修正角度


void park_next(park_t st);
float angle180(float a);
void park_start(void);//倒车开始;
void park_stop(void);
void park_step(float delta_d);






void subject1_task(void);
void subject1_stop(void);   // 退出时调用

float DELTA_D(void);
void show_pre(void);//显示初始路径
extern uint8 s1_record_mode;
extern uint8 s1_replay_mode;//复现标志位


void One_Start(void);//按键一科目一开始
void Record_Stop(void);//按键一停止记录
void Replay_Start(void);//按键二富先开始
void Map_display(void);
void Key_handel(void);
float speed_calc(void);
void record_step(float delta_d);
void show_can(float speed_c);
void replay_step(void);
void daoche_step(float delta_d);






double *subject1_get_x(void);
double *subject1_get_y(void);
int16   subject1_get_num(void);
void    subject1_set_num(int16 n);


#endif /* CODE_SUBJECT1_H_ */
