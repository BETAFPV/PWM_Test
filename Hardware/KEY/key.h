#ifndef __KEY_H__
#define __KEY_H__
#include "main.h"

//void BeepInit(void);
//void SetBeepStatus(uint8_t NewStatus,uint16_t Time);
    
typedef enum {
    KEY_NONE,       // 无按键操作
    KEY_UP,         // 上键
    KEY_DOWN,       // 下键
    KEY_LEFT,       // 左键
    KEY_RIGHT,      // 右键
    KEY_ENTER,       // 确认键
		KEY_1
} Key_TypeDef;

typedef enum {
    KEY_EVENT_NONE,    // 无事件
    KEY_EVENT_PRESS,   // 短按
    KEY_EVENT_HOLD     // 长按
} Key_Event_TypeDef;

void Key_Scan(void);
void transmitter_vtx_vdpoint(uint16_t vtx_vd);
Key_TypeDef Key_5D_Scan(Key_TypeDef *dev, Key_Event_TypeDef *event);
void shortbuzzing();
void longbuzzing();
#endif

