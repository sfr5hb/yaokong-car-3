/*
 * Subject1.h
 *
 *  Created on: 2026年6月2日
 *      Author: HP
 */

#ifndef CODE_SUBJECT1_H_
#define CODE_SUBJECT1_H_

void subject1_task(void);
void subject1_stop(void);   // 退出时调用

float DELTA_D(void);


double *subject1_get_x(void);
double *subject1_get_y(void);
int16   subject1_get_num(void);
void    subject1_set_num(int16 n);


#endif /* CODE_SUBJECT1_H_ */
