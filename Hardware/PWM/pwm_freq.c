#include "pwm_freq.h"
#include "MAIN.h"
#include "tim.h"
#define Tim1Period 8




float TimeFrequency[12];                //12个通道频率
float TimeDuty[12];                     //12个通道占空比

uint32_t Time8usCNT=0;

uint8_t Tim1CaptureStatus[4]={0};       //设置状态标志位
uint8_t Tim2CaptureStatus[4]={0};       //设置状态标志位
uint8_t Tim3CaptureStatus[4]={0};       //设置状态标志位
uint8_t Tim4CaptureStatus[4]={0};       //设置状态标志位

uint32_t Tim1UpTime[4]={0};             //定时器2捕获上升时间
uint32_t Tim1DowmTime[4]={0};           //定时器2捕获下降时间
uint32_t Tim1UpStartCapture[4]={0};     //捕获值
uint32_t Tim1DowmStartCapture[4]={0};   //捕获值

uint32_t Tim2UpTime[4]={0};             //定时器2捕获上升时间
uint32_t Tim2DowmTime[4]={0};           //定时器2捕获下降时间
uint32_t Tim2UpStartCapture[4]={0};     //捕获值
uint32_t Tim2DowmStartCapture[4]={0};   //捕获值

uint32_t Tim3UpTime[4]={0};             //定时器3捕获上升时间
uint32_t Tim3DowmTime[4]={0};           //定时器3捕获下降时间
uint32_t Tim3UpStartCapture[4]={0};     //捕获值
uint32_t Tim3DowmStartCapture[4]={0};   //捕获值

uint32_t Tim4UpTime[4]={0};             //定时器4捕获上升时间
uint32_t Tim4DowmTime[4]={0};           //定时器4捕获下降时间
uint32_t Tim4UpStartCapture[4]={0};     //捕获值
uint32_t Tim4DowmStartCapture[4]={0};   //捕获值

Pwm_channle_t Pwm_channle;

TIM_HandleTypeDef *htimtemp;


