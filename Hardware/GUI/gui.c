#include "lcd.h"
#include <stdint.h>
#include <math.h>
#include "string.h"
#include "font.h" 
#include "gui.h"
#include "stm32f4xx_hal.h"
#include "lcd.h"
#include "rf_port.h"
#include "key.h"  // 5D key driver header
#include "device.h" 
#include "flash.h" 
#include "freertos.h"
#include "task.h"
extern Rx_target * current_rx_target;
extern Rx_target Rx4,Rx6,Rx6_dual,Rx8_dual,SuperP;//PWM接收机
extern Rx_target SuperD,SuperXnano,SuperXmono;//rx接收机
extern const char version[];
extern parameter_t * Parameter_List[2];
extern Flash_para Global_parameter;


/*****************************************************************************************************

																							method

******************************************************************************************************/

/*******************************************************************
 * @name       :void GUI_DrawPoint(uint16_t x,uint16_t y,uint16_t color)
 * @date       :2018-08-09 
 * @function   :draw a point in LCD screen
 * @parameters :x:the x coordinate of the point
                y:the y coordinate of the point
								color:the color value of the point
 * @retvalue   :None
********************************************************************/
void GUI_DrawPoint(uint16_t x,uint16_t y,uint16_t color)
{
	LCD_SetCursor(x,y);//设置光标位置 
	Lcd_WriteData_16Bit(color); 
}

/*******************************************************************
 * @name       :void LCD_Fill(uint16_t sx,uint16_t sy,uint16_t ex,uint16_t ey,uint16_t color)
 * @date       :2018-08-09 
 * @function   :fill the specified area
 * @parameters :sx:the bebinning x coordinate of the specified area
                sy:the bebinning y coordinate of the specified area
								ex:the ending x coordinate of the specified area
								ey:the ending y coordinate of the specified area
								color:the filled color value
 * @retvalue   :None
********************************************************************/
void LCD_Fill(uint16_t sx,uint16_t sy,uint16_t ex,uint16_t ey,uint16_t color)
{  	
	uint16_t i,j;			
	uint16_t width=ex-sx+1; 		//得到填充的宽度
	uint16_t height=ey-sy+1;		//高度
	LCD_SetWindows(sx,sy,ex,ey);//设置显示窗口
	for(i=0;i<height;i++)
	{
		for(j=0;j<width;j++)
		Lcd_WriteData_16Bit(color);	//写入数据 	 
	}
	LCD_SetWindows(0,0,lcddev.width-1,lcddev.height-1);//恢复窗口设置为全屏
}

/*******************************************************************
 * @name       :void LCD_DrawLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
 * @date       :2018-08-09 
 * @function   :Draw a line between two points
 * @parameters :x1:the bebinning x coordinate of the line
                y1:the bebinning y coordinate of the line
								x2:the ending x coordinate of the line
								y2:the ending y coordinate of the line
 * @retvalue   :None
********************************************************************/
void LCD_DrawLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
	uint16_t t; 
	int xerr=0,yerr=0,delta_x,delta_y,distance; 
	int incx,incy,uRow,uCol; 

	delta_x=x2-x1; //计算坐标增量 
	delta_y=y2-y1; 
	uRow=x1; 
	uCol=y1; 
	if(delta_x>0)incx=1; //设置单步方向 
	else if(delta_x==0)incx=0;//垂直线 
	else {incx=-1;delta_x=-delta_x;} 
	if(delta_y>0)incy=1; 
	else if(delta_y==0)incy=0;//水平线 
	else{incy=-1;delta_y=-delta_y;} 
	if( delta_x>delta_y)distance=delta_x; //选取基本增量坐标轴 
	else distance=delta_y; 
	for(t=0;t<=distance+1;t++ )//画线输出 
	{  
		LCD_DrawPoint(uRow,uCol);//画点 
		xerr+=delta_x ; 
		yerr+=delta_y ; 
		if(xerr>distance) 
		{ 
			xerr-=distance; 
			uRow+=incx; 
		} 
		if(yerr>distance) 
		{ 
			yerr-=distance; 
			uCol+=incy; 
		} 
	}  
} 

/*****************************************************************************
 * @name       :void LCD_DrawRectangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
 * @date       :2018-08-09 
 * @function   :Draw a rectangle
 * @parameters :x1:the bebinning x coordinate of the rectangle
                y1:the bebinning y coordinate of the rectangle
								x2:the ending x coordinate of the rectangle
								y2:the ending y coordinate of the rectangle
 * @retvalue   :None
******************************************************************************/
void LCD_DrawRectangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
	LCD_DrawLine(x1,y1,x2,y1);
	LCD_DrawLine(x1,y1,x1,y2);
	LCD_DrawLine(x1,y2,x2,y2);
	LCD_DrawLine(x2,y1,x2,y2);
}  

/*****************************************************************************
 * @name       :void LCD_DrawFillRectangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
 * @date       :2018-08-09 
 * @function   :Filled a rectangle
 * @parameters :x1:the bebinning x coordinate of the filled rectangle
                y1:the bebinning y coordinate of the filled rectangle
								x2:the ending x coordinate of the filled rectangle
								y2:the ending y coordinate of the filled rectangle
 * @retvalue   :None
******************************************************************************/  
void LCD_DrawFillRectangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
	LCD_Fill(x1,y1,x2,y2,POINT_COLOR);
}
/*****************************************************************************
 * @name       :void LCD_DrawFillRectangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
 * @date       :2018-08-09 
 * @function   :Filled a rectangle
 * @parameters :x1:the bebinning x coordinate of the filled rectangle
                y1:the bebinning y coordinate of the filled rectangle
								x2:the ending x coordinate of the filled rectangle
								y2:the ending y coordinate of the filled rectangle
 * @retvalue   :None
******************************************************************************/  
void LCD_DrawFillRectangle_color(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2,u16 color)
{
	LCD_Fill(x1,y1,x2,y2,color);
}
 
/*****************************************************************************
 * @name       :void _draw_circle_8(int xc, int yc, int x, int y, uint16_t c)
 * @date       :2018-08-09 
 * @function   :8 symmetry circle drawing algorithm (internal call)
 * @parameters :xc:the x coordinate of the Circular center 
                yc:the y coordinate of the Circular center 
								x:the x coordinate relative to the Circular center 
								y:the y coordinate relative to the Circular center 
								c:the color value of the circle
 * @retvalue   :None
******************************************************************************/  
void _draw_circle_8(int xc, int yc, int x, int y, uint16_t c)
{
	GUI_DrawPoint(xc + x, yc + y, c);
	GUI_DrawPoint(xc - x, yc + y, c);
	GUI_DrawPoint(xc + x, yc - y, c);
	GUI_DrawPoint(xc - x, yc - y, c);
	GUI_DrawPoint(xc + y, yc + x, c);
	GUI_DrawPoint(xc - y, yc + x, c);
	GUI_DrawPoint(xc + y, yc - x, c);
	GUI_DrawPoint(xc - y, yc - x, c);
}

