/*
 * menu.c
 *
 *  Created on: 2026年4月25日
 *      Author: HP
 */


//菜单函数：调参，每此修改参数不需要从重复编译烧录
//将需要修改的参数显示在显示屏上，通过按键进行选择，修改后的参数保存进入flash，断电保存

#include "zf_common_headfile.h"

#define FLASH_SECTION_INDEX (0)
#define FLASH_PAGE_INDEX (8)

app_mode_t app_mode = APP_MODE_MENU;

//菜单对应的数组列表
menu_item menu[]={
    {1, "Navigation", NULL, NULL},
    {11, "Enter Nav", menu_enter_navigation, NULL},
    {12, "Back to Main", NULL, NULL},
    //之后可能还是需要图像处理，所以菜单部分加上了图像处理的选择；

    {3, "Image Mode", NULL, NULL},                                         // 图像模式
    {31, "Image view", menu_enter_inage, NULL},                                  // 显示原边线
    //{32, "Image Follow", menu_12, NULL},                                  // 图像菜单2
    {37, "Back to Main", NULL, NULL},                                      // 回到主菜单

    {2, "Tuning Mode", NULL, NULL},                                        // 调参模式
    {21, "Motor", NULL, NULL},                                   // 电机参数调整
    {211, "Motor_L", NULL, NULL},                                                                    // 电机参数调整——变积分——左轮
    {2111, "LeftPID.kp", menu_tuning, &LeftPID.kp},      // 电机参数调整——变积分——左轮Max_I
    {2112, "LeftPID.ki", menu_tuning, &LeftPID.ki},           // 电机参数调整——变积分——左轮CI
    {2113,"LeftPID.kd",menu_tuning,&LeftPID.kd},
    {2114, "Back to Main", NULL, NULL},                                   // 回到主菜单


    {212,"Motor_R",NULL,NULL},
    {2121,"RightPID.kp",menu_tuning,&RightPID.kp},
    {2122,"RightPID.ki",menu_tuning,&RightPID.ki},
    {2123,"RightPID.kd",menu_tuning,&RightPID.kd},
    {2124, "Back to Main", NULL, NULL},                                   // 回到主菜单


    {22,"Steer",NULL,NULL},
    {221,"Steer_n",NULL,NULL},
    {2211,"SteerPID.kp",menu_tuning,&SteerPID.kp},
    {2212,"SteerPID.ki",menu_tuning,&SteerPID.ki},
    {2213,"SteerPID.kd",menu_tuning,&SteerPID.kd},
    {2214, "Back to Main", NULL, NULL},



    {222,"Steer_y",NULL,NULL},
    {2221,"NavPID.kp",menu_tuning,&NavPID.kp},
    {2222,"NavPID.ki",menu_tuning,&NavPID.ki},
    {2223,"NavPID.kd",menu_tuning,&NavPID.kd},
    {2224, "Back to Main", NULL, NULL},


    {223,"NAV",NULL,NULL},

    {2234, "Back to Main", NULL, NULL},


};

menu_item *current_menu_item;

//进入导航模式（科目一）
void menu_enter_navigation(float32 *param, char name[30])
{
    (void)param;
    (void)name;
    app_mode = APP_MODE_NAVIGATION;
    key_flag_clear();
    ips200_clear();
//    ips200_show_string(1, 0, "Navigation");
//    ips200_show_string(1, 30, "K1 Record");
//    ips200_show_string(1, 60, "K2 Replay");
    ips200_show_string(1, 90, "K4 Back");
}
//进入图像查看模式（科目三）
void menu_enter_inage(float32 *param,char name[30])
{

    (void)param;
    (void)name;
    key_flag_clear();
    ips200_clear();

    app_mode=APP_MODE_IMAGE;
   // ips200_clear();
   // ips200_show_string(0, 90, "K4: Back");
}


//进入人车跟随模式


int key_switch(void)
{
    return (key1_flag || key2_flag || key3_flag || key4_flag);
}



void key_flag_clear(void)
{
    key1_flag=0,
    key2_flag=0,
    key3_flag=0,
    key4_flag=0;
}



bool have_sub_menu(int menu_id)//查看是否存在子菜单
{
    for(int i=0;i<sizeof(menu)/sizeof(menu[0]);i++)
    {
        if(menu[i].menu_id/10==menu_id)
        {
            return true;
        }
    }
    return false;
}

int show_sub_menu(int parent_id,int highlight_col)//显示子菜单，以及当前高亮菜单
{

    ips200_clear();
    int item_idx=0;
    for(int i=0;i<sizeof(menu)/sizeof(menu[0]);i++)
    {
        if(menu[i].menu_id/10==parent_id)
        {
            if(item_idx==highlight_col)
            {
                current_menu_item=&menu[i];
                ips200_set_color(RGB565_RED,RGB565_WHITE);
            }
            else
            {
                ips200_set_color(RGB565_GREEN,RGB565_WHITE);
            }
            ips200_show_string(1,30*item_idx,menu[i].menu_name);
            item_idx++;
        }
    }

    return item_idx;
}

