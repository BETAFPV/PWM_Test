#include "crfs.h"
#include "usart.h"
#include "main.h"
#include "rf_port.h"
RCDATA_t crsfData;

uint8_t CRSF_RawData_Flag;
extern uint32_t  RF_Transmitter_wd;
extern uint8_t  RF_Transmitter_state;//高频头在线状态

uint8_t RssiPercent=0;
uint8_t RXLQ=0;
int _channels[CRSF_NUM_CHANNELS];
crsfLinkStatistics_t _linkStatistics;

crsfLinkStatistics_t RX1_telemtering;
crsfLinkStatistics_t RX2_telemtering;
crsfLinkStatistics_t RX3_telemtering;
crsfLinkStatistics_t RX4_telemtering;
crsfLinkStatistics_t RX5_telemtering;
crsfLinkStatistics_t RX6_telemtering;

RX_online_flag RX1_online_flag,RX2_online_flag,RX3_online_flag,RX5_online_flag,RX6_online_flag;

int8_t RX1_RSSI,RX1_LQ_GET,//接收机回传信息
			 RX2_RSSI,RX2_LQ_GET,
			 RX3_RSSI,RX3_LQ_GET,
			 RX5_RSSI,RX5_LQ_GET,
		   RX6_RSSI,RX6_LQ_GET;					
					
int8_t TX_RSSI_GET,TX_LQ_GET;//高频头回传信息
unsigned char LINK_CONNECT_STATIC=0;
uint32_t LINK_CONNECT_Lasttime=0;
uint32_t LINK_CONNECT_timeout=75;

uint8_t _lut[256];//CRC初始化数组

uint8_t CRSF_RawData_1[CRSF_MAX_PACKET_LEN];
uint8_t CRSF_RawData_2[CRSF_MAX_PACKET_LEN];
uint8_t CRSF_RawData_3[CRSF_MAX_PACKET_LEN];
uint8_t CRSF_RawData_4[CRSF_MAX_PACKET_LEN];
uint8_t CRSF_RawData_5[CRSF_MAX_PACKET_LEN];
uint8_t CRSF_RawData_6[CRSF_MAX_PACKET_LEN];


long map(long x, long in_min, long in_max, long out_min, long out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}


void Crc8_init(uint8_t poly)
{
    for (int idx=0; idx<256; ++idx)
    {
        uint8_t crc = idx;
        for (int shift=0; shift<8; ++shift)
        {
            crc = (crc << 1) ^ ((crc & 0x80) ? poly : 0);
        }
        _lut[idx] = crc & 0xff;
    }
}


uint8_t CalcCRC(uint8_t *data, uint8_t len)
{
    uint8_t crc = 0;
    
    while (len--)
    {
        crc = _lut[crc ^ *data++];
    }
    return crc;
}

void merge_11bit_to_22bytes(const uint16_t src[16], uint8_t dest[22]) {
    // 1. 初始化输出数组（避免脏数据）
    memset(dest, 0, 22);

    // 2. 位指针：记录当前填充到第几位（0~175）
    uint16_t bit_offset = 0;

    // 3. 遍历16个11位数据，逐位拼接
    for (int i = 0; i < 16; i++) {
        // 确保输入数据是11位（超出则截断，避免溢出）
        uint16_t data = src[i] & 0x7FF;  // 0x7FF=二进制11个1，保留低11位

        // 遍历当前数据的每一位（0~10位，共11位）
        for (int bit = 0; bit < 11; bit++) {
            // 计算当前位对应的字节索引（0~21）
            uint8_t byte_idx = bit_offset / 8;
            // 计算当前位在字节内的偏移（0~7）
            uint8_t bit_in_byte = bit_offset % 8;

            // 将数据的第bit位写入目标字节的对应位置
            if (data & (1 << bit)) {
                dest[byte_idx] |= (1 << bit_in_byte);  // 置1
            } else {
                dest[byte_idx] &= ~(1 << bit_in_byte); // 置0
            }

            // 位指针递增
            bit_offset++;
        }
    }
}


