#include "dma.h"
#include "usart.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32F746开发板
//DMA驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2015/12/28
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	

DMA_HandleTypeDef   hdma_adcx;      //DMA句柄

//DMAx的各通道配置
//这里的传输形式是固定的,这点要根据不同的情况来修改
//从存储器->外设模式/8位数据宽度/存储器增量模式
//DMA_Streamx:DMA数据流,DMA1_Stream0~7/DMA2_Stream0~7
//chx:DMA通道选择,@ref DMA_channel DMA_CHANNEL_0~DMA_CHANNEL_7
//ADC_DMA初始化
void DMA_Config(void)
{
	__HAL_RCC_DMA2_CLK_ENABLE(); //开启DMA时钟  

	hdma_adcx.Instance = DMA2_Stream0; //数据流选择
	hdma_adcx.Init.Channel = DMA_CHANNEL_0; //通道选择
	hdma_adcx.Init.Direction = DMA_PERIPH_TO_MEMORY; //外设到存储器
	hdma_adcx.Init.PeriphInc = DMA_PINC_DISABLE; //外设非增量模式
	hdma_adcx.Init.MemInc = DMA_MINC_ENABLE; //存储器增量模式
	hdma_adcx.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD; //外设数据长度:32位
	hdma_adcx.Init.MemDataAlignment = DMA_MDATAALIGN_WORD; //存储器数据长度:32位
	hdma_adcx.Init.Mode = DMA_CIRCULAR; //循环模式(因为ADC是循环扫描模式)
	hdma_adcx.Init.Priority = DMA_PRIORITY_MEDIUM; //中等优先级
	hdma_adcx.Init.FIFOMode = DMA_FIFOMODE_DISABLE; //关闭FIFO             
	hdma_adcx.Init.FIFOThreshold = DMA_FIFO_THRESHOLD_FULL;      
	hdma_adcx.Init.MemBurst = DMA_MBURST_SINGLE; 
	hdma_adcx.Init.PeriphBurst = DMA_PBURST_SINGLE; 
	
	HAL_DMA_DeInit(&hdma_adcx); //先复位
	HAL_DMA_Init(&hdma_adcx); //再初始化

	__HAL_LINKDMA(&ADC1_Handler, DMA_Handle, hdma_adcx); //连接DMA和ADC外设的句柄
 
	HAL_NVIC_SetPriority(DMA2_Stream0_IRQn, 0, 0); //设置ADC_DMA中断优先级
	HAL_NVIC_EnableIRQ(DMA2_Stream0_IRQn); //使能ADC_DMA中断
}


 
 
