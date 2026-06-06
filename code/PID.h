/*
 * PID.h
 *
 *  Created on: 2026��4��4��
 *      Author: HP
 */

#ifndef _PID_H_
#define _PID_H_


// ת����ר�� PID �����ṹ��
typedef struct
{
    float kp;               // ����ϵ��
    float ki;               // ����ϵ��
    float kd;               // ΢��ϵ��

    float target;           // Ŀ������λ��
    float current;          // ��ǰ����λ��
    float error;            // ��ǰ���
    float last_error;       // �ϴ����

    float integrator;       // �����ۼ�ֵ
    float imax;             // �����޷��������ͣ�

    float output;           // PID �����
    float out_limit;        // ����޷� (���� 0-100 ��Ӧռ�ձ�)
} Steer_PID_t;//�ڻ�ת��



//����ʽPPID,���ֵ������
typedef struct
{
        float kp;
        float ki;
        float kd;
        float out_limit;        // ���ռ�ձ�����
        float last_error;       // e(k-1)
        float last_last_error;  // e(k-2)
        float last_output;      // ��һ�������ռ�ձ�
        float integrator;       // �����ۼ�ֵ
}Speed_PID_t;

typedef struct
{
   float kp,ki,kd;
   float target,current,error,last_error;
   float integrator,imax;
   float out_put,out_limit;

}Nav_PID_t;



// ȫ�ֱ�������
extern Steer_PID_t SteerPID;
extern Nav_PID_t NavPID;

extern Speed_PID_t LeftPID;
extern Speed_PID_t RightPID;

/*֮ǰpid//// �����ӿ�
//void Steer_PID_Init(void);
//float Steer_Position_Control(int32 target, int32 current);
*/

void SteerAngle_Init(void);
float SteerAngle_Ctrl(float target,float current);





void _Nav_PID_init(void);
float Nav_ctl(float target_yaw,float current_yaw);




//houlun
void Motor_PID_Init(void);
void Speed_Reset(Speed_PID_t *pid)
float Speed_PID(Speed_PID_t *pid,float target_speed,float current_speed);



float SPEED_POS(Speed_PID_t *pid,float target,float current);

//void NAV_STEER(void);

extern float yaw;


//�ⲿ��������
extern float target_yaw;//Ŀ�꺽���
extern int32 current_encoder_pos;//��ǰ������λ��
extern float steer_duty;//���ռ�ձ�


extern uint8 reach_flag;//״̬��־λ

#endif /* CODE_PID_H_ */