void split_22bytes_to_11bit(const uint8_t src[22], uint16_t dest[16]) {
    memset(dest, 0, 16 * sizeof(uint16_t));
    uint16_t bit_offset = 0;

    for (int i = 0; i < 16; i++) {
        uint16_t data = 0;
        for (int bit = 0; bit < 11; bit++) {
            uint8_t byte_idx = bit_offset / 8;
            uint8_t bit_in_byte = bit_offset % 8;

            if (src[byte_idx] & (1 << bit_in_byte)) {
                data |= (1 << bit);
            }

            bit_offset++;
        }
        dest[i] = data;
    }
}



int GetCrsfRssiDbm(void)
{
    int RssiDbm=0;
    if(_linkStatistics.active_antenna == 0)
    {
        RssiDbm = _linkStatistics.uplink_RSSI_1;
    }
    else
    {
        RssiDbm = _linkStatistics.uplink_RSSI_2;
    }
    return RssiDbm;
}


uint8_t GetCrsfLinkQuality(void)
{
    return _linkStatistics.uplink_Link_quality;
}

void GetRxTelemetering_info(crsfLinkStatistics_t *RX,crsfLinkStatistics_t *Receive_Tele)
{
			
			RX->uplink_RSSI_1=Receive_Tele->uplink_RSSI_1;							
			RX->uplink_RSSI_2= Receive_Tele->uplink_RSSI_2;				
			RX->uplink_Link_quality=Receive_Tele->uplink_Link_quality;	
			RX->uplink_SNR=Receive_Tele->uplink_SNR;						
			RX->active_antenna=Receive_Tele->active_antenna;				
			RX->rf_Mode=Receive_Tele->rf_Mode;							
			RX->uplink_TX_Power=Receive_Tele->uplink_TX_Power;			
			RX->downlink_RSSI=Receive_Tele->downlink_RSSI;				
			RX->downlink_Link_quality=Receive_Tele->downlink_Link_quality;
			RX->downlink_SNR=Receive_Tele->downlink_SNR;					

}


void PacketChannelsPacked(const crsf_header_t *p)
{
    crsf_channels_t *ch = (crsf_channels_t *)&p->data;
    _channels[0] = ch->ch0;
    _channels[1] = ch->ch1;
    _channels[2] = ch->ch2;
    _channels[3] = ch->ch3;
    _channels[4] = ch->ch4;
    _channels[5] = ch->ch5;
    _channels[6] = ch->ch6;
    _channels[7] = ch->ch7;
    _channels[8] = ch->ch8;
    _channels[9] = ch->ch9;
    _channels[10] = ch->ch10;
    _channels[11] = ch->ch11;
    _channels[12] = ch->ch12;
    _channels[13] = ch->ch13;
    _channels[14] = ch->ch14;
    _channels[15] = ch->ch15;

    for (unsigned int i=0; i<CRSF_NUM_CHANNELS; ++i)
        _channels[i] = map(_channels[i], CRSF_CHANNEL_VALUE_1000, CRSF_CHANNEL_VALUE_2000, 1000, 2000);

    RssiPercent = map(_channels[15],1000,2000,0,99);//Rssi矛袪毡詺谞讟脣舀矛邖藝貈止俑远謩值一纸樱
    RXLQ = map(_channels[14],1000,2000,0,99);//LQ
    
    crsfData.roll     = _channels[0];
    crsfData.pitch    = _channels[1];
    crsfData.throttle = _channels[2];
    crsfData.yaw      = _channels[3];
    crsfData.aux1     = _channels[4];
    crsfData.aux2     = _channels[5];
    crsfData.aux3     = _channels[6];
    crsfData.aux4     = _channels[7];
    crsfData.aux5     = _channels[8];
    crsfData.aux6     = _channels[9];
    crsfData.aux7     = _channels[10];
    crsfData.aux8     = _channels[11];
    crsfData.aux9     = _channels[12];
    crsfData.aux10    = _channels[13];
    crsfData.aux11    = _channels[14];
    crsfData.aux12    = _channels[15];
    //一帧私邼蔻隙为詨
//    if((crsfRcDataCallbackFunc != 0))
//        (*crsfRcDataCallbackFunc)(crsfData);
//    //狮變为l迵状态
//    SetElrsStatus(3);
}