/*****************************************************************************
 * @name       :void gui_circle(int xc, int yc,uint16_t c,int r, int fill)
 * @date       :2018-08-09 
 * @function   :Draw a circle of specified size at a specified location
 * @parameters :xc:the x coordinate of the Circular center 
                yc:the y coordinate of the Circular center 
								r:Circular radius
								fill:1-filling,0-no filling
 * @retvalue   :None
******************************************************************************/  
void gui_circle(int xc, int yc,uint16_t c,int r, int fill)
{
	int x = 0, y = r, yi, d;

	d = 3 - 2 * r;

	if (fill) 
	{
		// 如果填充（画实心圆）
		while (x <= y) 
        {
			for (yi = x; yi <= y; yi++)
            {
                _draw_circle_8(xc, yc, x, yi, c);
            }

			if (d < 0) 
            {
				d = d + 4 * x + 6;
			}
            else 
            {
				d = d + 4 * (x - y) + 10;
				y--;
			}
			x++;
		}
	} 
    else 
	{
		// 如果不填充（画空心圆）
		while (x <= y) 
        {
			_draw_circle_8(xc, yc, x, y, c);
			if (d < 0) 
            {
				d = d + 4 * x + 6;
			} 
            else 
            {
				d = d + 4 * (x - y) + 10;
				y--;
			}
			x++;
		}
	}
}

/*****************************************************************************
 * @name       :void Draw_Triangel(uint16_t x0,uint16_t y0,uint16_t x1,uint16_t y1,uint16_t x2,uint16_t y2)
 * @date       :2018-08-09 
 * @function   :Draw a triangle at a specified position
 * @parameters :x0:the bebinning x coordinate of the triangular edge 
                y0:the bebinning y coordinate of the triangular edge 
								x1:the vertex x coordinate of the triangular
								y1:the vertex y coordinate of the triangular
								x2:the ending x coordinate of the triangular edge 
								y2:the ending y coordinate of the triangular edge 
 * @retvalue   :None
******************************************************************************/ 
void Draw_Triangel(uint16_t x0,uint16_t y0,uint16_t x1,uint16_t y1,uint16_t x2,uint16_t y2)
{
	LCD_DrawLine(x0,y0,x1,y1);
	LCD_DrawLine(x1,y1,x2,y2);
	LCD_DrawLine(x2,y2,x0,y0);
}

static void _swap(uint16_t *a, uint16_t *b)
{
    uint16_t tmp;
    tmp = *a;
    *a = *b;
    *b = tmp;
}

/*****************************************************************************
 * @name       :void Fill_Triangel(uint16_t x0,uint16_t y0,uint16_t x1,uint16_t y1,uint16_t x2,uint16_t y2)
 * @date       :2018-08-09 
 * @function   :filling a triangle at a specified position
 * @parameters :x0:the bebinning x coordinate of the triangular edge 
                y0:the bebinning y coordinate of the triangular edge 
								x1:the vertex x coordinate of the triangular
								y1:the vertex y coordinate of the triangular
								x2:the ending x coordinate of the triangular edge 
								y2:the ending y coordinate of the triangular edge 
 * @retvalue   :None
******************************************************************************/ 
void Fill_Triangel(uint16_t x0,uint16_t y0,uint16_t x1,uint16_t y1,uint16_t x2,uint16_t y2)
{
	u16 Triangel_COLOR=RED;
	uint16_t a, b, y, last;
	int dx01, dy01, dx02, dy02, dx12, dy12;
	long sa = 0;
	long sb = 0;
 	if (y0 > y1) 
	{
        _swap(&y0,&y1); 
		_swap(&x0,&x1);
 	}
 	if (y1 > y2) 
	{
        _swap(&y2,&y1); 
		_swap(&x2,&x1);
 	}
    if (y0 > y1) 
	{
        _swap(&y0,&y1); 
		_swap(&x0,&x1);
    }
	if(y0 == y2) 
	{ 
		a = b = x0;
		if(x1 < a)
        {
            a = x1;
        }
        else if(x1 > b)
        {
            b = x1;
        }
        if(x2 < a)
        {
            a = x2;
        }
        else if(x2 > b)
        {
            b = x2;
        }
        LCD_Fill(a,y0,b,y0,Triangel_COLOR);
        return;
	}
	dx01 = x1 - x0;
	dy01 = y1 - y0;
	dx02 = x2 - x0;
	dy02 = y2 - y0;
	dx12 = x2 - x1;
	dy12 = y2 - y1;
	
	if(y1 == y2)
	{
		last = y1; 
	}
    else
	{
		last = y1-1; 
	}
	for(y=y0; y<=last; y++) 
	{
		a = x0 + sa / dy01;
		b = x0 + sb / dy02;
		sa += dx01;
        sb += dx02;
        if(a > b)
        {
            _swap(&a,&b);
        }
		LCD_Fill(a,y,b,y,Triangel_COLOR);
	}
	sa = dx12 * (y - y1);
	sb = dx02 * (y - y0);
	for(; y<=y2; y++) 
	{
		a = x1 + sa / dy12;
		b = x0 + sb / dy02;
		sa += dx12;
		sb += dx02;
		if(a > b)
		{
			_swap(&a,&b);
		}
		LCD_Fill(a,y,b,y,Triangel_COLOR);
	}
}

/*****************************************************************************
 * @name       :void LCD_ShowChar(uint16_t x,uint16_t y,uint16_t fc, uint16_t bc, uint8_t num,uint8_t size,uint8_t mode)
 * @date       :2018-08-09 
 * @function   :Display a single English character
 * @parameters :x:the bebinning x coordinate of the Character display position
                y:the bebinning y coordinate of the Character display position
								fc:the color value of display character
								bc:the background color of display character
								num:the ascii code of display character(0~94)
								size:the size of display character
								mode:0-no overlying,1-overlying
 * @retvalue   :None
******************************************************************************/ 
void LCD_ShowChar(uint16_t x,uint16_t y,uint16_t fc, uint16_t bc, uint8_t num,uint8_t size,uint8_t mode)
{  
    uint8_t temp;
    uint8_t pos,t;
	uint16_t colortemp=POINT_COLOR;      
		   
	num=num-' ';//得到偏移后的值
	LCD_SetWindows(x,y,x+size/2-1,y+size-1);//设置单个文字显示窗口
	if(!mode) //非叠加方式
	{		
		for(pos=0;pos<size;pos++)
		{
			if(size==12)temp=asc2_1206[num][pos];//调用1206字体
			else temp=asc2_1608[num][pos];		 //调用1608字体
			for(t=0;t<size/2;t++)
		    {                 
		        if(temp&0x01)Lcd_WriteData_16Bit(fc); 
				else Lcd_WriteData_16Bit(bc); 
				temp>>=1; 
				
		    }
			
		}	
	}else//叠加方式
	{
		for(pos=0;pos<size;pos++)
		{
			if(size==12)temp=asc2_1206[num][pos];//调用1206字体
			else temp=asc2_1608[num][pos];		 //调用1608字体
			for(t=0;t<size/2;t++)
		    {   
				POINT_COLOR=fc;              
		        if(temp&0x01)LCD_DrawPoint(x+t,y+pos);//画一个点    
		        temp>>=1; 
		    }
		}
	}
	POINT_COLOR=colortemp;	
	LCD_SetWindows(0,0,lcddev.width-1,lcddev.height-1);//恢复窗口为全屏    	   	 	  
}

/*****************************************************************************
 * @name       :void LCD_ShowString(uint16_t x,uint16_t y,uint8_t size,uint8_t *p,uint8_t mode)
 * @date       :2018-08-09 
 * @function   :Display English string
 * @parameters :x:the bebinning x coordinate of the English string
                y:the bebinning y coordinate of the English string
								p:the start address of the English string
								size:the size of display character
								mode:0-no overlying,1-overlying
 * @retvalue   :None
******************************************************************************/   	  
void LCD_ShowString(uint16_t x,uint16_t y,uint8_t size,uint8_t *p,uint8_t mode)
{         
    while((*p<='~')&&(*p>=' '))//判断是不是非法字符!
    {   
		if(x>(lcddev.width-1)||y>(lcddev.height-1)) 
		return;     
        LCD_ShowChar(x,y,POINT_COLOR,BACK_COLOR,*p,size,mode);
        x+=size/2;
        p++;
    }  
} 

