//#include "config.h"

/******************************************************************************* 
**                         fuda                                ** 
**                         Device: SC16IS725                                  ** 
**                         Handson &lt;2010.3.22>                               ** 
*******************************************************************************/ 
#include <includes.h>								// 配置头文件 
								
SC16IS752_BUF_A_T SC16IS752_BUF_A0;
SC16IS752_BUF_A_T SC16IS752_BUF_A1;
//SC16IS752_BUF_A_T SC16IS752_SBUF_A0;
//SC16IS752_BUF_A_T SC16IS752_SBUF_A1;

SC16IS752_BUF_A_T SC16IS752_BUF_B0;
SC16IS752_BUF_A_T SC16IS752_BUF_B1;
//SC16IS752_BUF_A_T SC16IS752_SBUF_B0;
//SC16IS752_BUF_A_T SC16IS752_SBUF_B1;



void SC16IS752_INT_Init(void)
{
	//  //exint 0
	IO0DIR &= ~(1<<16);
	PINSEL1 |= (1<<0);
	PINSEL1 &= ~(1<<1);

	EXTMODE |= 0x01;
	EXTPOLAR |= 0x00;
	EXTINT |= 0x01;

	VICIntEnable = 1 << 14;

	//	//exint 2
	IO0DIR &= ~(1<<7);
	PINSEL0 |= (1<<15);
	PINSEL0 |= (1<<14);

	EXTMODE |= 0x00;//0x04;
	EXTPOLAR |= 0x00;
	EXTINT |= 0x04;
	
	VICIntEnable = 1 << 16;

}
void SC16IS752_Reset(void)
{
	UART_EXT_DIR();	
	UART_EXT_H();
	OSTimeDly(OS_TICKS_PER_SEC/2);//OS_TICKS_PER_SEC
	HWDT_Feed();
	UART_EXT_L();	
	OSTimeDly(OS_TICKS_PER_SEC/2);
	HWDT_Feed();
	UART_EXT_H();
}

