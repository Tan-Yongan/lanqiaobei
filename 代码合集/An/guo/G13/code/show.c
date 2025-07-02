#include "headfile.h"

char text[30];

double PA4,PA5,PA1;
uint8_t X=1,Y=1;

void data_show()
{
	sprintf(text,"        DATA        ");
	LCD_DisplayStringLine(Line1,(uint8_t *)text);
	sprintf(text,"     PA4=%.2f        ",PA4);
	LCD_DisplayStringLine(Line3,(uint8_t *)text);
	sprintf(text,"     PA5=%.2f        ",PA5);
	LCD_DisplayStringLine(Line4,(uint8_t *)text);
	sprintf(text,"     PA1=%.2f        ",PA1);
	LCD_DisplayStringLine(Line5,(uint8_t *)text);
	
	sprintf(text,"MCP=%.2f        ",get_vol(&hadc1));
	LCD_DisplayStringLine(Line8,(uint8_t *)text);
	
	
	eeprom_write(254,66);
	sprintf(text,"eeprom %d       ",eeprom_read(254));
	LCD_DisplayStringLine(Line9,(uint8_t *)text);
	
}

void para_show()
{
	sprintf(text,"        PARA        ");
	LCD_DisplayStringLine(Line1,(uint8_t *)text);
	sprintf(text,"     X=%d        ",X);
	LCD_DisplayStringLine(Line3,(uint8_t *)text);
	sprintf(text,"     Y=%d        ",Y);
	LCD_DisplayStringLine(Line4,(uint8_t *)text);
	sprintf(text,"                ");
	LCD_DisplayStringLine(Line5,(uint8_t *)text);}

void rec_show()
{
	sprintf(text,"        REC        ");
	LCD_DisplayStringLine(Line1,(uint8_t *)text);
}
void lcd_show()
{
	switch(show_switch)
	{
		case 0:
			data_show();
			break;
		case 1:
			para_show();
			break;
		case 2:
			rec_show();
			break;
		default:
			break;
	}
	
}
//void lcd_show()
//{
//	sprintf(text,"        DATA        ");
//	LCD_DisplayStringLine(Line0,(uint8_t *)text);
//	sprintf(text,"R38_V=%.2f        ",data[0]);
//	LCD_DisplayStringLine(Line2,(uint8_t *)text);
//	sprintf(text,"R37_V=%.2f        ",data[1]);
//	LCD_DisplayStringLine(Line3,(uint8_t *)text);
//	sprintf(text,"MCP=%d        ",mcp4017_read());
//	LCD_DisplayStringLine(Line4,(uint8_t *)text);
//	
//	sprintf(text,"MCP_V=%.2f    ",get_vol(&hadc1));
//	LCD_DisplayStringLine(Line5,(uint8_t *)text);
//	
//	sprintf(text,"R39_F=%d    ",R39_F);
//	LCD_DisplayStringLine(Line6,(uint8_t *)text);
//	sprintf(text,"PA6_F=%d    ",PA6_F);
//	LCD_DisplayStringLine(Line7,(uint8_t *)text);
//}