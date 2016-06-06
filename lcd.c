#include"lcd.h"

char sheshi=(char)0xdf;
char dian='.';
char kong=' ';
char du[] = "C  ";
unsigned char first1[]={"SET: "};
unsigned char second[]={"NOW: "};

/*******************************************************************************
* �� �� ��         : Lcd1602_Delay1ms
* ��������		   : ��ʱ��������ʱ1ms
* ��    ��         : c
* ��    ��         : ��
* ˵    ��         : �ú�������12MHZ�����£�12��Ƶ��Ƭ������ʱ��
*******************************************************************************/

void Lcd1602_Delay1ms(uint c)   //��� 0us
{
    uchar a,b;
	for (; c>0; c--)
	{
		 for (b=199;b>0;b--)
		 {
		  	for(a=1;a>0;a--);
		 }      
	}
    	
}

/*******************************************************************************
* �� �� ��         : LcdWriteCom
* ��������		   : ��LCDд��һ���ֽڵ�����
* ��    ��         : com
* ��    ��         : ��
*******************************************************************************/
#ifndef 	LCD1602_4PINS	 //��û�ж������LCD1602_4PINSʱ
void LcdWriteCom(uchar com)	  //д������
{
	LCD1602_E = 0;     //ʹ��
	LCD1602_RS = 0;	   //ѡ��������
	LCD1602_RW = 0;	   //ѡ��д��
	
	LCD1602_DATAPINS = com;     //��������
	Lcd1602_Delay1ms(1);		//�ȴ������ȶ�

	LCD1602_E = 1;	          //д��ʱ��
	Lcd1602_Delay1ms(5);	  //����ʱ��
	LCD1602_E = 0;
}
#else 
void LcdWriteCom(uchar com)	  //д������
{
	LCD1602_E = 0;	 //ʹ������
	LCD1602_RS = 0;	 //ѡ��д������
	LCD1602_RW = 0;	 //ѡ��д��

	LCD1602_DATAPINS = com;	//����4λ�Ľ����ǽӵ�P0�ڵĸ���λ�����Դ��͸���λ���ø�
	Lcd1602_Delay1ms(1);

	LCD1602_E = 1;	 //д��ʱ��
	Lcd1602_Delay1ms(5);
	LCD1602_E = 0;

//	Lcd1602_Delay1ms(1);
	LCD1602_DATAPINS = com << 4; //���͵���λ
	Lcd1602_Delay1ms(1);

	LCD1602_E = 1;	 //д��ʱ��
	Lcd1602_Delay1ms(5);
	LCD1602_E = 0;
}
#endif
/*******************************************************************************
* �� �� ��         : LcdWriteData
* ��������		   : ��LCDд��һ���ֽڵ�����
* ��    ��         : dat
* ��    ��         : ��
*******************************************************************************/		   
#ifndef 	LCD1602_4PINS		   
void LcdWriteData(uchar dat)			//д������
{
	LCD1602_E = 0;	//ʹ������
	LCD1602_RS = 1;	//ѡ����������
	LCD1602_RW = 0;	//ѡ��д��

	LCD1602_DATAPINS = dat; //д������
	Lcd1602_Delay1ms(1);

	LCD1602_E = 1;   //д��ʱ��
	Lcd1602_Delay1ms(5);   //����ʱ��
	LCD1602_E = 0;
}
#else
void LcdWriteData(uchar dat)			//д������
{
	LCD1602_E = 0;	  //ʹ������
	LCD1602_RS = 1;	  //ѡ��д������
	LCD1602_RW = 0;	  //ѡ��д��

	LCD1602_DATAPINS = dat;	//����4λ�Ľ����ǽӵ�P0�ڵĸ���λ�����Դ��͸���λ���ø�
	Lcd1602_Delay1ms(1);

	LCD1602_E = 1;	  //д��ʱ��
	Lcd1602_Delay1ms(5);
	LCD1602_E = 0;

	LCD1602_DATAPINS = dat << 4; //д�����λ
	Lcd1602_Delay1ms(1);

	LCD1602_E = 1;	  //д��ʱ��
	Lcd1602_Delay1ms(5);
	LCD1602_E = 0;
}
#endif
/*******************************************************************************
* �� �� ��       : LcdInit()
* ��������		 : ��ʼ��LCD��
* ��    ��       : ��
* ��    ��       : ��
*******************************************************************************/		   
#ifndef		LCD1602_4PINS
void LcdInit()						  //LCD��ʼ���ӳ���
{
 	unsigned char i;
	unsigned char j;
	

	LcdWriteCom(0x38);  //����ʾ
	LcdWriteCom(0x0c);  //����ʾ����ʾ���
	LcdWriteCom(0x06);  //дһ��ָ���1
	LcdWriteCom(0x01);  //����
	LcdWriteCom(0x82);
	for(i=0;i<5;i++)
	{
		LcdWriteData(first1[i]);	
	}
	LcdWriteCom(0x8A);
	LcdWriteData(dian);
	LcdWriteCom(0x8C);
	LcdWriteData(kong);
	LcdWriteCom(0x8D);
	LcdWriteData(sheshi);
	LcdWriteCom(0x8E);
	for(j=0;j<3;j++)
	{
		LcdWriteData(du[j]);	
	}
	LcdWriteCom(0xC2);
  for(i=0;i<5;i++)
	{
		LcdWriteData(second[i]);	
	}
	LcdWriteCom(0x8A);
	LcdWriteData(dian);
	LcdWriteCom(0x8C);
	LcdWriteData(kong);
	LcdWriteCom(0xCD);
	LcdWriteData(sheshi);
	LcdWriteCom(0xCE);
	for(j=0;j<3;j++)
	{
		LcdWriteData(du[j]);	
	}
	//LcdWriteCom(0x82);  //��������ָ�����
}
#else
void LcdInit()						  //LCD��ʼ���ӳ���
{
	unsigned char i;
	unsigned char j;
	LcdWriteCom(0x32);	 //��8λ����תΪ4λ����
	LcdWriteCom(0x28);	 //����λ���µĳ�ʼ��
	LcdWriteCom(0x0c);  //����ʾ����ʾ���
	LcdWriteCom(0x06);  //дһ��ָ���1
	LcdWriteCom(0x01);  //����
	LcdWriteCom(0x82);
	for(i=0;i<5;i++)
	{
		LcdWriteData(first1[i]);	
	}
	LcdWriteCom(0x8A);
	LcdWriteData(dian);
	LcdWriteCom(0x8C);
	LcdWriteData(kong);
	LcdWriteCom(0x8D);
	LcdWriteData(sheshi);
	LcdWriteCom(0x8E);
	for(j=0;j<3;j++)
	{
		LcdWriteData(du[j]);	
	}
	LcdWriteCom(0xC2);
  for(i=0;i<5;i++)
	{
		LcdWriteData(second[i]);	
	}
	LcdWriteCom(0xCA);
	LcdWriteData(dian);
	LcdWriteCom(0xCC);
	LcdWriteData(kong);
	LcdWriteCom(0xCD);
	LcdWriteData(sheshi);
	LcdWriteCom(0xCE);
	for(j=0;j<3;j++)
	{
		LcdWriteData(du[j]);	
	}
	//LcdWriteCom(0x82);  //��������ָ�����
}
#endif
