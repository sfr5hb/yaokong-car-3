/*
 * menu.h
 *
 *  Created on: 2026年4月25日
 *      Author: HP
 */

#ifndef CODE_MENU_H_
#define CODE_MENU_H_


typedef struct
{
        int menu_id;                                        // 菜单索引号
        char menu_name[30];                                 // 菜单显示名称
        void (*menu_action)(float32 *param, char name[30]); // 子菜单执行函数
        float32 *param;                                     // 需要传入处理的参数

}menu_item;


typedef enum
{
        APP_MODE_MENU = 0,
        APP_MODE_SUBJECT1,
        APP_MODE_SUBJECT3,



}app_mode_t;//菜单模式枚举


//摄像头部分函数声明
void menu_enter_inage(float32 *param,char name[30]);



extern app_mode_t app_mode;

void menu_tuning(float32 *tuning,char name[30]);
void menu_enter_subject1(float32 *param, char name[30]);
void menu_enter_subject3(float32 *param, char name[30]);



extern menu_item menu[];



void key_flag_clear(void);
bool have_sub_menu(int menu_id);//查看是否存在子菜单
int show_sub_menu(int parent_id,int highlight_col);//显示子菜单，以及当前高亮菜单
void Menu_Switch(void);
int key_switch(void);
void flash_storage(void);
void flash_read(void);

void menu_reset(void);
//void menu_tuning(float32 *tuning,char name[30]);





#endif /* CODE_MENU_H_ */
