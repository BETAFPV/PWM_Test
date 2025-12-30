#include "key.h"
#include "rf_port.h"
#include "main.h"
#include "device.h"	
#include "freertos.h"	
#include "task.h"	
#include "lcd.h"	
#include "key.h"	
#include "usart.h"
#include "gui.h"
#include "flash.h"
//参数
u8 Test_Ready=0;
u8 test_mode=0;
extern u8 test_step;
//enum RX RX_Device=RX6; 
extern TaskHandle_t RX_TESTTask_Handle;
extern TaskHandle_t RX_MANULTask_Handle;
enum Test_Err RX_Test_ErrINFO;
enum device_Err device_ErrINFO;
extern RX_Device_t PWM_RX;//接收机实体
extern u16 channe_target_value;
extern TIM_HandleTypeDef htim1;
extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim3;
extern TIM_HandleTypeDef htim4;
extern TIM_HandleTypeDef htim6;
extern TIM_HandleTypeDef htim7;
extern float channe_target_fre;
Test_Err_struct Test_Err_Global={0};
extern u8 CRSF_FRAMETYPE_BARO_ALTITUDE_static;
extern u16 channe_target_value;

uint8_t Link_State=0;

RX_Device_t PWM_RX;//接收机实体

u8 RSSI_average_array[10];
u8 RSSI_LQ_average_cnt;
u8 LQ_average_array[10];


Rx_target Rx4,Rx6,Rx6_dual,Rx8_dual,SuperP=0;//PWM接收机
Rx_target SuperD,SuperXnano,SuperXmono=0;//rx接收机

Rx_target * current_rx_target;


Flash_para Global_init_parameter={0,33,99,1};
Flash_para Global_parameter={0};

parameter_t RSSI_th={"RSSI_t",3,&Global_parameter.RSSI_threshold};
parameter_t LQ_th={"LQ_t",3,&Global_parameter.LQ_threshold};

#define parameter_number 2
parameter_t * Parameter_List[parameter_number]=
{
	&RSSI_th,
	&LQ_th

};


///////////////////////////////////////////////////////////////////


void Start_Auto_Test()//开始自动测试
{
	
		CRSF_FRAMETYPE_BARO_ALTITUDE_static=0;
		PWM_RX.RSSI=0;
		PWM_RX.LQ=0;
		HAL_TIM_IC_Start_IT(&htim1,TIM_CHANNEL_1);
    HAL_TIM_IC_Start_IT(&htim1,TIM_CHANNEL_2);
//    HAL_TIM_IC_Start_IT(&htim2,TIM_CHANNEL_3);
//    HAL_TIM_IC_Start_IT(&htim2,TIM_CHANNEL_4);

	  HAL_TIM_IC_Start_IT(&htim2,TIM_CHANNEL_1);
    HAL_TIM_IC_Start_IT(&htim2,TIM_CHANNEL_2);
//    HAL_TIM_IC_Start_IT(&htim2,TIM_CHANNEL_3);
//    HAL_TIM_IC_Start_IT(&htim2,TIM_CHANNEL_4);

    HAL_TIM_IC_Start_IT(&htim3,TIM_CHANNEL_1);
    HAL_TIM_IC_Start_IT(&htim3,TIM_CHANNEL_2);
    HAL_TIM_IC_Start_IT(&htim3,TIM_CHANNEL_3);
    HAL_TIM_IC_Start_IT(&htim3,TIM_CHANNEL_4);
		
    HAL_TIM_IC_Start_IT(&htim4,TIM_CHANNEL_1);
    HAL_TIM_IC_Start_IT(&htim4,TIM_CHANNEL_2);
    HAL_TIM_IC_Start_IT(&htim4,TIM_CHANNEL_3);
    HAL_TIM_IC_Start_IT(&htim4,TIM_CHANNEL_4);
		vTaskResume(RX_TESTTask_Handle);	
		test_mode=1;
		Test_Ready=1;
		test_step=1;
		RSSI_LQ_average_cnt=0;
		memset(RSSI_average_array, 0, 10);
		memset(LQ_average_array, 0, 10);
}