/*****************************************************************************
 * @name       :void LCD_ShowString_color(uint16_t x,uint16_t y,uint8_t size,uint8_t *p,uint8_t mode)
 * @date       :2018-08-09 
 * @function   :Display English string
 * @parameters :x:the bebinning x coordinate of the English string
                y:the bebinning y coordinate of the English string
								fc:front color
								bc:background color
								p:the start address of the English string
								size:the size of display character
								mode:0-no overlying,1-overlying
 * @retvalue   :None
******************************************************************************/   	  
void LCD_ShowString_color(uint16_t x,uint16_t y,uint16_t fc, uint16_t bc,uint8_t size,uint8_t *p,uint8_t mode)
{         
    while((*p<='~')&&(*p>=' '))//判断是不是非法字符!
    {   
		if(x>(lcddev.width-1)||y>(lcddev.height-1)) 
		return;     
        LCD_ShowChar(x,y,fc,bc,*p,size,mode);
        x+=size/2;
        p++;
    }  
} 
/*****************************************************************************
 * @name       :uint32_t mypow(uint8_t m,uint8_t n)
 * @date       :2018-08-09 
 * @function   :get the nth power of m (internal call)
 * @parameters :m:the multiplier
                n:the power
 * @retvalue   :the nth power of m
******************************************************************************/ 
uint32_t mypow(uint8_t m,uint8_t n)
{
	uint32_t result=1;	 
	while(n--)result*=m;    
	return result;
}

/*****************************************************************************
 * @name       :void LCD_ShowNum(uint16_t x,uint16_t y,uint32_t num,uint8_t len,uint8_t size)
 * @date       :2018-08-09 
 * @function   :Display number
 * @parameters :x:the bebinning x coordinate of the number
                y:the bebinning y coordinate of the number
								num:the number(0~4294967295)
								len:the length of the display number
								size:the size of display number
 * @retvalue   :None
******************************************************************************/  			 
void LCD_ShowNum(uint16_t x,uint16_t y,uint32_t num,uint8_t len,uint8_t size)
{         	
	uint8_t t,temp;
	uint8_t enshow=0;						   
	for(t=0;t<len;t++)
	{
		temp=(num/mypow(10,len-t-1))%10;
		if(enshow==0&&t<(len-1))
		{
			if(temp==0)
			{
				LCD_ShowChar(x+(size/2)*t,y,POINT_COLOR,BACK_COLOR,' ',size,0);
				continue;
			}else enshow=1; 
		 	 
		}
	 	LCD_ShowChar(x+(size/2)*t,y,POINT_COLOR,BACK_COLOR,temp+'0',size,0); 
	}
} 
/*****************************************************************************
 * @name       :void LCD_ShowNum(uint16_t x,uint16_t y,uint32_t num,uint8_t len,uint8_t size)
 * @date       :2018-08-09 
 * @function   :Display number
 * @parameters :x:the bebinning x coordinate of the number
                y:the bebinning y coordinate of the number
								num:the number(0~4294967295)
								len:the length of the display number
								size:the size of display number
 * @retvalue   :None
******************************************************************************/  			 
void LCD_ShowNum_Color(uint16_t x,uint16_t y,uint16_t F_COLOR,uint16_t B_COLOR,uint32_t num,uint8_t len,uint8_t size)
{         	
	uint8_t t,temp;
	uint8_t enshow=0;						   
	for(t=0;t<len;t++)
	{
		temp=(num/mypow(10,len-t-1))%10;
		if(enshow==0&&t<(len-1))
		{
			if(temp==0)
			{
				LCD_ShowChar(x+(size/2)*t,y,F_COLOR,B_COLOR,' ',size,0);
				continue;
			}else enshow=1; 
		 	 
		}
	 	LCD_ShowChar(x+(size/2)*t,y,F_COLOR,B_COLOR,temp+'0',size,0); 
	}
}


///
//显示小数
void LCD_ShowFloat(uint16_t x, uint16_t y, float num, uint16_t fc, uint16_t bc, uint8_t font_size, uint8_t mode)
{
    char buf[6] = {0};  // 索引：0=符号,1=十位,2=个位,3=小数点,4=小数位,5=结束符
    int32_t scaled_num = (int32_t)roundf(num * 10.0f);
    if (scaled_num >= 0) {
        buf[0] = '+';  // 正数显示+
    } else {
        buf[0] = '-';  // 负数显示-
        scaled_num = -scaled_num;  // 取绝对值
    }
    uint32_t abs_num = (uint32_t)scaled_num;
    uint8_t tens = (abs_num / 100) % 10;    // 十位（两位整数高位，不足则为0）
    uint8_t units = (abs_num / 10) % 10;    // 个位（两位整数低位，不足则为0）
    uint8_t decimal = abs_num % 10;         // 一位小数位

    // 6. 极端值限制（避免整数部分超两位，如100.0→强制显示99.9）
    if (abs_num > 999) {  // 99.9放大10倍=999，超过则截断
        tens = 9;
        units = 9;
        decimal = 9;
    }

    buf[1] = tens + '0';    // 十位转字符（如0→'0'，9→'9'）
    buf[2] = units + '0';    // 个位转字符
    buf[3] = '.';            // 小数点
    buf[4] = decimal + '0';  // 小数位转字符
    buf[5] = '\0';           // 字符串结束符

    // 8. 调用LCD显示字符串
    LCD_ShowString_color(x, y, fc, bc, font_size, buf, mode);
}
									 
//void LCD_ShowFloat(uint16_t x, uint16_t y, float num, uint16_t fc, uint16_t bc, uint8_t font_size,uint8_t mode)  {
//    // 1. 定义固定长度缓冲区（格式：+99.9 → 共5字符 + 结束符，足够存储）
//    char buf[6] = {0};  

//    // 2. 核心格式化：%+04.1f → 强制符号位+总宽度4（含小数点）+1位小数+整数补0
//    //    %+04.1f 解析：
//    //    + ：强制显示正负号
//    //    0 ：不足宽度时补0（而非空格）
//    //    4 ：总宽度（符号+两位整数+小数点 → 刚好4位，配合1位小数）
//    //    .1：固定1位小数
//    sprintf(buf, "%+04.1f", num);

//    // 3. 可选：修正极端值（如数值超出±99.9时，截断为±99.9，避免格式错乱）
//    if (num > 99.9f) {
//        strcpy(buf, "+99.9");
//    } else if (num < -99.9f) {
//        strcpy(buf, "-99.9");
//    }

//    // 4. 调用LCD显示函数
//    LCD_ShowString_color(x, y, fc, bc, font_size, buf, mode);
//}
/*****************************************************************************
 * @name       :void LCD_ShowSingedNum_Color(uint16_t x,uint16_t y,int num,uint8_t len,uint8_t size)
 * @date       :2018-08-09 
 * @function   :Display number
 * @parameters :x:the bebinning x coordinate of the number
                y:the bebinning y coordinate of the number
								num:the signed number
								len:the length of the display number
								size:the size of display number
 * @retvalue   :None
******************************************************************************/  