void PacketLinkStatistics(const crsf_header_t *p)
{
    const crsfLinkStatistics_t *link = (crsfLinkStatistics_t *)p->data;
    memcpy(&_linkStatistics, link, sizeof(_linkStatistics));
}


extern TX  TX_MODEL;
uint8_t  *string_data_p;
uint8_t  string_data[20];
uint8_t  version_string_data[2];
//uint8_t  indexes_len=0;

//uint8_t *CRSF_RawData_point;
//uint8_t *CRSF_RawData_point;

char *Device_name;
u8 index_number;
void Packet_PING_Device_INFO(const crsf_header_t *p)
{
//	u8 length=CRSF_RawData_4[1];
//	Device_name=&CRSF_RawData_4[5];
//	index_number=CRSF_RawData_4[length-1];
	
	uint8_t  string_data_len;
	string_data_p=&CRSF_RawData_4[5];
	strcpy(string_data, string_data_p);//高频头命名
	string_data_len=strlen (string_data);
	version_string_data[0]=CRSF_RawData_4[5+string_data_len+10];//elrs版本号
	version_string_data[1]=CRSF_RawData_4[5+string_data_len+11];
	index_number=CRSF_RawData_4[p->frame_size-1];//索引数量
	TX_MODEL.index_num=index_number;
	
}

uint8_t BIND_indexes_num=0;

void PacketPARAMETER_INFO(const crsf_header_t *p)
{

	uint8_t  string_data_len;
	string_data_p=&CRSF_RawData_4[9];
	
	if(strstr(string_data_p,"Bind"))//寻找BIND的索引
	{
		BIND_indexes_num=CRSF_RawData_4[5];
		strcpy(string_data, string_data_p);
		
	
	}
	else
	{
	
	
	}
}


uint32_t vertical_speed_hex;
uint32_t altitude_hex;
u8 CRSF_FRAMETYPE_BARO_ALTITUDE_static=0;
float altitude=0;
float vertical_speed;



void Packet_BARO_ALTITUDE_INFO(const crsf_header_t *p)
{
	uint8_t  string_data_len;
	altitude_hex=(CRSF_RawData_4[3]<<8)|CRSF_RawData_4[4];
	altitude=((float)((int)altitude_hex-10000))/10;
//	vertical_speed_hex=(CRSF_RawData_2[5]<<8)|CRSF_RawData_2[6];
//	vertical_speed=vertical_speed_hex/100;
}

float RX_voltage;
float RX_voltage_HEX;
void Packet_BATTERY_SENSOR_INFO(const crsf_header_t *p)
{
	RX_voltage=CRSF_RawData_4[4];
	RX_voltage_HEX=RX_voltage;
	RX_voltage=RX_voltage/10;
}

