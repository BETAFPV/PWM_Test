#include "key.h"
#include "rf_port.h"
#include "main.h"
#include "flash.h"




#define KEY_UP_PORT				KeyE_GPIO_Port
#define KEY_DOWN_PORT			KeyC_GPIO_Port
#define KEY_LEFT_PORT			KeyA_GPIO_Port
#define KEY_RIGHT_PORT		KeyD_GPIO_Port
#define KEY_ENTER_PORT		KeyB_GPIO_Port

#define KEY_UP_PIN				KeyE_Pin
#define KEY_DOWN_PIN			KeyC_Pin
#define KEY_LEFT_PIN			KeyA_Pin
#define KEY_RIGHT_PIN			KeyD_Pin
#define KEY_ENTER_PIN			KeyB_Pin


#define KEY_1_PORT				KEY_1_GPIO_Port
#define KEY_2_PORT				KEY_2_GPIO_Port


#define KEY_1_PIN					KEY_1_Pin
#define KEY_2_PIN					KEY_2_Pin


uint8_t vtx_debug_data[6]={0xAB,0XFF,0XFF,0X0f,0X0f,0XBA};
extern uint8_t   rx_data[2];
uint16_t vtx_vdpoint=500;
extern  signed char para_pointer;
extern int gears_vdpoint;
extern uint16_t gears_vd[5];
uint8_t  DATA_TYPE=0;
u32 flash_write_test_word=0xaabb;

u8 Key_5D_state=0;
enum
{
	 up=1,
	 down,	
	 left,
	 right,
	 enter
}Key_5D;


void transmitter_vtx_vdpoint(uint16_t vtx_vd)
{

	
}


uint8_t key_5d_state;

void Key_Scan(void )
{

			if(HAL_GPIO_ReadPin(KEY_2_PORT,KEY_2_PIN)==RESET)//  BIND
		{
			uint16_t key_hold_time=0;
			if(HAL_GPIO_ReadPin(KEY_2_PORT,KEY_2_PIN)==RESET)//按下立刻操作
			{
				transmitter_BIND();
			Key_5D_state=7;
			}
			while(HAL_GPIO_ReadPin(KEY_2_PORT,KEY_2_PIN)==RESET)
			{	
				uint8_t i=0;
				for(i=255;i>1;i--)
				{__nop;}
				key_hold_time++;
			
			}//松开后操作
			
				if(key_hold_time>5&key_hold_time<100)
			{
					
			}
		}

}





