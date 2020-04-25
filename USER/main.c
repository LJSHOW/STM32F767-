#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "fan.h"
#include "timer.h"
#include "key.h"
#include "usart3.h"
#include "lcd.h"
#include "sdram.h"
#include "dht11.h"
#include "pcf8574.h"
#include "co.h"
#include "co2.h"
#include "pm.h"
#include "adc.h"
#include "gizwits_product.h" 

/************************************************
 ALIENTEK 阿波罗STM32F7开发板
 机智云-IOT_DHT11温湿度检测实验 
 技术支持：www.openedv.com
 淘宝店铺：http://eboard.taobao.com 
 关注微信公众平台微信号："正点原子"，免费获取STM32资料。
 广州市星翼电子科技有限公司  
 作者：正点原子 @ALIENTEK
************************************************/

/* 用户区当前设备状态结构体*/
dataPoint_t currentDataPoint;


//WIFI连接状态
//wifi_sta 0: 断开
//         1: 已连接
u8 wifi_sta=0;

//Gizwits协议初始化
void Gizwits_Init(void)
{
	TIM3_Init(10-1,10800-1); //1MS系统定时 
	uart3_init(9600);//WIFI初始化 
	memset((uint8_t*)&currentDataPoint, 0, sizeof(dataPoint_t));//设备状态结构体初始化
	gizwitsInit();//缓冲区初始化
	
}

	#define Temp_alarm  32   //温度上限（单位：C）
	#define Humi_alarm  70   //湿度上限（单位：%）
  #define CO_alarm  150   //一氧化碳上限（单位：%）
  #define CO2_alarm 700   //二氧化碳上限（单位：%）
  #define PM_alarm  250   //PM2.5上限（单位：%）

	#define alarm_num   3    //报警次数
	#define         REACTION_VOLTGAE             (0.020) //define the voltage drop of the sensor when move the sensor from air into 1000ppm CO2
	float           CO2Curve[3]  =  {2.602,ZERO_POINT_VOLTAGE,(REACTION_VOLTGAE/(2.602-3))};   
																											 //two points are taken from the curve. 
																											 //with these two points, a line is formed which is
																											 //"approximately equivalent" to the original curve.
																											 //data format:{ x, y, slope}; point1: (lg400, 0.324), point2: (lg4000, 0.280) 
																											 //slope = ( reaction voltage ) / (log400 杔og1000) 