extern u8 RSSI_average_array[10];
extern u8 RSSI_LQ_average_cnt;
extern u8 LQ_average_array[10];
extern u8 LQ_average_cnt;
const crsf_header_t * hdr ;
const crsf_header_t * hdr1 ;
const crsf_header_t * hdr2 ;
const crsf_header_t * hdr3 ;
const crsf_header_t * hdr4 ;
const crsf_header_t * hdr5 ;
const crsf_header_t * hdr6 ;
void ProcessPacketIn(uint8_t len)
{

    switch(CRSF_RawData_Flag)
    {
        case CRSF_RX1_GET://接收机1
					{
						 hdr1 = (crsf_header_t *)CRSF_RawData_1;
								if (hdr1->device_addr == CRSF_ADDRESS_FLIGHT_CONTROLLER)
								{
										switch (hdr1->type)
										{
												case CRSF_FRAMETYPE_RC_CHANNELS_PACKED:
														PacketChannelsPacked(hdr1);
														break;
												case CRSF_FRAMETYPE_LINK_STATISTICS:
														PacketLinkStatistics(hdr1);
														RX1_RSSI=GetCrsfRssiDbm();
														RX1_LQ_GET=GetCrsfLinkQuality(); 												
														GetRxTelemetering_info(&RX1_telemtering,&_linkStatistics);
														Feed_RX_wdg(1);

														break;
												case CRSF_FRAMETYPE_MSP_WRITE:
														break;
												case CRSF_FRAMETYPE_DEVICE_INFO:
														break;
										}
								} 
					}
			break;
			  case CRSF_RX2_GET://接收机2
				{
					hdr2 = (crsf_header_t *)CRSF_RawData_2;
				// CRSF_ADDRESS_FLIGHT_CONTROLLER
					if (hdr2->device_addr == CRSF_ADDRESS_FLIGHT_CONTROLLER)
					{
							switch (hdr2->type)
							{
									case CRSF_FRAMETYPE_RC_CHANNELS_PACKED:
											PacketChannelsPacked(hdr2);
									break;
									case CRSF_FRAMETYPE_LINK_STATISTICS:
											PacketLinkStatistics(hdr2);
											RX2_RSSI=GetCrsfRssiDbm();
											RX2_LQ_GET=GetCrsfLinkQuality(); 			
											GetRxTelemetering_info(&RX2_telemtering,&_linkStatistics);
											Feed_RX_wdg(2);
									break;
									case CRSF_FRAMETYPE_MSP_WRITE:
									break;
									case CRSF_FRAMETYPE_DEVICE_INFO:
									break;
							}
					}
				}
			break;
        case CRSF_RX3_GET: //接收机3
				{
					hdr3 = (crsf_header_t *)CRSF_RawData_3;
				// CRSF_ADDRESS_FLIGHT_CONTROLLER
					if (hdr3->device_addr == CRSF_ADDRESS_FLIGHT_CONTROLLER)
				{
							switch (hdr3->type)
							{
									case CRSF_FRAMETYPE_RC_CHANNELS_PACKED:
											PacketChannelsPacked(hdr3);
									break;
									case CRSF_FRAMETYPE_LINK_STATISTICS:
											PacketLinkStatistics(hdr3);
											RX3_RSSI=GetCrsfRssiDbm();
											RX3_LQ_GET=GetCrsfLinkQuality(); 			
											GetRxTelemetering_info(&RX3_telemtering,&_linkStatistics);
											Feed_RX_wdg(3);

									break;
									case CRSF_FRAMETYPE_MSP_WRITE:
									break;
									case CRSF_FRAMETYPE_DEVICE_INFO:\
									break;
							}
					}		
				}
			break;
			  case CRSF_RX4_GET://高频头
				{ 
						hdr4 = (crsf_header_t *)CRSF_RawData_4;
								// CRSF_ADDRESS_RADIO_TRANSMITTER
						if (hdr4->device_addr == CRSF_ADDRESS_RADIO_TRANSMITTER)
						{
								switch (hdr4->type)
								{
										case CRSF_FRAMETYPE_RC_CHANNELS_PACKED:
												PacketChannelsPacked(hdr4);
												break;
										case CRSF_FRAMETYPE_LINK_STATISTICS:
													PacketLinkStatistics(hdr4);
													LINK_CONNECT_STATIC=1;
													LINK_CONNECT_Lasttime=0;
													TX_RSSI_GET=GetCrsfRssiDbm();
													TX_LQ_GET=GetCrsfLinkQuality(); 
													if(RSSI_LQ_average_cnt<=10)
													{
														RSSI_average_array[RSSI_LQ_average_cnt]=-TX_RSSI_GET;
														LQ_average_array[RSSI_LQ_average_cnt]=TX_LQ_GET;
														RSSI_LQ_average_cnt++;
													}						
												break;
										case CRSF_FRAMETYPE_MSP_WRITE:
												break;
										case CRSF_FRAMETYPE_DEVICE_INFO:
												Packet_PING_Device_INFO(hdr4);
												break;
										case CRSF_FRAMETYPE_PARAMETER_SETTINGS_ENTRY://2B
												PacketPARAMETER_INFO(hdr4);
												break;
										case CRSF_FRAMETYPE_BARO_ALTITUDE://09
													if(CRSF_FRAMETYPE_BARO_ALTITUDE_static<=100)
													{CRSF_FRAMETYPE_BARO_ALTITUDE_static++;}													
													Packet_BARO_ALTITUDE_INFO(hdr4);
												break;
									
										case CRSF_FRAMETYPE_BATTERY_SENSOR://08
										Packet_BATTERY_SENSOR_INFO(hdr4);
												break;
										//Para_inex//
//										case CRSF_FRAMETYPE_BATTERY_SENSOR://08
//										Packet_BATTERY_SENSOR_INFO(hdr4);
//												break;
										
								}
						} // CRSF_ADDRESS_RADIO_TRANSMITTER
					}	
			break;
			case CRSF_RX5_GET: //接收机4
				{
					hdr5 = (crsf_header_t *)CRSF_RawData_5;
				// CRSF_ADDRESS_FLIGHT_CONTROLLER
					if (hdr5->device_addr == CRSF_ADDRESS_FLIGHT_CONTROLLER)
				{
							switch (hdr5->type)
							{
									case CRSF_FRAMETYPE_RC_CHANNELS_PACKED:
											PacketChannelsPacked(hdr5);
									break;
									case CRSF_FRAMETYPE_LINK_STATISTICS:
											PacketLinkStatistics(hdr5);
											RX5_RSSI=GetCrsfRssiDbm();
											RX5_LQ_GET=GetCrsfLinkQuality(); 	
											GetRxTelemetering_info(&RX5_telemtering,&_linkStatistics);
											Feed_RX_wdg(5);
//											RX4_LQ_GET=GetCrsfLinkQuality(); 
									break;
									case CRSF_FRAMETYPE_MSP_WRITE:
									break;
									case CRSF_FRAMETYPE_DEVICE_INFO:\
									break;
							}
					}		
				}
			break;
			case CRSF_RX6_GET: //接收机5
				{
					hdr6 = (crsf_header_t *)CRSF_RawData_6;
				// CRSF_ADDRESS_FLIGHT_CONTROLLER
					if (hdr6->device_addr == CRSF_ADDRESS_FLIGHT_CONTROLLER)
				{
							switch (hdr6->type)
							{
									case CRSF_FRAMETYPE_RC_CHANNELS_PACKED:
											PacketChannelsPacked(hdr6);
									break;
									case CRSF_FRAMETYPE_LINK_STATISTICS:
											PacketLinkStatistics(hdr6);
											RX6_RSSI=GetCrsfRssiDbm();
											RX6_LQ_GET=GetCrsfLinkQuality(); 	
											GetRxTelemetering_info(&RX6_telemtering,&_linkStatistics);
											Feed_RX_wdg(6);
//											RX5_LQ_GET=GetCrsfLinkQuality(); 
									break;
									case CRSF_FRAMETYPE_MSP_WRITE:
									break;
									case CRSF_FRAMETYPE_DEVICE_INFO:\
									break;
							}
					}		
				}
			break;	
   }

}

