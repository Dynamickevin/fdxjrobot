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
static void AM2320_begin_get_data(void* para); //��ʼ����һ�� ��ʪ�Ȳɼ�����
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
    gTimerFor_AM2320.TmrPeriod = OS_TICKS_PER_SEC*2 ;  //ÿ 2 �����һ�����ݲɼ�
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
*DESCRIPTION:  AM2320 оƬ״̬��ʼ�� �Լ�ϵͳ��ʼ��
*************************************************/
void AM2320_and_iic_init(void)
{
    soft_timer_init_timer( &gTimerFor_AM2320 , OS_TICKS_PER_SEC , AM2320_begin_get_data , 0 , 1 );
    BX_CALL(soft_timer_start)(&gTimerFor_AM2320);
}

//��ȡ�¶�ֵ
short AM2320_get_temper(void)
{
    return gnAM2320_temper;
}

//��ȡʪ��ֵ
short AM2320_get_humidi(void)
{
    return gnAM2320_humidi;
}

//#else //iic driver by hardware and operate list

IIC_Operation AM2320_IicSend_WakeUp;
IIC_Operation AM2320_IicSend_GetHM;
IIC_Operation AM2320_IicReadHM;

static void AM2320_begin_get_data(void* para); //��ʼ����һ�� ��ʪ�Ȳɼ�����
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
        IIC_PORT_1 ,           //���� IIC_PORT_1 ����
        &AM2320_IicReadHM ,      //������Ҫ���еĲ������ڸú����б�����
        0X00,            //�Ĵ�����ַ��Ҳ������ֵ�������豸û��ʹ�üĴ�����ַ����������
        read_from_AM2320 ,        //��Ҫ���յ�����
        8        //��Ҫ���յ����ݳ���
    );
    AM2320_IicReadHM.IicOperFlag &= ~IIC_OPER_FLAG_REG ; //������regֵ
    
    gTimerFor_AM2320.TmrPeriod = OS_TICKS_PER_SEC*2 ;  //ÿ 2 �����һ�����ݲɼ�
    gTimerFor_AM2320.Callback  = AM2320_begin_get_data ;
}

static void IIC_TO_AM2320_WakeOK(void* para)
{
    IIC_SendStop(IIC_PORT_1);
    iic_send_data(
        IIC_PORT_1,                //���� IIC_PORT_1 ����
        &AM2320_IicSend_GetHM ,    //������Ҫ���еĲ������ڸú����б�����
        send_to_AM2320[0] ,        //�Ĵ�����ַ��Ҳ������ֵ�������豸û��ʹ�üĴ�����ַ����������
        send_to_AM2320+1 ,         //��Ҫ���͵�����
        2                          //��Ҫ���͵����ݳ���
    );
    
    gTimerFor_AM2320.TmrPeriod = 2 ;  //ÿ 2 �����һ�����ݲɼ�
    gTimerFor_AM2320.Callback  = IIC_TO_AM2320_waitForGetOK ;
}

static void AM2320_begin_get_data(void* para)
{
    //iic_set_ack( 0 );
    //iic_timer_write( 0xB8  , send_to_AM2320+4 , 1 , IIC_FinCb_WakeUpEnd ); //0xA0
    
    iic_send_data(
        IIC_PORT_1,                //���� IIC_PORT_1 ����
        &AM2320_IicSend_WakeUp ,   //������Ҫ���еĲ������ڸú����б�����
        0X00 ,                     //�Ĵ�����ַ��Ҳ������ֵ�������豸û��ʹ�üĴ�����ַ����������
        send_to_AM2320+4 ,         //��Ҫ���͵�����
        0                          //��Ҫ���͵����ݳ���
     );
    AM2320_IicSend_WakeUp.IicOperFlag &= ~IIC_OPER_FLAG_REG ; //������regֵ
    AM2320_IicSend_WakeUp.IicOperFlag |= IIC_OPER_FLAG_NO_STOP; //������ STOP �ź�
    gTimerFor_AM2320.TmrPeriod = 1;
    gTimerFor_AM2320.Callback  = IIC_TO_AM2320_WakeOK ;
    //iic_send_data(
    //    IIC_PORT_1,                //���� IIC_PORT_1 ����
    //    &AM2320_IicSend_GetHM ,    //������Ҫ���еĲ������ڸú����б�����
    //    send_to_AM2320[0] ,        //�Ĵ�����ַ��Ҳ������ֵ�������豸û��ʹ�üĴ�����ַ����������
    //    send_to_AM2320+1 ,         //��Ҫ���͵�����
    //    2                          //��Ҫ���͵����ݳ���
    // );
}