void Stop_Auto_Test()//结束自动测试
{
		HAL_TIM_IC_Stop_IT(&htim1,TIM_CHANNEL_1);
    HAL_TIM_IC_Stop_IT(&htim1,TIM_CHANNEL_2);
//    HAL_TIM_IC_Start_IT(&htim2,TIM_CHANNEL_3);
//    HAL_TIM_IC_Start_IT(&htim2,TIM_CHANNEL_4);

	  HAL_TIM_IC_Stop_IT(&htim2,TIM_CHANNEL_1);
    HAL_TIM_IC_Stop_IT(&htim2,TIM_CHANNEL_2);
//    HAL_TIM_IC_Start_IT(&htim2,TIM_CHANNEL_3);
//    HAL_TIM_IC_Start_IT(&htim2,TIM_CHANNEL_4);

    HAL_TIM_IC_Stop_IT(&htim3,TIM_CHANNEL_1);
    HAL_TIM_IC_Stop_IT(&htim3,TIM_CHANNEL_2);
    HAL_TIM_IC_Stop_IT(&htim3,TIM_CHANNEL_3);
    HAL_TIM_IC_Stop_IT(&htim3,TIM_CHANNEL_4);
		
    HAL_TIM_IC_Stop_IT(&htim4,TIM_CHANNEL_1);
    HAL_TIM_IC_Stop_IT(&htim4,TIM_CHANNEL_2);
    HAL_TIM_IC_Stop_IT(&htim4,TIM_CHANNEL_3);
    HAL_TIM_IC_Stop_IT(&htim4,TIM_CHANNEL_4);
	vTaskSuspend(RX_TESTTask_Handle);
	test_step=0;
	test_mode=0;
	Test_Ready=0;
}

void Start_Manual_Test()//开始手动测试
{
			HAL_TIM_IC_Start_IT(&htim1,TIM_CHANNEL_1);
    HAL_TIM_IC_Start_IT(&htim1,TIM_CHANNEL_2);
//    HAL_TIM_IC_Start_IT(&htim2,TIM_CHANNEL_3);
//    HAL_TIM_IC_Start_IT(&htim2,TIM_CHANNEL_4);

	  HAL_TIM_IC_Start_IT(&htim2,TIM_CHANNEL_1);
    HAL_TIM_IC_Start_IT(&htim2,TIM_CHANNEL_2);
//    HAL_TIM_IC_Start_IT(&htim2,TIM_CHANNEL_3);
//    HAL_TIM_IC_Start_IT(&htim2,TIM_CHANNEL_4);

    HAL_TIM_IC_Start_IT(&htim3,TIM_CHANNEL_1);
    HAL_TIM_IC_Start_IT(&htim3,TIM_CHANNEL_2);
    HAL_TIM_IC_Start_IT(&htim3,TIM_CHANNEL_3);
    HAL_TIM_IC_Start_IT(&htim3,TIM_CHANNEL_4);
		
    HAL_TIM_IC_Start_IT(&htim4,TIM_CHANNEL_1);
    HAL_TIM_IC_Start_IT(&htim4,TIM_CHANNEL_2);
    HAL_TIM_IC_Start_IT(&htim4,TIM_CHANNEL_3);
    HAL_TIM_IC_Start_IT(&htim4,TIM_CHANNEL_4);

	vTaskResume(RX_MANULTask_Handle);
//	
	test_mode=2;
	Test_Ready=1;

}

void Stop_Manual_Test()//结束手动测试
{
		HAL_TIM_IC_Stop_IT(&htim1,TIM_CHANNEL_1);
    HAL_TIM_IC_Stop_IT(&htim1,TIM_CHANNEL_2);
//    HAL_TIM_IC_Start_IT(&htim2,TIM_CHANNEL_3);
//    HAL_TIM_IC_Start_IT(&htim2,TIM_CHANNEL_4);

	  HAL_TIM_IC_Stop_IT(&htim2,TIM_CHANNEL_1);
    HAL_TIM_IC_Stop_IT(&htim2,TIM_CHANNEL_2);
//    HAL_TIM_IC_Start_IT(&htim2,TIM_CHANNEL_3);
//    HAL_TIM_IC_Start_IT(&htim2,TIM_CHANNEL_4);

    HAL_TIM_IC_Stop_IT(&htim3,TIM_CHANNEL_1);
    HAL_TIM_IC_Stop_IT(&htim3,TIM_CHANNEL_2);
    HAL_TIM_IC_Stop_IT(&htim3,TIM_CHANNEL_3);
    HAL_TIM_IC_Stop_IT(&htim3,TIM_CHANNEL_4);
		
    HAL_TIM_IC_Stop_IT(&htim4,TIM_CHANNEL_1);
    HAL_TIM_IC_Stop_IT(&htim4,TIM_CHANNEL_2);
    HAL_TIM_IC_Stop_IT(&htim4,TIM_CHANNEL_3);
    HAL_TIM_IC_Stop_IT(&htim4,TIM_CHANNEL_4);

//	test_mode=0;
//	Test_Ready=0;
	vTaskSuspend(RX_MANULTask_Handle);
}