uint8_t len = 0;
uint8_t inCrc = 0;
uint8_t crc=0;
uint32_t dataCnt  ;
void CRSFReceived()
{

	switch(CRSF_RawData_Flag)
	{
			case CRSF_RX1_GET:      
			//UART1
		 { 
//				ch1_receive_state=0;
				if(CRSF_RawData_1[0] == CRSF_ADDRESS_FLIGHT_CONTROLLER && 0x0E == (CRSF_RawData_1[1] + 0X02) \
						&& CRSF_RawData_1[1] >= 3 && CRSF_RawData_1[1] <= CRSF_MAX_PACKET_LEN)//如果同步信号和长度正确
				{
						len = CRSF_RawData_1[1];                  //获取数据包长度
						inCrc = CRSF_RawData_1[2 + len - 1];      //获取数据包校验码
						crc= CalcCRC(&CRSF_RawData_1[2], len - 1);//计算数据包校验码
						if (crc == inCrc)                      	  //验证校验码
						{
						ProcessPacketIn(len);	
						} 							  //校验通过，解析数据包
						else  if(crc != inCrc)
						{
							memset(CRSF_RawData_1, 0, CRSF_MAX_PACKET_LEN);							   // 清除接收缓存
							break ;
						}
				}
				memset(CRSF_RawData_1, 0, CRSF_MAX_PACKET_LEN);							   // 清除接收缓存
			}
		 break ;
			
			case CRSF_RX2_GET:      
			//UART2
		 { 		 
				if(CRSF_RawData_2[0] == CRSF_ADDRESS_FLIGHT_CONTROLLER && 0x0E == (CRSF_RawData_2[1] + 0X02) \
						&& CRSF_RawData_2[1] >= 3 && CRSF_RawData_2[1] <= CRSF_MAX_PACKET_LEN)//如果同步信号和长度正确
				{
						len = CRSF_RawData_2[1];                  //获取数据包长度
						inCrc = CRSF_RawData_2[2 + len - 1];      //获取数据包校验码
						crc= CalcCRC(&CRSF_RawData_2[2], len - 1);//计算数据包校验码
						if (crc == inCrc)                      	  //验证校验码
						{ 
						ProcessPacketIn(len);
						} 							  //校验通过，解析数据包
						else  if(crc != inCrc)
						{
							memset(CRSF_RawData_2, 0, CRSF_MAX_PACKET_LEN);							   // 清除接收缓存							
							break ;
						}

				}
			 	 memset(CRSF_RawData_2, 0, CRSF_MAX_PACKET_LEN);							   // 清除接收缓存							
			}
		 break ;

			case CRSF_RX3_GET:      
			//UART3
		 { 		 
				if(CRSF_RawData_3[0] == CRSF_ADDRESS_FLIGHT_CONTROLLER && 0x0E == (CRSF_RawData_3[1] + 0X02) \
						&& CRSF_RawData_3[1] >= 3 && CRSF_RawData_3[1] <= CRSF_MAX_PACKET_LEN)//如果同步信号和长度正确
				{
						len = CRSF_RawData_3[1];                  //获取数据包长度
						inCrc = CRSF_RawData_3[2 + len - 1];      //获取数据包校验码
						crc= CalcCRC(&CRSF_RawData_3[2], len - 1);//计算数据包校验码
						if (crc == inCrc)                      	  //验证校验码
						{ 
							ProcessPacketIn(len);
						} 							  //校验通过，解析数据包
						else  if(crc != inCrc)
						{
							memset(CRSF_RawData_3, 0, CRSF_MAX_PACKET_LEN);							   // 清除接收缓存							
							break ;
						}

				}
			 	 memset(CRSF_RawData_3, 0, CRSF_MAX_PACKET_LEN);							   // 清除接收缓存							
			}
		 break ;
			
		case CRSF_RX4_GET://高频头      
			//UART4
		 { 		 
			 if(CRSF_RawData_4[0] == CRSF_ADDRESS_RADIO_TRANSMITTER  \
						&& CRSF_RawData_4[1] >= 3 && CRSF_RawData_4[1] <= CRSF_MAX_PACKET_LEN)//如果帧头是高频头
				{
						len = CRSF_RawData_4[1];                  //获取数据包长度
						inCrc = CRSF_RawData_4[2 + len - 1];      //获取数据包校验码
						crc= CalcCRC(&CRSF_RawData_4[2], len - 1);//计算数据包校验码
						if (crc == inCrc)                      	  //验证校验码
						{ 
							ProcessPacketIn(len);
						} 							  //校验通过，解析数据包
						else  if(crc != inCrc)
						{
							memset(CRSF_RawData_4, 0, CRSF_MAX_PACKET_LEN);							   // 清除接收缓存							
							break ;
						}
				}
			 	 memset(CRSF_RawData_4, 0, CRSF_MAX_PACKET_LEN);							   // 清除接收缓存							
			}
		 break ;
		case CRSF_RX5_GET:      
			//UART5
		 { 		 
				if(CRSF_RawData_5[0] == CRSF_ADDRESS_FLIGHT_CONTROLLER && 0x0E == (CRSF_RawData_5[1] + 0X02) \
						&& CRSF_RawData_5[1] >= 3 && CRSF_RawData_5[1] <= CRSF_MAX_PACKET_LEN)//如果同步信号和长度正确
				{
						len = CRSF_RawData_5[1];                  //获取数据包长度
						inCrc = CRSF_RawData_5[2 + len - 1];      //获取数据包校验码
						crc= CalcCRC(&CRSF_RawData_5[2], len - 1);//计算数据包校验码
						if (crc == inCrc)                      	  //验证校验码
						{ 
							ProcessPacketIn(len);
						} 							  //校验通过，解析数据包
						else  if(crc != inCrc)
						{
							memset(CRSF_RawData_5, 0, CRSF_MAX_PACKET_LEN);							   // 清除接收缓存							
							break ;
						}

				}
			 	 memset(CRSF_RawData_5, 0, CRSF_MAX_PACKET_LEN);							   // 清除接收缓存							
			}
		 break ;
		case CRSF_RX6_GET:      
			//UART6
		 { 		 
				if(CRSF_RawData_6[0] == CRSF_ADDRESS_FLIGHT_CONTROLLER && 0x0E == (CRSF_RawData_6[1] + 0X02) \
						&& CRSF_RawData_6[1] >= 3 && CRSF_RawData_6[1] <= CRSF_MAX_PACKET_LEN)//如果同步信号和长度正确
				{
						len = CRSF_RawData_6[1];                  //获取数据包长度
						inCrc = CRSF_RawData_6[2 + len - 1];      //获取数据包校验码
						crc= CalcCRC(&CRSF_RawData_6[2], len - 1);//计算数据包校验码
						if (crc == inCrc)                      	  //验证校验码
						{ 
							ProcessPacketIn(len);
						} 							  //校验通过，解析数据包
						else  if(crc != inCrc)
						{
							memset(CRSF_RawData_6, 0, CRSF_MAX_PACKET_LEN);							   // 清除接收缓存							
							break ;
						}

				}
			 	 memset(CRSF_RawData_6, 0, CRSF_MAX_PACKET_LEN);							   // 清除接收缓存							
			}
		 break ;
	}


}

