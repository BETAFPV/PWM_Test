#ifndef __PWM_FREQ_H__
#define __PWM_FREQ_H__
#include "pwm_freq.h"
#include "main.h"
typedef struct Pwm_channle_Duty
{
	float CH1;
	float CH2;	
	float CH3;
	float CH4;
	float CH5;
	float CH6;
	float CH7;
	float CH8;
	float CH9;
	float CH10;
	float CH11;
	float CH12;
}Pwm_channle_t;


void Get_Pwm_channle();
void Clean_Pwm_channle();


#endif