void LCD_ShowSingedNum_Color(uint16_t x,uint16_t y,uint16_t F_COLOR,uint16_t B_COLOR,int num,uint8_t len,uint8_t size)
{         	
	if(num>=0)
	{
		
		LCD_ShowNum_Color(x,y,F_COLOR,B_COLOR,num,len,size);
	}
	else if(num<0)
	{
		LCD_ShowChar(x-size/2,y,F_COLOR,B_COLOR,'-',size,1);
		LCD_ShowNum_Color(x,y,F_COLOR,B_COLOR,-num,len,size);
	}
		
		
		
	
}
/*****************************************************************************
 * @name       :void GUI_DrawFont16(uint16_t x, uint16_t y, uint16_t fc, uint16_t bc, uint8_t *s,uint8_t mode)
 * @date       :2018-08-09 
 * @function   :Display a single 16x16 Chinese character
 * @parameters :x:the bebinning x coordinate of the Chinese character
                y:the bebinning y coordinate of the Chinese character
								fc:the color value of Chinese character
								bc:the background color of Chinese character
								s:the start address of the Chinese character
								mode:0-no overlying,1-overlying
 * @retvalue   :None
******************************************************************************/ 
void GUI_DrawFont16(uint16_t x, uint16_t y, uint16_t fc, uint16_t bc, uint8_t *s,uint8_t mode)
{
	uint8_t i,j;
	uint16_t k;
	uint16_t HZnum;
	uint16_t x0=x;
	HZnum=sizeof(tfont16)/sizeof(typFNT_GB16);	//自动统计汉字数目
		
	for (k=0;k<HZnum;k++) 
	{
	  if ((tfont16[k].Index[0]==*(s))&&(tfont16[k].Index[1]==*(s+1)))
	  { 	LCD_SetWindows(x,y,x+16-1,y+16-1);
		    for(i=0;i<16*2;i++)
		    {
				for(j=0;j<8;j++)
		    	{	
					if(!mode) //非叠加方式
					{
						if(tfont16[k].Msk[i]&(0x80>>j))	Lcd_WriteData_16Bit(fc);
						else Lcd_WriteData_16Bit(bc);
					}
					else
					{
						POINT_COLOR=fc;
						if(tfont16[k].Msk[i]&(0x80>>j))	LCD_DrawPoint(x,y);//画一个点
						x++;
						if((x-x0)==16)
						{
							x=x0;
							y++;
							break;
						}
					}

				}
				
			}
			
			
		}				  	
		continue;  //查找到对应点阵字库立即退出，防止多个汉字重复取模带来影响
	}

	LCD_SetWindows(0,0,lcddev.width-1,lcddev.height-1);//恢复窗口为全屏  
} 
/*****************************************************************************
 * @name       :void GUI_DrawFont16(uint16_t x, uint16_t y, uint16_t fc, uint16_t bc, uint8_t *s,uint8_t mode)
 * @date       :2018-08-09 
 * @function   :Display a single 16x16 Chinese character
 * @parameters :x:the bebinning x coordinate of the Chinese character
                y:the bebinning y coordinate of the Chinese character
								fc:the color value of Chinese character
								bc:the background color of Chinese character
								s:the start address of the Chinese character
								mode:0-no overlying,1-overlying
 * @retvalue   :None
******************************************************************************/ 
void GUI_DrawFont16_index(uint16_t x, uint16_t y, uint16_t fc, uint16_t bc, uint8_t index,uint8_t mode)
{
	uint8_t i,j;
	uint16_t k;
	k=index;
	uint16_t HZnum;
	uint16_t x0=x;
	HZnum=sizeof(tfont16)/sizeof(typFNT_GB16);	//自动统计汉字数目
		
//	for (k=0;k<HZnum;k++) 
//	{
//	  if ((tfont16[k].Index[0]==*(s))&&(tfont16[k].Index[1]==*(s+1)))
//	  { 	
			LCD_SetWindows(x,y,x+16-1,y+16-1);
		    for(i=0;i<16*2;i++)
		    {
				for(j=0;j<8;j++)
		    	{	
					if(!mode) //非叠加方式
					{
						if(tfont16[k].Msk[i]&(0x80>>j))	Lcd_WriteData_16Bit(fc);
						else Lcd_WriteData_16Bit(bc);
					}
					else
					{
						POINT_COLOR=fc;
						if(tfont16[k].Msk[i]&(0x80>>j))	LCD_DrawPoint(x,y);//画一个点
						x++;
						if((x-x0)==16)
						{
							x=x0;
							y++;
							break;
						}
					}

				}
				
//			}
//			
//			
//		}				  	
		continue;  //查找到对应点阵字库立即退出，防止多个汉字重复取模带来影响
	}

	LCD_SetWindows(0,0,lcddev.width-1,lcddev.height-1);//恢复窗口为全屏  
} 
/*****************************************************************************
 * @name       :void GUI_DrawFont24(uint16_t x, uint16_t y, uint16_t fc, uint16_t bc, uint8_t *s,uint8_t mode)
 * @date       :2018-08-09 
 * @function   :Display a single 24x24 Chinese character
 * @parameters :x:the bebinning x coordinate of the Chinese character
                y:the bebinning y coordinate of the Chinese character
								fc:the color value of Chinese character
								bc:the background color of Chinese character
								s:the start address of the Chinese character
								mode:0-no overlying,1-overlying
 * @retvalue   :None
******************************************************************************/ 
void GUI_DrawFont24(uint16_t x, uint16_t y, uint16_t fc, uint16_t bc, uint8_t *s,uint8_t mode)
{
	uint8_t i,j;
	uint16_t k;
	uint16_t HZnum;
	uint16_t x0=x;
	HZnum=sizeof(tfont24)/sizeof(typFNT_GB24);	//自动统计汉字数目
		
    for (k=0;k<HZnum;k++) 
    {
        if ((tfont24[k].Index[0]==*(s))&&(tfont24[k].Index[1]==*(s+1)))
        { 	
            LCD_SetWindows(x,y,x+24-1,y+24-1);
            for(i=0;i<24*3;i++)
            {
                for(j=0;j<8;j++)
                {
                    if(!mode) //非叠加方式
                    {
                        if(tfont24[k].Msk[i]&(0x80>>j))	Lcd_WriteData_16Bit(fc);
                        else Lcd_WriteData_16Bit(bc);
                    }
                    else
                    {
                        POINT_COLOR=fc;
                        if(tfont24[k].Msk[i]&(0x80>>j))	LCD_DrawPoint(x,y);//画一个点
                        x++;
                        if((x-x0)==24)
                        {
                            x=x0;
                            y++;
                            break;
                        }
                    }
                }
            }
        }				  	
        continue;  //查找到对应点阵字库立即退出，防止多个汉字重复取模带来影响
    }

	LCD_SetWindows(0,0,lcddev.width-1,lcddev.height-1);//恢复窗口为全屏  
}

/*****************************************************************************
 * @name       :void GUI_DrawFont32(uint16_t x, uint16_t y, uint16_t fc, uint16_t bc, uint8_t *s,uint8_t mode)
 * @date       :2018-08-09 
 * @function   :Display a single 32x32 Chinese character
 * @parameters :x:the bebinning x coordinate of the Chinese character
                y:the bebinning y coordinate of the Chinese character
								fc:the color value of Chinese character
								bc:the background color of Chinese character
								s:the start address of the Chinese character
								mode:0-no overlying,1-overlying
 * @retvalue   :None
******************************************************************************/ 
void GUI_DrawFont32(uint16_t x, uint16_t y, uint16_t fc, uint16_t bc, uint8_t *s,uint8_t mode)
{
	uint8_t i,j;
	uint16_t k;
	uint16_t HZnum;
	uint16_t x0=x;
	HZnum=sizeof(tfont32)/sizeof(typFNT_GB32);	//自动统计汉字数目
	for (k=0;k<HZnum;k++) 
    {
        if ((tfont32[k].Index[0]==*(s))&&(tfont32[k].Index[1]==*(s+1)))
        { 	LCD_SetWindows(x,y,x+32-1,y+32-1);
            for(i=0;i<32*4;i++)
            {
                for(j=0;j<8;j++)
                {
                    if(!mode) //非叠加方式
                    {
                        if(tfont32[k].Msk[i]&(0x80>>j))	Lcd_WriteData_16Bit(fc);
                        else Lcd_WriteData_16Bit(bc);
                    }
                    else
                    {
                        POINT_COLOR=fc;
                        if(tfont32[k].Msk[i]&(0x80>>j))	LCD_DrawPoint(x,y);//画一个点
                        x++;
                        if((x-x0)==32)
                        {
                            x=x0;
                            y++;
                            break;
                        }
                    }
                }
            }
            
            
        }				  	
        continue;  //查找到对应点阵字库立即退出，防止多个汉字重复取模带来影响
    }
	
	LCD_SetWindows(0,0,lcddev.width-1,lcddev.height-1);//恢复窗口为全屏  
} 

