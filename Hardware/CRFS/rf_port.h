#ifndef __RF_PORT_H__
#define __RF_PORT_H__
#include "main.h"

//void BeepInit(void);
//void SetBeepStatus(uint8_t NewStatus,uint16_t Time);
typedef struct transmitter
{
	u8 init_state;
	
	char name[20];
	u8 index_num;
	u16 packet_rate;
	u8 power;//mw
	
	
}TX;    
		
		
uint8_t traverse_indexes_form_TX();
uint8_t degub_TX();//debug
void transmitter_BIND();
void transmitter_CHANNLE();//¸ßÆµÍ·²Ù×÷
unsigned char Get_LINK_CONNECT_STATIC();
uint8_t PING_TX_INFO();
#endif

