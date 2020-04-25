#include "dma.h"
#include "usart.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32F746������
//DMA��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2015/12/28
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	

DMA_HandleTypeDef   hdma_adcx;      //DMA���

//DMAx�ĸ�ͨ������
//����Ĵ�����ʽ�ǹ̶���,���Ҫ���ݲ�ͬ��������޸�
//�Ӵ洢��->����ģʽ/8λ���ݿ��/�洢������ģʽ
//DMA_Streamx:DMA������,DMA1_Stream0~7/DMA2_Stream0~7
//chx:DMAͨ��ѡ��,@ref DMA_channel DMA_CHANNEL_0~DMA_CHANNEL_7
//ADC_DMA��ʼ��
void DMA_Config(void)
{
	__HAL_RCC_DMA2_CLK_ENABLE(); //����DMAʱ��  

	hdma_adcx.Instance = DMA2_Stream0; //������ѡ��
	hdma_adcx.Init.Channel = DMA_CHANNEL_0; //ͨ��ѡ��
	hdma_adcx.Init.Direction = DMA_PERIPH_TO_MEMORY; //���赽�洢��
	hdma_adcx.Init.PeriphInc = DMA_PINC_DISABLE; //���������ģʽ
	hdma_adcx.Init.MemInc = DMA_MINC_ENABLE; //�洢������ģʽ
	hdma_adcx.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD; //�������ݳ���:32λ
	hdma_adcx.Init.MemDataAlignment = DMA_MDATAALIGN_WORD; //�洢�����ݳ���:32λ
	hdma_adcx.Init.Mode = DMA_CIRCULAR; //ѭ��ģʽ(��ΪADC��ѭ��ɨ��ģʽ)
	hdma_adcx.Init.Priority = DMA_PRIORITY_MEDIUM; //�е����ȼ�
	hdma_adcx.Init.FIFOMode = DMA_FIFOMODE_DISABLE; //�ر�FIFO             
	hdma_adcx.Init.FIFOThreshold = DMA_FIFO_THRESHOLD_FULL;      
	hdma_adcx.Init.MemBurst = DMA_MBURST_SINGLE; 
	hdma_adcx.Init.PeriphBurst = DMA_PBURST_SINGLE; 
	
	HAL_DMA_DeInit(&hdma_adcx); //�ȸ�λ
	HAL_DMA_Init(&hdma_adcx); //�ٳ�ʼ��

	__HAL_LINKDMA(&ADC1_Handler, DMA_Handle, hdma_adcx); //����DMA��ADC����ľ��
 
	HAL_NVIC_SetPriority(DMA2_Stream0_IRQn, 0, 0); //����ADC_DMA�ж����ȼ�
	HAL_NVIC_EnableIRQ(DMA2_Stream0_IRQn); //ʹ��ADC_DMA�ж�
}


 
 