uint8_t Feed_RX_wdg(uint8_t Rx_Num)
{
	if(Rx_Num==1)
	{
		RX1_online_flag=0;
	}
	else if(Rx_Num==2)
	{
		RX2_online_flag=0;
	}
	else if(Rx_Num==3)
	{
		RX3_online_flag=0;
	}
	else if(Rx_Num==5)
	{
		RX5_online_flag=0;
	}
	else if(Rx_Num==6)
	{
		RX6_online_flag=0;
	}	
}

uint8_t RX_wdg()
{
	RX1_online_flag++;
	RX2_online_flag++;
	RX3_online_flag++;
	RX5_online_flag++;
	RX6_online_flag++;
	if(RX1_online_flag>1000)//1s没更新则清空回传信息
	{
		memset(&RX1_telemtering, 0, sizeof(crsfLinkStatistics_t));
		RX1_RSSI=0;
		RX1_LQ_GET=0;
		RX1_online_flag=1000;
	}
	if(RX2_online_flag>1000)
	{
		memset(&RX2_telemtering, 0, sizeof(crsfLinkStatistics_t));
		RX2_RSSI=0;
		RX2_LQ_GET=0;
		RX2_online_flag=1000;
	}
	if(RX3_online_flag>1000)
	{
		memset(&RX3_telemtering, 0, sizeof(crsfLinkStatistics_t));
		RX3_RSSI=0;
		RX3_LQ_GET=0;
		RX3_online_flag=1000;
	}
		if(RX5_online_flag>1000)
	{
		memset(&RX5_telemtering, 0, sizeof(crsfLinkStatistics_t));
		RX5_RSSI=0;
		RX5_LQ_GET=0;
		RX5_online_flag=1000;
	}
		if(RX6_online_flag>1000)
	{
		memset(&RX6_telemtering, 0, sizeof(crsfLinkStatistics_t));
		RX6_RSSI=0;
		RX6_LQ_GET=0;
		RX6_online_flag=1000;
	}
}


