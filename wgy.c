/*****************************************************/

/*****************************************************/
#include<reg51.h>
#include"lcd.h"
#define Kp 1	//����ϵ�� 
#define Ki 0.25 //����ϵ��
#define Kd 1	//΢��ϵ��

#define speed 1
 //Motor
sbit PH1 = P2^4;	  //��������ܽ�
sbit PH2 = P2^3;
sbit I01 = P2^2;
sbit I11 = P2^1;
sbit I02 = P2^0;
sbit I12 = P1^7;

unsigned char m,n,p,mm,nn,pp;  			   //�¶ȵĸ�λ��ʮλ��С��
unsigned char test_temp;		   //�¶ȼ춨��־
unsigned char key_set_flag;		 //�����趨�����־
unsigned char key_hold;			   //�������ֱ�־
unsigned char Change_step=1;	 //�¶����ò���
unsigned char first=1;    //�״μ��ȱ�־

int Real_temp;		//ʵ���¶�
int Set_temp;		  //�����¶�
//int Disp_temp;		//��ʾ�¶�
int last_error;		//�ϴ����
float I_term;		  //ǰ���²��

int PID_MAX;
unsigned int out,PWMT,counter;
int time;	    //�ɿع����崥��ʱ��

sbit DQ=P1^6;	//����DS18b20����
sbit PWM=P2^2;	//PWM��������
sbit heat=P1^5; //���ȼ̵�����������


/*****��ʱ�ӳ���*****/
void delay(unsigned int t)
{
	for(;t>0;t--);
}
void delay_50us(unsigned int t)
{
	unsigned char j;
	for(;t>0;t--)
		for(j=19;j>0;j--);
}

void motor_delay(int time)
{ 
  int i,j;
  
  for(j=0; j <= time; j++)
   for(i =0 ; i <= 120; i++);
}

void Go()
{	 //����������Ƴ�������˫���Ե����Ĺ���
    PH1 = 0;  
    I01 = 0;
		I11 = 0;  
	

		PH2 = 0;  
		I02 = 1;
		I12 = 1;   

		motor_delay(speed);
		
		PH1 = 0;  
		I01 = 1;  
		I11 = 1;
	
	
		PH2 = 1;  
		I02 = 0;  
		I12 = 0;

		motor_delay(speed);
		
		PH1 = 1;   
    I01 = 0;   
		I11 = 0;
	

		PH2 = 1;  
		I02 = 1;  
		I12 = 1;
	           
		motor_delay(speed);
		
		PH1 = 1;   
		I01 = 1;
		I11 = 1;
	

		PH2 = 0;  
		I02 = 0;
		I12 = 0;

		motor_delay(speed);
               
}

/*****��ʼ��DS18B20*****/
unsigned char Init_DS18B20(void)
{
  unsigned char x=0;
  DQ = 1;      //DQ��λ
  delay(8);    //������ʱ
  DQ = 0;      //��Ƭ����DQ����
  delay(80);   //��ȷ��ʱ������480us
  DQ = 1;      //��������
  delay(8);
  x = DQ;      //������ʱ��,��x=0���ʼ���ɹ�,x=1���ʼ��ʧ��
  delay(4);
  return x;
}

/*****��һ���ֽ�*****/
unsigned char ReadOneChar(void)
{
  unsigned char i=0;
  unsigned char dat = 0;
  for (i=8;i>0;i--)
  {
    DQ = 0;     // �������ź�
    dat>>=1;
    DQ = 1;     // �������ź�
    if(DQ)
    	dat|=0x80;
	delay(4);
  }
  return(dat);
}

/*****дһ���ֽ�*****/
void WriteOneChar(unsigned char dat)
{
  unsigned char i=0;
  for (i=8; i>0; i--)
  {
    DQ = 0;
    DQ = dat&0x01; 
	delay(4);
    DQ = 1;
    dat>>=1;
  }
  delay(4);
}

/*****��ȡ�¶�*****/
int ReadTemperature(void)
{
  unsigned char a=0;
  unsigned char b=0;
  unsigned int t=0;

  t=Init_DS18B20();
  if(t) return Real_temp;
  WriteOneChar(0xCC);  //����������кŵĲ���
  WriteOneChar(0x44);  //�����¶�ת��
//  delay(10);
  t=Init_DS18B20();
  if(t) return Real_temp;
  WriteOneChar(0xCC);  //����������кŵĲ���
  WriteOneChar(0xBE);  //��ȡ�¶ȼĴ���
  a=ReadOneChar();     //����8λ
  b=ReadOneChar();     //����8λ
  t=b;
  t<<=8;
  t=t|a;
  if(t<=0||t>0x900) return Real_temp;
  return(t);
}


extern void LcdInit();
extern void show1(signed int dd);
extern void show2(signed int dd);

