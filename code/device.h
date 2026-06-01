/*
 * device.h
 *
 *  Created on: 2026Äê4ÔÂ12ÈÕ
 *      Author: HP
 */
#include "zf_common_headfile.h"

#ifndef CODE_DEVICE_H_
#define CODE_DEVICE_H_

#define LED1                    (P20_9)
#define LED2                    (P20_8)
#define LED3                    (P21_5)
#define LED4                    (P21_4)
#define KEY1                    (P20_6)
#define KEY2                    (P20_7)
#define KEY3                    (P11_2)
#define KEY4                    (P11_3)
#define SWITCH1                 (P33_11)
#define SWITCH2                 (P33_12)




extern uint8 key1_flag;
extern uint8 key2_flag;
extern uint8 key3_flag;
extern uint8 key4_flag;
extern uint8 key1_state_last;
extern uint8 key2_state_last;
extern uint8 key3_state_last;
extern uint8 key4_state_last;
extern uint8 key1_state;
extern uint8 key2_state;
extern uint8 key3_state;
extern uint8 key4_state;

void mykey_Init(void);
void key_scan(void);
void ALL_Init(void);
#endif /* CODE_DEVICE_H_ */
