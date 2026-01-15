#ifndef __DEVICE_H__
#define __DEVICE_H__
#include "main.h"
#include "pwm_freq.h"
//void BeepInit(void);
//void SetBeepStatus(uint8_t NewStatus,uint16_t Time);

#define timeout_max 200//10ms*time_out_max



#define Pwm_RX 0x0000
#define Normal_RX 0x8000




//测试ERR
typedef struct Test_Err_structment
{		
		u8 RSSI_LQ_Err_S;
		u8 SENSOR_Err_S;
		u8 CHANNLE_Err_S;
}Test_Err_struct;


//设备ERR
enum device_Err
{
		TX_Err,
		RX_Err,
};

//接收机设备结构体
typedef struct RX_Device
{
									
		int8_t  RSSI;
		int8_t  RSSI2;
		uint8_t TRSS;
		uint8_t LQ;
		float ALT;	
		float Vbat;	
		struct Pwm_channle_Duty Channle;
}RX_Device_t;


//全局参数结构体
typedef struct parameter_struct
{
	const char *str;
	u8 type;//0=null,1=bool,2=u8,3=int,4=float
	void *para_p;
}parameter_t;



typedef struct Flash_parameter
{
									//
		u16 * RX_device_t;
		u32  RSSI_threshold;
		u32 LQ_threshold;
//		float ALT_threshold;	
//		float Vbat_threshold;	
		u32 check_bit;//if read check_bit!=1,init the para
	
}Flash_para;


//0bit-RSSInumb 4bit~1bit-sersor  8~5bit-Pwm_Channles 16bit-normal rx or pwm rx
//1bit-alt 2bit-vbat 3bit-  4bit-
typedef u16 Rx_target;



u8  Rssi_Lq_test();
u8  Sensor_test();
u8  Channle_test();
void Start_Manual_Test();
void Stop_Manual_Test();
void Start_Auto_Test();
void Stop_Auto_Test();
void Test_result();
u8 Auto_test(u8 * current_step);


void Manual_test_display();


void Creat_rx(Rx_target* Rx,u8 antenna,u8 sensor,u8 channles,u8 Rx_type);
void Get_PWM_RX_Info();
void Update_PWM_RX_Info();
uint8_t Get_normal_or_pwm();
void Rx_device_init();


void Flash_para_init();
u8 Save_para();
u8 read_para(Flash_para * parameter_struct);
u8 scan_para(const char *str);

    

#endif