/*****************************************************************************
 * @name       :void Show_Str(uint16_t x, uint16_t y, uint16_t fc, uint16_t bc, uint8_t *str,uint8_t size,uint8_t mode)
 * @date       :2018-08-09 
 * @function   :Display Chinese and English strings
 * @parameters :x:the bebinning x coordinate of the Chinese and English strings
                y:the bebinning y coordinate of the Chinese and English strings
								fc:the color value of Chinese and English strings
								bc:the background color of Chinese and English strings
								str:the start address of the Chinese and English strings
								size:the size of Chinese and English strings
								mode:0-no overlying,1-overlying
 * @retvalue   :None
******************************************************************************/	   		   
void Show_Str(uint16_t x, uint16_t y, uint16_t fc, uint16_t bc, uint8_t *str,uint8_t size,uint8_t mode)
{					
	uint16_t x0=x;							  	  
  	uint8_t bHz=0;     //字符或者中文 
    while(*str!=0)//数据未结束
    { 
        if(!bHz)
        {
			if(x>(lcddev.width-size/2)||y>(lcddev.height-size)) 
			return; 
	        if(*str>0x80)bHz=1;//中文 
	        else              //字符
	        {          
		        if(*str==0x0D)//换行符号
		        {         
		            y+=size;
					x=x0;
		            str++; 
		        }  
		        else
				{
					if(size>16)//字库中没有集成12X24 16X32的英文字体,用8X16代替
					{  
					LCD_ShowChar(x,y,fc,bc,*str,16,mode);
					x+=8; //字符,为全字的一半 
					}
					else
					{
					LCD_ShowChar(x,y,fc,bc,*str,size,mode);
					x+=size/2; //字符,为全字的一半 
					}
				} 
				str++; 
		        
	        }
        }else//中文 
        {   
			if(x>(lcddev.width-size)||y>(lcddev.height-size)) 
			return;  
            bHz=0;//有汉字库    
			if(size==32)
			GUI_DrawFont32(x,y,fc,bc,str,mode);	 	
			else if(size==24)
			GUI_DrawFont24(x,y,fc,bc,str,mode);	
			else
			GUI_DrawFont16(x,y,fc,bc,str,mode);
				
	        str+=2; 
	        x+=size;//下一个汉字偏移	    
        }						 
    }   
}

/*****************************************************************************
 * @name       :void Gui_StrCenter(uint16_t x, uint16_t y, uint16_t fc, uint16_t bc, uint8_t *str,uint8_t size,uint8_t mode)
 * @date       :2018-08-09 
 * @function   :Centered display of English and Chinese strings
 * @parameters :x:the bebinning x coordinate of the Chinese and English strings
                y:the bebinning y coordinate of the Chinese and English strings
								fc:the color value of Chinese and English strings
								bc:the background color of Chinese and English strings
								str:the start address of the Chinese and English strings
								size:the size of Chinese and English strings
								mode:0-no overlying,1-overlying
 * @retvalue   :None
******************************************************************************/ 
void Gui_StrCenter(uint16_t x, uint16_t y, uint16_t fc, uint16_t bc, uint8_t *str,uint8_t size,uint8_t mode)
{
	uint16_t len=strlen((const char *)str);
	uint16_t x1=(lcddev.width-len*8)/2;
	Show_Str(x1,y,fc,bc,str,size,mode);
} 
 
/*****************************************************************************
 * @name       :void Gui_Drawbmp16(uint16_t x,uint16_t y,const unsigned char *p)
 * @date       :2018-08-09 
 * @function   :Display a 16-bit BMP image
 * @parameters :x:the bebinning x coordinate of the BMP image
                y:the bebinning y coordinate of the BMP image
								p:the start address of image array
 * @retvalue   :None
******************************************************************************/ 
void Gui_Drawbmp16(uint16_t x,uint16_t y,const unsigned char *p)
{
  	int i; 
	unsigned char picH,picL; 
	LCD_SetWindows(x,y,x+40-1,y+40-1);//窗口设置
    for(i=0;i<40*40;i++)
	{	
	 	picL=*(p+i*2);	//数据低位在前
		picH=*(p+i*2+1);				
		Lcd_WriteData_16Bit(picH<<8|picL);  						
	}	
	LCD_SetWindows(0,0,lcddev.width-1,lcddev.height-1);//恢复显示窗口为全屏	
}

/*********************************************************************
-
-   函数名: void LcdDisplay(uint8_t Row,uint8_t *str)
-   功能  : 在指定LCD屏幕行上显示,
-   输入  : Row:需要显示的行,范围0-7; *str:显示的数据
-   返回  : 无
-
**********************************************************************/
void LcdDisplay(uint8_t Row,uint8_t *str)
{
    uint16_t bc = WHITE;
    switch(Row)
    {
        case 0 :Show_Str(2,  0,BLUE,bc,str,16,0);break;
        case 1 :Show_Str(2, 16,BLUE,bc,str,16,0);break;
        case 2 :Show_Str(2, 32,BLUE,bc,str,16,0);break;
        case 3 :Show_Str(2, 48,BLUE,bc,str,16,0);break;
        case 4 :Show_Str(2, 64,BLUE,bc,str,16,0);break;
        case 5 :Show_Str(2, 80,BLUE,bc,str,16,0);break;
        case 6 :Show_Str(2, 96,BLUE,bc,str,16,0);break;
        case 7 :Show_Str(2,112,BLUE,bc,str,16,0);break;
    }
}



/*****************************************************************************************************

																							menu

******************************************************************************************************/


void LCD_DrawString(uint16_t x,uint16_t y,const char*p,uint8_t size,uint16_t color,uint16_t bc)
{
	LCD_ShowString_color(x,y,color,bc,size,(uint8_t *)p,0);
}


Key_TypeDef key_dev;
// Menu level definition
typedef enum {
    MENU_LEVEL_MAIN,        // Main menu
    MENU_LEVEL_AUTO_TEST,   // Auto test page
    MENU_LEVEL_MANUAL_TEST, // Manual test submenu
    MENU_LEVEL_RECEIVER,    // Receiver selection submenu
    MENU_LEVEL_SYSTEM,       // System settings submenu
		
		PARA_CHANGE
} MenuLevel;

// Main menu items
static const char* mainMenuItems[] = {
    "Auto Test",
    "Manual Test",
    "Receiver Sel",
    "System Setup"
};
#define MAIN_MENU_COUNT 4

Menu_list main_Menu={mainMenuItems,MAIN_MENU_COUNT};


// Manual test submenu items
static const char* manualTestItems[] = {
    "Voltage Test",
    "Current Test",
    "Signal Test",
};
#define MANUAL_MENU_COUNT 3
Menu_list manualTest_Menu={manualTestItems,MANUAL_MENU_COUNT};


