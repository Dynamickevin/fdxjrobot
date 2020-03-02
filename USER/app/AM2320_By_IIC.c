//#include "box_os_user.h"
//#include "iic_driver.h"
#include <includes.h>
#include "hard_i2c_driver.h"

short int gnAM2320_temper = 0;
short int gnAM2320_humidi = 0;


//static SoftTimer gTimerFor_AM2320;
//static u8 send_to_AM2320[8] = { 0X03 , 0X00 , 0X04 , 0 , 0 , 0 , 0 };
//static u8 read_from_AM2320[10];

#if 0
static void AM2320_begin_get_data(void* para); //开始进行一次 温湿度采集函数
static void IIC_TO_ReadData(void* para)
{
    if ( (((int)(para))<0X0100) && (0X03 == read_from_AM2320[0]) )
    {
        gnAM2320_temper = read_from_AM2320[4]<<8|read_from_AM2320[5];
        gnAM2320_humidi = read_from_AM2320[2]<<8|read_from_AM2320[3];
        //USART_DEBUG_OUT( "IIC R=%d %d %d %d\n" ,para, (int)read_from_AM2320[0] , gnAM2320_temper , gnAM2320_humidi );
        read_from_AM2320[0] = 0;
    }
    else{
        USART_DEBUG_OUT( "IIC Err=%d\n" , para );
    }
    gTimerFor_AM2320.TmrPeriod = OS_TICKS_PER_SEC*2 ;  //每 2 秒进行一次数据采集
    gTimerFor_AM2320.Callback  = AM2320_begin_get_data ;
}
static void IIC_TO_AM2320_waitForGetOK(void* para)
{
    iic_timer_read( 0xB8 , read_from_AM2320 , 8 , IIC_TO_ReadData );
}
static void IIC_FinCb_WriteData(void* para)
{
    //USART_DEBUG_OUT( "IIC W=%d\n" , para );
    gTimerFor_AM2320.TmrDly    = 0;
    gTimerFor_AM2320.TmrPeriod = 2;
    gTimerFor_AM2320.Callback  = IIC_TO_AM2320_waitForGetOK ;
}
static void IIC_FinCb_WakeUpEnd(void* para)
{
    //USART_DEBUG_OUT( "IIC up=%d\n" , para );
    //iic_set_ack( 1 );
    iic_timer_write( 0xB8  , send_to_AM2320 , 3 , IIC_FinCb_WriteData );
}

static void AM2320_begin_get_data(void* para)
{
    iic_set_ack( 0 );
    iic_timer_write( 0xB8  , send_to_AM2320+4 , 1 , IIC_FinCb_WakeUpEnd ); //0xA0
}

/************************************************* 
*Function:		AM2320_and_iic_init
*Input:			
*OUTPUT:		void
*Return:		
*DESCRIPTION:  AM2320 芯片状态初始化 以及系统初始化
*************************************************/
void AM2320_and_iic_init(void)
{
    soft_timer_init_timer( &gTimerFor_AM2320 , OS_TICKS_PER_SEC , AM2320_begin_get_data , 0 , 1 );
    BX_CALL(soft_timer_start)(&gTimerFor_AM2320);
}

//获取温度值
short AM2320_get_temper(void)
{
    return gnAM2320_temper;
}

//获取湿度值
short AM2320_get_humidi(void)
{
    return gnAM2320_humidi;
}

//#else //iic driver by hardware and operate list

IIC_Operation AM2320_IicSend_WakeUp;
IIC_Operation AM2320_IicSend_GetHM;
IIC_Operation AM2320_IicReadHM;

static void AM2320_begin_get_data(void* para); //开始进行一次 温湿度采集函数
static void IIC_TO_ReadData(void* para)
{
    if ( (((int)(para))<0X0100) && (0X03 == read_from_AM2320[0]) )
    {
        gnAM2320_temper = read_from_AM2320[4]<<8|read_from_AM2320[5];
        gnAM2320_humidi = read_from_AM2320[2]<<8|read_from_AM2320[3];
        //USART_DEBUG_OUT( "\nIIC R=%d %d %d %d\n" , (int)para , (int)read_from_AM2320[0] , (int)gnAM2320_temper , (int)gnAM2320_humidi );
        //uart_send_format( UART_PORT_1 ,"\nIIC R=%d %d %d %d\n" , (int)para , (int)read_from_AM2320[0] , (int)gnAM2320_temper , (int)gnAM2320_humidi );
        read_from_AM2320[0] = 0;
    }
    else{
        USART_DEBUG_OUT( "IIC Err=%d\n" , para );
    }
}
static void IIC_TO_AM2320_waitForGetOK(void* para)
{
    //iic_timer_read( 0xB8 , read_from_AM2320 , 8 , IIC_TO_ReadData );
    iic_read_data(
        IIC_PORT_1 ,           //传入 IIC_PORT_1 即可
        &AM2320_IicReadHM ,      //传入需要进行的操作，在该函数中被配置
        0X00,            //寄存器地址，也是命令值（部分设备没有使用寄存器地址概念，而是命令）
        read_from_AM2320 ,        //需要接收的数据
        8        //需要接收的数据长度
    );
    AM2320_IicReadHM.IicOperFlag &= ~IIC_OPER_FLAG_REG ; //不发送reg值
    
    gTimerFor_AM2320.TmrPeriod = OS_TICKS_PER_SEC*2 ;  //每 2 秒进行一次数据采集
    gTimerFor_AM2320.Callback  = AM2320_begin_get_data ;
}

