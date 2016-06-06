/*****************************************************/

/*****************************************************/
#include<reg51.h>
#include"lcd.h"
#define Kp 1	//比例系数 
#define Ki 0.25 //积分系数
#define Kd 1	//微分系数

#define speed 1
 //Motor
sbit PH1 = P2^4;	  //步进电机管脚
sbit PH2 = P2^3;
sbit I01 = P2^2;
sbit I11 = P2^1;
sbit I02 = P2^0;
sbit I12 = P1^7;

unsigned char m,n,p,mm,nn,pp;  			   //温度的个位、十位、小数
unsigned char test_temp;		   //温度检定标志
unsigned char key_set_flag;		 //按键设定进入标志
unsigned char key_hold;			   //按键保持标志
unsigned char Change_step=1;	 //温度设置步进
unsigned char first=1;    //首次加热标志

int Real_temp;		//实际温度
int Set_temp;		  //设置温度
//int Disp_temp;		//显示温度
int last_error;		//上次误差
float I_term;		  //前面温差和

int PID_MAX;
unsigned int out,PWMT,counter;
int time;	    //可控硅脉冲触发时刻

sbit DQ=P1^6;	//定义DS18b20引脚
sbit PWM=P2^2;	//PWM控制引脚
sbit heat=P1^5; //加热继电器控制引脚


/*****延时子程序*****/
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
{	 //步进电机控制程序四线双极性单四拍工作
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

/*****初始化DS18B20*****/
unsigned char Init_DS18B20(void)
{
  unsigned char x=0;
  DQ = 1;      //DQ复位
  delay(8);    //稍作延时
  DQ = 0;      //单片机将DQ拉低
  delay(80);   //精确延时，大于480us
  DQ = 1;      //拉高总线
  delay(8);
  x = DQ;      //稍作延时后,若x=0则初始化成功,x=1则初始化失败
  delay(4);
  return x;
}

/*****读一个字节*****/
unsigned char ReadOneChar(void)
{
  unsigned char i=0;
  unsigned char dat = 0;
  for (i=8;i>0;i--)
  {
    DQ = 0;     // 给脉冲信号
    dat>>=1;
    DQ = 1;     // 给脉冲信号
    if(DQ)
    	dat|=0x80;
	delay(4);
  }
  return(dat);
}

/*****写一个字节*****/
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

/*****读取温度*****/
int ReadTemperature(void)
{
  unsigned char a=0;
  unsigned char b=0;
  unsigned int t=0;

  t=Init_DS18B20();
  if(t) return Real_temp;
  WriteOneChar(0xCC);  //跳过读序号列号的操作
  WriteOneChar(0x44);  //启动温度转换
//  delay(10);
  t=Init_DS18B20();
  if(t) return Real_temp;
  WriteOneChar(0xCC);  //跳过读序号列号的操作
  WriteOneChar(0xBE);  //读取温度寄存器
  a=ReadOneChar();     //读低8位
  b=ReadOneChar();     //读高8位
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
	
	key_value&=0x07;	//提取按键键值
	
	if(key_value!=7)	//按键按下
	{	
		if(!key_hold)	  //按键第一次按下
		{	
			key_hold=1;	  //按键保持按下状态			
			switch (key_value)
			{
				//set键按下
				case 6:	{	if(key_set_flag==0)		//首次进入温度设置
							{
								key_set_flag=1;		//进入温度设定状态			
							}else
							{
								key_set_flag=0;	   	//清空温度设定标志
								Change_step=1;	   	//恢复缺省设定步进值
							}
							break;
						}
				//+键按下
				case 5:	{	if(key_set_flag) {Set_temp+=Change_step;show1(Set_temp);}
					
							break;
						}
				//-键按下
				case 3:	{	if(key_set_flag) {Set_temp-=Change_step;show1(Set_temp);}
							break;
						}
				default:	break;
			}

		}
		else if(key_set_flag)
		{	//按键一直处于按下状态
			key_hold++;
			if(key_hold>5) 	//按键计数超过设定时长
			{	
				Change_step=16;	//设定步长按1度步进
				key_hold=0;		//清空按键保持状态,进入加减按键功能
			}
						 
		}
	}else if(key_hold)
	{	
		key_hold=0;  //按键松开状态
		Change_step=1;
	}
}

int PID(int Set_value,int Real_value) //标准PID温度控制算法
{
	int error;
	float P_term, D_term;
	int pid_out;

	error=Set_value - Real_value;//误差量

	P_term =Kp*error;	//比例量
	
	I_term+=Ki*error;	//积分量
	if(I_term>PID_MAX) I_term=PID_MAX;	//限定积分量上限
	else if(I_term<0) I_term=0;			    //限定积分量下限

	D_term =Kd*(error - last_error); 	//微分量

	last_error=error;	//缓存当前误差量

	pid_out=(signed int)(P_term+I_term+D_term);	//PID控制量计算
	if(pid_out>PID_MAX) pid_out=PID_MAX;		//控制量上限=PID_MAX
	else if(pid_out<0) pid_out=0;				//控制量下限=0

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
	PWMT=128;			//128级步进PWM控制		
	PID_MAX=PWMT;+
	counter=0;
	out=0;
	PWM=0;

	I_term=0;
	last_error=0;

	Set_temp=45;		//初始设定温度45度
	Set_temp<<=4;
	show1(Set_temp);
	Real_temp=Set_temp;
//show2(Real_temp);
	
  	Init_DS18B20();
  	WriteOneChar(0xCC);	//跳过读序号列号的操作
  	WriteOneChar(0x44); //启动温度转换
	 delay_50us(15000);	  //等待温度测量

	TMOD=0x01;			//定时器0模式1
	TH0=0xFD;			 //初始计数字用于780us定时
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
			test_temp+=1;	//5*20ms=100ms 进行一次温度检定
			counter=0;
		}
		if(test_temp==20) 	//温度检定标志置位,进入温度PID调节
		{
			Real_temp=ReadTemperature(); //采集当前实际温度

			out=PID(Set_temp,Real_temp); //PID程序
			
		time=~((9500/PID_MAX)*(PID_MAX-out)+250); //可控硅触发时刻计数
			
			test_temp=0;				 //检定完成,清空检定标志
		}

	 	Real_temp=ReadTemperature();	//读取当前温度	
		
		show2(Real_temp);
		out=PID(Set_temp,Real_temp); 	//PID程序
		
		//显示温度(设定状态时显示设定温度,非设定状态时显示实际温度)		
		//if(!key_set_flag) Disp_temp=Real_temp;
			
		//		display(Disp_temp);
		
		key_set();	//按键温度设置

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
			test_temp=1;	//5*20ms=100ms 进行一次温度检定
			counter=0;
		}
		if(test_temp) 	//温度检定标志置位,进入温度PID调节
		{
			Real_temp=ReadTemperature(); //采集当前实际温度
			out=PID(Set_temp,Real_temp); //PID程序
			
		time=~((9500/PID_MAX)*(PID_MAX-out)+250); //可控硅触发时刻计数
			
			test_temp=0;				 //检定完成,清空检定标志
		}

	 	Real_temp=ReadTemperature();	//读取当前温度	
		show2(Real_temp);
		out=PID(Set_temp,Real_temp); 	//PID程序
		
		//显示温度(设定状态时显示设定温度,非设定状态时显示实际温度)		

		
		key_set();	//按键温度设置

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