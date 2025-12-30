#include "lcd.h"
#include "stdlib.h"
#include "main.h"
////////////////////////////////////////////////////////////////////
//-----------------LCD端口定义---------------- 

#define	LCD_RS_SET        	LCD_DC_GPIO_Port->BSRR  = LCD_DC_Pin
#define	LCD_CS_SET          SPI_CS_GPIO_Port->BSRR  = SPI_CS_Pin
#define	LCD_RST_SET         LCD_RES_GPIO_Port->BSRR = LCD_RES_Pin

#define	LCD_RS_CLR          LCD_DC_GPIO_Port->BSRR  = (uint32_t)LCD_DC_Pin  << 16u
#define	LCD_CS_CLR          SPI_CS_GPIO_Port->BSRR  = (uint32_t)SPI_CS_Pin  << 16u
#define	LCD_RST_CLR         LCD_RES_GPIO_Port->BSRR = (uint32_t)LCD_RES_Pin << 16u

//管理LCD重要参数
//默认为竖屏
_lcd_dev lcddev;
extern SPI_HandleTypeDef hspi1;
//画笔颜色,背景颜色
uint16_t POINT_COLOR = 0x0000,BACK_COLOR = 0xFFFF;  
uint16_t DeviceCode;	 
extern 
void SPI_OutData(uint8_t Data)
{
    HAL_SPI_Transmit(&hspi1,&Data,1,20);
}

/*****************************************************************************
 * @name       :void LCD_WR_REG(uint8_t data)
 * @date       :2018-08-09 
 * @function   :Write an 8-bit command to the LCD screen
 * @parameters :data:Command value to be written
 * @retvalue   :None
******************************************************************************/
void LCD_WR_REG(uint8_t data)
{ 
    LCD_CS_CLR;     
    LCD_RS_CLR;	  
    SPI_OutData(data);
    LCD_CS_SET;	
}

/*****************************************************************************
 * @name       :void LCD_WR_DATA(uint8_t data)
 * @date       :2018-08-09 
 * @function   :Write an 8-bit data to the LCD screen
 * @parameters :data:data value to be written
 * @retvalue   :None
******************************************************************************/
void LCD_WR_DATA(uint8_t data)
{
    LCD_CS_CLR;
    LCD_RS_SET;
    SPI_OutData(data);
    LCD_CS_SET;
}

/*****************************************************************************
 * @name       :void LCD_WriteReg(uint8_t LCD_Reg, uint16_t LCD_RegValue)
 * @date       :2018-08-09 
 * @function   :Write data into registers
 * @parameters :LCD_Reg:Register address
                LCD_RegValue:Data to be written
 * @retvalue   :None
******************************************************************************/
void LCD_WriteReg(uint8_t LCD_Reg, uint16_t LCD_RegValue)
{	
    LCD_WR_REG(LCD_Reg);  
    LCD_WR_DATA(LCD_RegValue);
}	   

/*****************************************************************************
 * @name       :void LCD_WriteRAM_Prepare(void)
 * @date       :2018-08-09 
 * @function   :Write GRAM
 * @parameters :None
 * @retvalue   :None
******************************************************************************/	 
void LCD_WriteRAM_Prepare(void)
{
    LCD_WR_REG(lcddev.wramcmd);
}	 

/*****************************************************************************
 * @name       :void Lcd_WriteData_16Bit(uint16_t Data)
 * @date       :2018-08-09 
 * @function   :Write an 16-bit command to the LCD screen
 * @parameters :Data:Data to be written
 * @retvalue   :None
******************************************************************************/	

void Lcd_WriteData_16Bit(uint16_t Data)
{	
		
    LCD_CS_CLR;
    LCD_RS_SET;  
		SPI_OutData(Data>>8);
		SPI_OutData(Data);
//		HAL_SPI_Transmit_DMA(&hspi1,(uint8_t*)&Data,2);
//while (HAL_SPI_GetState(&hspi1) != HAL_SPI_STATE_READY) {
////    HAL_Delay(1);
////    if (timeout-- == 0) {
////        // 超时处理（如复位SPI、报错等）
////        Error_Handler();
////        break;
////    }
//}

    LCD_CS_SET;
}