// Receiver selection submenu items
static const char* receiverItems[] = {
    "RX4",
    "RX6/Micro",
    "RX6 Dual",
	  "RX8 Dual",
	  "SuperP",
		"nano",
		"SuperD",

};
#define RECEIVER_MENU_COUNT 7
Menu_list receiver_Menu={receiverItems,RECEIVER_MENU_COUNT};


// System settings submenu items
static const char* systemItems[] = {
    "*LQ_t",
    "*RSSI_t",
		"Version     v1.0.1",
};
#define SYSTEM_MENU_COUNT 3

Menu_list system_MENU={systemItems,SYSTEM_MENU_COUNT};

Menu_list *CurrentMenu=&main_Menu;
static const char** MenuItems_List[]=
{
	mainMenuItems,
	manualTestItems,
	receiverItems,
	systemItems
};

// Menu state structure
typedef struct {
    MenuLevel currentLevel;  // Current menu level
    uint8_t currentIndex;    // Current selected index
    uint8_t prevIndex[5];    // History selected index for each level
} MenuState;

// Global menu state
 MenuState menuState = {
    .currentLevel = MENU_LEVEL_MAIN,
    .currentIndex = 0
};

/**
 * Draw menu item
 */
char para_str[5];
u8 p;
void drawMenuItem(uint16_t y, const char* text, uint8_t isSelected)
{
    // Draw blue background for selected item
    if (isSelected)
		{
			LCD_DrawRectangle(3, y+2, LCD_WIDTH-1, y+22);
			LCD_SetWindows(3, y+2, LCD_WIDTH-1, y+22);
			LCD_WriteRAM_Prepare();
			if(text[0]=='*')//参数标志位如果字符串开头是*则是可调参数
			{
			 LCD_DrawString(10, y+5, &text[0]+1, 16, 0x0000,GREEN);  // Assume 16pt font exists
				p=scan_para(&text[0]+1);
				if(p!=255)
				{
					sprintf(para_str, "%d", (u32)(*((u32*)(Parameter_List[p]->para_p))));
				}
			LCD_DrawString(LCD_WIDTH-35, y+5, para_str, 16, 0x0000,WHITE);  // Assume 16pt font exists
			}
			else
			{
				 LCD_DrawString(10, y+5, text, 16, 0x0000,GREEN);  // Assume 16pt font exists
			}
    } 
		else
		{
			if(text[0]=='*')//参数标志位如果字符串开头是*则是可调参数
			{
			 LCD_DrawString(10, y+5, &text[0]+1, 16, 0x0000,WHITE);  // Assume 16pt font exists
				p=scan_para(&text[0]+1);
				if(p!=255)
				{
				sprintf(para_str, "%d", (u32)(*((u32*)(Parameter_List[p]->para_p))));
				}
			LCD_DrawString(LCD_WIDTH-35, y+5, para_str, 16, 0x0000,WHITE);  // Assume 16pt font exists
			}
			else
			{
				 LCD_DrawString(10, y+5, text, 16, 0x0000,WHITE);  // Assume 16pt font exists
			}
    }
}
 
/*
* Draw scroll menu
*/

void drawScrollMenu(u16 x,u16 y, u8 size)
{
//	u8 i=CurrentMenu->list_num/5;
	u8  top_p=menuState.currentIndex/5;
	u8  cnt=0;
	for (uint8_t i = top_p*5;((i-(top_p*5))<5)&(i <CurrentMenu->list_num); i++) {
		drawMenuItem(x + cnt*y, CurrentMenu->str[i], (i == menuState.currentIndex));
		cnt++;
	}		
	
}

/**
 * Draw main menu
 */
 void drawMainMenu(void) {
    LCD_Clear(0xffff);  // Clear screen with black
    LCD_DrawString(50, 0, "Main Menu", 16, RED,WHITE);  // Green title
    
    // Draw menu items (arranged top to bottom)
    for (uint8_t i = 0; i < MAIN_MENU_COUNT; i++) {
        drawMenuItem(20 + i*22, mainMenuItems[i], (i == menuState.currentIndex));
    }
    
}

/**
 * Draw manual test submenu
 */
 void drawManualTestMenu(void) {
    LCD_Clear(0xffff);
    LCD_DrawString(30, 0, "Manual Test", 16, RED,WHITE);
		if(Get_normal_or_pwm()==1)
		{
				LCD_DrawString(5, 20, "RSSI:", 16, BLACK,WHITE);
				LCD_DrawString(90, 20, "LQ:", 16, BLACK,WHITE);

			 if((((*current_rx_target)>>1)&(0x01))==1) 
				 {

				 LCD_DrawString(90, 40, "Vb:", 16, BLACK,WHITE);
				 }		 
			 if((((*current_rx_target)>>2)&(0x01))==1) 
				 {
					LCD_DrawString(5, 40, "ALT:", 16, BLACK,WHITE);
				 }		 

			 switch(((*current_rx_target)>>5)&(0xf)) 		 
				 {
					case 4:
					LCD_DrawString(5, 60, "ch1:", 16, BLACK,WHITE);
					LCD_DrawString(90, 60, "ch2:", 16, BLACK,WHITE);			
					LCD_DrawString(5, 75, "ch3:", 16, BLACK,WHITE);
					LCD_DrawString(90, 75, "ch4:", 16, BLACK,WHITE);
					break;
					case 6:
					LCD_DrawString(5, 60, "ch1:", 16, BLACK,WHITE);
					LCD_DrawString(90, 60, "ch2:", 16, BLACK,WHITE);			
					LCD_DrawString(5, 75, "ch3:", 16, BLACK,WHITE);
					LCD_DrawString(90, 75, "ch4:", 16, BLACK,WHITE);				
					
					LCD_DrawString(5, 90, "ch5:", 16, BLACK,WHITE);
					LCD_DrawString(90, 90, "ch6:", 16, BLACK,WHITE);
					break;
					case 8:	
					LCD_DrawString(5, 60, "ch1:", 16, BLACK,WHITE);
					LCD_DrawString(90, 60, "ch2:", 16, BLACK,WHITE);			
					LCD_DrawString(5, 75, "ch3:", 16, BLACK,WHITE);
					LCD_DrawString(90, 75, "ch4:", 16, BLACK,WHITE);				
					LCD_DrawString(5, 90, "ch5:", 16, BLACK,WHITE);
					LCD_DrawString(90, 90, "ch6:", 16, BLACK,WHITE);
					LCD_DrawString(5, 105, "ch7:", 16, BLACK,WHITE);
					LCD_DrawString(90, 105, "ch8:", 16, BLACK,WHITE);					
					break;
					case 14:	
					LCD_DrawString(5, 60, "ch1:", 12, BLACK,WHITE);	LCD_DrawString(65, 60, "ch2:", 12, BLACK,WHITE);LCD_DrawString(115, 60, "ch3:", 12, BLACK,WHITE);
					LCD_DrawString(5, 72, "ch4:", 12, BLACK,WHITE);	LCD_DrawString(65, 72, "ch5:", 12, BLACK,WHITE);LCD_DrawString(115, 72, "ch6:", 12, BLACK,WHITE);
					LCD_DrawString(5, 84, "ch7:", 12, BLACK,WHITE);	LCD_DrawString(65, 84, "ch8:", 12, BLACK,WHITE);LCD_DrawString(115, 84, "ch9:", 12, BLACK,WHITE);
					LCD_DrawString(5, 96, "ch10:", 12, BLACK,WHITE);LCD_DrawString(65, 96, "ch11:", 12, BLACK,WHITE);LCD_DrawString(115, 96, "ch12:", 12, BLACK,WHITE);
					LCD_DrawString(5, 108, "ch13:", 12, BLACK,WHITE);LCD_DrawString(65, 108, "ch14:", 12, BLACK,WHITE);
					break;				 
				 }				
			}
		else if(Get_normal_or_pwm()==0)
		{
				LCD_DrawString(5, 20, "1RSSI:", 16, BLACK,WHITE);    LCD_DrawString(90, 20, "1LQ:", 16, BLACK,WHITE);		
				LCD_DrawString(5, 36, "1RQLY:", 16, BLACK,WHITE);    LCD_DrawString(90, 36, "1TQLY:", 16, BLACK,WHITE);		

				LCD_DrawString(5, 56, "2RSSI:", 16, BLACK,WHITE);    LCD_DrawString(90, 56, "2LQ:", 16, BLACK,WHITE);		
				LCD_DrawString(5, 72, "2RQLY:", 16, BLACK,WHITE);    LCD_DrawString(90, 72, "2TQLY:", 16, BLACK,WHITE);							

				LCD_DrawString(5, 92, "3RSSI:", 16, BLACK,WHITE);    LCD_DrawString(90, 92, "3LQ:", 16, BLACK,WHITE);		
				LCD_DrawString(5, 108, "3RQLY:", 16, BLACK,WHITE);    LCD_DrawString(90, 108, "3TQLY:", 16, BLACK,WHITE);										
		}
	 
	 
	

}