void Init_SC16IS752(void)
{
	
	SC16IS752_Reset();
	HWDT_Feed();
	OSTimeDly(50);
	HWDT_Feed();
	
	PINSEL1 &= ~(1<<29);
	PINSEL1 &= ~(1<<28);

	PINSEL1 &= ~(1<<26);
	PINSEL1 &= ~(1<<27);

/*****************************************/

/*****************************************/	
	//******************************* 
	// device 1 rs-232
	//*******************************/	
	WriteRegister(SC16_LCR,ChannelA,0x87);
    WriteRegister(SC16_LCR,ChannelA,0x03);			//8 bit,no parity,1 stop bits
	WriteRegister(SC16_FCR,ChannelA,0x06);			//enable ,reset FIFO
	WriteRegister(SC16_FCR,ChannelA,0xB7);			//enable  FIFO 56
    WriteRegister(SC16_LCR,ChannelA,0x83);
    WriteRegister(SC16_DLL,ChannelA,0x06);			//19200bps
    //WriteRegister(SC16_DLL,ChannelA,0x0c);			//9600bps
    WriteRegister(SC16_DLH,ChannelA,0x00);          //SetBaudrate 
	WriteRegister(SC16_LCR,ChannelA,0x03);			//can't sleep mode
	WriteRegister(SC16_IER,ChannelA,0x01);			//接收中断使能


	WriteRegister(SC16_LCR,ChannelB,0x87);
    WriteRegister(SC16_LCR,ChannelB,0x03);			//8 bit,no parity,1 stop bits
	WriteRegister(SC16_FCR,ChannelB,0x06);			//enable ,reset FIFO
	WriteRegister(SC16_FCR,ChannelB,0xB7);			//enable  FIFO 56
    WriteRegister(SC16_LCR,ChannelB,0x83);
    WriteRegister(SC16_DLL,ChannelB,0x06);			//19200bps
    WriteRegister(SC16_DLH,ChannelB,0x00);          //SetBaudrate 
	WriteRegister(SC16_LCR,ChannelB,0x03);			//can't sleep mode	
	WriteRegister(SC16_IER,ChannelB,0x01);


	//******************************* 
	// device 2
	//*******************************/

	//*****ChannelA 
	//set the Baudrate  
	/*
	WriteRegisterB(SC16_LCR,ChannelA,0xbf);
	WriteRegisterB(SC16_EFR,ChannelA,0x10);//new add
	WriteRegisterB(SC16_LCR,ChannelA,0x87);
    WriteRegisterB(SC16_LCR,ChannelA,0x03);			//8 bit,no parity,1 stop bits
	WriteRegisterB(SC16_FCR,ChannelA,0x06);			//enable ,reset FIFO
	WriteRegisterB(SC16_FCR,ChannelA,0xB7);			//enable  FIFO 56
    WriteRegisterB(SC16_LCR,ChannelA,0x83);
	WriteRegisterB(SC16_EFCR,ChannelA,0x31);
    WriteRegisterB(SC16_DLL,ChannelA,0x30);			//2400bps
    WriteRegisterB(SC16_DLH,ChannelA,0x00);          //SetBaudrate 
	WriteRegisterB(SC16_LCR,ChannelA,0x03);			//can't sleep mode
	WriteRegisterB(SC16_IER,ChannelA,0x01);			//接收中断使能
	*/
	WriteRegisterB(SC16_LCR,ChannelA,0x80);
	WriteRegisterB(SC16_EFCR,ChannelA,0x31);
    WriteRegisterB(SC16_DLL,ChannelA,0x30);			//2400bps
    //WriteRegisterB(SC16_LCR,ChannelB,0xbf);
	//WriteRegisterB(SC16_EFR,ChannelB,0x10);
    WriteRegisterB(SC16_FCR,ChannelA,0x06);			//enable ,reset FIFO
	WriteRegisterB(SC16_FCR,ChannelA,0xB7);			//enable  FIFO 56
	WriteRegisterB(SC16_LCR,ChannelA,0x03);
	WriteRegisterB(SC16_EFCR,ChannelA,0x31);

	 
	//*****ChannelB 
	//set the Baudrate 

	WriteRegisterB(SC16_LCR,ChannelB,0xbf);
	WriteRegisterB(SC16_EFR,ChannelB,0x10);//new add		
	WriteRegisterB(SC16_LCR,ChannelB,0x87);
    WriteRegisterB(SC16_LCR,ChannelB,0x03);			//8 bit,no parity,1 stop bits
	WriteRegisterB(SC16_FCR,ChannelB,0x06);			//enable ,reset FIFO
	WriteRegisterB(SC16_FCR,ChannelB,0xB7);			//enable  FIFO 56
    WriteRegisterB(SC16_LCR,ChannelB,0x83);
  //  WriteRegisterB(SC16_DLL,ChannelB,0x06);			//19200bps
  //  WriteRegisterB(SC16_DLH,ChannelB,0x00);          //SetBaudrate   
  	WriteRegisterB(SC16_EFCR,ChannelB,0x31);
	WriteRegisterB(SC16_DLL,ChannelB,0x30);		  //2400bps
    WriteRegisterB(SC16_DLH,ChannelB,0x00);          //SetBaudrate 
	WriteRegisterB(SC16_LCR,ChannelB,0x03);			//can't sleep mode
	WriteRegisterB(SC16_IER,ChannelB,0x01);			//接收中断使能	
	
	SC16IS752_INT_Init();
	memset(&SC16IS752_BUF_A0, 0, sizeof(SC16IS752_BUF_A_T));
	memset(&SC16IS752_BUF_A1, 0, sizeof(SC16IS752_BUF_A_T));
	memset(&SC16IS752_BUF_B0, 0, sizeof(SC16IS752_BUF_A_T));
	memset(&SC16IS752_BUF_B1, 0, sizeof(SC16IS752_BUF_A_T));
	
	SC16IS752_BUF_A0.counter = 0;
	SC16IS752_BUF_A1.counter = 0;
	SC16IS752_BUF_B0.counter = 0;
	SC16IS752_BUF_B1.counter = 0;
	
}
void EINT0_Exception(void)
{
	uint8 temp = 0;
	uint8 err,err1;
	DATA_CONTROL_BLOCK  *dp;
	uint8 *dp1;
	//uint8 ch; 
	

	EXTINT |= 0x01;
	//Uart0SendSTR("\r\n");
	//stprintf("\r\r");

	ReadRegister(SC16_IIR,ChannelA,&temp);
	switch ((temp&0x3f))
	{
		case 0x0c:
		{
			ReadRegister(SC16_LSR,ChannelA,&temp);
			while ( (temp&0x01)==0x01 )
			{
				ReadRegister(SC16_RHR,ChannelA,&SC16IS752_BUF_A0.buffer[SC16IS752_BUF_A0.counter++]);
				SC16IS752_BUF_A0.counter %= SC16IS752_RTX_LENGTH; 
				ReadRegister(SC16_LSR,ChannelA,&temp);
			}

			dp = OSMemGet(p_msgBuffer,&err);
			dp1 = OSMemGet(mem160ptr,&err1);
			if((err == OS_NO_ERR)&&(err1 == OS_NO_ERR)/*&&(Uart0.RecvCount<=220)*/)
			{
			   dp->type = LOCAL_DEBUG_IN_MSG;
			   dp->count = SC16IS752_BUF_A0.counter;
			   CopyBuffer(&SC16IS752_BUF_A0.buffer[0],dp1,SC16IS752_BUF_A0.counter);
			   dp->point = (uint8 *)dp1;
			   //OSQPost(GetWeatherQueue,(void*)dp); 
			   if(OS_NO_ERR != OSQPost(RemoteRecQueue,(void*)dp))
			   {
				   OSMemPut(mem160ptr,dp1);
				   OSMemPut(p_msgBuffer,dp);
			   }
			}
			else
			{
				if(err== OS_NO_ERR)
				{
					OSMemPut(p_msgBuffer,dp);
				}	
				if(err1== OS_NO_ERR)
				{
					OSMemPut(mem160ptr,dp1);
				}
			}
			SC16IS752_BUF_A0.counter = 0;
		}break;
	
		case 0x04:
		{
			RecvChannel1A(SC16_RHR,ChannelA,&SC16IS752_BUF_A0.buffer[SC16IS752_BUF_A0.counter],SC16IS752_A_FIFO_LEN);
			SC16IS752_BUF_A0.counter += SC16IS752_A_FIFO_LEN;
			SC16IS752_BUF_A0.counter %= SC16IS752_RTX_LENGTH;
		
		}break;

		
		default:
		{
		}break;
	}

	ReadRegister(SC16_IIR,ChannelB,&temp);

	switch ( (temp&0x3f) )
	{
		case 0x0c:
		{
			ReadRegister(SC16_LSR,ChannelB,&temp);
			while ( (temp&0x01)==0x01 )
			{
				ReadRegister(SC16_RHR,ChannelB,&SC16IS752_BUF_A1.buffer[SC16IS752_BUF_A1.counter++]);
				SC16IS752_BUF_A1.counter %= SC16IS752_RTX_LENGTH;
				ReadRegister(SC16_LSR,ChannelB,&temp);
			}
			#if 0
			dp = OSMemGet(p_msgBuffer,&err);
			dp1 = OSMemGet(mem160ptr,&err1);
			if((err == OS_NO_ERR)&&(err1 == OS_NO_ERR)/*&&(Uart0.RecvCount<=220)*/)
			{
			   dp->type = WEATHER_VOLTAGE_MSG; 
			   dp->count = SC16IS752_BUF_A1.counter;
			   CopyBuffer(&SC16IS752_BUF_A1.buffer[0],dp1,SC16IS752_BUF_A1.counter);
			   dp->point = (uint8 *)dp1;
			   OSQPost(GetWeatherQueue,(void*)dp); 
			}
			else
			{
				if(err== OS_NO_ERR)
				{
					OSMemPut(p_msgBuffer,dp);
				}	
				if(err1== OS_NO_ERR)
				{
					OSMemPut(mem160ptr,dp1);
				}
			}

			//Uart0SendDAT(SC16IS752_BUF_A1.buffer,SC16IS752_BUF_A1.counter);
			SC16IS752_BUF_A1.counter = 0;
			#endif
		}break;
	
		case 0x04:
		{
			RecvChannel1B(SC16_RHR,ChannelB,&SC16IS752_BUF_A1.buffer[SC16IS752_BUF_A1.counter++],SC16IS752_A_FIFO_LEN);
			SC16IS752_BUF_A1.counter += (SC16IS752_A_FIFO_LEN-1);
			SC16IS752_BUF_A1.counter %= SC16IS752_RTX_LENGTH;
		}break;

		
		default:
		{
		}break;
	}

	VICVectAddr = 0x00;            // 通知中断控制器中断结束
}
void ReadRegister(uint8 Reg,uint8 Channel,uint8 *dp)
{
	Reg<<=3;
	Reg=Reg+Channel;     
	I2C_ReadNByte(SC16IS752A,1,Reg,dp,1);
}
void WriteRegister(uint8 Reg,uint8 Channel,uint8 Value)
{
	Reg<<=3;
	Reg=Reg+Channel;     
	I2C_WriteNByte(SC16IS752A,1,Reg,&Value,1);
}
void RecvChannel1A(uint8 Reg,uint8 Channel,uint8 *dp,uint16 length)
{ 
	Reg<<=3;
	Reg=Reg+Channel;     
	I2C_ReadNByte(SC16IS752A,1,Reg,dp,length); 

} 
void RecvChannel1B(uint8 Reg,uint8 Channel,uint8 *dp,uint16 length)
{
	Reg<<=3;
	Reg=Reg+Channel;     
	I2C_ReadNByte(SC16IS752A,1,Reg,dp,length); 
} 