/*****************************************************************************
 * @name       :void LCD_DrawPoint(uint16_t x,uint16_t y)
 * @date       :2018-08-09 
 * @function   :Write a pixel data at a specified location
 * @parameters :x:the x coordinate of the pixel
                y:the y coordinate of the pixel
 * @retvalue   :None
******************************************************************************/	
void LCD_DrawPoint(uint16_t x,uint16_t y)
{
    LCD_SetCursor(x,y);//设置光标位置 
    Lcd_WriteData_16Bit(POINT_COLOR); 
}

/*****************************************************************************
 * @name       :void LCD_Clear(uint16_t Color)
 * @date       :2018-08-09 
 * @function   :Full screen filled LCD screen
 * @parameters :color:Filled color
 * @retvalue   :None
******************************************************************************/	
void LCD_Clear(uint16_t Color)
{
    unsigned int i,m;  
    LCD_SetWindows(0,0,lcddev.width,lcddev.height);   
    LCD_CS_CLR;
    LCD_RS_SET;
    for(i=0;i<lcddev.height;i++)
    {
        for(m=0;m<lcddev.width;m++)
        {	
            Lcd_WriteData_16Bit(Color);
        }
    }
    LCD_CS_SET;
		
		Global_dynamic_display();
} 

// 定义全局缓冲区（存储一行像素数据，130×2=260字节，需放在全局变量区）
//static uint8_t lcd_dma_line_buf[2 * LCD_W];  // 

//void LCD_Clear(uint16_t Color) {
//    // 1. 填充一行像素数据到缓冲区（转换为RGB565的字节序）
//    uint8_t color_high = (Color >> 8) & 0xFF;  // 高8位
//    uint8_t color_low = Color & 0xFF;          // 低8位
//    for (uint16_t i = 0; i < LCD_W; i++) {
//        lcd_dma_line_buf[2*i] = color_high;    // 高字节在前（根据屏幕调整顺序）
//        lcd_dma_line_buf[2*i + 1] = color_low; // 低字节在后
//    }
//    LCD_SetWindows(0,0,lcddev.width,lcddev.height);   
//    LCD_CS_CLR;
//    LCD_RS_SET;

//    // 4. 用DMA循环发送所有行数据（共lcddev.height行）
//    for (uint16_t row = 0; row < lcddev.height; row++) {
//        // 启动DMA发送一行数据（260字节）
//        HAL_SPI_Transmit_DMA(&hspi1, lcd_dma_line_buf, 2 * LCD_W);

//        while (HAL_SPI_GetState(&hspi1) != HAL_SPI_STATE_READY);
//    }

//    // 5. 结束传输
//    LCD_CS_SET;  // 拉高片选
//}

/*****************************************************************************
 * @name       :void LCD_RESET(void)
 * @date       :2018-08-09 
 * @function   :Reset LCD screen
 * @parameters :None
 * @retvalue   :None
******************************************************************************/	
void LCD_RESET(void)
{
	LCD_RST_CLR;
    
	HAL_Delay(100);	
	LCD_RST_SET;
	HAL_Delay(50);
}

