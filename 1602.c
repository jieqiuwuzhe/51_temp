
#include<reg51.h>	
#include"lcd.h"

//--����ȫ�ֱ���--//


extern unsigned char m,n,p,mm,nn,pp;

/*******************************************************************************
* �� �� ��         : main
* ��������		   : ������
* ��    ��         : ��
* ��    ��         : ��
*******************************************************************************/
void show1(signed int dd)
{
	int tt=0; 
  tt= (unsigned int)(dd*0.625+0.5); //�Ŵ�10���������������
	m=(char)((tt/100)+48);		    //�����ʮλ
	n=(char)(((tt%100)/10)+48);	//�������λ
	p=(char)((tt%10)+48);		    //����С��λ

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
		
  tt= (unsigned int)(dd*0.625+0.5); //�Ŵ�10���������������
	mm=(char)((tt/100)+48);		    //�����ʮλ
	nn=(char)(((tt%100)/10)+48);	//�������λ
	pp=(char)((tt%10)+48);		    //����С��λ
		
	LcdWriteCom(0xC8);
	LcdWriteData(mm);
	LcdWriteCom(0xC9);
	LcdWriteData(nn);
	LcdWriteCom(0xCB);
	LcdWriteData(pp);

	
		
}