/**
  ******************************************************************************
  * @file    hard_i2c_driver.c
  * $Author: caiguangzhu $
  * $Revision: 87 $
  * $Date:: 2019-09-22 10:59:21 +0800 #$
  * @brief   硬件I2C相关函数。
  ******************************************************************************
  * @attention
  *
  *<h3><center>&copy; Copyright 2009-2012, ViewTool</center>
  *<center><a href="http:\\www.viewtool.com">http://www.viewtool.com</a></center>
  *<center>All Rights Reserved</center></h3>
  * 
  ******************************************************************************
  */
/** @addtogroup AM2320_I2C
  * @{
  */
/** @defgroup Ginkgo_HARD_I2C 
  * @brief Ginkgo 硬件I2C模块
  * @{
  */

/* Includes ------------------------------------------------------------------*/
//#include "main.h"
//#include "hard_i2c_driver.h"
#include "includes.h"
/* Private define ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
I2C_INIT_INFO I2C_Info;	///<I2C控制器相关信息

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  硬件I2C初始化 。
  * @param  I2C_Info I2C初始化信息。
  * @retval 初始化数据状态。
  */
uint8 VT_I2C_HardInit(I2C_INIT_INFO *I2C_Info)
{
	I2C_InitTypeDef  I2C_InitStructure;
   	GPIO_InitTypeDef  GPIO_InitStructure;
   	if(0 == I2C_Info->channel)
	{
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1,ENABLE);
	   	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO , ENABLE);
	
	   	/* Configure I2C1 pins: PB6->SCL and PB7->SDA */
	   	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_6 | GPIO_Pin_7;
	   	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	   	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
	   	GPIO_Init(GPIOB, &GPIO_InitStructure);
		
	   	I2C_DeInit(I2C1);
	   	I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
	   	I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
	   	I2C_InitStructure.I2C_OwnAddress1 = 0x30;
	   	I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
		if(I2C_Info->slaveAddr>>8)
		{
			I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_10bit;
		}
		else
		{
	   		I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
		}
	   	I2C_InitStructure.I2C_ClockSpeed = I2C_Info->speed;
	    
	   	I2C_Cmd(I2C1, ENABLE);
	   	I2C_Init(I2C1, &I2C_InitStructure);
	
	   	I2C_AcknowledgeConfig(I2C1, ENABLE);
   	}
	
	else
	{
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C2,ENABLE);
	   	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO , ENABLE);
	
	   	/* Configure I2C1 pins: PB10->SCL and PB11->SDA */
	   	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_10 | GPIO_Pin_11;
	   	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	   	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
	   	GPIO_Init(GPIOB, &GPIO_InitStructure);
			
	   	I2C_DeInit(I2C2);
	   	I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
	   	I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
	   	I2C_InitStructure.I2C_OwnAddress1 = 0x30;
	   	I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
		if(I2C_Info->slaveAddr>>8)
		{
			I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_10bit;
		}
		else
		{
	   		I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
		}
	   	I2C_InitStructure.I2C_ClockSpeed = I2C_Info->speed;
	    
	   	I2C_Cmd(I2C2, ENABLE);
	   	I2C_Init(I2C2, &I2C_InitStructure);
	
	   	I2C_AcknowledgeConfig(I2C2, ENABLE);
	}
	return 0;
}
/**
  * @brief  通过硬件I2C写数据 。
  * @param  I2Cx 硬件I2C通道。
  * @param  subaddr 写数据的子地址，若不存在子地址则该数据无意义。
  * @param  s 需要写的数据缓冲区地址。
  * @param  num 需要写的数据字节数。
  * @retval 写数据状态。
  */
uint8 VT_I2C_HardWriteNByte(I2C_TypeDef *I2Cx, uint32_t subaddr, uint8_t *s, uint32_t num)
{
  	uint32_t subaddrNum=I2C_Info.subAddrWidth;
	uint32_t retry=0;
	uint8_t *p_data=s;
	I2C_GenerateSTART(I2Cx, ENABLE);   //产生一个起始信号
	retry=0;
  	while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_MODE_SELECT))
	{
		retry++;
		if(retry>=1000)
		{
			I2C_GenerateSTOP(I2Cx, ENABLE);
			return MASTER_MODE_SELECT_FAILED;	
		}
	}
	//发送设备地址
	if(I2C_Info.slaveAddr>>8)
	{
  		//I2C_Send7bitAddress(I2Cx, I2C_Addr, I2C_Direction_Transmitter);
	}
	else
	{
		I2C_Send7bitAddress(I2Cx, I2C_Info.slaveAddr, I2C_Direction_Transmitter);
		retry=0;
		while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
		{
			retry++;
			if(retry>=1000)
			{
				I2C_GenerateSTOP(I2Cx, ENABLE);   //产生一个停止信号
				return MASTER_TRANSMITTER_MODE_SELECTED_FAILED;	
			}
		}
	}
  	
	//发送子地址
	while(subaddrNum>0)
	{
	  	I2C_SendData(I2Cx, subaddr>>((subaddrNum-1)*8));
		retry=0;
		while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
		{
			retry++;
			if(retry>=1000)
			{
				I2C_GenerateSTOP(I2Cx, ENABLE);		//产生一个停止信号
				return MASTER_BYTE_TRANSMITTED_FAILED;	
			}
		}
		subaddrNum--;
	}
	//发送数据
	while(num--)
	{
	  	I2C_SendData(I2Cx, *p_data++); 
		retry=0;
  		while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
		{
			retry++;
			if(retry>=1000)
			{
				I2C_GenerateSTOP(I2Cx, ENABLE);			//产生一个停止信号
				return MASTER_BYTE_TRANSMITTED_FAILED;	
			}
		}
	}
	//停止总线
	I2C_GenerateSTOP(I2Cx, ENABLE);
	return 0;
}
/**
  * @brief  通过硬件I2C读数据 。
  * @param  I2Cx 硬件I2C通道。
  * @param  subaddr 读数据的子地址，若不存在子地址则该数据无意义。
  * @param  s 需要读的数据存储缓冲区地址。
  * @param  num 需要读的数据字节数。
  * @retval 读数据状态。
  */
