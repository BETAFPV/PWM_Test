/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "lcd.h"
#include "gui.h"
#include "USART.h"
#include "device.h"
#include "flash.h"
#include "pwm_freq.h"
#include "crfs.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
extern u16 channe_target_value;
extern Rx_target * current_rx_target;
extern Rx_target Rx4,Rx6,Rx6_dual,Rx8_dual;//PWM接收机
extern Rx_target SuperD,SuperXnano,SuperXmono;//rx接收机
			extern float channe_target_fre;
extern	u8 CRSF_FRAMETYPE_BARO_ALTITUDE_static;

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */
extern u8 Test_Ready;



/* USER CODE END Variables */
osThreadId defaultTaskHandle;

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
osThreadId BaseTask_1_Handle;
void BaseTask_1_Fun(void const * argument);


osThreadId BaseTask_2_Handle;
void BaseTask_2_Fun(void const * argument);

osThreadId BaseTask_3_Handle;
void BaseTask_3_Fun(void const * argument);


osThreadId RX_TESTTask_Handle;
void RX_TEST_Auto_Task_Fun(void const * argument);


osThreadId RX_MANULTask_Handle;
void RX_TEST_MANUL_Task_Fun(void const * argument);

osThreadId TransmiterTask_Handle;
void Transmiter_Task_Fun(void const * argument);
/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void const * argument);

extern void MX_USB_DEVICE_Init(void);
void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* GetIdleTaskMemory prototype (linked to static allocation support) */
void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize );

/* USER CODE BEGIN GET_IDLE_TASK_MEMORY */
static StaticTask_t xIdleTaskTCBBuffer;
static StackType_t xIdleStack[configMINIMAL_STACK_SIZE];

void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize )
{
  *ppxIdleTaskTCBBuffer = &xIdleTaskTCBBuffer;
  *ppxIdleTaskStackBuffer = &xIdleStack[0];
  *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
  /* place for user code */
}
/* USER CODE END GET_IDLE_TASK_MEMORY */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* definition and creation of defaultTask */
  osThreadDef(defaultTask, StartDefaultTask, osPriorityNormal, 0, 128);
  defaultTaskHandle = osThreadCreate(osThread(defaultTask), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */

  /* USER CODE END RTOS_THREADS */

}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void const * argument)
{
  /* init code for USB_DEVICE */
  MX_USB_DEVICE_Init();
  /* USER CODE BEGIN StartDefaultTask */
	
//	LcdInit();	

		 osThreadDef(BaseTask_1_Handle, BaseTask_1_Fun, osPriorityNormal, 0, 1024);
		BaseTask_1_Handle = osThreadCreate(osThread(BaseTask_1_Handle), NULL);
		
		osThreadDef(BaseTask_2_Handle, BaseTask_2_Fun, osPriorityAboveNormal, 0, 128);
		BaseTask_2_Handle = osThreadCreate(osThread(BaseTask_2_Handle), NULL);

		osThreadDef(BaseTask_3_Handle, BaseTask_3_Fun, osPriorityNormal, 0, 128);
		BaseTask_3_Handle = osThreadCreate(osThread(BaseTask_3_Handle), NULL);	
	
		osThreadDef(RX_TESTTask_Handle, RX_TEST_Auto_Task_Fun, osPriorityAboveNormal, 0, 2028);
		RX_TESTTask_Handle = osThreadCreate(osThread(RX_TESTTask_Handle), NULL);
		vTaskSuspend(RX_TESTTask_Handle);
		osThreadDef(RX_MANULTask_Handle, RX_TEST_MANUL_Task_Fun, osPriorityAboveNormal, 0, 1024);
		RX_MANULTask_Handle = osThreadCreate(osThread(RX_MANULTask_Handle), NULL);
		vTaskSuspend(RX_MANULTask_Handle);	
		osThreadDef(TransmiterTask_Handle, Transmiter_Task_Fun, osPriorityNormal, 0, 128);
		TransmiterTask_Handle = osThreadCreate(osThread(TransmiterTask_Handle), NULL);
		

//	LCD_Clear(RED);
  /* Infinite loop */
  for(;;)
  {

		
//		
    osDelay(1000);
  }
  /* USER CODE END StartDefaultTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
void BaseTask_1_Fun(void const * argument)
{
		LCD_Init();
	drawMainMenu();
	HAL_Delay(300);
  for(;;)
  {
		Key_Scan( );
		Menu_Process();
    osDelay(10);
  }
}



extern u8 Key_5D_state;
void BaseTask_2_Fun(void const * argument)
{
  for(;;)
  {

		Get_Pwm_channle();
		Update_PWM_RX_Info();
		Get_PWM_RX_Info();
		dynamic_display();
    osDelay(20);
  }
}

void BaseTask_3_Fun(void const * argument)
{
  for(;;)
  {
		static u8 t;
		if(t)
		{
			channe_target_value=300;
			channe_target_fre=5.1;
			t=0;
		}
		else
		{
			channe_target_value=2047;
			channe_target_fre=10;
			t=1;
		}
		Global_dynamic_display();
    osDelay(600);
  }


}


extern u8 test_mode;
u8 test_step=0;
void RX_TEST_Auto_Task_Fun(void const * argument)
{
u8 i=0;
  for(;;)
  {
		if(test_mode==1)
		{
			if(Test_Ready==1)//自动测试
			{
				
				while(test_step<=4)
				{
					i=0;
					Auto_test(&test_step);
					osDelay(10);	
				}
			}
		}
    osDelay(10);
  }
}


extern Flash_para Global_parameter;
extern unsigned char LINK_CONNECT_STATIC;
extern RX_Device_t PWM_RX;//接收机实体
extern crsfLinkStatistics_t RX1_telemtering;
extern crsfLinkStatistics_t RX2_telemtering;
extern crsfLinkStatistics_t RX3_telemtering;
extern crsfLinkStatistics_t RX4_telemtering;
u16 td1;
void RX_TEST_MANUL_Task_Fun(void const * argument)
{

	
  for(;;)
  {
		if(Test_Ready==1)
		{
			while(test_mode==2)
			{
				
//				td1=*current_rx_target&0x8000;
					Manual_test_display();

				osDelay(10);
			}			
		}
    osDelay(10);
  }
}



void Transmiter_Task_Fun(void const * argument)
{
  for(;;)
  {
	transmitter_CHANNLE();
   osDelay(10);
		
  }
}
/* USER CODE END Application */