/**
 * Draw receiver selection submenu
 */


void drawReceiverMenu(void) {
    LCD_Clear(0xffff);
    LCD_DrawString(30, 0, "Receiver Sel", 16, 0x07E0,WHITE);
    drawScrollMenu(20,22,16);
//    for (uint8_t i = 0; i < RECEIVER_MENU_COUNT; i++) {
//        drawMenuItem(20 + i*22, receiverItems[i], (i == menuState.currentIndex));
//    }
    
}

/**
 * Draw system settings submenu
 */
static void drawSystemMenu(void) {
    LCD_Clear(0xffff);
    LCD_DrawString(30, 0, "System Setup", 16, 0x07E0,WHITE);
		drawScrollMenu(20,22,16);
//    for (uint8_t i = 0; i < SYSTEM_MENU_COUNT; i++) {
//        drawMenuItem(20 + i*22, systemItems[i], (i == menuState.currentIndex));
//    }
    
}

/**
 * Draw auto test page
 */
//Rx_target * current_rx_target;

extern u8 test_step;
 void drawAutoTestPage(void) {
    LCD_Clear(0xffff);
     // Red title
	 LCD_DrawRectangle(10, 125, 150, 129);
		if(*current_rx_target==Rx4)
		{
			 LCD_DrawString(60, 0, "Rx4", 16, RED,WHITE);		
		}
		else if(*current_rx_target==Rx6)
		{
			 LCD_DrawString(40, 0, "Rx6/Micro", 16, RED,WHITE);		
		}
		else if(*current_rx_target==Rx6_dual)
		{
			LCD_DrawString(50, 0, "Rx6Dual", 16, RED,WHITE);		
		
		}
		else if(*current_rx_target==Rx8_dual)
		{
			LCD_DrawString(50, 0, "Rx8Dual", 16, RED,WHITE);		

		}
		else if(*current_rx_target==SuperP)
		{
			LCD_DrawString(50, 0, "SuperP", 16, RED,WHITE);		

		}
    // Draw test progress bar
    
}
 

/**
 * Draw Para change page
 */

void DrawParaChangeMenu(void) 
{
		scan_para(systemItems[menuState.currentIndex]);
		LCD_DrawFillRectangle_color(10, 30, 150, 100,GRAY);
//    LCD_DrawString(50, 0, "Main Menu", 16, RED,WHITE);  // Green title
    LCD_DrawString(60, 40,systemItems[menuState.currentIndex]+1, 16, WHITE,GRAY);  // PARA
		LCD_ShowNum_Color(65,65,BLACK,GRAY,(u32)(*((u32*)(Parameter_List[scan_para(systemItems[menuState.currentIndex]+1)]->para_p))),3,16);
		LCD_DrawString(20, 65, "-", 16, RED,GRAY);
		LCD_DrawString(130, 65, "+", 16, RED,GRAY);
		
    
}
	








/**
 * Handle main menu input主页面
 */
extern TaskHandle_t RX_TESTTask_Handle;

 void handleMainMenuInput(Key_TypeDef key) {
    switch (key) {
        case KEY_UP:
            menuState.currentIndex = (menuState.currentIndex > 0) ? 
                                   menuState.currentIndex - 1 : MAIN_MENU_COUNT - 1;
            drawMainMenu();
            break;
            
        case KEY_DOWN:
            menuState.currentIndex = (menuState.currentIndex < MAIN_MENU_COUNT - 1) ? 
                                   menuState.currentIndex + 1 : 0;
            drawMainMenu();
            break;
            
        case KEY_ENTER:
        case KEY_RIGHT:
            // Save current index and enter submenu
            menuState.prevIndex[menuState.currentLevel] = menuState.currentIndex;
            
            switch (menuState.currentIndex) {
                case 0:  // 选择自动测试
                    menuState.currentLevel = MENU_LEVEL_AUTO_TEST;
                    drawAutoTestPage();
										LCD_DrawString(20, 60, "Press test key", 16, 0x0000,WHITE);
										HAL_Delay(100);
                    break;
                case 1:  // 选择手动模式
                    menuState.currentLevel = MENU_LEVEL_MANUAL_TEST;
                    menuState.currentIndex = 0;
										drawManualTestMenu();
										Start_Manual_Test();
                    break;
                case 2:  // 选择接收机
										CurrentMenu=&receiver_Menu;
                    menuState.currentLevel = MENU_LEVEL_RECEIVER;
                    menuState.currentIndex = 0;
                    drawReceiverMenu();
                    break;
                case 3:  // 选择系统设置
										CurrentMenu=&system_MENU;
                    menuState.currentLevel = MENU_LEVEL_SYSTEM;
                    menuState.currentIndex = 0;
                    drawSystemMenu();
                    break;
            }
            break;
           case KEY_LEFT:
						traverse_indexes_form_TX();
						
            break;
						

					default:
           break;
    }
}

/**
 * Handle manual test menu input手动测试界面
 */
 void handleManualTestInput(Key_TypeDef key) {
    switch (key) {
        case KEY_UP:

            break;
            
        case KEY_DOWN:
            break;
        case KEY_LEFT:
            // Return to main menu
            menuState.currentLevel = MENU_LEVEL_MAIN;
            menuState.currentIndex = menuState.prevIndex[MENU_LEVEL_MAIN];
						Stop_Manual_Test();
            drawMainMenu();
						
            break;
            
        case KEY_ENTER:
        case KEY_RIGHT:

            break;
            
        default:
            break;
    }
}