uint8 VT_I2C_HardReadNByte(I2C_TypeDef *I2Cx, uint32_t subaddr,uint8_t *s,uint32_t num)
{
  	int32_t	subaddrNum=I2C_Info.subAddrWidth;
	uint8_t *p_data=s;	
	uint32_t retry=0;
	while(I2C_GetFlagStatus(I2Cx, I2C_FLAG_BUSY))
	{
		retry++;
		if(retry>=1000)
		{
			I2C_GenerateSTOP(I2Cx, ENABLE);
			return MASTER_GET_I2C_FLAG_BUSY_FAILED;	
		}
	}
	I2C_AcknowledgeConfig(I2Cx, ENABLE);
    I2C_GenerateSTART(I2Cx, ENABLE);
	retry=0;
    while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_MODE_SELECT))
	{
		retry++;
		if(retry>=1000)
		{
			I2C_GenerateSTOP(I2Cx, ENABLE);
			return MASTER_MODE_SELECT_FAILED;	
		}
	}
	if(subaddrNum>0)
	{
		//发送设备地址
		if(I2C_Info.slaveAddr>>8)
		{
	  		//I2C_Send7bitAddress(I2Cx, I2C_Addr, I2C_Direction_Transmitter);
		}
		else
		{
			I2C_Send7bitAddress(I2Cx, I2C_Info.slaveAddr, I2C_Direction_Transmitter);
			retry=0;
			while (!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
			{
				retry++;
				if(retry>=1000)
				{
					I2C_GenerateSTOP(I2Cx, ENABLE);
					return MASTER_TRANSMITTER_MODE_SELECTED_FAILED;	
				}
			}
		} 
		//发送子地址
		while(subaddrNum>0)
		{
		  	I2C_SendData(I2Cx, subaddr>>((subaddrNum-1)*8));
			retry=0;
			while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
			{
				retry++;
				if(retry>=1000)
				{
					I2C_GenerateSTOP(I2Cx, ENABLE);
					return MASTER_BYTE_TRANSMITTED_FAILED;	
				}
			}
			subaddrNum--;
		}
		//重新启动I2C总线
		I2C_GenerateSTART(I2Cx, ENABLE);
		retry=0;
		while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_MODE_SELECT))
		{
			retry++;
			if(retry>=1000)
			{
				I2C_GenerateSTOP(I2Cx, ENABLE);
				return MASTER_MODE_SELECT_FAILED;	
			}
		}
		//发送读设备地址
		I2C_Send7bitAddress(I2Cx, I2C_Info.slaveAddr, I2C_Direction_Receiver);
		retry=0;
		while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED))
		{
			retry++;
			if(retry>=1000)
			{
				I2C_GenerateSTOP(I2Cx, ENABLE);
				return MASTER_RECEIVER_MODE_SELECTED_FAILED;	
			}
		}
	}
	else
	{
		I2C_Send7bitAddress(I2Cx, I2C_Info.slaveAddr, I2C_Direction_Receiver);
		retry=0;
		while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED))
		{
			retry++;
			if(retry>=1000)
			{
				I2C_GenerateSTOP(I2Cx, ENABLE);
				return MASTER_RECEIVER_MODE_SELECTED_FAILED;	
			}
		}
	}
	//读取数据
    while (num)
    {
		if(num==1)
		{
     		I2C_AcknowledgeConfig(I2Cx, DISABLE);
    		I2C_GenerateSTOP(I2Cx, ENABLE);
		}
	    retry = 0;
		while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_RECEIVED))
		{
			retry++;
			if(retry>=1000)
			{
				I2C_GenerateSTOP(I2Cx, ENABLE);
				return MASTER_BYTE_RECEIVED_FAILED;	
			}
		}
	    *p_data = I2C_ReceiveData(I2Cx);
	    p_data++;
	    /* Decrement the read bytes counter */
	    num--;
    }

	I2C_AcknowledgeConfig(I2Cx, ENABLE);
	return 0;
}
/**
  * @}
  */
/**
  * @}
  */