void  RX_1_DataDeal()
{
    if(__HAL_UART_GET_FLAG(&huart1,UART_FLAG_IDLE) == 1)
    {
         CRSF_RawData_Flag=CRSF_RX1_GET;//设置此标志位说明是串口1进入的中断
        __HAL_UART_CLEAR_IDLEFLAG(&huart1);//接收后清除标志位
        HAL_UART_DMAStop(&huart1);
        CRSFReceived();
        HAL_UART_Receive_DMA(&huart1,CRSF_RawData_1, 67);
    }
}

void  RX_2_DataDeal()
{
    if(__HAL_UART_GET_FLAG(&huart2,UART_FLAG_IDLE) == 1)
    {
			  
         CRSF_RawData_Flag=CRSF_RX2_GET;//设置此标志位说明是串口2进入的中断
        __HAL_UART_CLEAR_IDLEFLAG(&huart2);//接收后清除标志位
        HAL_UART_DMAStop(&huart2);
        CRSFReceived();
        HAL_UART_Receive_DMA(&huart2,CRSF_RawData_2, 67);
    }
}

void  RX_3_DataDeal()
{
    if(__HAL_UART_GET_FLAG(&huart3,UART_FLAG_IDLE) == 1)
    {
			  
         CRSF_RawData_Flag=CRSF_RX3_GET;//设置此标志位说明是串口3进入的中断
        __HAL_UART_CLEAR_IDLEFLAG(&huart3);//接收后清除标志位
        HAL_UART_DMAStop(&huart3);
        CRSFReceived();
        HAL_UART_Receive_DMA(&huart3,CRSF_RawData_3, 67);
    }
}

