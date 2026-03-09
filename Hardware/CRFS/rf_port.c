#include "rf_port.h"
#include "usart.h"
#include "crfs.h"
#include "freertos.h"
#define rf_huart_port huart4
uint32_t  RF_Transmitter_wd=0;
uint8_t  RF_Transmitter_state=0;
u16 channe_target_value=2000;
float channe_target_fre=0;
TX  TX_MODEL;
uint8_t ping_phrase[]={0xEE, 0x04, 0x28, 0x00, 0xEA, 0x54};
uint8_t ReceiveForTX_buff[64];

uint8_t debug_TX_buff[8]={0xEE,0x06,0x2C, 0xEE, 0xEF, 0x01, 0x00, 0x76};

uint8_t temp_phrase[] ={0xEE, 0x04, 0x28, 0x00, 0xEA, 0x54};


uint8_t BIND_phrase[]={0xEE, 0x06, 0x2D, 0xEE, 0xEF, 0x20, 0x00, 0x7E};
uint8_t traverse_indexes_phrase[]={0xEE, 0x06, 0x2C, 0xEE, 0xEF, 0x01, 0x00, 0x00};
uint8_t CHANNLE_phrase[26]={0xEE, 0x18, 0x16,0xe0, 0x03, 0x1f, 0xf8, 0xc0,0x07,0x3e,0xf0,0x81,0x0f,0x7c,0xE0,0x03,0x1F,0xF8,0xC0,0x07,0x3E,0xF0,0x81,0x0F,0x7C,0xAD};
uint8_t CHANNLE_hex[22]={0xD9, 0xF3, 0x1E, 0x53, 0xEC,0x07,0xBE,0x56,0x4C,0xBC,0x15,0xE0,0x03,0x1F,0xF8,0xC0,0x07,0x3E,0xF0,0x81,0x0F,0x7C};//16个11位通道值合并
uint16_t CHANNLE_value[16]={1000,1000,1000,1000,2000,2000,2000,2000,500,500,500,500,500,500,500,500};//通道值

//ee 18 16 d9 f3 1e 53 ec 07 be 56 4c bc 15 e0 03 1f f8 c0 07 3e f0 81 0f 7c 52
extern uint8_t  *string_data_p;
extern uint8_t  string_data[20];
extern uint8_t  version_string_data[2];
extern uint8_t  indexes_len;


void ping_to_transmitter()//获取高频头信息
{
	HAL_UART_Transmit(&rf_huart_port,ping_phrase,6,10);
}


//uint8_t * field_indexes[20];
uint8_t field_indexes[30][20];

extern uint8_t BIND_indexes_num;
extern u8 index_number;
uint8_t  PING_TX_INFO()//debug
{
	uint8_t crc=0;
	static uint8_t i=0;
//	int length = sizeof(temp_phrase) / sizeof(temp_phrase[0]);
//	length=length-2;
	if(index_number==0)
	{
//		memcpy(temp_phrase,ping_phrase,sizeof(ping_phrase) / sizeof(ping_phrase[0]));
		HAL_UART_Transmit(&rf_huart_port,ping_phrase,6,20);	
	}
	else if(BIND_indexes_num==0)
	{
			for(i=0;i<index_number;i++)//根据索引数量遍历
		{
			debug_TX_buff[5]=i;
			crc= CalcCRC(&debug_TX_buff[2], debug_TX_buff[1]-1);
			debug_TX_buff[7]=crc;
			HAL_UART_Transmit(&rf_huart_port,debug_TX_buff,8,50);
			osDelay(20);

		}
	
	}	
}


uint8_t traverse_indexes_form_TX()//遍历高频头索引信息
{
	uint8_t crc=0;
	static uint8_t i=0;
	ping_to_transmitter();//获取高频头信息
	HAL_UART_Receive_IT(&rf_huart_port,ReceiveForTX_buff,64);

//	for(i=0;i<field_indexes_num;i++)//根据索引数量遍历
//	{
////		field_indexes[i]=(uint8_t *)malloc(15);//为父索引申请内存
//		HAL_Delay(20);
//		traverse_indexes_phrase[5]=i;
//		crc= CalcCRC(&traverse_indexes_phrase[2], traverse_indexes_phrase[1]-1);
//		traverse_indexes_phrase[7]=crc;
//		HAL_UAR T_Transmit(&rf_huart_port,ReceiveForTX_buff,64,50);
//	}
	
}


extern  uint8_t BIND_indexes_num;
void transmitter_BIND()//高频头绑定操作
{
//	
	  static uint8_t BindData[]= {0xEC,0x04,0x32,'b','d',0x23};	
	BIND_phrase[5]=BIND_indexes_num;
	BIND_phrase[7]=CalcCRC(&BIND_phrase[2], BIND_phrase[1]-1);
		HAL_UART_Transmit(&rf_huart_port,BIND_phrase,8,20);
		HAL_UART_Transmit(&huart1,BindData,6,10);
		HAL_UART_Transmit(&huart2,BindData,6,10);
		HAL_UART_Transmit(&huart3,BindData,6,10);
		HAL_UART_Transmit(&huart1,BindData,6,10);
		HAL_UART_Transmit(&huart2,BindData,6,10);
		HAL_UART_Transmit(&huart3,BindData,6,10);				
		HAL_UART_Transmit(&huart1,BindData,6,10);
		HAL_UART_Transmit(&huart2,BindData,6,10);
		HAL_UART_Transmit(&huart3,BindData,6,10);
		osDelay(10);
		HAL_UART_Transmit(&rf_huart_port,BIND_phrase,8,20);
		HAL_UART_Transmit(&rf_huart_port,BIND_phrase,8,20);
		HAL_UART_Transmit(&rf_huart_port,BIND_phrase,8,20);
		HAL_UART_Transmit(&rf_huart_port,BIND_phrase,8,20);
		HAL_UART_Transmit(&rf_huart_port,BIND_phrase,8,20);

}


void transmitter_CHANNLE()//高频头操作
{
//	
	uint8_t crc=0;
	uint8_t i=0;
	for(i=0;i<16;i++)
	{
		CHANNLE_value[i]=channe_target_value;
	}
	merge_11bit_to_22bytes(CHANNLE_value,CHANNLE_hex);
	memcpy(&CHANNLE_phrase[3],CHANNLE_hex,22);
	crc= CalcCRC(&CHANNLE_phrase[2], CHANNLE_phrase[1]-1);
	CHANNLE_phrase[25]=crc;
	HAL_UART_Transmit_IT(&rf_huart_port,CHANNLE_phrase,26);
}


//void transmitter_holding_state()//高频头是否接入
//{
////	
//	RF_Transmitter_wd++;
//	if(RF_Transmitter_wd>=3000)//超过三秒没有收到高频头讯息
//	{
//		RF_Transmitter_state=0;
//		RF_Transmitter_wd=3000;
//	}
//	else
//	{
//	RF_Transmitter_state=1;
//	}
//		
//}

extern uint32_t LINK_CONNECT_Lasttime;
extern uint32_t LINK_CONNECT_timeout;
extern unsigned char LINK_CONNECT_STATIC;
unsigned char Get_LINK_CONNECT_STATIC()
{
	return LINK_CONNECT_STATIC;
}