extern int8_t RX_RSSI_GET,RX_LQ_GET,TX_RSSI_GET,TX_LQ_GET;
u8 Rssi_Lq_test_display()
{
		u16 RSSI_color;
		u16 LQ_color;
		if(PWM_RX.RSSI>Global_parameter.RSSI_threshold)
		{
			RSSI_color=RED;
			
		}
		else if(PWM_RX.RSSI>0&&PWM_RX.RSSI<=Global_parameter.RSSI_threshold)
		{
			RSSI_color=GREEN;
			
			
		}
			LCD_ShowNum_Color(70,40,RSSI_color,GRAY,PWM_RX.RSSI,3,16);				
		if(PWM_RX.LQ<Global_parameter.LQ_threshold)
		{
		LQ_color=RED;
		}
			
		else if(PWM_RX.LQ>=Global_parameter.LQ_threshold)
		{
			LQ_color=GREEN;
	
		}
			LCD_ShowNum_Color(70,55,LQ_color,GRAY,PWM_RX.LQ,3,16);	

}

u16 ALT_color;
u16 Vbat_color;
u8 Sensor_test_display()
{
		 if((((*current_rx_target)>>2)&(0x01))==1) 
		 {
				if((CRSF_FRAMETYPE_BARO_ALTITUDE_static>=1) )
				{
					ALT_color=GREEN;
				}
				else
				{
					ALT_color=RED;
				}
				LCD_DrawString(70, 40, "     ", 12, ALT_color,ALT_color);
		 }		 
	 if((((*current_rx_target)>>1)&(0x01))==1) 
		 {
				if((PWM_RX.Vbat>4.3)&(PWM_RX.Vbat<5.2))
				{
				Vbat_color=GREEN;
				}
					
				else 
				{
					Vbat_color=RED;
				}
				LCD_ShowFloat(70,55,PWM_RX.Vbat,Vbat_color,GRAY,16,0);	
		 }
}