/**************************************************************************** 
* 名    称: RS232A_sendst 
* 功    能: 启动调试发送 
* 入口参数: 无 
* 出口参数: 无 
****************************************************************************/ 
void RS232A_sendst (uint8 *sp,uint8 len) 
{  
	uint8 Reg = SC16_THR;

	Reg<<=3;
	Reg=Reg+ChannelA;

	I2C_WriteNByte(SC16IS752A,1,Reg,sp,len);

} 
/***********电压*************/
void RS232B_sendst (uint8 *sp,uint8 len) 
{ 
	uint8 Reg = SC16_THR;

	Reg<<=3;
	Reg=Reg+ChannelB;

	I2C_WriteNByte(SC16IS752A,1,Reg,sp,len);

}




void EINT2_Exception(void) 
{   
	uint8 temp; 
  	//DATA_CONTROL_BLOCK  *dp;
	//uint8 *dp1;
	//uint8 err,err1;

	EXTINT |= 0x04;
	//Uart0SendSTR("\r\n");
	//stprintf("\r\r");
	
	//ChannelA*************************************************************** 
	ReadRegisterB(SC16_IIR,ChannelA,&temp);	// 		 
	switch(temp&0X3F) 
	{ 
		case 0x0c :
			ReadRegisterB(SC16_LSR,ChannelA,&temp);
			while ( (temp&0x01)==0x01 )
			{
				ReadRegisterB(SC16_RHR,ChannelA,&SC16IS752_BUF_B0.buffer[SC16IS752_BUF_B0.counter++]);
				SC16IS752_BUF_B0.counter %= SC16IS752_RTX_LENGTH;
				ReadRegisterB(SC16_LSR,ChannelA,&temp);
			}
			#if 0
			dp = OSMemGet(p_msgBuffer,&err);
			dp1 = OSMemGet(mem160ptr,&err1);
			if((err == OS_NO_ERR)&&(err1 == OS_NO_ERR)/*&&(Uart0.RecvCount<=220)*/)
			{
			   dp->type = WEATHER_PICTURE_MSG; 
			   dp->count = SC16IS752_BUF_B0.counter;
			   CopyBuffer(&SC16IS752_BUF_B0.buffer[0],dp1,SC16IS752_BUF_B0.counter);
			   dp->point = (uint8 *)dp1;
			   OSQPost(GetWeatherQueue,(void*)dp); 
			}
			else
			{
				if(err== OS_NO_ERR)
				{
					OSMemPut(p_msgBuffer,dp);
				}	
				if(err1== OS_NO_ERR)
				{
					OSMemPut(mem160ptr,dp1);
				}
			}
			SC16IS752_BUF_B0.counter = 0;
			#endif
			break; 
		case 0x04 :
			RecvChannel2A(SC16_RHR,ChannelA,&SC16IS752_BUF_B0.buffer[SC16IS752_BUF_B0.counter++],SC16IS752_A_FIFO_LEN); 
			SC16IS752_BUF_B0.counter += (SC16IS752_A_FIFO_LEN-1);
			SC16IS752_BUF_B0.counter %= SC16IS752_RTX_LENGTH;
			break; 
		default :break; 
	} 
	 
	//ChannelB*************************************************************** 
	ReadRegisterB(SC16_IIR,ChannelB,&temp);	// 
	switch(temp&0X3F) 
	{ 
		case 0x0c :			 
			ReadRegisterB(SC16_LSR,ChannelB,&temp);
			while ( (temp&0x01)==0x01 )
			{
				ReadRegisterB(SC16_RHR,ChannelB,&SC16IS752_BUF_B1.buffer[SC16IS752_BUF_B1.counter++]);
				SC16IS752_BUF_B1.counter %= SC16IS752_RTX_LENGTH;
				ReadRegisterB(SC16_LSR,ChannelB,&temp);
			}
			#if 0
			dp = OSMemGet(p_msgBuffer,&err);
			dp1 = OSMemGet(mem160ptr,&err1);
			if((err == OS_NO_ERR)&&(err1 == OS_NO_ERR)/*&&(Uart0.RecvCount<=220)*/)
			{
			   dp->type = WEATHER_TIME_MSG;
			   dp->count = SC16IS752_BUF_B1.counter;
			   CopyBuffer(&SC16IS752_BUF_B1.buffer[0],dp1,SC16IS752_BUF_B1.counter);
			   dp->point = (uint8 *)dp1;
			   OSQPost(GetWeatherQueue,(void*)dp); 
			}
			else
			{
				if(err== OS_NO_ERR)
				{
					OSMemPut(p_msgBuffer,dp);
				}	
				if(err1== OS_NO_ERR)
				{
					OSMemPut(mem160ptr,dp1);
				}
			}
			SC16IS752_BUF_B1.counter = 0;
			#endif
			break; 
		case 0x04 ://RHR中断 
		
			RecvChannel2B(SC16_RHR,ChannelB,&SC16IS752_BUF_B1.buffer[SC16IS752_BUF_B1.counter++],SC16IS752_A_FIFO_LEN); 
			SC16IS752_BUF_B1.counter += (SC16IS752_A_FIFO_LEN-1);
			SC16IS752_BUF_B1.counter %= SC16IS752_RTX_LENGTH;
			break; 
		default :break; 
	} 
	
   VICVectAddr = 0x00;						// 向量中断结束 
} 