static void IIC_TO_AM2320_WakeOK(void* para)
{
    IIC_SendStop(IIC_PORT_1);
    iic_send_data(
        IIC_PORT_1,                //传入 IIC_PORT_1 即可
        &AM2320_IicSend_GetHM ,    //传入需要进行的操作，在该函数中被配置
        send_to_AM2320[0] ,        //寄存器地址，也是命令值（部分设备没有使用寄存器地址概念，而是命令）
        send_to_AM2320+1 ,         //需要发送的数据
        2                          //需要发送的数据长度
    );
    
    gTimerFor_AM2320.TmrPeriod = 2 ;  //每 2 秒进行一次数据采集
    gTimerFor_AM2320.Callback  = IIC_TO_AM2320_waitForGetOK ;
}

static void AM2320_begin_get_data(void* para)
{
    //iic_set_ack( 0 );
    //iic_timer_write( 0xB8  , send_to_AM2320+4 , 1 , IIC_FinCb_WakeUpEnd ); //0xA0
    
    iic_send_data(
        IIC_PORT_1,                //传入 IIC_PORT_1 即可
        &AM2320_IicSend_WakeUp ,   //传入需要进行的操作，在该函数中被配置
        0X00 ,                     //寄存器地址，也是命令值（部分设备没有使用寄存器地址概念，而是命令）
        send_to_AM2320+4 ,         //需要发送的数据
        0                          //需要发送的数据长度
     );
    AM2320_IicSend_WakeUp.IicOperFlag &= ~IIC_OPER_FLAG_REG ; //不发送reg值
    AM2320_IicSend_WakeUp.IicOperFlag |= IIC_OPER_FLAG_NO_STOP; //不发送 STOP 信号
    gTimerFor_AM2320.TmrPeriod = 1;
    gTimerFor_AM2320.Callback  = IIC_TO_AM2320_WakeOK ;
    //iic_send_data(
    //    IIC_PORT_1,                //传入 IIC_PORT_1 即可
    //    &AM2320_IicSend_GetHM ,    //传入需要进行的操作，在该函数中被配置
    //    send_to_AM2320[0] ,        //寄存器地址，也是命令值（部分设备没有使用寄存器地址概念，而是命令）
    //    send_to_AM2320+1 ,         //需要发送的数据
    //    2                          //需要发送的数据长度
    // );
}

/************************************************* 
*Function:		AM2320_and_iic_init
*Input:			
*OUTPUT:		void
*Return:		
*DESCRIPTION:  AM2320 芯片状态初始化 以及系统初始化
*************************************************/
void AM2320_and_iic_init(void)
{
    iic_init_operation(
        &AM2320_IicSend_WakeUp ,     //传入需要进行的操作，必须先进行配置
        0xB8 ,        //设备地址
        NULL ,      //操作完成时的回调函数;会在软件中断中被调用
        NULL   //完成回调函数参数
    );
    iic_init_operation(
        &AM2320_IicSend_GetHM ,     //传入需要进行的操作，必须先进行配置
        0xB8 ,        //设备地址
        NULL ,      //操作完成时的回调函数;会在软件中断中被调用
        NULL   //完成回调函数参数
    );
    iic_init_operation(
        &AM2320_IicReadHM ,     //传入需要进行的操作，必须先进行配置
        0xB8 ,        //设备地址
        IIC_TO_ReadData ,      //操作完成时的回调函数;会在软件中断中被调用
        NULL   //完成回调函数参数
    );
    
    //iic1_init();//zs
    //soft_timer_init_timer( &gTimerFor_AM2320 , OS_TICKS_PER_SEC/10 , AM2320_begin_get_data , 0 , 1 );//zs
    //BX_CALL(soft_timer_start)(&gTimerFor_AM2320);//zs
}