u8 Channle_test_display()
{
		u16 Channle_color;
		switch(((*current_rx_target)>>5)&(0xf)) 		 
	 {
		case 4:
		Channle_color=PWM_RX.Channle.CH1*100;
		LCD_ShowNum_Color(50,40	,Channle_color,GRAY,PWM_RX.Channle.CH1,3,16);
		Channle_color=PWM_RX.Channle.CH2*1000;
		LCD_ShowNum_Color(120,40	,Channle_color,GRAY,PWM_RX.Channle.CH2,3,16);
		Channle_color=PWM_RX.Channle.CH3*1000;		
		LCD_ShowNum_Color(50,55	,Channle_color,GRAY,PWM_RX.Channle.CH3,3,16);
		Channle_color=PWM_RX.Channle.CH4*1000;
		LCD_ShowNum_Color(120,55	,Channle_color,GRAY,PWM_RX.Channle.CH4,3,16);
		break;
		case 6:
		Channle_color=PWM_RX.Channle.CH1*100;
		LCD_ShowNum_Color(50,40	,Channle_color,GRAY,PWM_RX.Channle.CH1,3,16);
		Channle_color=PWM_RX.Channle.CH2*1000;
		LCD_ShowNum_Color(120,40	,Channle_color,GRAY,PWM_RX.Channle.CH2,3,16);
		Channle_color=PWM_RX.Channle.CH3*1000;		
		LCD_ShowNum_Color(50,55	,Channle_color,GRAY,PWM_RX.Channle.CH3,3,16);
		Channle_color=PWM_RX.Channle.CH4*1000;
		LCD_ShowNum_Color(120,55	,Channle_color,GRAY,PWM_RX.Channle.CH4,3,16);
		Channle_color=PWM_RX.Channle.CH5*1000;		
		LCD_ShowNum_Color(50,70	,Channle_color,GRAY,PWM_RX.Channle.CH5,3,16);
		Channle_color=PWM_RX.Channle.CH6*1000;
		LCD_ShowNum_Color(120,70	,Channle_color,GRAY,PWM_RX.Channle.CH6,3,16);							
		break;
		case 8:	
		Channle_color=PWM_RX.Channle.CH1*100;
		LCD_ShowNum_Color(50,40	,Channle_color,GRAY,PWM_RX.Channle.CH1,3,16);
		Channle_color=PWM_RX.Channle.CH2*1000;
		LCD_ShowNum_Color(120,40	,Channle_color,GRAY,PWM_RX.Channle.CH2,3,16);
		Channle_color=PWM_RX.Channle.CH3*1000;		
		LCD_ShowNum_Color(50,55	,Channle_color,GRAY,PWM_RX.Channle.CH3,3,16);
		Channle_color=PWM_RX.Channle.CH4*1000;
		LCD_ShowNum_Color(120,55	,Channle_color,GRAY,PWM_RX.Channle.CH4,3,16);
		Channle_color=PWM_RX.Channle.CH5*1000;		
		LCD_ShowNum_Color(50,70	,Channle_color,GRAY,PWM_RX.Channle.CH5,3,16);
		Channle_color=PWM_RX.Channle.CH6*1000;
		LCD_ShowNum_Color(120,70	,Channle_color,GRAY,PWM_RX.Channle.CH6,3,16);	
		Channle_color=PWM_RX.Channle.CH7*1000;
		LCD_ShowNum_Color(50,85	,Channle_color,GRAY,PWM_RX.Channle.CH7,3,16);
		Channle_color=PWM_RX.Channle.CH8*1000;
		LCD_ShowNum_Color(120,85	,Channle_color,GRAY,PWM_RX.Channle.CH8,3,16);							
		break;
		case 14:	
		Channle_color=PWM_RX.Channle.CH1*100;
		LCD_ShowNum_Color(50,40	,Channle_color,GRAY,PWM_RX.Channle.CH1,3,12);
		Channle_color=PWM_RX.Channle.CH2*1000;
		LCD_ShowNum_Color(120,40	,Channle_color,GRAY,PWM_RX.Channle.CH2,3,12);
		Channle_color=PWM_RX.Channle.CH3*1000;		
		LCD_ShowNum_Color(50,52	,Channle_color,GRAY,PWM_RX.Channle.CH3,3,12);
		Channle_color=PWM_RX.Channle.CH4*1000;
		LCD_ShowNum_Color(120,52	,Channle_color,GRAY,PWM_RX.Channle.CH4,3,12);
		Channle_color=PWM_RX.Channle.CH5*1000;		
		LCD_ShowNum_Color(50,64	,Channle_color,GRAY,PWM_RX.Channle.CH5,3,12);
		Channle_color=PWM_RX.Channle.CH6*1000;
		LCD_ShowNum_Color(120,64	,Channle_color,GRAY,PWM_RX.Channle.CH6,3,12);	
		Channle_color=PWM_RX.Channle.CH7*1000;
		LCD_ShowNum_Color(50,76	,Channle_color,GRAY,PWM_RX.Channle.CH7,3,12);
		Channle_color=PWM_RX.Channle.CH8*1000;
		LCD_ShowNum_Color(120,76	,Channle_color,GRAY,PWM_RX.Channle.CH8,3,12);	

		Channle_color=PWM_RX.Channle.CH9*100;
		LCD_ShowNum_Color(50,88	,Channle_color,GRAY,PWM_RX.Channle.CH9,3,12);
		Channle_color=PWM_RX.Channle.CH10*1000;
		LCD_ShowNum_Color(120,88	,Channle_color,GRAY,PWM_RX.Channle.CH10,3,12);
		Channle_color=PWM_RX.Channle.CH11*1000;		
		LCD_ShowNum_Color(50,100	,Channle_color,GRAY,PWM_RX.Channle.CH11,3,12);
		Channle_color=PWM_RX.Channle.CH12*1000;
		LCD_ShowNum_Color(120,100	,Channle_color,GRAY,PWM_RX.Channle.CH12,3,12);
//		Channle_color=PWM_RX.Channle.CH5*1000;		
//		LCD_ShowNum_Color(50,70	,Channle_color,GRAY,PWM_RX.Channle.CH5,3,12);
//		Channle_color=PWM_RX.Channle.CH6*1000;
//		LCD_ShowNum_Color(120,70	,Channle_color,GRAY,PWM_RX.Channle.CH6,3,12);			
		break;
	 }
}


