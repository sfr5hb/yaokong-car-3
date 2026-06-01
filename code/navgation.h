/*
 * navgation.h
 *
 *  Created on: 2026年4月16日
 *      Author: HP
 */

#ifndef CODE_NAVGATION_H_
#define CODE_NAVGATION_H_


// 全局变量声明（extern）
extern double current_x;
extern double current_y;
extern int32 last_rear_encoder;
extern float last_yaw;

extern const float mileage_me;
extern float daoche_start;
extern float daoche_dist;
extern float nav_lookahead_dist;

extern uint8 record_mode;
extern uint8 replay_mode;
extern uint8 daoche_mode;
extern int16 target_point_idx;

extern double path_x[];
extern double path_y[];
extern int16 path_num;

//static float wrap_to_180(float a);
//static float unwrap_near_ref(float ref_unwrapped, float ang_wrapped);
//


float DELTA_D(void);
void update_post(float delta_d);

void daoche_task(void);
float mileage_update(void);
void record_path(void);
void path_replay(void);
void nav_stop_all(void);
void nav_task(void);

#endif /* CODE_NAVGATION_H_ */