//获取温度值
short AM2320_get_temper(void)
{
    return gnAM2320_temper;
}

//获取湿度值
short AM2320_get_humidi(void)
{
    return gnAM2320_humidi;
}

#else




/**
  * @brief  延时函数
  * @param  Time 延时时间参数
  * @retval None
  */
void Delay(uint32_t Time)
{
	while(Time--);
}

/************************************************* 
*Function:		AM2320_and_iic_init
*Input:			
*OUTPUT:		void
*Return:		
*DESCRIPTION:  AM2320 芯片状态初始化 以及系统初始化
*************************************************/

extern I2C_INIT_INFO I2C_Info;	///<I2C控制器相关信息



void AM2320_and_iic_init(void)
{

	#define AM2320_ADDRESS 0xB8
	
	//uint8 AM2320_TX_Buffer[] = {0x03,0x00,0x04};//读温湿度命令(无CRC校验)
	//uint8 AM2320_RX_Buffer[12];
	
	//根据AM2320传感器设置I2C初始化数据
	I2C_Info.controlMode = 1;		//硬件控制
	I2C_Info.speed=100000;			//100K
	I2C_Info.subAddrMode=2;			//无子地址模式
	I2C_Info.subAddrWidth=0;		//子地址为0字节宽度
	I2C_Info.slaveAddr=0xB8;		//AM2320传感器I2C地址
	I2C_Info.channel=0;				//使用I2C1
	VT_I2C_HardInit(&I2C_Info);
		
}


void AM2320_Get_Data(void)
{

	uint8		ret;
	uint8   AM2320Wake[2] ;
	uint8   Send_to_AM2320Data[8];
	uint8   Read_from_AM2320Data[10];
	
	AM2320Wake[0] = 0x00;
	VT_I2C_HardWriteNByte(I2C1,0,AM2320Wake,1);//随便发送一个数据出去，将传感器从睡眠中唤醒
	//OSTimeDly(1);
		
	Send_to_AM2320Data[0]=0x03;
	Send_to_AM2320Data[1]=0x00;
	Send_to_AM2320Data[2]=0x04;
	ret = VT_I2C_HardWriteNByte(I2C1,0,Send_to_AM2320Data,3);//发送读取温度和湿度的指令
	if(ret)
	{
		stprintf(ID_DEBUG,"I2C写数据错误:  ");
		nprintf(ID_DEBUG,ret,0,DEC);
		stprintf(ID_DEBUG,"\r\n");
		
	}
	OSTimeDly(2);
	//Delay(100000);//必须加这个延时，否则传感器没准备好数据会出现读数据错误
	//GpioSetL(GPIO_LED_SHOW1);
	ret = VT_I2C_HardReadNByte(I2C1,0,Read_from_AM2320Data,8);//读取温度和湿度
	if(ret)
	{
		stprintf(ID_DEBUG,"I2C写数据错误:  ");
		nprintf(ID_DEBUG,ret,0,DEC);
		stprintf(ID_DEBUG,"\r\n");
		
	}
	else
	{
		
		if((0X03 == Read_from_AM2320Data[0]) && (0X04 == Read_from_AM2320Data[1]))
		{
			gnAM2320_humidi = ((Read_from_AM2320Data[2]<<8)|Read_from_AM2320Data[3]);
			//stprintf(ID_DEBUG,"湿度：:  ");
			//nprintf(ID_DEBUG,gnAM2320_humidi,0,DEC);
			//stprintf(ID_DEBUG,"\r\n");
			
			gnAM2320_temper = ((Read_from_AM2320Data[4]<<8)|Read_from_AM2320Data[5]);

			//判断温度是否是负值
			if(gnAM2320_temper >= 0)
			{
				gnAM2320_temper = gnAM2320_temper;
			}
			else
			{
				gnAM2320_temper = (-1) * gnAM2320_temper;
			}
			
			//stprintf(ID_DEBUG,"温度：:  ");
			//nprintf(ID_DEBUG,gnAM2320_temper,0,DEC);
			//stprintf(ID_DEBUG,"\r\n");
			
		}
		else
		{

			stprintf(ID_DEBUG,"IIC is Err ");
		
		}
		
	}
	//OSTimeDly(OS_TICKS_PER_SEC);
	//Delay(10000000);
	
}


//获取温度值
short AM2320_get_temper(void)
{
    return gnAM2320_temper;
}

//获取湿度值
short AM2320_get_humidi(void)
{
    return gnAM2320_humidi;
}



#endif