u8 Auto_test_display(u8 * current_step)
{
	static u8 last_step;
	if(last_step!=(*current_step))
	{
		switch (*current_step)		
			{
				case 0:
					
				break;
				case 1://
					LCD_DrawFillRectangle_color(10, 22, 150, 120,GRAY);
					LCD_DrawString(20, 25, "Rssi Lq Testing", 16, RED,GRAY);
					LCD_DrawString(20, 40, "RSSI:", 16, BLACK,GRAY);
					LCD_DrawString(20, 55, "LQ:", 16, BLACK,GRAY);
//10, 125, 150, 129
				break;
				case 2://
					LCD_DrawFillRectangle_color(10, 22, 150, 120,GRAY);
					LCD_DrawString(20, 25, "Sensor testing", 16, RED,GRAY);
				 if((((*current_rx_target)>>1)&(0x01))==1) 
					 {
							LCD_DrawString(20, 55, "Vbat:", 16, BLACK,GRAY);
					 }		 
				 if((((*current_rx_target)>>2)&(0x01))==1) 
					 {
							LCD_DrawString(20, 40, "ALT:", 16, BLACK,GRAY);					

					 }
					 LCD_DrawFillRectangle_color(11, 126, 10+140*1/3-1, 128,GREEN);

				break;
				case 3://
					LCD_DrawFillRectangle_color(10, 22, 150, 120,GRAY);

					LCD_DrawString(20, 25, "Channle_testing", 16, RED,GRAY);
					LCD_DrawFillRectangle_color(11, 126, 10+140*2/3-1, 128,GREEN);					

				break;		
				case 4://RESULT
					LCD_DrawFillRectangle_color(11, 126, 10+140-1, 128,GREEN);
					
				break;		
			}
	}
	last_step=(*current_step);
}




