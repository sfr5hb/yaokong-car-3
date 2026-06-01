/*
 * flash.h
 *
 *  Created on: 2026年4月16日
 *      Author: HP
 */

#ifndef CODE_FLASH_H_
#define CODE_FLASH_H_

#define PATH_SECTION 0//flash扇区
#define NUM_PAGE     9//第10页路径点数量
#define X_PAGE           10     // 第11页存X坐标
#define Y_PAGE           11     // 第12页存Y坐标

#define MAX_PATH_POINTS  300    // 最多存300个点（大概10cm一个点）


typedef union {
    uint32_t uint32_type;
    uint8_t uint8_array[8];
    float float_type;   // float类型变量
    double double_type; // double类型变量
} flash_union_buffer_t;//缓冲区类型变量

void CUN_FLASH(void);
void QV_FLASH(void);




#endif /* CODE_FLASH_H_ */
