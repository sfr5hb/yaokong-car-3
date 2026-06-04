/*
 * flash.c
 *
 *  Created on: 2026年4月16日
 *      Author: HP
 */

#include "zf_common_headfile.h"

//double path_x[MAX_PATH_POINTS];
//double path_y[MAX_PATH_POINTS];
//int16  path_num = 0;

flash_union_buffer_t path_buf;

void nav_flash_save(double *x, double *y, int16 num)
{
    // ---------- 1. 保存点数量 ----------
        flash_buffer_clear();
        flash_union_buffer[0].uint32_type = (uint32)num;

        if(flash_check(0, NUM_PAGE))
            flash_erase_page(0, NUM_PAGE);

        flash_write_page_from_buffer(0, NUM_PAGE);

     // ---------- 2. 保存所有X坐标 ----------
       flash_buffer_clear();
       for(int i = 0; i < num; i++)
        {
           path_buf.double_type = x[i];
           for(int j = 0; j < 8; j++)

               flash_union_buffer[8 * i + j].uint8_type = path_buf.uint8_array[j];

         }

        if(flash_check(0, X_PAGE))
                flash_erase_page(0, X_PAGE);

         flash_write_page_from_buffer(0, X_PAGE);


     // ---------- 3. 保存所有Y坐标 ----------
       flash_buffer_clear();
       for(int i = 0; i < num; i++)
       {
           path_buf.double_type = y[i];
           for(int j = 0; j < 8; j++)
             flash_union_buffer[8*i+j].uint8_type = path_buf.uint8_array[j];
       }

       if(flash_check(0, Y_PAGE))
                 flash_erase_page(0, Y_PAGE);

         flash_write_page_from_buffer(0, Y_PAGE);

        ips200_show_string(0, 150, "Path Saved");

}


void nav_flash_load(double *x, double *y, int16 *num)
{
    // ---------- 1. 读点数量 ----------
    if(!flash_check(0, NUM_PAGE)) { *num = 0; return; }

       flash_read_page_to_buffer(0, NUM_PAGE);
       *num = (int16)flash_union_buffer[0].uint32_type;
       if(*num > MAX_PATH_POINTS) *num = MAX_PATH_POINTS;


   // ---------- 2. 读X坐标 ----------
        if(flash_check(0, X_PAGE))
            {
                flash_read_page_to_buffer(0, X_PAGE);
                for(int i = 0; i < *num; i++)
                {
                    for(int j = 0; j < 8; j++)
                    {
                        path_buf.uint8_array[j] = flash_union_buffer[8 * i + j].uint8_type;
                    }
                    x[i] = path_buf.double_type;
                }
            }

   // ---------- 3. 读Y坐标 ----------
        if(flash_check(0, Y_PAGE))
            {
                flash_read_page_to_buffer(0, Y_PAGE);
                for(int i = 0; i < *num; i++)
                {
                    for(int j = 0; j < 8; j++)
                        path_buf.uint8_array[j] = flash_union_buffer[8*i+j].uint8_type;
                    y[i] = path_buf.double_type;
                }
            }
            ips200_show_string(0, 150, "Path Loaded");

}



// ==================== 科目三 ====================
// Page4布局：
//   index 0      : 数量
//   index 1~200  : X坐标（100个double，每个占2个uint32）
//   index 201~400: Y坐标
void rc_flash_save(double *x, double *y, int16 num)
{
    flash_buffer_clear();
    flash_union_buffer[0].uint32_type = (uint32)num;

    for(int i = 0; i < num; i++)
    {
        path_buf.double_type = x[i];
        flash_union_buffer[1 + i*2 + 0].uint32_type = path_buf.uint32_array[0];
        flash_union_buffer[1 + i*2 + 1].uint32_type = path_buf.uint32_array[1];
    }
    for(int i = 0; i < num; i++)
    {
        path_buf.double_type = y[i];
        flash_union_buffer[201 + i*2 + 0].uint32_type = path_buf.uint32_array[0];
        flash_union_buffer[201 + i*2 + 1].uint32_type = path_buf.uint32_array[1];
    }

    if(flash_check(0, RC_PAGE)) flash_erase_page(0, RC_PAGE);
    flash_write_page_from_buffer(0, RC_PAGE);

    ips200_show_string(0, 150, "RC Saved");
}

void rc_flash_load(double *x, double *y, int16 *num)
{
    if(!flash_check(0, RC_PAGE)) { *num = 0; return; }

    flash_read_page_to_buffer(0, RC_PAGE);
    *num = (int16)flash_union_buffer[0].uint32_type;
    if(*num > FOLLOW_MAX_POINTS) *num = FOLLOW_MAX_POINTS;

    for(int i = 0; i < *num; i++)
    {
        path_buf.uint32_array[0] = flash_union_buffer[1 + i*2 + 0].uint32_type;
        path_buf.uint32_array[1] = flash_union_buffer[1 + i*2 + 1].uint32_type;
        x[i] = path_buf.double_type;
    }
    for(int i = 0; i < *num; i++)
    {
        path_buf.uint32_array[0] = flash_union_buffer[201 + i*2 + 0].uint32_type;
        path_buf.uint32_array[1] = flash_union_buffer[201 + i*2 + 1].uint32_type;
        y[i] = path_buf.double_type;
    }

    ips200_show_string(0, 150, "RC Loaded");
}