u32 RSSI_average_value,LQ_average_value;
u16 RLpass_cnt;
u16 SENSpass_cnt;
u8 sensor_check=0;
u16 Channlepass_cnt;
u16 time_cnt;
u16 ttest;
u8 Auto_test(u8 * current_step)
{

//RSSI&LQ
	if((*current_step)==1)
	{	
			if(RSSI_LQ_average_cnt>=10)
			{
				u8 i;
					RSSI_average_value=0;
					LQ_average_value=0;
					for (i = 0; i < 10; i++) {
						RSSI_average_value = RSSI_average_value+RSSI_average_array[i];
						LQ_average_value = LQ_average_value+LQ_average_array[i];
					}
					RSSI_average_value=RSSI_average_value/10;
					LQ_average_value=LQ_average_value/10;
					if((RSSI_average_value>0)&(RSSI_average_value<Global_parameter.RSSI_threshold)&(PWM_RX.LQ>Global_parameter.LQ_threshold))
					{
						RLpass_cnt++;
					}
			}		
//		if(
//			
//			((PWM_RX.RSSI<Global_parameter.RSSI_threshold)&(PWM_RX.RSSI>0))
//			&(PWM_RX.LQ>Global_parameter.LQ_threshold)
//			)
//		{
//			
//			

////			if(*current_rx_target==1)//是否为双天线
////			{
////				if(PWM_RX.RSSI2<30)
////				{
////					RLpass_cnt++;
////				}
////			}
////			else
////			{
//			RLpass_cnt++;
////			}
//		}
	}
/////////////////////////////////////
		
//SENSOR
		ttest=((*current_rx_target)>>1)&0x0f;
	if((((*current_rx_target)>>1)&0x0f)!=0)
	{
			if((((*current_rx_target)>>1)&(0x01))==1) 
			 {

					if((PWM_RX.Vbat>4.3)&(PWM_RX.Vbat<5.2))
					{		
						sensor_check=sensor_check|(0x1<<1);
					}
						else
						{
							sensor_check=sensor_check&0xfd;
						}						
				}		 
			if((((*current_rx_target)>>2)&(0x01))==1) 
			 {
						if(CRSF_FRAMETYPE_BARO_ALTITUDE_static>=1)
					{		
						sensor_check=sensor_check|(0x1<<2);
					}	
					else
					{
						sensor_check=sensor_check&0xfb;		
					}
			 }
	//		 ttest=(((*current_rx_target)>>1)&(0x0f));
			 if((((*current_rx_target)>>1)&(0x0f))==(sensor_check>>1))
			 {
				SENSpass_cnt++;
			 }
	 }
/////////////////////////////////////
//Channles
	 float min_threshold=channe_target_fre-0.2,max_threshold=channe_target_fre+0.2;
		 
	if((*current_step)==3)
	{
		if((PWM_RX.Channle.CH1>min_threshold)&(PWM_RX.Channle.CH1<max_threshold)
				&(PWM_RX.Channle.CH2>min_threshold)&(PWM_RX.Channle.CH2<max_threshold)
				&(PWM_RX.Channle.CH3>min_threshold)&(PWM_RX.Channle.CH3<max_threshold)
				&(PWM_RX.Channle.CH4>min_threshold)&(PWM_RX.Channle.CH4<max_threshold)
			)
			{
				switch(((*current_rx_target)>>5)&(0xf)) 		 
			 {
				case 4:
						Channlepass_cnt++;
				break;
				case 6:
					if((PWM_RX.Channle.CH5>min_threshold)&(PWM_RX.Channle.CH5<max_threshold)
							&(PWM_RX.Channle.CH6>min_threshold)&(PWM_RX.Channle.CH6<max_threshold)
	//						&(PWM_RX.Channle.CH7<4)&(PWM_RX.Channle.CH8<4)
						)
						{Channlepass_cnt++;}
				break;
				case 8:	
					if(
						(PWM_RX.Channle.CH5>min_threshold)&(PWM_RX.Channle.CH5<max_threshold)
						&(PWM_RX.Channle.CH6>min_threshold)&(PWM_RX.Channle.CH6<max_threshold)
						&(PWM_RX.Channle.CH7>min_threshold)&(PWM_RX.Channle.CH7<max_threshold)
						&(PWM_RX.Channle.CH8>min_threshold)&(PWM_RX.Channle.CH8<max_threshold)
						)
						{Channlepass_cnt++;}		
				break;

				case 14:	
					if(
						(PWM_RX.Channle.CH5>min_threshold)&(PWM_RX.Channle.CH5<max_threshold)
						&(PWM_RX.Channle.CH6>min_threshold)&(PWM_RX.Channle.CH6<max_threshold)
						&(PWM_RX.Channle.CH7>min_threshold)&(PWM_RX.Channle.CH7<max_threshold)
						&(PWM_RX.Channle.CH8>min_threshold)&(PWM_RX.Channle.CH8<max_threshold)
						&(PWM_RX.Channle.CH9>min_threshold)&(PWM_RX.Channle.CH9<max_threshold)
						&(PWM_RX.Channle.CH10>min_threshold)&(PWM_RX.Channle.CH10<max_threshold)
						&(PWM_RX.Channle.CH11>min_threshold)&(PWM_RX.Channle.CH11<max_threshold)
						&(PWM_RX.Channle.CH12>min_threshold)&(PWM_RX.Channle.CH12<max_threshold)
//						&(PWM_RX.Channle.CH14>min_threshold)&(PWM_RX.Channle.CH14<max_threshold)
						)
						{Channlepass_cnt++;}		
				break;							
			 }		
		}		
	}	

/////////////////////////////////////	
	
	time_cnt++;
	Auto_test_display(current_step);
	switch (*current_step)		
	{
		case 0:
			time_cnt=0;
			RLpass_cnt=0;
			sensor_check=0;
			SENSpass_cnt=0;
			Channlepass_cnt=0;
			
		break;
		case 1://RSSI&LQ判断
			Rssi_Lq_test_display();

			if((RLpass_cnt>=1)&(time_cnt>100))
			{	
				Test_Err_Global.RSSI_LQ_Err_S=0;
				LCD_ShowString_color(50,75,GREEN,GRAY,16,"-PASS-",0);
				osDelay(200);
				(*current_step)++;
				time_cnt=0;				
			}
			else if(time_cnt>500)
			{
				Test_Err_Global.RSSI_LQ_Err_S=1;
				LCD_ShowString_color(50,75,RED,GRAY,16,"-FAIL-",0);
				osDelay(200);				
				(*current_step)++;
				time_cnt=0;				
			}				
		break;
		case 2://SENSOR判断
			if((((*current_rx_target)>>1)&0x0f)!=0)
			{
				Sensor_test_display();
				if((SENSpass_cnt>50)&(time_cnt>50))
				{	
					Test_Err_Global.SENSOR_Err_S=0;
					LCD_ShowString_color(50,75,GREEN,GRAY,16,"-PASS-",0);
					osDelay(200);
					(*current_step)++;
					time_cnt=0;				
				}
				else if(time_cnt>300)
				{
					Test_Err_Global.SENSOR_Err_S=1;
					LCD_ShowString_color(50,75,RED,GRAY,16,"-FAIL-",0);
					osDelay(200);
					(*current_step)++;
					time_cnt=0;				
				}	
			}
			else
			{
					(*current_step)++;
					time_cnt=0;	
			}
		break;
		case 3://CHANNLES判断
			Channle_test_display();
			if((Channlepass_cnt>30)&(time_cnt>50))
			{	
				Test_Err_Global.CHANNLE_Err_S=0;
				LCD_ShowString_color(50,75,GREEN,GRAY,16,"-PASS-",0);
				osDelay(200);
				(*current_step)++;
				time_cnt=0;				
			}
			else if(time_cnt>300)
			{
				Test_Err_Global.CHANNLE_Err_S=1;
				LCD_ShowString_color(50,75,RED,GRAY,16,"-FAIL-",0);
				osDelay(200);
				(*current_step)++;
				time_cnt=0;				
			}	
		break;		
		case 4://RESULT
			Test_result();
			(*current_step)=0;
		break;		
		
	}
	
}

