/*
 * path.h
 *
 *  Created on: 2026年6月2日
 *      Author: HP
 */

#ifndef CODE_PATH_H_
#define CODE_PATH_H_


typedef enum {
    PATH_FORWARD = 0,   // 正序，科目一：从点0走到点N
    PATH_REVERSE = 1,   // 倒序，科目三：从点N走回点0
} path_dir_t;



typedef struct {
    double    *x;               // 路径点X坐标数组
    double    *y;               // 路径点Y坐标数组
    int16     *num;             // 当前路径点总数
    double    *cur_x;           // 当前位置X
    double    *cur_y;           // 当前位置Y
    float     *yaw_zero;        // 航向基准（记录开始时的yaw）
    float     *last_yaw;        // 上一次的相对yaw，用于坐标积分
    float     *accum_dist;      // 累积里程，用于记录间隔判断
    int16     *target_idx;      // 当前目标点索引
    float      record_interval; // 记录间隔（米）
    float      arrive_dist;     // 到点判定距离（米）
    path_dir_t direction;       // 复现方向

    float      nav_out_1;//转角输出，根据这个输出调整速度
    uint8 point_flag;//防抖标志位，只有在距离大于某个判定距离之后再切换点位
} path_t;

int path_replay(path_t *p);
void path_record(path_t *p,float delta_d);
void post_update(path_t *p,float delta_d);













#endif /* CODE_PATH_H_ */