void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
	htimtemp=htim;
	if (htim->Instance==htim12.Instance)
	{
		if(htim->Channel==HAL_TIM_ACTIVE_CHANNEL_1)
        {
            switch(Tim1CaptureStatus[0])
            {
                case 0:
                    Tim1UpStartCapture[0] = Time8usCNT;
                    __HAL_TIM_SET_CAPTUREPOLARITY(&htim12,TIM_CHANNEL_1,TIM_ICPOLARITY_FALLING);  //设置下降沿捕获
                    Tim1CaptureStatus[0]++;
                break;
                case 1:
                    Tim1DowmStartCapture[0] = Time8usCNT;
                    Tim1UpTime[0] = Tim1DowmStartCapture[0] - Tim1UpStartCapture[0];
                    __HAL_TIM_SET_CAPTUREPOLARITY(&htim12,TIM_CHANNEL_1,TIM_ICPOLARITY_RISING);  //设置上升沿捕获
                    Tim1CaptureStatus[0]++;
                break;
                case 2:
                    Tim1DowmTime[0] = Time8usCNT - Tim1DowmStartCapture[0];
                    HAL_TIM_IC_Stop_IT(&htim12,TIM_CHANNEL_1);  //停止输入捕获
                    TimeFrequency[2]=1000000.0/(Tim1UpTime[0]+Tim1DowmTime[0])/Tim1Period;
                    TimeDuty[2]=100.0*Tim1UpTime[0]/((Tim1UpTime[0]+Tim1DowmTime[0]));
                
                    Tim1CaptureStatus[0]=0;  //清空标志位
                    __HAL_TIM_SET_CAPTUREPOLARITY(&htim12,TIM_CHANNEL_1,TIM_INPUTCHANNELPOLARITY_RISING);
                    HAL_TIM_IC_Start_IT(&htim12,TIM_CHANNEL_1);
                break;
            }
        }
        if(htim->Channel==HAL_TIM_ACTIVE_CHANNEL_2)
        {
            switch(Tim1CaptureStatus[1])
            {
                case 0:
                    Tim1UpStartCapture[1] = Time8usCNT;
                    __HAL_TIM_SET_CAPTUREPOLARITY(&htim12,TIM_CHANNEL_2,TIM_ICPOLARITY_FALLING);  //设置下降沿捕获
                    Tim1CaptureStatus[1]++;
                break;
                case 1:
                    Tim1DowmStartCapture[1] = Time8usCNT;
                    Tim1UpTime[1] = Tim1DowmStartCapture[1] - Tim1UpStartCapture[1];
                    __HAL_TIM_SET_CAPTUREPOLARITY(&htim12,TIM_CHANNEL_2,TIM_ICPOLARITY_RISING);  //设置上升沿捕获
                    Tim1CaptureStatus[1]++;
                break;
                case 2:
                    Tim1DowmTime[1] = Time8usCNT - Tim1DowmStartCapture[1];
                    HAL_TIM_IC_Stop_IT(&htim12,TIM_CHANNEL_2);  //停止输入捕获
                    TimeFrequency[3]=1000000.0/(Tim1UpTime[1]+Tim1DowmTime[1])/Tim1Period;
                    TimeDuty[3]=100.0*Tim1UpTime[1]/((Tim1UpTime[1]+Tim1DowmTime[1]));
                
                    Tim1CaptureStatus[1]=0;  //清空标志位
                    __HAL_TIM_SET_CAPTUREPOLARITY(&htim12,TIM_CHANNEL_2,TIM_INPUTCHANNELPOLARITY_RISING);
                    HAL_TIM_IC_Start_IT(&htim12,TIM_CHANNEL_2);
                break;
            }
        }

    }
	if (htim->Instance==htim2.Instance)
	{
		if(htim->Channel==HAL_TIM_ACTIVE_CHANNEL_1)
        {
            switch(Tim2CaptureStatus[0])
            {
                case 0:
                    Tim2UpStartCapture[0] = Time8usCNT;
                    __HAL_TIM_SET_CAPTUREPOLARITY(&htim2,TIM_CHANNEL_1,TIM_ICPOLARITY_FALLING);  //设置下降沿捕获
                    Tim2CaptureStatus[0]++;
                break;
                case 1:
                    Tim2DowmStartCapture[0] = Time8usCNT;
                    Tim2UpTime[0] = Tim2DowmStartCapture[0] - Tim2UpStartCapture[0];
                    __HAL_TIM_SET_CAPTUREPOLARITY(&htim2,TIM_CHANNEL_1,TIM_ICPOLARITY_RISING);  //设置上升沿捕获
                    Tim2CaptureStatus[0]++;
                break;
                case 2:
                    Tim2DowmTime[0] = Time8usCNT - Tim2DowmStartCapture[0];
                    HAL_TIM_IC_Stop_IT(&htim2,TIM_CHANNEL_1);  //停止输入捕获
                    TimeFrequency[0]=1000000.0/(Tim2UpTime[0]+Tim2DowmTime[0])/Tim1Period;
                    TimeDuty[0]=100.0*Tim2UpTime[0]/((Tim2UpTime[0]+Tim2DowmTime[0]));
                
                    Tim2CaptureStatus[0]=0;  //清空标志位
                    __HAL_TIM_SET_CAPTUREPOLARITY(&htim2,TIM_CHANNEL_1,TIM_INPUTCHANNELPOLARITY_RISING);
                    HAL_TIM_IC_Start_IT(&htim2,TIM_CHANNEL_1);
                break;
            }
        }
        if(htim->Channel==HAL_TIM_ACTIVE_CHANNEL_2)
        {
            switch(Tim2CaptureStatus[1])
            {
                case 0:
                    Tim2UpStartCapture[1] = Time8usCNT;
                    __HAL_TIM_SET_CAPTUREPOLARITY(&htim2,TIM_CHANNEL_2,TIM_ICPOLARITY_FALLING);  //设置下降沿捕获
                    Tim2CaptureStatus[1]++;
                break;
                case 1:
                    Tim2DowmStartCapture[1] = Time8usCNT;
                    Tim2UpTime[1] = Tim2DowmStartCapture[1] - Tim2UpStartCapture[1];
                    __HAL_TIM_SET_CAPTUREPOLARITY(&htim2,TIM_CHANNEL_2,TIM_ICPOLARITY_RISING);  //设置上升沿捕获
                    Tim2CaptureStatus[1]++;
                break;
                case 2:
                    Tim2DowmTime[1] = Time8usCNT - Tim2DowmStartCapture[1];
                    HAL_TIM_IC_Stop_IT(&htim2,TIM_CHANNEL_2);  //停止输入捕获
                    TimeFrequency[1]=1000000.0/(Tim2UpTime[1]+Tim2DowmTime[1])/Tim1Period;
                    TimeDuty[1]=100.0*Tim2UpTime[1]/((Tim2UpTime[1]+Tim2DowmTime[1]));
                
                    Tim2CaptureStatus[1]=0;  //清空标志位
                    __HAL_TIM_SET_CAPTUREPOLARITY(&htim2,TIM_CHANNEL_2,TIM_INPUTCHANNELPOLARITY_RISING);
                    HAL_TIM_IC_Start_IT(&htim2,TIM_CHANNEL_2);
                break;
            }
        }
//        if(htim->Channel==HAL_TIM_ACTIVE_CHANNEL_3)
//        {
//            switch(Tim2CaptureStatus[2])
//            {
//                case 0:
//                    Tim2UpStartCapture[2] = Time8usCNT;
//                    __HAL_TIM_SET_CAPTUREPOLARITY(&htim2,TIM_CHANNEL_3,TIM_ICPOLARITY_FALLING);  //设置下降沿捕获
//                    Tim2CaptureStatus[2]++;
//                break;
//                case 1:
//                    Tim2DowmStartCapture[2] = Time8usCNT;
//                    Tim2UpTime[2] = Tim2DowmStartCapture[2] - Tim2UpStartCapture[2];
//                    __HAL_TIM_SET_CAPTUREPOLARITY(&htim2,TIM_CHANNEL_3,TIM_ICPOLARITY_RISING);  //设置上升沿捕获
//                    Tim2CaptureStatus[2]++;
//                break;
//                case 2:
//                    Tim2DowmTime[2] = Time8usCNT - Tim2DowmStartCapture[2];
//                    HAL_TIM_IC_Stop_IT(&htim2,TIM_CHANNEL_3);  //停止输入捕获
//                    TimeFrequency[2]=1000000.0/(Tim2UpTime[2]+Tim2DowmTime[2])/Tim1Period;
//                    TimeDuty[2]=100.0*Tim2UpTime[2]/((Tim2UpTime[2]+Tim2DowmTime[2]));
//                
//                    Tim2CaptureStatus[2]=0;  //清空标志位
//                    __HAL_TIM_SET_CAPTUREPOLARITY(&htim2,TIM_CHANNEL_3,TIM_INPUTCHANNELPOLARITY_RISING);
//                    HAL_TIM_IC_Start_IT(&htim2,TIM_CHANNEL_3);
//                break;
//            }
//        }
//        if(htim->Channel==HAL_TIM_ACTIVE_CHANNEL_4)
//        {
//            switch(Tim2CaptureStatus[3])
//            {
//                case 0:
//                    Tim2UpStartCapture[3] = Time8usCNT;
//                    __HAL_TIM_SET_CAPTUREPOLARITY(&htim2,TIM_CHANNEL_4,TIM_ICPOLARITY_FALLING);  //设置下降沿捕获
//                    Tim2CaptureStatus[3]++;
//                break;
//                case 1:
//                    Tim2DowmStartCapture[3] = Time8usCNT;
//                    Tim2UpTime[3] = Tim2DowmStartCapture[3] - Tim2UpStartCapture[3];
//                    __HAL_TIM_SET_CAPTUREPOLARITY(&htim2,TIM_CHANNEL_4,TIM_ICPOLARITY_RISING);  //设置上升沿捕获
//                    Tim2CaptureStatus[3]++;
//                break;
//                case 2:
//                    Tim2DowmTime[3] = Time8usCNT - Tim2DowmStartCapture[3];
//                    HAL_TIM_IC_Stop_IT(&htim2,TIM_CHANNEL_4);  //停止输入捕获
//                    TimeFrequency[3]=1000000.0/(Tim2UpTime[3]+Tim2DowmTime[3])/Tim1Period;
//                    TimeDuty[3]=100.0*Tim2UpTime[3]/((Tim2UpTime[3]+Tim2DowmTime[3]));
//                
//                    Tim2CaptureStatus[3]=0;  //清空标志位
//                    __HAL_TIM_SET_CAPTUREPOLARITY(&htim2,TIM_CHANNEL_4,TIM_INPUTCHANNELPOLARITY_RISING);
//                    HAL_TIM_IC_Start_IT(&htim2,TIM_CHANNEL_4);
//                break;
//            }
//        }
    }
    if (htim->Instance==htim3.Instance)
	{
		if(htim->Channel==HAL_TIM_ACTIVE_CHANNEL_1)
        {
            switch(Tim3CaptureStatus[0])
            {
                case 0:
                    Tim3UpStartCapture[0] = Time8usCNT;
                    __HAL_TIM_SET_CAPTUREPOLARITY(&htim3,TIM_CHANNEL_1,TIM_ICPOLARITY_FALLING);  //设置下降沿捕获
                    Tim3CaptureStatus[0]++;
                break;
                case 1:
                    Tim3DowmStartCapture[0] = Time8usCNT;
                    Tim3UpTime[0] = Tim3DowmStartCapture[0] - Tim3UpStartCapture[0];
                    __HAL_TIM_SET_CAPTUREPOLARITY(&htim3,TIM_CHANNEL_1,TIM_ICPOLARITY_RISING);  //设置上升沿捕获
                    Tim3CaptureStatus[0]++;
                break;
                case 2:
                    Tim3DowmTime[0] = Time8usCNT - Tim3DowmStartCapture[0];
                    HAL_TIM_IC_Stop_IT(&htim3,TIM_CHANNEL_1);  //停止输入捕获
                    TimeFrequency[4]=1000000.0/(Tim3UpTime[0]+Tim3DowmTime[0])/Tim1Period;
                    TimeDuty[4]=100.0*Tim3UpTime[0]/((Tim3UpTime[0]+Tim3DowmTime[0]));
                
                    Tim3CaptureStatus[0]=0;  //清空标志位
                    __HAL_TIM_SET_CAPTUREPOLARITY(&htim3,TIM_CHANNEL_1,TIM_INPUTCHANNELPOLARITY_RISING);
                    HAL_TIM_IC_Start_IT(&htim3,TIM_CHANNEL_1);
                break;
            }
        }
        if(htim->Channel==HAL_TIM_ACTIVE_CHANNEL_2)
        {
            switch(Tim3CaptureStatus[1])
            {
                case 0:
                    Tim3UpStartCapture[1] = Time8usCNT;
                    __HAL_TIM_SET_CAPTUREPOLARITY(&htim3,TIM_CHANNEL_2,TIM_ICPOLARITY_FALLING);  //设置下降沿捕获
                    Tim3CaptureStatus[1]++;
                break;
                case 1:
                    Tim3DowmStartCapture[1] = Time8usCNT;
                    Tim3UpTime[1] = Tim3DowmStartCapture[1] - Tim3UpStartCapture[1];
                    __HAL_TIM_SET_CAPTUREPOLARITY(&htim3,TIM_CHANNEL_2,TIM_ICPOLARITY_RISING);  //设置上升沿捕获
                    Tim3CaptureStatus[1]++;
                break;
                case 2:
                    Tim3DowmTime[1] = Time8usCNT - Tim3DowmStartCapture[1];
                    HAL_TIM_IC_Stop_IT(&htim3,TIM_CHANNEL_2);  //停止输入捕获
                    TimeFrequency[5]=1000000.0/(Tim3UpTime[1]+Tim3DowmTime[1])/Tim1Period;
                    TimeDuty[5]=100.0*Tim3UpTime[1]/((Tim3UpTime[1]+Tim3DowmTime[1]));
                
                    Tim3CaptureStatus[1]=0;  //清空标志位
                    __HAL_TIM_SET_CAPTUREPOLARITY(&htim3,TIM_CHANNEL_2,TIM_INPUTCHANNELPOLARITY_RISING);
                    HAL_TIM_IC_Start_IT(&htim3,TIM_CHANNEL_2);
                break;
            }
        }
        if(htim->Channel==HAL_TIM_ACTIVE_CHANNEL_3)
        {
            switch(Tim3CaptureStatus[2])
            {
                case 0:
                    Tim3UpStartCapture[2] = Time8usCNT;
                    __HAL_TIM_SET_CAPTUREPOLARITY(&htim3,TIM_CHANNEL_3,TIM_ICPOLARITY_FALLING);  //设置下降沿捕获
                    Tim3CaptureStatus[2]++;
                break;
                case 1:
                    Tim3DowmStartCapture[2] = Time8usCNT;
                    Tim3UpTime[2] = Tim3DowmStartCapture[2] - Tim3UpStartCapture[2];
                    __HAL_TIM_SET_CAPTUREPOLARITY(&htim3,TIM_CHANNEL_3,TIM_ICPOLARITY_RISING);  //设置上升沿捕获
                    Tim3CaptureStatus[2]++;
                break;
                case 2:
                    Tim3DowmTime[2] = Time8usCNT - Tim3DowmStartCapture[2];
                    HAL_TIM_IC_Stop_IT(&htim3,TIM_CHANNEL_3);  //停止输入捕获
                    TimeFrequency[6]=1000000.0/(Tim3UpTime[2]+Tim3DowmTime[2])/Tim1Period;
                    TimeDuty[6]=100.0*Tim3UpTime[2]/((Tim3UpTime[2]+Tim3DowmTime[2]));
                
                    Tim3CaptureStatus[2]=0;  //清空标志位
                    __HAL_TIM_SET_CAPTUREPOLARITY(&htim3,TIM_CHANNEL_3,TIM_INPUTCHANNELPOLARITY_RISING);
                    HAL_TIM_IC_Start_IT(&htim3,TIM_CHANNEL_3);
                break;
            }
        }
        if(htim->Channel==HAL_TIM_ACTIVE_CHANNEL_4)
        {
            switch(Tim3CaptureStatus[3])
            {
                case 0:
                    Tim3UpStartCapture[3] = Time8usCNT;
                    __HAL_TIM_SET_CAPTUREPOLARITY(&htim3,TIM_CHANNEL_4,TIM_ICPOLARITY_FALLING);  //设置下降沿捕获
                    Tim3CaptureStatus[3]++;
                break;
                case 1:
                    Tim3DowmStartCapture[3] = Time8usCNT;
                    Tim3UpTime[3] = Tim3DowmStartCapture[3] - Tim3UpStartCapture[3];
                    __HAL_TIM_SET_CAPTUREPOLARITY(&htim3,TIM_CHANNEL_4,TIM_ICPOLARITY_RISING);  //设置上升沿捕获
                    Tim3CaptureStatus[3]++;
                break;
                case 2:
                    Tim3DowmTime[3] = Time8usCNT - Tim3DowmStartCapture[3];
                    HAL_TIM_IC_Stop_IT(&htim3,TIM_CHANNEL_4);  //停止输入捕获
                    TimeFrequency[7]=1000000.0/(Tim3UpTime[3]+Tim3DowmTime[3])/Tim1Period;
                    TimeDuty[7]=100.0*Tim3UpTime[3]/((Tim3UpTime[3]+Tim3DowmTime[3]));
                
                    Tim3CaptureStatus[3]=0;  //清空标志位
                    __HAL_TIM_SET_CAPTUREPOLARITY(&htim3,TIM_CHANNEL_4,TIM_INPUTCHANNELPOLARITY_RISING);
                    HAL_TIM_IC_Start_IT(&htim3,TIM_CHANNEL_4);
                break;
            }
        }
    }
    if (htim->Instance==htim4.Instance)
	{
		if(htim->Channel==HAL_TIM_ACTIVE_CHANNEL_1)
        {
            switch(Tim4CaptureStatus[0])
            {
                case 0:
                    Tim4UpStartCapture[0] = Time8usCNT;
                    __HAL_TIM_SET_CAPTUREPOLARITY(&htim4,TIM_CHANNEL_1,TIM_ICPOLARITY_FALLING);  //设置下降沿捕获
                    Tim4CaptureStatus[0]++;
                break;
                case 1:
                    Tim4DowmStartCapture[0] = Time8usCNT;
                    Tim4UpTime[0] = Tim4DowmStartCapture[0] - Tim4UpStartCapture[0];
                    __HAL_TIM_SET_CAPTUREPOLARITY(&htim4,TIM_CHANNEL_1,TIM_ICPOLARITY_RISING);  //设置上升沿捕获
                    Tim4CaptureStatus[0]++;
                break;
                case 2:
                    Tim4DowmTime[0] = Time8usCNT - Tim4DowmStartCapture[0];
                    HAL_TIM_IC_Stop_IT(&htim4,TIM_CHANNEL_1);  //停止输入捕获
                    TimeFrequency[8]=1000000.0/(Tim4UpTime[0]+Tim4DowmTime[0])/Tim1Period;
                    TimeDuty[8]=100.0*Tim4UpTime[0]/((Tim4UpTime[0]+Tim4DowmTime[0]));
                
                    Tim4CaptureStatus[0]=0;  //清空标志位
                    __HAL_TIM_SET_CAPTUREPOLARITY(&htim4,TIM_CHANNEL_1,TIM_INPUTCHANNELPOLARITY_RISING);
                    HAL_TIM_IC_Start_IT(&htim4,TIM_CHANNEL_1);
                break;
            }
        }
        if(htim->Channel==HAL_TIM_ACTIVE_CHANNEL_2)
        {
            switch(Tim4CaptureStatus[1])
            {
                case 0:
                    Tim4UpStartCapture[1] = Time8usCNT;
                    __HAL_TIM_SET_CAPTUREPOLARITY(&htim4,TIM_CHANNEL_2,TIM_ICPOLARITY_FALLING);  //设置下降沿捕获
                    Tim4CaptureStatus[1]++;
                break;
                case 1:
                    Tim4DowmStartCapture[1] = Time8usCNT;
                    Tim4UpTime[1] = Tim4DowmStartCapture[1] - Tim4UpStartCapture[1];
                    __HAL_TIM_SET_CAPTUREPOLARITY(&htim4,TIM_CHANNEL_2,TIM_ICPOLARITY_RISING);  //设置上升沿捕获
                    Tim4CaptureStatus[1]++;
                break;
                case 2:
                    Tim4DowmTime[1] = Time8usCNT - Tim4DowmStartCapture[1];
                    HAL_TIM_IC_Stop_IT(&htim4,TIM_CHANNEL_2);  //停止输入捕获
                    TimeFrequency[9]=1000000.0/(Tim4UpTime[1]+Tim4DowmTime[1])/Tim1Period;
                    TimeDuty[9]=100.0*Tim4UpTime[1]/((Tim4UpTime[1]+Tim4DowmTime[1]));
                
                    Tim4CaptureStatus[1]=0;  //清空标志位
                    __HAL_TIM_SET_CAPTUREPOLARITY(&htim4,TIM_CHANNEL_2,TIM_INPUTCHANNELPOLARITY_RISING);
                    HAL_TIM_IC_Start_IT(&htim4,TIM_CHANNEL_2);
                break;
            }
        }
        if(htim->Channel==HAL_TIM_ACTIVE_CHANNEL_3)
        {
            switch(Tim4CaptureStatus[2])
            {
                case 0:
                    Tim4UpStartCapture[2] = Time8usCNT;
                    __HAL_TIM_SET_CAPTUREPOLARITY(&htim4,TIM_CHANNEL_3,TIM_ICPOLARITY_FALLING);  //设置下降沿捕获
                    Tim4CaptureStatus[2]++;
                break;
                case 1:
                    Tim4DowmStartCapture[2] = Time8usCNT;
                    Tim4UpTime[2] = Tim4DowmStartCapture[2] - Tim4UpStartCapture[2];
                    __HAL_TIM_SET_CAPTUREPOLARITY(&htim4,TIM_CHANNEL_3,TIM_ICPOLARITY_RISING);  //设置上升沿捕获
                    Tim4CaptureStatus[2]++;
                break;
                case 2:
                    Tim4DowmTime[2] = Time8usCNT - Tim4DowmStartCapture[2];
                    HAL_TIM_IC_Stop_IT(&htim4,TIM_CHANNEL_3);  //停止输入捕获
                    TimeFrequency[10]=1000000.0/(Tim4UpTime[2]+Tim4DowmTime[2])/Tim1Period;
                    TimeDuty[10]=100.0*Tim4UpTime[2]/((Tim4UpTime[2]+Tim4DowmTime[2]));
                
                    Tim4CaptureStatus[2]=0;  //清空标志位
                    __HAL_TIM_SET_CAPTUREPOLARITY(&htim4,TIM_CHANNEL_3,TIM_INPUTCHANNELPOLARITY_RISING);
                    HAL_TIM_IC_Start_IT(&htim4,TIM_CHANNEL_3);
                break;
            }
        }
        if(htim->Channel==HAL_TIM_ACTIVE_CHANNEL_4)
        {
            switch(Tim4CaptureStatus[3])
            {
                case 0:
                    Tim4UpStartCapture[3] = Time8usCNT;
                    __HAL_TIM_SET_CAPTUREPOLARITY(&htim4,TIM_CHANNEL_4,TIM_ICPOLARITY_FALLING);  //设置下降沿捕获
                    Tim4CaptureStatus[3]++;
                break;
                case 1:
                    Tim4DowmStartCapture[3] = Time8usCNT;
                    Tim4UpTime[3] = Tim4DowmStartCapture[3] - Tim4UpStartCapture[3];
                    __HAL_TIM_SET_CAPTUREPOLARITY(&htim4,TIM_CHANNEL_4,TIM_ICPOLARITY_RISING);  //设置上升沿捕获
                    Tim4CaptureStatus[3]++;
                break;
                case 2:
                    Tim4DowmTime[3] = Time8usCNT - Tim4DowmStartCapture[3];
                    HAL_TIM_IC_Stop_IT(&htim4,TIM_CHANNEL_4);  //停止输入捕获
                    TimeFrequency[11]=1000000.0/(Tim4UpTime[3]+Tim4DowmTime[3])/Tim1Period;
                    TimeDuty[11]=100.0*Tim4UpTime[3]/((Tim4UpTime[3]+Tim4DowmTime[3]));
                
                    Tim4CaptureStatus[3]=0;  //清空标志位
                    __HAL_TIM_SET_CAPTUREPOLARITY(&htim4,TIM_CHANNEL_4,TIM_INPUTCHANNELPOLARITY_RISING);
                    HAL_TIM_IC_Start_IT(&htim4,TIM_CHANNEL_4);
                break;
            }
        }
    }
}

void Get_Pwm_channle()//获取PWM 结构体Pwm_channle
{
	Pwm_channle.CH1=TimeDuty[8];
	Pwm_channle.CH2=TimeDuty[9];
	Pwm_channle.CH3=TimeDuty[10];
	Pwm_channle.CH4=TimeDuty[11];
	Pwm_channle.CH5=TimeDuty[4];
	Pwm_channle.CH6=TimeDuty[5];
	Pwm_channle.CH7=TimeDuty[6];
	Pwm_channle.CH8=TimeDuty[7];
	Pwm_channle.CH9=TimeDuty[0];
	Pwm_channle.CH10=TimeDuty[1];
	Pwm_channle.CH11=TimeDuty[2];
	Pwm_channle.CH12=TimeDuty[3];
}

void Clean_Pwm_channle()//
{
	memset(TimeDuty, 0, sizeof(TimeDuty));
//	memset(TimeDuty, 0, sizeof(TimeDuty));

	
}