void key_set(void)
{
	unsigned char key_value;
		
	key_value=P1;
	
	key_value&=0x07;	//��ȡ������ֵ
	
	if(key_value!=7)	//��������
	{	
		if(!key_hold)	  //������һ�ΰ���
		{	
			key_hold=1;	  //�������ְ���״̬			
			switch (key_value)
			{
				//set������
				case 6:	{	if(key_set_flag==0)		//�״ν����¶�����
							{
								key_set_flag=1;		//�����¶��趨״̬			
							}else
							{
								key_set_flag=0;	   	//����¶��趨��־
								Change_step=1;	   	//�ָ�ȱʡ�趨����ֵ
							}
							break;
						}
				//+������
				case 5:	{	if(key_set_flag) {Set_temp+=Change_step;show1(Set_temp);}
					
							break;
						}
				//-������
				case 3:	{	if(key_set_flag) {Set_temp-=Change_step;show1(Set_temp);}
							break;
						}
				default:	break;
			}

		}
		else if(key_set_flag)
		{	//����һֱ���ڰ���״̬
			key_hold++;
			if(key_hold>5) 	//�������������趨ʱ��
			{	
				Change_step=16;	//�趨������1�Ȳ���
				key_hold=0;		//��հ�������״̬,����Ӽ���������
			}
						 
		}
	}else if(key_hold)
	{	
		key_hold=0;  //�����ɿ�״̬
		Change_step=1;
	}
}

int PID(int Set_value,int Real_value) //��׼PID�¶ȿ����㷨
{
	int error;
	float P_term, D_term;
	int pid_out;

	error=Set_value - Real_value;//�����

	P_term =Kp*error;	//������
	
	I_term+=Ki*error;	//������
	if(I_term>PID_MAX) I_term=PID_MAX;	//�޶�����������
	else if(I_term<0) I_term=0;			    //�޶�����������

	D_term =Kd*(error - last_error); 	//΢����

	last_error=error;	//���浱ǰ�����

	pid_out=(signed int)(P_term+I_term+D_term);	//PID����������
	if(pid_out>PID_MAX) pid_out=PID_MAX;		//����������=PID_MAX
	else if(pid_out<0) pid_out=0;				//����������=0

	return(pid_out);	
}


void Init0(void) interrupt 0
{
	TH0=time>>8;
	TL0=time&0x00FF;
	
	TF0=0;
	ET0=1;

	counter++;
}

void T0_int(void) interrupt 1
{
	TH0=0xFF;
	TL0=0x80;

	if(PWM) 
	{	PWM=0;
		TH0=0xDA;
		TL0=0x00;		
	}
	else	PWM=1;
}


void main()
{	
	//motor=0;
	LcdInit();
	heat=1;
	PWMT=128;			//128������PWM����		
	PID_MAX=PWMT;+
	counter=0;
	out=0;
	PWM=0;

	I_term=0;
	last_error=0;

	Set_temp=45;		//��ʼ�趨�¶�45��
	Set_temp<<=4;
	show1(Set_temp);
	Real_temp=Set_temp;
//show2(Real_temp);
	
  	Init_DS18B20();
  	WriteOneChar(0xCC);	//����������кŵĲ���
  	WriteOneChar(0x44); //�����¶�ת��
	 delay_50us(15000);	  //�ȴ��¶Ȳ���

	TMOD=0x01;			//��ʱ��0ģʽ1
	TH0=0xFD;			 //��ʼ����������780us��ʱ
	TL0=0x00;			
	TR0=1;
	ET0=1;
	IT0=1;
	EX0=1; 
	EA=1;

	while(first)
	{
		heat=0;
//		motor=0;
		if(counter>=5) 
		{
			test_temp+=1;	//5*20ms=100ms ����һ���¶ȼ춨
			counter=0;
		}
		if(test_temp==20) 	//�¶ȼ춨��־��λ,�����¶�PID����
		{
			Real_temp=ReadTemperature(); //�ɼ���ǰʵ���¶�

			out=PID(Set_temp,Real_temp); //PID����
			
		time=~((9500/PID_MAX)*(PID_MAX-out)+250); //�ɿع败��ʱ�̼���
			
			test_temp=0;				 //�춨���,��ռ춨��־
		}

	 	Real_temp=ReadTemperature();	//��ȡ��ǰ�¶�	
		
		show2(Real_temp);
		out=PID(Set_temp,Real_temp); 	//PID����
		
		//��ʾ�¶�(�趨״̬ʱ��ʾ�趨�¶�,���趨״̬ʱ��ʾʵ���¶�)		
		//if(!key_set_flag) Disp_temp=Real_temp;
			
		//		display(Disp_temp);
		
		key_set();	//�����¶�����

		if(Real_temp>=961)//1550-96.8
		{
			first=0;
			heat=1;
			Go();
		}
		
	}
	
	while(1)
	{	if(counter>=5) 
		{
			test_temp=1;	//5*20ms=100ms ����һ���¶ȼ춨
			counter=0;
		}
		if(test_temp) 	//�¶ȼ춨��־��λ,�����¶�PID����
		{
			Real_temp=ReadTemperature(); //�ɼ���ǰʵ���¶�
			out=PID(Set_temp,Real_temp); //PID����
			
		time=~((9500/PID_MAX)*(PID_MAX-out)+250); //�ɿع败��ʱ�̼���
			
			test_temp=0;				 //�춨���,��ռ춨��־
		}

	 	Real_temp=ReadTemperature();	//��ȡ��ǰ�¶�	
		show2(Real_temp);
		out=PID(Set_temp,Real_temp); 	//PID����
		
		//��ʾ�¶�(�趨״̬ʱ��ʾ�趨�¶�,���趨״̬ʱ��ʾʵ���¶�)		

		
		key_set();	//�����¶�����

		if(Real_temp <(Set_temp))
		{
				heat=0;
		}
		else
		{
				heat=1;
				if(Real_temp > (Set_temp+50))
				{
					Go();
				}

	}
}
}