enum Test_Err Test_result()
{
		u16 time_out_cnt=0;
		LCD_DrawFillRectangle_color(10, 22, 150, 120,GRAY);
	
		LCD_DrawString(40, 25, "Test_result", 16, RED,GRAY);
		LCD_DrawString(20, 40, "RSSI_LQ---", 16, BLACK,GRAY);
		LCD_DrawString(20, 55, "Sensor----", 16, BLACK,GRAY);
		LCD_DrawString(20, 70, "Channle---", 16, BLACK,GRAY);
			
		if(Test_Err_Global.RSSI_LQ_Err_S==1)
		{
			LCD_DrawString(105, 40, "FAIL", 16, RED,GRAY);
		}
		else
		{
			LCD_DrawString(105, 40, "PASS", 16, GREEN,GRAY);
		}
		if((((*current_rx_target)>>1)&0x0f)!=0)
		{
				if(Test_Err_Global.SENSOR_Err_S==1)
				{
					LCD_DrawString(105, 55, "FAIL", 16, RED,GRAY);
				}
				else
				{
					LCD_DrawString(105, 55, "PASS", 16, GREEN,GRAY);
				}
		}
		if(Test_Err_Global.CHANNLE_Err_S==1)
		{
			LCD_DrawString(105, 70, "FAIL", 16, RED,GRAY);
		}
		else
		{
			LCD_DrawString(105, 70, "PASS", 16, GREEN,GRAY);
		}
		if((Test_Err_Global.RSSI_LQ_Err_S==0)&(Test_Err_Global.SENSOR_Err_S==0)&(Test_Err_Global.CHANNLE_Err_S==0))//pass
		{
		shortbuzzing();
		osDelay(100);	
		shortbuzzing();
		}
		else{	shortbuzzing();}//fail
		
		Test_Err_Global.RSSI_LQ_Err_S=0;
		Test_Err_Global.SENSOR_Err_S=0;
		Test_Err_Global.CHANNLE_Err_S=0;
		
		time_out_cnt++;	
		osDelay(500);
		
}

enum device_Err device_check()
{
	while(Link_State==0)//未连接
	{
		if(0)//未检测到高频头
		{
		
			
			device_ErrINFO=TX_Err;
		}
		if(0)//未检测到接收机
		{
			
			
			device_ErrINFO=RX_Err;
		}	
		osDelay(200);
	}
}

void Show_test_result()
{
	u16 time_out_cnt=0;;
	while(1)
	{
		
		
		time_out_cnt++;	
		osDelay(10);
		if(time_out_cnt>300)
		{
			return;
		}
	}
}

//u8 antenna:	antenna number
//u8 sensor: 
//
void Creat_rx(Rx_target* Rx,u8 antenna,u8 sensor,u8 channles)
{
	*Rx=0;
	if(antenna==2)
	{
		*Rx=*Rx|0x01;
	}else
	{
		*Rx=*Rx|0x00;
	}

	*Rx|=sensor<<1;
	*Rx|=channles<<5;
//	current_rx_target=Rx;
}

void Rx_device_init()
{
	u16 * check_rx_last;
	Creat_rx(&Rx4,0,3,4);
	Creat_rx(&Rx6,0,1,6);
	Creat_rx(&Rx6_dual,0,3,6);
	Creat_rx(&Rx8_dual,0,1,8);
	Creat_rx(&SuperP,0,0,14);
//	stmflash_read(0x080e0000,(u32 *)&check_rx_last,1);
	if((u32)Global_parameter.RX_device_t==0)
	{
		Global_parameter.RX_device_t=&Rx4;
		current_rx_target=Global_parameter.RX_device_t;
		stmflash_write(0x080E0000,(u32 *)&Global_init_parameter,sizeof(Flash_para));
	}
	else
	{
		current_rx_target=Global_parameter.RX_device_t;
	}
}




u8 Get_rx_antenna_numbers()
{
	Rx_target* Rx;
	Rx=current_rx_target;
	
	return (*Rx>>1)&0x01;
}

