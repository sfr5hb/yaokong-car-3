/*
 * flash.c
 *
 *  Created on: 2026年4月16日
 *      Author: HP
 */

#include "zf_common_headfile.h"

double path_x[MAX_PATH_POINTS];
double path_y[MAX_PATH_POINTS];
int16  path_num = 0;

flash_union_buffer_t path_buffer[300];

void CUN_FLASH(void)
{
    // ---------- 1. 保存点数量 ----------
        flash_buffer_clear();
        flash_union_buffer[0].uint32_type = (uint32)path_num;

        if(flash_check(PATH_SECTION, NUM_PAGE))
            flash_erase_page(PATH_SECTION, NUM_PAGE);

        flash_write_page_from_buffer(PATH_SECTION, NUM_PAGE);

     // ---------- 2. 保存所有X坐标 ----------
       for(int i = 0; i < path_num; i++)
            {
                path_buffer[i].double_type = path_x[i];
            }
            for(int i = 0; i < path_num; i++)
            {
                for(int j = 0; j < 8; j++)
                {
                    flash_union_buffer[8 * i + j].uint8_type = path_buffer[i].uint8_array[j];
                }
            }

        if(flash_check(PATH_SECTION, X_PAGE))
                flash_erase_page(PATH_SECTION, X_PAGE);

         flash_write_page_from_buffer(PATH_SECTION, X_PAGE);


     // ---------- 3. 保存所有Y坐标 ----------
       for(int i = 0; i < path_num; i++)
             {
                 path_buffer[i].double_type = path_y[i];
             }
       for(int i = 0; i < path_num; i++)
             {
                 for(int j = 0; j < 8; j++)
                 {
                     flash_union_buffer[8 * i + j].uint8_type = path_buffer[i].uint8_array[j];
                 }
             }

       if(flash_check(PATH_SECTION, Y_PAGE))
                 flash_erase_page(PATH_SECTION, Y_PAGE);

         flash_write_page_from_buffer(PATH_SECTION, Y_PAGE);

        ips200_show_string(0, 150, "Path Saved");

}


void QV_FLASH(void)
{
    // ---------- 1. 读点数量 ----------
        if(flash_check(PATH_SECTION, NUM_PAGE))
        {
            flash_read_page_to_buffer(PATH_SECTION, NUM_PAGE);
            path_num = (int16)flash_union_buffer[0].uint32_type;
        }
        else
        {
            path_num = 0;
        }
        if(path_num > MAX_PATH_POINTS) path_num = MAX_PATH_POINTS;

   // ---------- 2. 读X坐标 ----------
        if(flash_check(PATH_SECTION, X_PAGE))
            {
                flash_read_page_to_buffer(PATH_SECTION, X_PAGE);
                for(int i = 0; i < path_num; i++)
                {
                    for(int j = 0; j < 8; j++)
                    {
                        path_buffer[i].uint8_array[j] = flash_union_buffer[8 * i + j].uint8_type;
                    }
                    path_x[i] = path_buffer[i].double_type;
                }
            }

   // ---------- 3. 读Y坐标 ----------
        if(flash_check(PATH_SECTION, Y_PAGE))
            {
                flash_read_page_to_buffer(PATH_SECTION, Y_PAGE);
                for(int i = 0; i < path_num; i++)
                {
                    for(int j = 0; j < 8; j++)
                    {
                        path_buffer[i].uint8_array[j] = flash_union_buffer[8 * i + j].uint8_type;
                    }
                    path_y[i] = path_buffer[i].double_type;
                }
            }

            ips200_show_string(0, 150, "Path Loaded");

}