/************************************************* 
*Function:		AM2320_and_iic_init
*Input:			
*OUTPUT:		void
*Return:		
*DESCRIPTION:  AM2320 оƬ״̬��ʼ�� �Լ�ϵͳ��ʼ��
*************************************************/
void AM2320_and_iic_init(void)
{
    iic_init_operation(
        &AM2320_IicSend_WakeUp ,     //������Ҫ���еĲ����������Ƚ�������
        0xB8 ,        //�豸��ַ
        NULL ,      //�������ʱ�Ļص�����;���������ж��б�����
        NULL   //��ɻص���������
    );
    iic_init_operation(
        &AM2320_IicSend_GetHM ,     //������Ҫ���еĲ����������Ƚ�������
        0xB8 ,        //�豸��ַ
        NULL ,      //�������ʱ�Ļص�����;���������ж��б�����
        NULL   //��ɻص���������
    );
    iic_init_operation(
        &AM2320_IicReadHM ,     //������Ҫ���еĲ����������Ƚ�������
        0xB8 ,        //�豸��ַ
        IIC_TO_ReadData ,      //�������ʱ�Ļص�����;���������ж��б�����
        NULL   //��ɻص���������
    );
    
    //iic1_init();//zs
    //soft_timer_init_timer( &gTimerFor_AM2320 , OS_TICKS_PER_SEC/10 , AM2320_begin_get_data , 0 , 1 );//zs
    //BX_CALL(soft_timer_start)(&gTimerFor_AM2320);//zs
}

//��ȡ�¶�ֵ
short AM2320_get_temper(void)
{
    return gnAM2320_temper;
}

//��ȡʪ��ֵ
short AM2320_get_humidi(void)
{
    return gnAM2320_humidi;
}

#else




/**
  * @brief  ��ʱ����
  * @param  Time ��ʱʱ�����
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
*DESCRIPTION:  AM2320 оƬ״̬��ʼ�� �Լ�ϵͳ��ʼ��
*************************************************/

extern I2C_INIT_INFO I2C_Info;	///<I2C�����������Ϣ



void AM2320_and_iic_init(void)
{

	#define AM2320_ADDRESS 0xB8
	
	//uint8 AM2320_TX_Buffer[] = {0x03,0x00,0x04};//����ʪ������(��CRCУ��)
	//uint8 AM2320_RX_Buffer[12];
	
	//����AM2320����������I2C��ʼ������
	I2C_Info.controlMode = 1;		//Ӳ������
	I2C_Info.speed=100000;			//100K
	I2C_Info.subAddrMode=2;			//���ӵ�ַģʽ
	I2C_Info.subAddrWidth=0;		//�ӵ�ַΪ0�ֽڿ���
	I2C_Info.slaveAddr=0xB8;		//AM2320������I2C��ַ
	I2C_Info.channel=0;				//ʹ��I2C1
	VT_I2C_HardInit(&I2C_Info);
		
}


void AM2320_Get_Data(void)
{

	uint8		ret;
	uint8   AM2320Wake[2] ;
	uint8   Send_to_AM2320Data[8];
	uint8   Read_from_AM2320Data[10];
	
	AM2320Wake[0] = 0x00;
	VT_I2C_HardWriteNByte(I2C1,0,AM2320Wake,1);//��㷢��һ�����ݳ�ȥ������������˯���л���
	//OSTimeDly(1);
		
	Send_to_AM2320Data[0]=0x03;
	Send_to_AM2320Data[1]=0x00;
	Send_to_AM2320Data[2]=0x04;
	ret = VT_I2C_HardWriteNByte(I2C1,0,Send_to_AM2320Data,3);//���Ͷ�ȡ�¶Ⱥ�ʪ�ȵ�ָ��
	if(ret)
	{
		stprintf(ID_DEBUG,"I2Cд���ݴ���:  ");
		nprintf(ID_DEBUG,ret,0,DEC);
		stprintf(ID_DEBUG,"\r\n");
		
	}
	OSTimeDly(2);
	//Delay(100000);//����������ʱ�����򴫸���û׼�������ݻ���ֶ����ݴ���
	//GpioSetL(GPIO_LED_SHOW1);
	ret = VT_I2C_HardReadNByte(I2C1,0,Read_from_AM2320Data,8);//��ȡ�¶Ⱥ�ʪ��
	if(ret)
	{
		stprintf(ID_DEBUG,"I2Cд���ݴ���:  ");
		nprintf(ID_DEBUG,ret,0,DEC);
		stprintf(ID_DEBUG,"\r\n");
		
	}
	else
	{
		
		if((0X03 == Read_from_AM2320Data[0]) && (0X04 == Read_from_AM2320Data[1]))
		{
			gnAM2320_humidi = ((Read_from_AM2320Data[2]<<8)|Read_from_AM2320Data[3]);
			//stprintf(ID_DEBUG,"ʪ�ȣ�:  ");
			//nprintf(ID_DEBUG,gnAM2320_humidi,0,DEC);
			//stprintf(ID_DEBUG,"\r\n");
			
			gnAM2320_temper = ((Read_from_AM2320Data[4]<<8)|Read_from_AM2320Data[5]);

			//�ж��¶��Ƿ��Ǹ�ֵ
			if(gnAM2320_temper >= 0)
			{
				gnAM2320_temper = gnAM2320_temper;
			}
			else
			{
				gnAM2320_temper = (-1) * gnAM2320_temper;
			}
			
			//stprintf(ID_DEBUG,"�¶ȣ�:  ");
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


//��ȡ�¶�ֵ
short AM2320_get_temper(void)
{
    return gnAM2320_temper;
}

//��ȡʪ��ֵ
short AM2320_get_humidi(void)
{
    return gnAM2320_humidi;
}



#endif