/*****************************************************************************
 * @name       :void LCD_RESET(void)
 * @date       :2018-08-09 
 * @function   :Initialization LCD screen
 * @parameters :None
 * @retvalue   :None
******************************************************************************/	 	 
uint8_t LCD_READY=0;
void LCD_Init(void)
{									 
 	LCD_RESET(); //LCD 复位
    HAL_Delay(120);	
//************* ST7735S初始化**********//	
	LCD_WR_REG(0x11);//Sleep exit 
    LCD_WR_REG(0x11);//Sleep exit
	HAL_Delay(120);	
	//ST7735R Frame Rate
	LCD_WR_REG(0xB1); 
	LCD_WR_DATA(0x01); 
	LCD_WR_DATA(0x2C); 
	LCD_WR_DATA(0x2D); 
	LCD_WR_REG(0xB2); 
	LCD_WR_DATA(0x01); 
	LCD_WR_DATA(0x2C); 
	LCD_WR_DATA(0x2D); 
	LCD_WR_REG(0xB3); 
	LCD_WR_DATA(0x01); 
	LCD_WR_DATA(0x2C); 
	LCD_WR_DATA(0x2D); 
	LCD_WR_DATA(0x01); 
	LCD_WR_DATA(0x2C); 
	LCD_WR_DATA(0x2D); 	
	LCD_WR_REG(0xB4); //Column inversion 
	LCD_WR_DATA(0x07); 	
	//ST7735R Power Sequence
	LCD_WR_REG(0xC0); 
	LCD_WR_DATA(0xA2); 
	LCD_WR_DATA(0x02); 
	LCD_WR_DATA(0x84); 
	LCD_WR_REG(0xC1); 
	LCD_WR_DATA(0xC5); 
	LCD_WR_REG(0xC2); 
	LCD_WR_DATA(0x0A); 
	LCD_WR_DATA(0x00); 
	LCD_WR_REG(0xC3); 
	LCD_WR_DATA(0x8A); 
	LCD_WR_DATA(0x2A); 
	LCD_WR_REG(0xC4); 
	LCD_WR_DATA(0x8A); 
	LCD_WR_DATA(0xEE); 
	LCD_WR_REG(0xC5); //VCOM 
	LCD_WR_DATA(0x0E); 	
	LCD_WR_REG(0x36); //MX, MY, RGB mode 
	LCD_WR_DATA(0xC0); 
	//ST7735R Gamma Sequence
	LCD_WR_REG(0xe0); 
	LCD_WR_DATA(0x0f); 
	LCD_WR_DATA(0x1a); 
	LCD_WR_DATA(0x0f); 
	LCD_WR_DATA(0x18); 
	LCD_WR_DATA(0x2f); 
	LCD_WR_DATA(0x28); 
	LCD_WR_DATA(0x20); 
	LCD_WR_DATA(0x22); 
	LCD_WR_DATA(0x1f); 
	LCD_WR_DATA(0x1b); 
	LCD_WR_DATA(0x23); 
	LCD_WR_DATA(0x37); 
	LCD_WR_DATA(0x00); 	
	LCD_WR_DATA(0x07); 
	LCD_WR_DATA(0x02); 
	LCD_WR_DATA(0x10); 
	LCD_WR_REG(0xe1); 
	LCD_WR_DATA(0x0f); 
	LCD_WR_DATA(0x1b); 
	LCD_WR_DATA(0x0f); 
	LCD_WR_DATA(0x17); 
	LCD_WR_DATA(0x33); 
	LCD_WR_DATA(0x2c); 
	LCD_WR_DATA(0x29); 
	LCD_WR_DATA(0x2e); 
	LCD_WR_DATA(0x30); 
	LCD_WR_DATA(0x30); 
	LCD_WR_DATA(0x39); 
	LCD_WR_DATA(0x3f); 
	LCD_WR_DATA(0x00); 
	LCD_WR_DATA(0x07); 
	LCD_WR_DATA(0x03); 
	LCD_WR_DATA(0x10);  	
	LCD_WR_REG(0x2a);
	LCD_WR_DATA(0x00);
	LCD_WR_DATA(0x00);
	LCD_WR_DATA(0x00);
	LCD_WR_DATA(0x7f);
	LCD_WR_REG(0x2b);
	LCD_WR_DATA(0x00);
	LCD_WR_DATA(0x00);
	LCD_WR_DATA(0x00);
	LCD_WR_DATA(0x9f);
	LCD_WR_REG(0xF0); //Enable test command  
	LCD_WR_DATA(0x01); 
	LCD_WR_REG(0xF6); //Disable ram power save mode 
	LCD_WR_DATA(0x00); 	
	LCD_WR_REG(0x3A); //65k mode 
	LCD_WR_DATA(0x05); 
	LCD_WR_REG(0x29);//Display on	 	
    LCD_direction(USE_HORIZONTAL);//设置LCD显示方向 
	LCD_Clear(WHITE);//清全屏白色
    LCD_direction(1);
		LCD_READY=1;
}
 
