/*
 * flash.h
 *
 *  Created on: 2026年4月16日
 *      Author: HP
 */

#ifndef CODE_FLASH_H_
#define CODE_FLASH_H_

//科目一
#define PATH_SECTION 0//flash扇区
#define NUM_PAGE     9//第10页路径点数量
#define X_PAGE           10     // 第11页存X坐标
#define Y_PAGE           11     // 第12页存Y坐标

#define MAX_PATH_POINTS  300    // 最多存300个点（大概10cm一个点）


//科目三
#define RC_PAGE   4
#define FOLLOW_MAX_POINTS   100


typedef union {
    uint32_t uint32_type;
    uint32_t  uint32_array[2];
    uint8_t uint8_array[8];
    float float_type;   // float类型变量
    double double_type; // double类型变量
} flash_union_buffer_t;//缓冲区类型变量



// Flash存取用的外部接口（flash.c里实现）
extern void nav_flash_save(double *x, double *y, int16 num);
extern void nav_flash_load(double *x, double *y, int16 *num);

void rc_flash_save(double *x, double *y, int16 num);
void rc_flash_load(double *x, double *y, int16 *num);



#endif /* CODE_FLASH_H_ */