//数据采集
void userHandle(void)
{
  static u8 t=0,fan;
	static u8 temp1,hum1;
	static float temp,MQ7_VALUE,CO2_VALUE,PM_VALUE;
	static u16 adcx;
  static u8 temp_num=0;
	static u8 humi_num=0;
	static u8 CO_num=0;
	static u8 CO2_num=0;
	static u8 PM_num=0;
  
	if(wifi_sta)
	{
		
		
/****************************************************************************************************/			 
			adcx=Get_Adc_Average(ADC_CHANNEL_5,20);//获取通道5的转换值，20次取平均			
			temp=(float)adcx*(3.3/4096);          //获取计算后的带小数的实际电压值，比如3.1111
			MQ7_VALUE=MQ7_PPM_GET(temp);
			
			 
/****************************************************************************************************/			 
			adcx=Get_Adc_Average(ADC_CHANNEL_6,20);//获取通道6的转换值，20次取平均
			temp=(float)adcx*(3.3/4096);          //获取计算后的带小数的实际电压值，比如3.1111
			CO2_VALUE = MGGetPercentage(temp,CO2Curve);
		  printf("CO2_VALUE:%.4f\n\t",CO2_VALUE);
/****************************************************************************************************/			 
			adcx=Get_Adc_Average(ADC_CHANNEL_7,20);//获取通道7的转换值，20次取平均
			temp=(float)adcx*(3.3/4096);          //获取计算后的带小数的实际电压值，比如3.1111
			PM_VALUE= pm_valueget(temp);		
/****************************************************************************************************/	
	    LCD_ShowxNum(20+70+30,70+20+60+60,MQ7_VALUE,4,16,0);    //显示一氧化碳浓度
			LCD_ShowxNum(20+70+30,70+20+80+60,PM_VALUE,4,16,0);    //显示PM2.5浓度
		
/****************************************************************************************************/			 
			if(CO2_VALUE== -1)
				{	
					LCD_ShowString(20+70+30,70+20+40+60,200,16,16," 400");
					currentDataPoint.valueMG811 = 400;
				}
				else if (CO2_VALUE!= -1)
					{	
						LCD_ShowxNum(20+70+30,70+20+40+60,CO2_VALUE,4,16,0);    //显示CO2浓度
						currentDataPoint.valueMG811 = CO2_VALUE ;
					}					
			currentDataPoint.valueGP2Y1014AU = PM_VALUE;	
			currentDataPoint.valueMQ_7 = MQ7_VALUE;
/****************************************************************************************************/			 					
			 if(CO2_VALUE>=CO_alarm)//湿度>报警值 
			 {
				  CO2_num++;
				 if(CO2_VALUE>=alarm_num)//报警达到一定次数
				 {
					 CO2_num=0;
					 currentDataPoint.valueMG811_alarm=1;//湿度达上限报警
				     LCD_Fill(20+70+30+70,50+20+20+60+40,20+70+50+70,70+15+20+60+40,RED);
				 }else
				 { 
					 if(currentDataPoint.valueMG811_alarm==0)
					   LCD_Fill(20+70+30+70,50+20+20+60+40,20+70+50+70,70+15+20+60+40,GREEN);
				 }
				 
			 }else  
			 {
				 currentDataPoint.valueMG811_alarm=0;
				 LCD_Fill(20+70+30+70,50+20+20+60+40,20+70+50+70,70+15+20+60+40,GREEN);
			 }
/****************************************************************************************************/			 			 
		if(MQ7_VALUE>=CO_alarm)//湿度>报警值 
			 {
				 CO_num++;
				 if(CO_num>=alarm_num)//报警达到一定次数
				 {
					 CO_num=0;
					 currentDataPoint.valueMQ_7_alarm=1;//湿度达上限报警
				     LCD_Fill(20+70+30+70,50+20+20+60+40+20,20+70+50+70,70+15+20+60+40+20,RED);
				 }else
				 { 
					 if(currentDataPoint.valueMQ_7_alarm==0)
					   LCD_Fill(20+70+30+70,50+20+20+60+40+20,20+70+50+70,70+15+20+60+40+20,GREEN);
				 }
				 
			 }else  
			 {
				 currentDataPoint.valueMQ_7_alarm=0;
				 LCD_Fill(20+70+30+70,50+20+20+60+40+20,20+70+50+70,70+15+20+60+40+20,GREEN);
			 }
/****************************************************************************************************/			 			 
	if(PM_VALUE>=PM_alarm)//湿度>报警值 
			 {
				  PM_num++;
				 if(PM_VALUE>=alarm_num)//报警达到一定次数
				 {
					 PM_num=0;
					 currentDataPoint.valueGP2Y1014AU_ALARM=1;//湿度达上限报警
				     LCD_Fill(20+70+30+70+10,50+20+20+60+40+20+20,20+70+50+70+10,70+15+20+60+40+20+20,RED);
				 }else
				 { 
					 if(currentDataPoint.valueGP2Y1014AU_ALARM==0)
					   LCD_Fill(20+70+30+70+10,50+20+20+60+40+20+20,20+70+50+70+10,70+15+20+60+40+20+20,GREEN);
				 }
				 
			 }else  
			 {
				 currentDataPoint.valueGP2Y1014AU_ALARM=0;
				 LCD_Fill(20+70+30+70+10,50+20+20+60+40+20+20,20+70+50+70+10,70+15+20+60+40+20+20,GREEN);
			 }
    fan = HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_3);//判断当前电机开关量
	if(fan==1)
	{
	   currentDataPoint.valuefan= 1;
	 LCD_ShowString(20+70+30,70+20+100+60,200,16,16," ON ");
	}
	else if(fan!=1)
	{
	   currentDataPoint.valuefan= 0;
		 LCD_ShowString(20+70+30,70+20+100+60,200,16,16," OFF  ");
	}  
				 		
		 if(t==10)//每2S读取一次
		 {
			 t=0;
			 PCF8574_ReadBit(BEEP_IO);   //读取一次PCF8574的任意一个IO，使其释放掉PB12引脚，
                                        //否则读取DHT11可能会出问题 
			DHT11_Read_Data(&temp1,&hum1);//读取DHT11传感器	
	 
			 
			LCD_ShowxNum(20+70+15,50+20+60,temp1,3,16,0); 
			LCD_ShowxNum(20+70+15,70+20+60,hum1,3,16,0); 
		
			currentDataPoint.valuetemperature = temp1 ;//温度数据
			currentDataPoint.valuehumidity = hum1;//湿度数据
		
			 if(temp1>=Temp_alarm)//温度>报警值 
			 {  
				 temp_num++;
				 if(temp_num>=alarm_num)//报警达到一定次数
				 {
					 temp_num=0;
					 currentDataPoint.valuetemperature_alarm=1;//温度达上限报警
				     LCD_Fill(20+70+60,50+20+60,20+70+80,70+15+60,RED);//(sx,sy),(ex,ey):填充矩形对角坐标,区域大小为:(ex-sx+1)*(ey-sy+1)   
				 }else
                 {
					 if(currentDataPoint.valuetemperature_alarm==0)
					  LCD_Fill(20+70+60,50+20+60,20+70+80,70+15+60,GREEN);
				 }	  
				 
			 }else  
			 {
				 currentDataPoint.valuetemperature_alarm=0;
				 LCD_Fill(20+70+60,50+20+60,20+70+80,70+15+60,GREEN);
				 
			 }
			 if(hum1>=Humi_alarm)//湿度>报警值 
			 {
				 humi_num++;
				 if(humi_num>=alarm_num)//报警达到一定次数
				 {
					 humi_num=0;
					 currentDataPoint.valuehumidity_alarm=1;//湿度达上限报警
				     LCD_Fill(20+70+60,50+20+20+60,20+70+80,70+15+20+60,RED);
				 }else
				 { 
					 if(currentDataPoint.valuehumidity_alarm==0)
					   LCD_Fill(20+70+60,50+20+20+60,20+70+80,70+15+20+60,GREEN);
				 }
				 
			 }else  
			 {
				 currentDataPoint.valuehumidity_alarm=0;
				 LCD_Fill(20+70+60,50+20+20+60,20+70+80,70+15+20+60,GREEN);
			 }
		 }
		 if(t%2)  LED0_Toggle;
		 
		  t++;
	}
	else
	{
		if(temp_num!=0||humi_num!=0) 
		{
			temp_num=0;humi_num=0;
		}
	}
  
}
//主函数
int main(void)
{   
	u8 key;
	
	u8 wifi_con=0;//记录wifi连接状态 1:连接 0:断开
	u8 buff[20]={0};
    Cache_Enable();                 //打开L1-Cache
    HAL_Init();				        //初始化HAL库
    Stm32_Clock_Init(432,25,2,9);   //设置时钟,216Mhz 
    delay_init(216);                //延时初始化
	uart_init(115200);		        //串口初始化
    LED_Init();                     //初始化LED
	  Fan_Init();
    KEY_Init();                     //初始化按键
	  MY_ADC_Init();                  //初始化ADC1通道5
	SDRAM_Init();                   //SDRAM初始化
	LCD_Init();			   	        //初始化LCD 
	PCF8574_Init();                 //初始化PCF8574
	Gizwits_Init();                 //协议初始化
	POINT_COLOR=RED;		        //设置字体为红色 
	
	POINT_COLOR=BLUE;		        //设置字体为蓝色 
    PCF8574_ReadBit(BEEP_IO);       //由于DHT11和PCF8574的中断引脚共用一个IO，
								    //所以在初始化DHT11之前要先读取一次PCF8574的任意一个IO，
								    //使其释放掉中断引脚所占用的IO(PB12引脚),否则初始化DHT11会出问题  
	while(DHT11_Init())	//DHT11初始化	
	{
		LCD_ShowString(65,50+60,200,16,16,"DHT11 Error");
		delay_ms(200);
		LCD_Fill(65,50,239,130+16,WHITE);
 		delay_ms(200);
	}
	LCD_ShowString(65,50+60,200,16,16,"DHT11 OK");
    sprintf((char*)buff,"Temp alarm: %dC",Temp_alarm);
	LCD_ShowString(60,10,200,16,16,buff);
	memset(buff,0x00,20);
	sprintf((char*)buff,"Humi alarm: %d%%",Humi_alarm);
	LCD_ShowString(60,30,200,16,16,buff);
  sprintf((char*)buff,"CO alarm:  %dPPM",CO_alarm);
	LCD_ShowString(60,50,200,16,16,buff);
	sprintf((char*)buff,"CO2 alarm: %dPPM",CO2_alarm);
	LCD_ShowString(60,70,200,16,16,buff);
	sprintf((char*)buff,"PM2.5 alarm: %dPPM",PM_alarm);
	LCD_ShowString(45,90,200,16,16,buff);
	POINT_COLOR=RED;		//设置字体为红色
	LCD_ShowString(65,50+40+20+60,200,16,16,"wifi: close   ");
	POINT_COLOR=BLUE;		//设置字体为蓝色
	LCD_ShowString(65,50+20+60,200,16,16,"Temp:   C");
  LCD_ShowString(65,70+20+60,200,16,16,"Humi:  0%");	
	LCD_ShowString(65,70+20+40+60,200,16,16,"CO2:        PPM");	
  LCD_ShowString(65,70+20+60+60,200,16,16,"CO:         PPM");	
  LCD_ShowString(65,70+20+80+60,200,16,16,"PM2.5:      g/m");	
  LCD_ShowString(170+16,70+20+76+60,200,30,16,"3");	
  LCD_ShowString(65,70+20+100+60,200,16,16,"FAN:");
	printf("--------机智云IOT-DHT11温湿度检测报警实验----------\r\n");
	printf("KEY1:AirLink连接模式\t KEY_UP:复位\r\n\r\n");                                        
    while(1)
    {
	
	
       if(wifi_con!=wifi_sta)
	   {
		   wifi_con=wifi_sta;
		   POINT_COLOR=RED;		//设置字体为红色
		   wifi_con?LCD_ShowString(65+40,50+40+20+60,200,16,16,"connect"):LCD_ShowString(65,50+40+20+60,200,16,16,"close   ");
		   POINT_COLOR=BLUE;	//设置字体为蓝色
	   }
	   userHandle();//用户采集
         
       gizwitsHandle((dataPoint_t *)&currentDataPoint);//协议处理
 		
	    key = KEY_Scan(0);
		if(key==KEY1_PRES)//KEY1按键
		{
			printf("WIFI进入AirLink连接模式\r\n");
			gizwitsSetMode(WIFI_AIRLINK_MODE);//Air-link模式接入
		}			
		if(key==WKUP_PRES)//KEY_UP按键
		{  
			printf("WIFI复位，请重新配置连接\r\n");
			gizwitsSetMode(WIFI_RESET_MODE);//WIFI复位
			LCD_Fill(120,150+60+10,140,165+60+10,WHITE);
			LCD_Fill(120,175+60+10,140,190+60+10,WHITE);
			POINT_COLOR=BLUE;		//设置字体为蓝色 
			LCD_ShowxNum(70,150+60+10,0,3,16,0); 
			LCD_ShowxNum(70,175+60+10,0,3,16,0); 
			wifi_sta=0;//标志wifi已断开
			LED0(1);
		}
		delay_ms(200);
		LED1_Toggle;
	}  
}
