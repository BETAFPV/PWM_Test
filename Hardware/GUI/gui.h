#ifndef __GUI_H__
#define __GUI_H__
#include "main.h"
#include "key.h"
#include "lcd.h"


#define LCD_WIDTH LCD_H
#define LCD_HEIGHT LCD_W

typedef struct Menu_list_struct{

	const char** str;
	u8 list_num;
	
} Menu_list;




void GUI_DrawPoint(uint16_t x,uint16_t y,uint16_t color);
void LCD_Fill(uint16_t sx,uint16_t sy,uint16_t ex,uint16_t ey,uint16_t color);
void LCD_DrawLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);
void LCD_DrawRectangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);
void Draw_Circle(uint16_t x0,uint16_t y0,uint16_t fc,uint8_t r);
void Draw_Triangel(uint16_t x0,uint16_t y0,uint16_t x1,uint16_t y1,uint16_t x2,uint16_t y2);
void Fill_Triangel(uint16_t x0,uint16_t y0,uint16_t x1,uint16_t y1,uint16_t x2,uint16_t y2);
void LCD_ShowChar(uint16_t x,uint16_t y,uint16_t fc, uint16_t bc, uint8_t num,uint8_t size,uint8_t mode);
void LCD_ShowNum(uint16_t x,uint16_t y,uint32_t num,uint8_t len,uint8_t size);
void LCD_ShowNum_Color(uint16_t x,uint16_t y,uint16_t F_COLOR,uint16_t B_COLOR,uint32_t num,uint8_t len,uint8_t size);
void LCD_ShowSingedNum_Color(uint16_t x,uint16_t y,uint16_t F_COLOR,uint16_t B_COLOR,int num,uint8_t len,uint8_t size);


void LCD_Show2Num(uint16_t x,uint16_t y,uint16_t num,uint8_t len,uint8_t size,uint8_t mode);
void LCD_ShowString(uint16_t x,uint16_t y,uint8_t size,uint8_t *p,uint8_t mode);
void LCD_ShowString_color(uint16_t x,uint16_t y,uint16_t fc, uint16_t bc,uint8_t size,uint8_t *p,uint8_t mode);
void LCD_ShowFloat(uint16_t x, uint16_t y, float num, uint16_t fc, uint16_t bc, uint8_t font_size,uint8_t mode);

void GUI_DrawFont16(uint16_t x, uint16_t y, uint16_t fc, uint16_t bc, uint8_t *s,uint8_t mode);
void GUI_DrawFont24(uint16_t x, uint16_t y, uint16_t fc, uint16_t bc, uint8_t *s,uint8_t mode);
void GUI_DrawFont32(uint16_t x, uint16_t y, uint16_t fc, uint16_t bc, uint8_t *s,uint8_t mode);
void Show_Str(uint16_t x, uint16_t y, uint16_t fc, uint16_t bc, uint8_t *str,uint8_t size,uint8_t mode);
void Gui_Drawbmp16(uint16_t x,uint16_t y,const unsigned char *p);
void gui_circle(int xc, int yc,uint16_t c,int r, int fill);
void Gui_StrCenter(uint16_t x, uint16_t y, uint16_t fc, uint16_t bc, uint8_t *str,uint8_t size,uint8_t mode);
void LCD_DrawFillRectangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);
void LCD_DrawFillRectangle_color(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2,u16 color);
void LcdDisplay(uint8_t Row,uint8_t *str);
void draw_tick(uint16_t x,uint16_t y);
void GUI_DrawFont16_index(uint16_t x, uint16_t y, uint16_t fc, uint16_t bc, uint8_t index,uint8_t mode);
void LCD_DrawString(uint16_t x,uint16_t y,const char*p,uint8_t size,uint16_t color,uint16_t bc);

// Function declarations
void Global_dynamic_display();
void dynamic_display();
 void drawMainMenu(void);
 void drawManualTestMenu(void);
 void drawReceiverMenu(void);
 void drawSystemMenu(void);
 void drawAutoTestPage(void);
 void drawMenuItem(uint16_t y, const char* text, uint8_t isSelected);
 void handleMainMenuInput(Key_TypeDef key);
 void handleManualTestInput(Key_TypeDef key);
 void handleReceiverInput(Key_TypeDef key);
 void handleSystemInput(Key_TypeDef key);
 void handleAutoTestInput(Key_TypeDef key);
#endif