/**
* Handle receiver selection menu input接收机选择界面
 */
 void handleReceiverInput(Key_TypeDef key) {
    switch (key) {
        case KEY_UP:
            menuState.currentIndex = (menuState.currentIndex > 0) ? 
                                   menuState.currentIndex - 1 : RECEIVER_MENU_COUNT - 1;
            drawReceiverMenu();
            break;
            
        case KEY_DOWN:
            menuState.currentIndex = (menuState.currentIndex < RECEIVER_MENU_COUNT - 1) ? 
                                   menuState.currentIndex + 1 : 0;
            drawReceiverMenu();
            break;
            
        case KEY_LEFT:
            menuState.currentLevel = MENU_LEVEL_MAIN;
            menuState.currentIndex = menuState.prevIndex[MENU_LEVEL_MAIN];
            drawMainMenu();
            break;
            
        case KEY_ENTER:
        case KEY_RIGHT:
//            if (menuState.currentIndex == RECEIVER_MENU_COUNT) {
//                menuState.currentLevel = MENU_LEVEL_MAIN;
//                menuState.currentIndex = menuState.prevIndex[MENU_LEVEL_MAIN];
//                drawMainMenu();
//            } else {
//                LCD_Clear(0xffff);					
							if(menuState.currentIndex==0)
							{
								current_rx_target=&Rx4;
							
							}
							if(menuState.currentIndex==1)
							{
								current_rx_target=&Rx6;
							}
							if(menuState.currentIndex==2)
							{
								current_rx_target=&Rx6_dual;
							
							}	
							if(menuState.currentIndex==3)
							{
								current_rx_target=&Rx8_dual;							
							}
							if(menuState.currentIndex==4)
							{
								current_rx_target=&SuperP;							
							}
							if(menuState.currentIndex==5)
							{
								current_rx_target=&SuperXnano;							
							}
							if(menuState.currentIndex==6)
							{
								current_rx_target=&SuperD;							
							}								
						Global_parameter.RX_device_t=current_rx_target;
							LCD_DrawFillRectangle_color(10, 40, 150, 90,GRAY);
							LCD_DrawString(35, 60, "saving...", 16, 0x0000,GRAY);
						stmflash_write(0x080E0000,(u32 *)&Global_parameter,sizeof(Flash_para));
//							osDelay(200);
            menuState.currentLevel = MENU_LEVEL_MAIN;
            menuState.currentIndex = menuState.prevIndex[MENU_LEVEL_MAIN];
            drawMainMenu();
//            }
            break;
            
        default:
            break;
    }
}

/**
 * Handle system settings menu input系统设置界面
 */
u8 para_update=0;
void handleSystemInput(Key_TypeDef key) {
	 
    switch (key) {
        case KEY_UP:
            menuState.currentIndex = (menuState.currentIndex > 0) ? 
                                   menuState.currentIndex - 1 : SYSTEM_MENU_COUNT - 1;
            drawSystemMenu();
            break;
            
        case KEY_DOWN:
            menuState.currentIndex = (menuState.currentIndex < SYSTEM_MENU_COUNT - 1) ? 
                                   menuState.currentIndex + 1 : 0;
            drawSystemMenu();
            break;
            
        case KEY_LEFT:
            menuState.currentLevel = MENU_LEVEL_MAIN;
            menuState.currentIndex = menuState.prevIndex[MENU_LEVEL_MAIN];
						if(para_update==1)
						{
							LCD_DrawFillRectangle_color(10, 40, 150, 90,GRAY);
							LCD_DrawString(35, 60, "saving...", 16, 0x0000,GRAY);
							stmflash_write(0x080E0000,(u32 *)&Global_parameter,sizeof(Flash_para));	
							para_update=0;							
						}

            drawMainMenu();
            break;
            
        case KEY_ENTER:
        case KEY_RIGHT:
						 menuState.prevIndex[menuState.currentLevel] = menuState.currentIndex;
            if ((systemItems[menuState.currentIndex])[0] =='*') {
                  menuState.currentLevel = PARA_CHANGE;
									para_update=1;
									DrawParaChangeMenu();
            }
//						else {
//                LCD_Clear(0xffff);
//                LCD_DrawString(20, 60, systemItems[menuState.currentIndex], 16, 0x07E0,WHITE);
//                LCD_DrawString(10, 100, "Press any key", 16, 0x0000,WHITE);
//                LCD_DrawString(10, 120, "to return", 16, 0x0000,WHITE);
//                HAL_Delay(1000);
//                drawSystemMenu();
//            }
            break;
            
        default:
            break;
    }
}

/**
 * Handle auto test page input自动测试界面
 */
 void handleAutoTestInput(Key_TypeDef key) {
    switch (key) {
        case KEY_LEFT:
            // Return to main menu
            menuState.currentLevel = MENU_LEVEL_MAIN;
            menuState.currentIndex = menuState.prevIndex[MENU_LEVEL_MAIN];
						Stop_Auto_Test();
            drawMainMenu();
						
            break;
        case KEY_1:    
        case KEY_ENTER:
						Stop_Auto_Test();
						LCD_Clear(0xffff);
						HAL_Delay(200);
            drawAutoTestPage();
						Start_Auto_Test();
            break;
            
        default:
            break;
    }
}
 


/**
 * Para page input参数选择界面
 */
 void Para_change_Input(Key_TypeDef key) {
    switch (key) {
        case KEY_LEFT:
            // Return to main menu
							(u32)(*((u32*)(Parameter_List[scan_para(systemItems[menuState.currentIndex]+1)]->para_p)))--;
							LCD_ShowNum_Color(65,65,BLACK,GRAY,(u32)(*((u32*)(Parameter_List[scan_para(systemItems[menuState.currentIndex]+1)]->para_p))),3,16);

            break;
				case KEY_RIGHT:
							(u32)(*((u32*)(Parameter_List[scan_para(systemItems[menuState.currentIndex]+1)]->para_p)))++;
							LCD_ShowNum_Color(65,65,BLACK,GRAY,(u32)(*((u32*)(Parameter_List[scan_para(systemItems[menuState.currentIndex]+1)]->para_p))),3,16);

            break;					
        case KEY_ENTER:
							
						   menuState.currentLevel = MENU_LEVEL_SYSTEM;
               menuState.currentIndex = menuState.prevIndex[MENU_LEVEL_SYSTEM];
               drawSystemMenu();
            break;
						
            
        default:
            break;
    }
}

/**
 * Main menu processing function 界面按键回调函数
 */
void Menu_Process(void) {
    Key_Event_TypeDef event;
    Key_TypeDef key = Key_5D_Scan(&key_dev, &event);
    
    // Only process short press events
    if (key != KEY_NONE && event == KEY_EVENT_PRESS) {
        switch (menuState.currentLevel) {
            case MENU_LEVEL_MAIN:
                handleMainMenuInput(key);
                break;
            case MENU_LEVEL_AUTO_TEST:
                handleAutoTestInput(key);
                break;
            case MENU_LEVEL_MANUAL_TEST:
                handleManualTestInput(key);
                break;
            case MENU_LEVEL_RECEIVER:
                handleReceiverInput(key);
                break;
            case MENU_LEVEL_SYSTEM:
                handleSystemInput(key);
                break;
            case PARA_CHANGE:
                Para_change_Input(key);
                break;						

            default:
                break;
        }
    }
}


void Global_dynamic_display()//全局动态刷新
{
		if(Get_LINK_CONNECT_STATIC()==1)
		{
			
			LCD_Fill(147,0,160,18,GRAY);
			LCD_ShowString_color(150,2,WHITE, GRAY,16,"C",1);
			
		}
		else if(Get_LINK_CONNECT_STATIC()==0)
		{
			LCD_Fill(147,0,160,18,WHITE);
		}	

}



void dynamic_display()//界面切换时刷新
{
	static u8 link_state=0;
	if(link_state!=Get_LINK_CONNECT_STATIC())
	{
		if(Get_LINK_CONNECT_STATIC()==1)
		{
			
			LCD_Fill(147,0,160,18,GRAY);
			LCD_ShowString_color(150,2,WHITE, GRAY,16,"C",1);
		}
		else if(Get_LINK_CONNECT_STATIC()==0)
		{
			LCD_Fill(147,0,160,18,WHITE);
		}	
		link_state=Get_LINK_CONNECT_STATIC();
	}

}