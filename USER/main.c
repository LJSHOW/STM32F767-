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
 ALIENTEK ������STM32F7������
 ������-IOT_DHT11��ʪ�ȼ��ʵ�� 
 ����֧�֣�www.openedv.com
 �Ա����̣�http://eboard.taobao.com 
 ��ע΢�Ź���ƽ̨΢�źţ�"����ԭ��"����ѻ�ȡSTM32���ϡ�
 ������������ӿƼ����޹�˾  
 ���ߣ�����ԭ�� @ALIENTEK
************************************************/

/* �û�����ǰ�豸״̬�ṹ��*/
dataPoint_t currentDataPoint;


//WIFI����״̬
//wifi_sta 0: �Ͽ�
//         1: ������
u8 wifi_sta=0;

//GizwitsЭ���ʼ��
void Gizwits_Init(void)
{
	TIM3_Init(10-1,10800-1); //1MSϵͳ��ʱ 
	uart3_init(9600);//WIFI��ʼ�� 
	memset((uint8_t*)&currentDataPoint, 0, sizeof(dataPoint_t));//�豸״̬�ṹ���ʼ��
	gizwitsInit();//��������ʼ��
	
}

	#define Temp_alarm  32   //�¶����ޣ���λ��C��
	#define Humi_alarm  70   //ʪ�����ޣ���λ��%��
  #define CO_alarm  150   //һ����̼���ޣ���λ��%��
  #define CO2_alarm 700   //������̼���ޣ���λ��%��
  #define PM_alarm  250   //PM2.5���ޣ���λ��%��

	#define alarm_num   3    //��������
	#define         REACTION_VOLTGAE             (0.020) //define the voltage drop of the sensor when move the sensor from air into 1000ppm CO2
	float           CO2Curve[3]  =  {2.602,ZERO_POINT_VOLTAGE,(REACTION_VOLTGAE/(2.602-3))};   
																											 //two points are taken from the curve. 
																											 //with these two points, a line is formed which is
																											 //"approximately equivalent" to the original curve.
																											 //data format:{ x, y, slope}; point1: (lg400, 0.324), point2: (lg4000, 0.280) 
																											 //slope = ( reaction voltage ) / (log400 �log1000) 

//���ݲɼ�
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
			adcx=Get_Adc_Average(ADC_CHANNEL_5,20);//��ȡͨ��5��ת��ֵ��20��ȡƽ��			
			temp=(float)adcx*(3.3/4096);          //��ȡ�����Ĵ�С����ʵ�ʵ�ѹֵ������3.1111
			MQ7_VALUE=MQ7_PPM_GET(temp);
			
			 
/****************************************************************************************************/			 
			adcx=Get_Adc_Average(ADC_CHANNEL_6,20);//��ȡͨ��6��ת��ֵ��20��ȡƽ��
			temp=(float)adcx*(3.3/4096);          //��ȡ�����Ĵ�С����ʵ�ʵ�ѹֵ������3.1111
			CO2_VALUE = MGGetPercentage(temp,CO2Curve);
		  printf("CO2_VALUE:%.4f\n\t",CO2_VALUE);
/****************************************************************************************************/			 
			adcx=Get_Adc_Average(ADC_CHANNEL_7,20);//��ȡͨ��7��ת��ֵ��20��ȡƽ��
			temp=(float)adcx*(3.3/4096);          //��ȡ�����Ĵ�С����ʵ�ʵ�ѹֵ������3.1111
			PM_VALUE= pm_valueget(temp);		
/****************************************************************************************************/	
	    LCD_ShowxNum(20+70+30,70+20+60+60,MQ7_VALUE,4,16,0);    //��ʾһ����̼Ũ��
			LCD_ShowxNum(20+70+30,70+20+80+60,PM_VALUE,4,16,0);    //��ʾPM2.5Ũ��
		
/****************************************************************************************************/			 
			if(CO2_VALUE== -1)
				{	
					LCD_ShowString(20+70+30,70+20+40+60,200,16,16," 400");
					currentDataPoint.valueMG811 = 400;
				}
				else if (CO2_VALUE!= -1)
					{	
						LCD_ShowxNum(20+70+30,70+20+40+60,CO2_VALUE,4,16,0);    //��ʾCO2Ũ��
						currentDataPoint.valueMG811 = CO2_VALUE ;
					}					
			currentDataPoint.valueGP2Y1014AU = PM_VALUE;	
			currentDataPoint.valueMQ_7 = MQ7_VALUE;