void ReadRegisterB(uint8 Reg,uint8 Channel,uint8 *dp)
{
	Reg<<=3;
	Reg=Reg+Channel;     
	I2C_ReadNByte(SC16IS752B,1,Reg,dp,1);
}

void WriteRegisterB(uint8 Reg,uint8 Channel,uint8 Value) 
{ 
	 Reg<<=3;
	 Reg=Reg+Channel;	  
	 I2C_WriteNByte(SC16IS752B,1,Reg,&Value,1);

} 

void RecvChannel2A(uint8 Reg,uint8 Channel,uint8 *dp,uint16 length)
{ 
	Reg<<=3;
	Reg=Reg+Channel;     
	I2C_ReadNByte(SC16IS752B,1,Reg,dp,length); 

} 
void RecvChannel2B(uint8 Reg,uint8 Channel,uint8 *dp,uint16 length)
{
	Reg<<=3;
	Reg=Reg+Channel;     
	I2C_ReadNByte(SC16IS752B,1,Reg,dp,length); 
} 

/**************************************************************************** 
* 名    称: RS485A_sendst 
* 功    能: 启动uart0发送 
* 入口参数: 无 
* 出口参数: 无 
****************************************************************************/ 
void RS485A_sendst (uint8 *sp, uint8 len) 
{
	char i; 
	uint8 Reg = SC16_THR;

	Reg<<=3;
	Reg=Reg+ChannelA; 
	
 	SC16IS752_BUF_B0.counter = 0;
	
	for (i=50; i>0; i--) ;									// 延时6uS, 确保485转换结束 

	I2C_WriteNByte(SC16IS752B,1,Reg,sp,len);

} 
/**************************************************************************** 
* 名    称: RS485B_sendst 
* 功    能: 启动uartSC16IS752B发送 
* 入口参数: 无 
* 出口参数: 无 
****************************************************************************/ 
void RS485B_sendst (uint8 *sp,uint8 len) 
{ 
	char i; 
	uint8 Reg = SC16_THR;

	Reg<<=3;
	Reg=Reg+ChannelB;  	
 	SC16IS752_BUF_B1.counter = 0;
	
	for (i=50; i>0; i--) ;									// 延时6uS, 确保485转换结束 

	I2C_WriteNByte(SC16IS752B,1,Reg,sp,len);

} 

