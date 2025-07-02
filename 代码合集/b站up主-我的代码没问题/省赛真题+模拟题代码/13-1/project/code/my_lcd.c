#include "headfile.h"

char psd_text[20];
char pw1_text[20];
char pw2_text[20];
char pw3_text[20];
	
char sta_text[20];
char fre_text[20];
char duty_text[20];

uint8_t pw_init_flag[3]={1,1,1};

void lcd_show1(void)
{
	sprintf(psd_text,"       PSD");
	if(pw_init_flag[0]){sprintf(pw1_text,"    B1:@");}else{sprintf(pw1_text,"    B1:%d",pw_wait[0]);}
	if(pw_init_flag[1]){sprintf(pw2_text,"    B2:@");}else{sprintf(pw2_text,"    B2:%d",pw_wait[1]);}
	if(pw_init_flag[2]){sprintf(pw3_text,"    B3:@");}else{sprintf(pw3_text,"    B3:%d",pw_wait[2]);}
	LCD_DisplayStringLine(Line1,(uint8_t *)psd_text);
  LCD_DisplayStringLine(Line3,(uint8_t *)pw1_text);
	LCD_DisplayStringLine(Line4,(uint8_t *)pw2_text);
	LCD_DisplayStringLine(Line5,(uint8_t *)pw3_text);
}

void lcd_show2(void)
{
  LCD_ClearLine(Line5);
	sprintf(sta_text,"       STA");
	sprintf(fre_text,"    F:2000Hz");
	sprintf(duty_text,"    D:10%%");
	LCD_DisplayStringLine(Line1,(uint8_t *)sta_text);
  LCD_DisplayStringLine(Line3,(uint8_t *)fre_text);
	LCD_DisplayStringLine(Line4,(uint8_t *)duty_text);

}


void lcd_show1_init(void)
{
  LCD_ClearLine(Line3);
  LCD_ClearLine(Line4);
	pw_init_flag[0]=1;
	pw_init_flag[1]=1;
	pw_init_flag[2]=1;
  pw_wait[0]=-1;
  pw_wait[1]=-1;
  pw_wait[2]=-1;

}