void Menu_Switch(void)
{
    static int parent_menu_id=0;
    static int highlight_col=0;//设定高亮行号
    static int menu_item_count=0;
    menu_item_count=show_sub_menu(parent_menu_id,highlight_col);

    if(key_switch())
        {
            if(key1_flag&&highlight_col>0)//按下按键一减少当前行数
            {
                highlight_col--;
            }

            else if (key2_flag && highlight_col < menu_item_count - 1) // 按下按键2增加当前行数
            {
                highlight_col++;//按下按键二增加当前行数
            }
            else if (key4_flag)
            {
                highlight_col = 0;
                parent_menu_id = parent_menu_id / 10;
            }
            else if (key3_flag)
            {
               if (have_sub_menu(current_menu_item->menu_id))
               {
                    highlight_col = 0;
                    parent_menu_id = current_menu_item->menu_id;
               }
               else if (strcmp(current_menu_item->menu_name, "Back to Main") == 0) // 如果当前菜单为“Back to Main”，则返回主界面
               {
                    highlight_col = 0;
                    parent_menu_id = 0;
               }
               else if (current_menu_item->menu_action) // 启动对应的抽象函数menu_action
               {
                     current_menu_item->menu_action(current_menu_item->param, current_menu_item->menu_name);
                }
            }
         //menu_item_count = show_sub_menu(parent_menu_id, highlight_col);
         key_flag_clear();
        }
    system_delay_ms(10);
    }

void flash_storage(void)
{
    if(flash_check(FLASH_SECTION_INDEX,FLASH_PAGE_INDEX))
       flash_erase_page(FLASH_SECTION_INDEX,FLASH_PAGE_INDEX);
    flash_buffer_clear();

    //向缓冲区存入数据
    flash_union_buffer[0].float_type=LeftPID.kp;
    flash_union_buffer[1].float_type=LeftPID.ki;
    flash_union_buffer[2].float_type=LeftPID.kd;
    flash_union_buffer[3].float_type=RightPID.kp;
    flash_union_buffer[4].float_type=RightPID.ki;
    flash_union_buffer[5].float_type=RightPID.kd;
//转向电机数据
    flash_union_buffer[6].float_type=SteerPID.kp;
    flash_union_buffer[7].float_type=SteerPID.ki;
    flash_union_buffer[8].float_type=SteerPID.kd;

    flash_union_buffer[9].float_type=NavPID.kp;
    flash_union_buffer[10].float_type=NavPID.ki;
    flash_union_buffer[11].float_type=NavPID.kd;




    //对应扇区页码写入缓冲区数据
    flash_write_page_from_buffer(FLASH_SECTION_INDEX,FLASH_PAGE_INDEX);

    flash_buffer_clear();

    system_delay_ms(200);
}

void flash_read(void)
{
    if(!flash_check(FLASH_SECTION_INDEX,FLASH_PAGE_INDEX))
    {
        flash_buffer_clear();
        return;
    }

    flash_read_page_to_buffer(FLASH_SECTION_INDEX,FLASH_PAGE_INDEX);

    LeftPID.kp=flash_union_buffer[0].float_type;
    LeftPID.ki=flash_union_buffer[1].float_type;
    LeftPID.kd=flash_union_buffer[2].float_type;
    RightPID.kp=flash_union_buffer[3].float_type;
    RightPID.ki=flash_union_buffer[4].float_type;
    RightPID.kd=flash_union_buffer[5].float_type;

    SteerPID.kp=flash_union_buffer[6].float_type;
    SteerPID.ki=flash_union_buffer[7].float_type;
    SteerPID.kd=flash_union_buffer[8].float_type;

    NavPID.kp=flash_union_buffer[9].float_type;
    NavPID.ki=flash_union_buffer[10].float_type;
    NavPID.kd=flash_union_buffer[11].float_type;



    flash_buffer_clear();
}




/*
 * 主体函数需要实现的功能：1.读取菜单的索引号，并设定当前行，标记高亮
 * 2.调用显示函数
 * 3.读取按键变化，并调用检查子菜单函数，如果存在子菜单或者有可执行的功能则进行跳转或执行函数
 * 4.当读取到到当前菜单为“Back to Main”时跳转回主菜单。
 */

void menu_tuning(float32 *tuning,char name[30])
{
    char buf[30];//由于ips200屏幕大小为240*180，逐飞提供默认字库大小为8*16，屏幕横向无法显示超过30个字符
    char *menu_name=name;
    float32 level[4]={0.01,0.1,1,10};
    int current_level=0;

    ips200_clear();
    ips200_show_string(1,0,"Change_level:");
    sprintf(buf,"X%5.2f:",level[current_level]);
    ips200_show_string(1,30,buf);
    key_flag_clear();


    while(1)
    {
       key_scan();
       if(key1_flag || key2_flag || key3_flag || key4_flag)   // ← 再判断标志位
       {

            if(key1_flag&&current_level<4)
            {
                current_level+=1;
            }

            else if(key2_flag&&current_level>0)
            {
                current_level-=1;
            }
            else if(key3_flag)
            {
                ips200_clear();
                sprintf(buf,"%s:",menu_name);
                ips200_show_string(1,0,buf);
                sprintf(buf,"%5.2f",*tuning);
                ips200_show_string(1,30,buf);
                key_flag_clear();
                while(1)
                {
                    key_scan();
                    if(key1_flag || key2_flag || key3_flag || key4_flag)
                    {
                        if(key1_flag)
                        {
                            *tuning+=level[current_level];
                        }
                        else if(key2_flag)
                        {
                            *tuning-=level[current_level];
                        }
                        else if(key3_flag)
                        {
                            return;
                        }
                        else if(key4_flag)
                        {
                            flash_storage();
                        }
                        ips200_clear();
                        sprintf(buf, "%s:",menu_name);
                        ips200_show_string(1,0,buf);
                        sprintf(buf,"%5.2f",*tuning);
                        ips200_show_string(1,30,buf);
                        key_flag_clear();
                    }
                }
            }

            ips200_clear();
            ips200_show_string(1,0,"Change_level:");
            sprintf(buf,"X%5.2f:",level[current_level]);
            ips200_show_string(1,30,buf);
            key_flag_clear();

           }
        }
    }