/*****************************************************************************
 * @name       :void LCD_SetWindows(uint16_t xStar, uint16_t yStar,uint16_t xEnd,uint16_t yEnd)
 * @date       :2018-08-09 
 * @function   :Setting LCD display window
 * @parameters :xStar:the bebinning x coordinate of the LCD display window
								yStar:the bebinning y coordinate of the LCD display window
								xEnd:the endning x coordinate of the LCD display window
								yEnd:the endning y coordinate of the LCD display window
 * @retvalue   :None
******************************************************************************/ 
void LCD_SetWindows(uint16_t xStar, uint16_t yStar,uint16_t xEnd,uint16_t yEnd)
{	
	LCD_WR_REG(lcddev.setxcmd);	
	LCD_WR_DATA(0x00);
	LCD_WR_DATA(xStar);		
	LCD_WR_DATA(0x00);
	LCD_WR_DATA(xEnd);

	LCD_WR_REG(lcddev.setycmd);	
	LCD_WR_DATA(0x00);
	LCD_WR_DATA(yStar);		
	LCD_WR_DATA(0x00);
	LCD_WR_DATA(yEnd);

	LCD_WriteRAM_Prepare();	//开始写入GRAM			
}   

/*****************************************************************************
 * @name       :void LCD_SetCursor(uint16_t Xpos, uint16_t Ypos)
 * @date       :2018-08-09 
 * @function   :Set coordinate value
 * @parameters :Xpos:the  x coordinate of the pixel
								Ypos:the  y coordinate of the pixel
 * @retvalue   :None
******************************************************************************/ 
void LCD_SetCursor(uint16_t Xpos, uint16_t Ypos)
{	  	    			
	LCD_SetWindows(Xpos,Ypos,Xpos,Ypos);	
} 

/*****************************************************************************
 * @name       :void LCD_direction(uint8_t direction)
 * @date       :2018-08-09 
 * @function   :Setting the display direction of LCD screen
 * @parameters :direction:0-0 degree
                          1-90 degree
													2-180 degree
													3-270 degree
 * @retvalue   :None
******************************************************************************/ 
void LCD_direction(uint8_t direction)
{ 
    lcddev.setxcmd=0x2A;
    lcddev.setycmd=0x2B;
    lcddev.wramcmd=0x2C;
	switch(direction)
    {		  
		case 0:						 	 		
			lcddev.width=LCD_W;
			lcddev.height=LCD_H;		
			LCD_WriteReg(0x36,(0<<3)|(1<<6)|(1<<7));//BGR==1,MY==0,MX==0,MV==0
		break;
		case 1:
			lcddev.width=LCD_H;
			lcddev.height=LCD_W;
			LCD_WriteReg(0x36,(0<<3)|(1<<7)|(1<<5));//BGR==1,MY==1,MX==0,MV==1
		break;
		case 2:						 	 		
			lcddev.width=LCD_W;
			lcddev.height=LCD_H;	
			LCD_WriteReg(0x36,(0<<3)|(0<<6)|(0<<7));//BGR==1,MY==0,MX==0,MV==0
		break;
		case 3:
			lcddev.width=LCD_H;
			lcddev.height=LCD_W;
			LCD_WriteReg(0x36,(0<<3)|(0<<7)|(1<<6)|(1<<5));//BGR==1,MY==1,MX==0,MV==1
		break;	
		default:break;
	}		
}	 