Key_TypeDef Key_5D_Scan(Key_TypeDef *dev, Key_Event_TypeDef *event)
{
		*event = KEY_EVENT_NONE;
			if(HAL_GPIO_ReadPin(KEY_UP_PORT,KEY_UP_PIN)==RESET)//下
		{
			u32 key_hold_time=0;
			if(HAL_GPIO_ReadPin(KEY_UP_PORT,KEY_UP_PIN)==RESET)
			{
			*event=KEY_EVENT_PRESS;
			Key_5D_state=1;
			}
			while(HAL_GPIO_ReadPin(KEY_UP_PORT,KEY_UP_PIN)==RESET)
			{
				uint16_t i=0;
				for(i=65535;i>1;i--)
				{__nop();}
				key_hold_time++;
			}
			if(key_hold_time>5&key_hold_time<100)
			{

			}
			return KEY_UP;
		}

			if(HAL_GPIO_ReadPin(KEY_DOWN_PORT,KEY_DOWN_PIN)==RESET)//下
		{
			u32 key_hold_time=0;
			if(HAL_GPIO_ReadPin(KEY_DOWN_PORT,KEY_DOWN_PIN)==RESET)
			{
			Key_5D_state=2;
				*event=KEY_EVENT_PRESS;
			}
			while(HAL_GPIO_ReadPin(KEY_DOWN_PORT,KEY_DOWN_PIN)==RESET)
			{
				uint16_t i=0;
				for(i=65535;i>1;i--)
				{__nop();}
				key_hold_time++;
			}
			if(key_hold_time>5&key_hold_time<100)
			{

			}
			return KEY_DOWN;
		}
		
			if(HAL_GPIO_ReadPin(KEY_LEFT_PORT,KEY_LEFT_PIN)==RESET)//左
		{
			u32 key_hold_time=0;
			if(HAL_GPIO_ReadPin(KEY_LEFT_PORT,KEY_LEFT_PIN)==RESET)
			{
			Key_5D_state=3;
				*event=KEY_EVENT_PRESS;

			}
			while(HAL_GPIO_ReadPin(KEY_LEFT_PORT,KEY_LEFT_PIN)==RESET)
			{
				uint16_t i=0;
				for(i=15535;i>1;i--)
				{__nop();}
				key_hold_time++;
				if(key_hold_time>500)
				{
				if(key_hold_time%20==0)
				{

				}
							
				}
//				if(key_hold_time>400)
//				{
////					parameter_pointer_selectEN=1;

//					break;
//				}
			}
				if(key_hold_time>5&key_hold_time<100)
			{
			}
			return KEY_LEFT;
		}

		
		
		
			if(HAL_GPIO_ReadPin(KEY_RIGHT_PORT,KEY_RIGHT_PIN)==RESET)//右
		{
			u32 key_hold_time=0;
			if(HAL_GPIO_ReadPin(KEY_RIGHT_PORT,KEY_RIGHT_PIN)==RESET)
			{
			Key_5D_state=4;
				*event=KEY_EVENT_PRESS;

			}
			while(HAL_GPIO_ReadPin(KEY_RIGHT_PORT,KEY_RIGHT_PIN)==RESET)
			{
				uint16_t i=0;
				for(i=15535;i>1;i--)
				{__nop();}
				key_hold_time++;
				if(key_hold_time>500)
				{

							
				}
			}
				if(key_hold_time>5&key_hold_time<100)
			{


			}
			return KEY_RIGHT;
		}
		
		
		
			if(HAL_GPIO_ReadPin(KEY_ENTER_PORT,KEY_ENTER_PIN)==RESET)//中
		{
			u32 key_hold_time=0;
			if(HAL_GPIO_ReadPin(KEY_ENTER_PORT,KEY_ENTER_PIN)==RESET)
			{
				Key_5D_state=5;
				*event=KEY_EVENT_PRESS;


			}
			while(HAL_GPIO_ReadPin(KEY_ENTER_PORT,KEY_ENTER_PIN)==RESET)//长按切换屏幕
			{
				uint16_t i=0;
				for(i=65535;i>1;i--)
				{__nop();}
				key_hold_time++;
				if(key_hold_time>400)
				{
				
					break;
				}
//				else if(key_hold_time&&key_hold_time<100)
//				{
//					
//					break;
//				}
			}
			return KEY_ENTER;
		}

		if(HAL_GPIO_ReadPin(KEY_1_PORT,KEY_1_PIN)==RESET)//TEST
		{
			uint16_t key_hold_time=0;
			
			if(HAL_GPIO_ReadPin(KEY_1_PORT,KEY_1_PIN)==RESET)//按下立刻操作
			{
				
//				transmitter_BIND();
//			Key_5D_state=6;
				*event=KEY_EVENT_PRESS;
			}
			while(HAL_GPIO_ReadPin(KEY_1_PORT,KEY_1_PIN)==RESET)
			{	
				
				uint8_t i=0;
				for(i=255;i>1;i--)
				{__nop;}
				key_hold_time++;
			
			}//松开后操作
			
				if(key_hold_time>5&key_hold_time<100)
			{
				
			}
//			stmflash_write(0x080E8888,&flashed_data,1);
			shortbuzzing();
			return KEY_1;
		}
}



void shortbuzzing()
{
	HAL_GPIO_WritePin(BUZZ_GPIO_Port,BUZZ_Pin,1);
	osDelay(100);
//	HAL_Delay(100);
	HAL_GPIO_WritePin(BUZZ_GPIO_Port,BUZZ_Pin,0);
}



// 