/****************************************************************************************************/			 					
			 if(CO2_VALUE>=CO_alarm)//ʪ��>����ֵ 
			 {
				  CO2_num++;
				 if(CO2_VALUE>=alarm_num)//�����ﵽһ������
				 {
					 CO2_num=0;
					 currentDataPoint.valueMG811_alarm=1;//ʪ�ȴ����ޱ���
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
		if(MQ7_VALUE>=CO_alarm)//ʪ��>����ֵ 
			 {
				 CO_num++;
				 if(CO_num>=alarm_num)//�����ﵽһ������
				 {
					 CO_num=0;
					 currentDataPoint.valueMQ_7_alarm=1;//ʪ�ȴ����ޱ���
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
	if(PM_VALUE>=PM_alarm)//ʪ��>����ֵ 
			 {
				  PM_num++;
				 if(PM_VALUE>=alarm_num)//�����ﵽһ������
				 {
					 PM_num=0;
					 currentDataPoint.valueGP2Y1014AU_ALARM=1;//ʪ�ȴ����ޱ���
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
    fan = HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_3);//�жϵ�ǰ���������
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
				 		
		 if(t==10)//ÿ2S��ȡһ��
		 {
			 t=0;
			 PCF8574_ReadBit(BEEP_IO);   //��ȡһ��PCF8574������һ��IO��ʹ���ͷŵ�PB12���ţ�
                                        //�����ȡDHT11���ܻ������ 
			DHT11_Read_Data(&temp1,&hum1);//��ȡDHT11������	
	 
			 
			LCD_ShowxNum(20+70+15,50+20+60,temp1,3,16,0); 
			LCD_ShowxNum(20+70+15,70+20+60,hum1,3,16,0); 
		
			currentDataPoint.valuetemperature = temp1 ;//�¶�����
			currentDataPoint.valuehumidity = hum1;//ʪ������
		
			 if(temp1>=Temp_alarm)//�¶�>����ֵ 
			 {  
				 temp_num++;
				 if(temp_num>=alarm_num)//�����ﵽһ������
				 {
					 temp_num=0;
					 currentDataPoint.valuetemperature_alarm=1;//�¶ȴ����ޱ���
				     LCD_Fill(20+70+60,50+20+60,20+70+80,70+15+60,RED);//(sx,sy),(ex,ey):�����ζԽ�����,�����СΪ:(ex-sx+1)*(ey-sy+1)   
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
			 if(hum1>=Humi_alarm)//ʪ��>����ֵ 
			 {
				 humi_num++;
				 if(humi_num>=alarm_num)//�����ﵽһ������
				 {
					 humi_num=0;
					 currentDataPoint.valuehumidity_alarm=1;//ʪ�ȴ����ޱ���
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
//������
int main(void)
{   
	u8 key;
	
	u8 wifi_con=0;//��¼wifi����״̬ 1:���� 0:�Ͽ�
	u8 buff[20]={0};
    Cache_Enable();                 //��L1-Cache
    HAL_Init();				        //��ʼ��HAL��
    Stm32_Clock_Init(432,25,2,9);   //����ʱ��,216Mhz 
    delay_init(216);                //��ʱ��ʼ��
	uart_init(115200);		        //���ڳ�ʼ��
    LED_Init();                     //��ʼ��LED
	  Fan_Init();
    KEY_Init();                     //��ʼ������
	  MY_ADC_Init();                  //��ʼ��ADC1ͨ��5
	SDRAM_Init();                   //SDRAM��ʼ��
	LCD_Init();			   	        //��ʼ��LCD 
	PCF8574_Init();                 //��ʼ��PCF8574
	Gizwits_Init();                 //Э���ʼ��
	POINT_COLOR=RED;		        //��������Ϊ��ɫ 
	
	POINT_COLOR=BLUE;		        //��������Ϊ��ɫ 
    PCF8574_ReadBit(BEEP_IO);       //����DHT11��PCF8574���ж����Ź���һ��IO��
								    //�����ڳ�ʼ��DHT11֮ǰҪ�ȶ�ȡһ��PCF8574������һ��IO��
								    //ʹ���ͷŵ��ж�������ռ�õ�IO(PB12����),�����ʼ��DHT11�������  
	while(DHT11_Init())	//DHT11��ʼ��	
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
	POINT_COLOR=RED;		//��������Ϊ��ɫ
	LCD_ShowString(65,50+40+20+60,200,16,16,"wifi: close   ");
	POINT_COLOR=BLUE;		//��������Ϊ��ɫ
	LCD_ShowString(65,50+20+60,200,16,16,"Temp:   C");
  LCD_ShowString(65,70+20+60,200,16,16,"Humi:  0%");	
	LCD_ShowString(65,70+20+40+60,200,16,16,"CO2:        PPM");	
  LCD_ShowString(65,70+20+60+60,200,16,16,"CO:         PPM");	
  LCD_ShowString(65,70+20+80+60,200,16,16,"PM2.5:      g/m");	
  LCD_ShowString(170+16,70+20+76+60,200,30,16,"3");	
  LCD_ShowString(65,70+20+100+60,200,16,16,"FAN:");
	printf("--------������IOT-DHT11��ʪ�ȼ�ⱨ��ʵ��----------\r\n");
	printf("KEY1:AirLink����ģʽ\t KEY_UP:��λ\r\n\r\n");                                        
    while(1)
    {
	
	
       if(wifi_con!=wifi_sta)
	   {
		   wifi_con=wifi_sta;
		   POINT_COLOR=RED;		//��������Ϊ��ɫ
		   wifi_con?LCD_ShowString(65+40,50+40+20+60,200,16,16,"connect"):LCD_ShowString(65,50+40+20+60,200,16,16,"close   ");
		   POINT_COLOR=BLUE;	//��������Ϊ��ɫ
	   }
	   userHandle();//�û��ɼ�
         
       gizwitsHandle((dataPoint_t *)&currentDataPoint);//Э�鴦��
 		
	    key = KEY_Scan(0);
		if(key==KEY1_PRES)//KEY1����
		{
			printf("WIFI����AirLink����ģʽ\r\n");
			gizwitsSetMode(WIFI_AIRLINK_MODE);//Air-linkģʽ����
		}			
		if(key==WKUP_PRES)//KEY_UP����
		{  
			printf("WIFI��λ����������������\r\n");
			gizwitsSetMode(WIFI_RESET_MODE);//WIFI��λ
			LCD_Fill(120,150+60+10,140,165+60+10,WHITE);
			LCD_Fill(120,175+60+10,140,190+60+10,WHITE);
			POINT_COLOR=BLUE;		//��������Ϊ��ɫ 
			LCD_ShowxNum(70,150+60+10,0,3,16,0); 
			LCD_ShowxNum(70,175+60+10,0,3,16,0); 
			wifi_sta=0;//��־wifi�ѶϿ�
			LED0(1);
		}
		delay_ms(200);
		LED1_Toggle;
	}  
}