u8 Get_rx_sensor()
{
	Rx_target* Rx;
	Rx=current_rx_target;
	
	return (*Rx>>1)&0x0f;
}

u8 Get_rx_channle_numbers()
{
	Rx_target* Rx;
	Rx=current_rx_target;
	
	return (*Rx>>5)&0x0f;
}


extern float altitude;
extern float vertical_speed;
extern float RX_voltage;
extern int8_t RX_RSSI_GET,RX_LQ_GET,TX_RSSI_GET,TX_LQ_GET;
extern Pwm_channle_t Pwm_channle;

void Get_PWM_RX_Info()//获取接收机状态
{
//	PWM_RX.RSSI=-TX_RSSI_GET;
//	PWM_RX.LQ=TX_LQ_GET;
//	PWM_RX.ALT=altitude;
//	PWM_RX.Vbat=RX_voltage;
//	PWM_RX.Channle=Pwm_channle;

}


extern unsigned char LINK_CONNECT_STATIC;
extern  uint32_t LINK_CONNECT_Lasttime;
extern  uint32_t LINK_CONNECT_timeout;

void Update_PWM_RX_Info()//获取接收机状态
{		
		if(LINK_CONNECT_Lasttime>LINK_CONNECT_timeout)
		{
			LINK_CONNECT_STATIC=0;			
		}
		else
		{
			LINK_CONNECT_Lasttime++;			
		}
		
		if(Get_LINK_CONNECT_STATIC()==0)
		{
			PWM_RX.RSSI=0;
			PWM_RX.LQ=0;
			PWM_RX.ALT=0;
			PWM_RX.Vbat=0;
			PWM_RX.Channle.CH1=0;
			PWM_RX.Channle.CH2=0;
			PWM_RX.Channle.CH3=0;
			PWM_RX.Channle.CH4=0;
			PWM_RX.Channle.CH5=0;
			PWM_RX.Channle.CH6=0;
			PWM_RX.Channle.CH7=0;
			PWM_RX.Channle.CH8=0;
		}
		else if (Get_LINK_CONNECT_STATIC()==1)
		{
			PWM_RX.RSSI=-TX_RSSI_GET;
			PWM_RX.LQ=TX_LQ_GET;
			PWM_RX.ALT=altitude;
			PWM_RX.Vbat=RX_voltage;
			PWM_RX.Channle=Pwm_channle;	
		}	
}


u8 scan_para(const char *str)
{
	u8 i=0;
	for(i=0;i<parameter_number;i++)
	{	
		if(strcmp(Parameter_List[i]->str,str)==0)
		{
			return i;			
		}
	}
	return 255;
	
}	

u32 flashed_flag=0;
u32 flashed_data=1;
void Flash_para_init()
{		
	Flash_para * para_struct_p=malloc(sizeof(Flash_para));
	read_para(para_struct_p);
	

//	stmflash_read(0x080E8888,&flashed_flag,1);
	if(para_struct_p->check_bit==1)
	{
		read_para(&Global_parameter);
		
	}
	else
	{
		stmflash_write(0x080E0000,(u32 *)&Global_init_parameter,sizeof(Flash_para));
	}
		
}


u8 read_para(Flash_para * parameter_struct)
{
	u8 para_size=0;
	para_size=sizeof(Flash_para);
	if(para_size%4!=0)
	{
		para_size+=para_size%4;
	}
	 stmflash_read(0x080E0000,(u32 *)parameter_struct,para_size);
}


u8 Save_para(const char *str,void *para_value)
{
	u8 t;
	t=scan_para(str);
	if(t==255)
	{
		return 0;
	}
	else//save
	{
		switch (Parameter_List[t]->type)
		{
			case 0:
				break;
			case 1:
				if(*((u32*)para_value)!=0)
				{
					*((u32*)para_value)=1;
				}
				else
				{
					*((u32*)para_value)=0;
				}
    		*(u32*)(Parameter_List[t]->para_p)=*(u32*)para_value;				
				
				break;
			case 2:		
					
					*(u32*)(Parameter_List[t]->para_p)=*(u32*)para_value;						
				break;
			case 3:		
					
					*(int*)(Parameter_List[t]->para_p)=*(int*)para_value;						
				break;
			case 4:		
					
					*(float*)(Parameter_List[t]->para_p)=*(float*)para_value;						
				break;
		}
//		*(Parameter_List[t]->para_p)=*para_value;
	
	}
	return 1;
}