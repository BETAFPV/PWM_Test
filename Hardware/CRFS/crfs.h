#ifndef __CRFS_H__
#define __CRFS_H__
#include "main.h"
#define CRSF_MAX_PACKET_LEN     64
#define CRSF_RX1_GET 1
#define CRSF_RX2_GET 2
#define CRSF_RX3_GET 3
#define CRSF_RX4_GET 4
#define CRSF_data_num       60

#define CRSF_NUM_CHANNELS       16


#define PACKED __attribute__((packed))

typedef enum//CRFS协议地址
{
    CRSF_ADDRESS_BROADCAST = 0x00,
    CRSF_ADDRESS_USB = 0x10,
    CRSF_ADDRESS_TBS_CORE_PNP_PRO = 0x80,
    CRSF_ADDRESS_RESERVED1 = 0x8A,
    CRSF_ADDRESS_CURRENT_SENSOR = 0xC0,
    CRSF_ADDRESS_GPS = 0xC2,
    CRSF_ADDRESS_TBS_BLACKBOX = 0xC4,
    CRSF_ADDRESS_FLIGHT_CONTROLLER = 0xC8,  // RX TO FC
    CRSF_ADDRESS_RESERVED2 = 0xCA,
    CRSF_ADDRESS_RACE_TAG = 0xCC,
    CRSF_ADDRESS_RADIO_TRANSMITTER = 0xEA,	//TX高频头
    CRSF_ADDRESS_CRSF_RECEIVER = 0xEC,      //FC To RX
    CRSF_ADDRESS_CRSF_TRANSMITTER = 0xEE,		//Transmitter module遥控器
//		CRSF_ADDRESS_ELRS_LUA = 0xEF,						//
} crsf_addr_e;



typedef struct crsf_header_s//CRFS包类型
{
    uint8_t device_addr; // from crsf_addr_e
    uint8_t frame_size;  // counts size after this byte, so it must be the payload size + 2 (type and crc)
    uint8_t type;        // from crsf_frame_type_e
    uint8_t data[CRSF_data_num];
} PACKED crsf_header_t;



typedef enum//CRFS协议包类型
{
    CRSF_FRAMETYPE_GPS = 0x02,
    CRSF_FRAMETYPE_BATTERY_SENSOR = 0x08,
    CRSF_FRAMETYPE_BARO_ALTITUDE = 0x09,
    CRSF_FRAMETYPE_HEARTBEAT = 0x0B,
    CRSF_FRAMETYPE_LINK_STATISTICS = 0x14,
    CRSF_FRAMETYPE_OPENTX_SYNC = 0x10,
    CRSF_FRAMETYPE_RADIO_ID = 0x3A,
    CRSF_FRAMETYPE_RC_CHANNELS_PACKED = 0x16,
    CRSF_FRAMETYPE_SUBSET_RC_CHANNELS_PACKED = 0x17,
    CRSF_FRAMETYPE_LINK_STATISTICS_RX = 0x1C,
    CRSF_FRAMETYPE_LINK_STATISTICS_TX = 0x1D,
    CRSF_FRAMETYPE_ATTITUDE = 0x1E,
    CRSF_FRAMETYPE_FLIGHT_MODE = 0x21,
    // Extended Header Frames, range: 0x28 to 0x96
    CRSF_FRAMETYPE_DEVICE_PING = 0x28,
    CRSF_FRAMETYPE_DEVICE_INFO = 0x29,
    CRSF_FRAMETYPE_PARAMETER_SETTINGS_ENTRY = 0x2B,
    CRSF_FRAMETYPE_PARAMETER_READ = 0x2C,
    CRSF_FRAMETYPE_PARAMETER_WRITE = 0x2D,
    CRSF_FRAMETYPE_COMMAND = 0x32,
    // MSP commands
    CRSF_FRAMETYPE_MSP_REQ = 0x7A,   // response request using msp sequence as command
    CRSF_FRAMETYPE_MSP_RESP = 0x7B,  // reply with 58 byte chunked binary
    CRSF_FRAMETYPE_MSP_WRITE = 0x7C, // write with 8 byte chunked binary (OpenTX outbound telemetry buffer limit)
    CRSF_FRAMETYPE_DISPLAYPORT_CMD = 0x7D, // displayport control command
} crsf_frame_type_e;


typedef struct crsfPayloadLinkstatistics_s//CRFS回传包结构
{
    int8_t	uplink_RSSI_1;
    uint8_t uplink_RSSI_2;
    uint8_t uplink_Link_quality;
    int8_t 	uplink_SNR;
    uint8_t active_antenna;
    uint8_t rf_Mode;
    uint8_t uplink_TX_Power;
    uint8_t downlink_RSSI;
    uint8_t downlink_Link_quality;
    int8_t 	downlink_SNR;
} crsfLinkStatistics_t;

typedef uint16_t RX_online_flag;

void PacketLinkStatistics(const crsf_header_t *p);


typedef struct crsf_channels_s//CRFS通道包结构，每个通道11位，16个通道
{
    unsigned ch0 : 11;
    unsigned ch1 : 11;
    unsigned ch2 : 11;
    unsigned ch3 : 11;
    unsigned ch4 : 11;
    unsigned ch5 : 11;
    unsigned ch6 : 11;
    unsigned ch7 : 11;
    unsigned ch8 : 11;
    unsigned ch9 : 11;
    unsigned ch10 : 11;
    unsigned ch11 : 11;
    unsigned ch12 : 11;
    unsigned ch13 : 11;
    unsigned ch14 : 11;
    unsigned ch15 : 11;
} PACKED crsf_channels_t;

#define CRSF_CHANNEL_VALUE_MIN  172
#define CRSF_CHANNEL_VALUE_1000 191
#define CRSF_CHANNEL_VALUE_MID  992
#define CRSF_CHANNEL_VALUE_2000 1792
#define CRSF_CHANNEL_VALUE_MAX  1811


typedef struct
{
    int16_t roll;
    int16_t pitch;
    int16_t yaw;
    int16_t throttle;
    int16_t aux1;
    int16_t aux2;
    int16_t aux3;
    int16_t aux4;
    int16_t aux5;
    int16_t aux6;
    int16_t aux7;
    int16_t aux8;
    int16_t aux9;
    int16_t aux10;
    int16_t aux11;
    int16_t aux12;
    uint8_t flag;
} RCDATA_t;


void RX_1_DataDeal();
void RX_2_DataDeal();
void RX_3_DataDeal();
void RX_4_DataDeal();
uint8_t Feed_RX_wdg(uint8_t Rx_Num);
uint8_t RX_wdg();
void CRSFReceived(void);
void Crc8_init(uint8_t poly);
void merge_11bit_to_22bytes(const uint16_t src[16], uint8_t dest[22]);
void split_22bytes_to_11bit(const uint8_t src[22], uint16_t dest[16]);
#endif

