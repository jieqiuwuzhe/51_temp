
#include<reg51.h>	
#include"lcd.h"

//--定义全局变量--//


extern unsigned char m,n,p,mm,nn,pp;

/*******************************************************************************
* 函 数 名         : main
* 函数功能		   : 主函数
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/
void show1(signed int dd)
{
	int tt=0; 
  tt= (unsigned int)(dd*0.625+0.5); //放大10倍输出并四舍五入
	m=(char)((tt/100)+48);		    //分离出十位
	n=(char)(((tt%100)/10)+48);	//分理出个位
	p=(char)((tt%10)+48);		    //分理处小数位

	LcdWriteCom(0x88);
	LcdWriteData(m);
	LcdWriteCom(0x89);
	LcdWriteData(n);
	LcdWriteCom(0x8B);
	LcdWriteData(p);
	
}	
	
void show2(signed int dd)
{	
	int tt=0; 
		
  tt= (unsigned int)(dd*0.625+0.5); //放大10倍输出并四舍五入
	mm=(char)((tt/100)+48);		    //分离出十位
	nn=(char)(((tt%100)/10)+48);	//分理出个位
	pp=(char)((tt%10)+48);		    //分理处小数位
		
	LcdWriteCom(0xC8);
	LcdWriteData(mm);
	LcdWriteCom(0xC9);
	LcdWriteData(nn);
	LcdWriteCom(0xCB);
	LcdWriteData(pp);

	
		
}