void  RX_4_DataDeal()
{
    if(__HAL_UART_GET_FLAG(&huart4,UART_FLAG_IDLE) == 1)
    {
			  
         CRSF_RawData_Flag=CRSF_RX4_GET;//设置此标志位说明是串口4进入的中断
        __HAL_UART_CLEAR_IDLEFLAG(&huart4);//接收后清除标志位
        HAL_UART_DMAStop(&huart4);
        CRSFReceived();
        HAL_UART_Receive_DMA(&huart4,CRSF_RawData_4, 67);
    }
}
void  RX_5_DataDeal()
{
    if(__HAL_UART_GET_FLAG(&huart5,UART_FLAG_IDLE) == 1)
    {
			  
         CRSF_RawData_Flag=CRSF_RX5_GET;//设置此标志位说明是串口4进入的中断
        __HAL_UART_CLEAR_IDLEFLAG(&huart5);//接收后清除标志位
        HAL_UART_DMAStop(&huart5);
        CRSFReceived();
        HAL_UART_Receive_DMA(&huart5,CRSF_RawData_5, 67);
    }
}
void  RX_6_DataDeal()
{
    if(__HAL_UART_GET_FLAG(&huart6,UART_FLAG_IDLE) == 1)
    {
			  
         CRSF_RawData_Flag=CRSF_RX6_GET;//设置此标志位说明是串口4进入的中断
        __HAL_UART_CLEAR_IDLEFLAG(&huart6);//接收后清除标志位
        HAL_UART_DMAStop(&huart6);
        CRSFReceived();
        HAL_UART_Receive_DMA(&huart6,CRSF_RawData_6, 67);
    }
}