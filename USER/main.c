////////////////////   **********   robot_xunjianrobot   **********   ////////////////////

//20200310修改注释

//测试git,20200310修改注释
//测试ssh功能

#include <includes.h>
#include "stm32f10x.h"                  // Device header file


OS_EVENT	*log_sem;
DATA_CONTROL_BLOCK msgBuffer[100];
OS_MEM		*p_msgBuffer;

OS_MEM		*mem160ptr;
INT8U		 mem_160[MEM160_NUM][220];

OS_EVENT	*mem512_sem;
OS_MEM		*mem512ptr;

INT8U		 mem_512[MEM512_NUM][512];

extern int				 gui_key;
extern OS_EVENT  		*uart_q;
extern void 		    *RcvUARTQ[Q_MAX_SIZE];
extern OS_EVENT    	    *sem_time;
extern RbtState          gRbtState;

static  OS_STK       App_TaskStartStk[APP_TASK_START_STK_SIZE]; //开始任务堆栈
static  OS_STK		 TaskStk_Weather [WEATHER_LENGH];			//打印输出任务堆栈
static  OS_STK		 TaskStk_Timer [TIMER_LENGH];				//时间队列任务堆栈
static  OS_STK		 TaskStk_Local [LOCAL_LENGH];				//本地任务堆栈
//static  OS_STK	 TaskStk_Remote[REMOTE_LENGH];				//远程任务堆栈
static  OS_STK		 TaskStk_Remote_Rec [REMOTE_REC_LENGH];		//命令解析任务堆栈

//队列长度
#define 	REMOTEREC_Q_LEN		32
#define 	LOCAL_Q_LEN			10
#define		WEATHER_Q_LEN		32
#define		TIMER_Q_LEN			32

void *local_Q[LOCAL_Q_LEN]; 			//本地队列
void *timer_Q[TIMER_Q_LEN];				//时间队列
void *weather_Q[WEATHER_Q_LEN];			//打印输出队列
void *RemoteRec_Q[REMOTEREC_Q_LEN];		//命令解析队列


//高德摄像头参数定义
u8 CurrentPtzSetCom;
u8 GTZMHDCmdSetStep1;
u8 GTZMHDCmdSetStep2;
u8 GTZMHDCmdSetOk;
u8 DoGTZMHDCmdSetRet;
u8 GTZMHDCmdGetStep1;
u8 GTZMHDCmdGetStep2;
u8 GTZMHDCmdGetOk;
u8 DoGTZMHDCmdGetRet;
OS_EVENT	*PtzSem;

//从动轮码盘参数定义
signed short CodeAB_Start;
char CodeZ_Start;
signed long gCodeZ;
signed long CodeAB_Last;

//板载LED指示灯初始化,B14,B15
void led_show_init(void)
{
    GpioInit(RCC_APB2Periph_GPIOB,GPIOB,GPIO_Pin_14|GPIO_Pin_15,GPIO_Mode_Out_PP);
    GpioSetL(GPIOB,GPIO_Pin_14);
    GpioSetL(GPIOB,GPIO_Pin_15);
}

//看门狗电路初始化, C0
void WatchDog_init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);
	GPIO_INIT_OUT_PP(GPIO_CTL_FED_WTDG);
}

//一些IO口初始化过程
void GlobalUserIO_Init(void)
{

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE | RCC_APB2Periph_GPIOD | RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOA , ENABLE);

	//串口通信433模块
	GPIO_INIT_OUT_PP(GPIO_CTR_RF433_M0);
    GPIO_INIT_OUT_PP(GPIO_CTR_RF433_M1);
    GpioSetL(GPIO_CTR_RF433_M0);
    GpioSetL(GPIO_CTR_RF433_M1);
	
	//外部指示灯
	GPIO_INIT_OUT_PP(GPIO_LED_OUT_SHOW1);
	GPIO_INIT_OUT_PP(GPIO_LED_OUT_SHOW2);
	GpioSetH(GPIO_LED_OUT_SHOW1);
	GpioSetH(GPIO_LED_OUT_SHOW2);
	//GpioSetL(GPIO_LED_OUT_SHOW1);
	//GpioSetL(GPIO_LED_OUT_SHOW2);
	
	//板载指示灯
    GPIO_INIT_OUT_PP(GPIO_LED_SHOW1);
    //GpioSetL(GPIO_LED_SHOW1);
    GpioSetH(GPIO_LED_SHOW1);
    GPIO_INIT_OUT_PP(GPIO_LED_SHOW2);
    //GpioSetL(GPIO_LED_SHOW2);
    GpioSetH(GPIO_LED_SHOW2);

	//电机电源、抱闸相关
    GPIO_INIT_OUT_PP( GPIO_MOTOR_PWR_EN); 
	//GpioSetH( GPIO_MOTOR_PWR_EN);
	GpioSetL( GPIO_MOTOR_PWR_EN);
	 
    GPIO_INIT_OUT_PP( GPIO_BREAK_MEN); 
 	//GpioSetH( GPIO_BREAK_MEN);
	GpioSetL( GPIO_BREAK_MEN);
    //GPIO_INIT_OUT_PP( GPIO_4G_EN ); 
    //GpioSetL( GPIO_4G_EN);
    
    //设备电源管理相关
    GPIO_INIT_OUT_PP( GPIO_CAM_AV_EN); 
	GpioSetH( GPIO_CAM_AV_EN);
    GPIO_INIT_OUT_PP( GPIO_GPS_EN); 
	GpioSetH( GPIO_GPS_EN);
    GPIO_INIT_OUT_PP( GPIO_CAM_24V_EN); 
	GpioSetH( GPIO_CAM_24V_EN);
	GPIO_INIT_OUT_PP( GPIO_CAM_12V_EN); 
	GpioSetH( GPIO_CAM_12V_EN);
    GPIO_INIT_OUT_PP( GPIO_12V_EN); 
	GpioSetH( GPIO_12V_EN);
    GPIO_INIT_OUT_PP( GPIO_CTR_CAM_AV_SW); 
	GpioSetH( GPIO_CTR_CAM_AV_SW);

	//充电相关I/O
	GPIO_INIT_OUT_PP( GPIO_BAT_CHARGE_EN);
	GpioSetL( GPIO_BAT_CHARGE_EN);
    GPIO_INIT_IN_FLOAT(GPIO_CHK_CHAOXUE1);
    GPIO_INIT_IN_FLOAT(GPIO_CHK_CHAOXUE2);
    GPIO_INIT_IN_FLOAT(GPIO_CHK_CHAG_JOIN );
    GPIO_INIT_IN_FLOAT(GPIO_CHK_CHAG_SHORT);

	//备用输出
    GPIO_INIT_OUT_PP(GPIO_OUT_SPARE1);
    GpioSetL(GPIO_OUT_SPARE1);
    GPIO_INIT_OUT_PP(GPIO_CTL_ARM_RST);
	//GpioSetH(GPIO_CTL_ARM_RST);
}

//从动轮码盘的Z相初始化为外部中断
void Whl_CodeZ_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	EXTI_InitTypeDef EXTI_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	//从动轮码盘的Z相        	D14
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD,ENABLE);						  
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;			
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; 
	GPIO_Init(GPIOD, &GPIO_InitStructure);

	GPIO_EXTILineConfig(GPIO_PortSourceGPIOD, GPIO_PinSource14);
    EXTI_ClearITPendingBit(EXTI_Line14);
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
    EXTI_InitStructure.EXTI_Line = EXTI_Line14;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_3);  													
    NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;   
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;   
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;	      
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

//从动轮码盘的外部中断函数
void EXTI15_10_IRQ_ISR(void)
{
    signed short code;
	//signed long temp;//,CodeZ;
	//signed long CodeAB_Last;

	code=GET_SLAVE_WHEEL_CODE();
	if ( EXTI_GetITStatus(EXTI_Line14) != RESET) 
	{    
        if((code-CodeAB_Last)>600)//正转一圈
    	{   
			gCodeZ=gCodeZ+1;
    	}
		else if((CodeAB_Last-code)>600)//反转一圈
    	{
			gCodeZ=gCodeZ-1;
    	}
		else
		{
		
		}
		CodeAB_Last=code;

		/*
		if(CodeZ_Start==1)		
		{
            code=GET_SLAVE_WHEEL_CODE()-CodeAB_Start;
			if(code<0)
			{
				temp=-code;
			}
 			temp=(temp+400)/800;
			CodeZ=temp;
			if(code<0)
			{
				GET_SLAVE_WHEEL_CODE()=-CodeZ*800+CodeAB_Start;
			}
			else
			{
				GET_SLAVE_WHEEL_CODE()=CodeZ*800+CodeAB_Start;
			}
		}
		else
		{
			CodeZ_Start=1;
			CodeAB_Start=GET_SLAVE_WHEEL_CODE();//记录第一个Z脉冲来临时的编码器的值
		}
		*/
		EXTI_ClearITPendingBit(EXTI_Line14);
	}						
}

//高德红外摄像机控制函数
void DoPTZ(void)
{
	/*
	uint8 err; 
    OSSemPend(PtzSem,0,&err);
	if(OS_ERR_NONE != err)
	{
		//return;
	}  
	*/

	//设置云台参数 俯仰角和方位角
	if(GTZMHDCmdSetStep1==1)
    {
		GTZMHDCmdSetStep2=1;
		SetState1GTZMHD();
		OSTimeDly(OS_TICKS_PER_SEC/5);
		GTZMHDCmdSetStep1=0;
    }
	//设置云台参数 焦距
	if(GTZMHDCmdSetStep2==1)
    {
		GTZMHDCmdSetOk=0;
		DoGTZMHDCmdSetRet=1;
		SetState2GTZMHD();
		OSTimeDly(OS_TICKS_PER_SEC/2);
		GTZMHDCmdSetStep2=0;
    }
    
    if(DoGTZMHDCmdSetRet==1)
	{
		DoGTZMHDCmdSetRet=0;
		SendGTZMHDCmdSetRet(GTZMHDCmdSetOk);//发出设置云台的结果
	}

	//获取云台参数俯仰角和方位角
	if(GTZMHDCmdGetStep1==1)
	{
		GTZMHDCmdGetStep2=1;
		GetState1GTZMHD();
		OSTimeDly(OS_TICKS_PER_SEC/5);
		GTZMHDCmdGetStep1=0;
	}

	//获取云台参数焦距
	if(GTZMHDCmdGetStep2==1)
	{
		GTZMHDCmdGetOk=0;
		DoGTZMHDCmdGetRet=1;
		GetState2GTZMHD();
		OSTimeDly(OS_TICKS_PER_SEC/2);
		GTZMHDCmdGetStep2=0;
	}

	if(DoGTZMHDCmdGetRet==1)
	{
		DoGTZMHDCmdGetRet=0;
		SendGTZMHDCmdGetRet(GTZMHDCmdGetOk);//发出获取云台的结果
	}
}

/* Private function prototypes -----------------------------------------------*/
#if (OS_VIEW_MODULE == DEF_ENABLED)
extern void  App_OSViewTaskCreate   (void);
#endif

static  void  App_TaskStart			(void	*p_arg);  
extern  void  App_TaskCreate        (void);

INT32S main (void)
{
    CPU_INT08U  os_err;
	os_err = os_err; 

   __disable_irq();	// 关闭全局中断，ucosii要求必须先关闭全局中断
	SystemInit();	
    WatchDog_init();
   
	/* Note:  由于使用UCOS, 在OS运行之前运行,注意别使能任何中断. */

    OSInit();                        /* Initialize "uC/OS-II, The Real-Time Kernel". */
	
    Boot_HWDT_Feed();
	os_err = OSTaskCreateExt((void (*)(void *)) App_TaskStart,  /* Create the start task. */
                             (void          * ) 0,
                             (OS_STK        * )&App_TaskStartStk[APP_TASK_START_STK_SIZE - 1],
                             (INT8U           ) APP_TASK_START_PRIO,
                             (INT16U          ) APP_TASK_START_PRIO,
                             (OS_STK        * )&App_TaskStartStk[0],
                             (INT32U          ) APP_TASK_START_STK_SIZE,
                             (void          * )0,
                             (INT16U          )(OS_TASK_OPT_STK_CLR | OS_TASK_OPT_STK_CHK));
	
#if OS_TASK_NAME_EN > 0
    OSTaskNameSet(APP_TASK_START_PRIO, (CPU_INT08U *)"Start Task", &os_err);
#endif

	OSStart(); /* Start multitasking (i.e. give control to uC/OS-II).  */
   
	return (0);
}



/*
*********************************************************************************************************
*                                          App_TaskStart()
*
* Description : The startup task.  The uC/OS-II ticker should only be initialize once multitasking starts.
*
* Argument(s) : p_arg       Argument passed to 'App_TaskStart()' by 'OSTaskCreate()'.
*
* Return(s)   : none.
*
* Caller(s)   : This is a task.
*
* Note(s)     : none.
*********************************************************************************************************
*/	  

static  void  App_TaskStart (void *p_arg)
{
	uint8 err;
	uint8 j;
	
	(void)p_arg;

 	/***************  Initialize hardware ***************/

    Boot_HWDT_Feed();
    OS_CPU_SysTickInit();                                    /* Initialize the SysTick.                              */

    NVIC_SetPriority(SysTick_IRQn, 0x0);
	__enable_irq();											// 开启全局中断

#if (OS_TASK_STAT_EN > 0)
    OSStatInit();                                            /* Determine CPU capacity.                              */
#endif

/* Create application tasks.                               */
  
#if (OS_VIEW_MODULE == DEF_ENABLED)
	App_OSViewTaskCreate();
#endif	
    user_adc_dma_init();    //ADC转换 初始化 并且使能开启
	AM2320_and_iic_init();//温湿度传感器模块初始化
	Boot_HWDT_Feed();		//
	GlobalUserIO_Init();	//用户I/O口初始化
    USART1_Config();        //串口1，用于系统打印调试
    USART2_Config();        //串口2，用于与LINUX系统通讯
    USART3_Config();        //串口3，用于与RF433通讯
    UART4_Config();         //串口4，用于与GPS模块通讯
    UART5_Config();         //串口5，用于与云台相机通讯
	Boot_HWDT_Feed();  		//
	TimerCode_DefaultFunction_Init(4);  //从动轮轮子编码器 定时器、数据采集初始化 
	PWM_Init_Tim8_CH3_CH4();			//电机PWM波初始化配置函数
	SetPwm_Tim8_CH4(0); 
	SetPwm_Tim8_CH3(0);
	Boot_HWDT_Feed();
	zt_motor_slave_driver_init();     //从动轮电机  		   初始化 
    zt_motor_master_driver_init();    //主动轮电机  		   初始化

	  //机器人名称
	  //gRbtState.RobotName[0]='?';
	  gRbtState.RobotName[0]='R';
	  gRbtState.RobotName[1]='b';
	  gRbtState.RobotName[2]='t';
	  gRbtState.RobotName[3]='9';
	  gRbtState.RobotName[4]='9';
	  gRbtState.RobotName[5]='9';
	  gRbtState.RobotName[6]='9';
	  //gRbtState.RobotName[7]='6';
	  
	  gSlaveMtAnaly.s_SlvMtState = SLAVE_MT_CTRL_HANDLE ;
	  gRbtState.bRf433Mode3 =false;
	  
	  gRbtState.StateSwitchFlag[0] = 'I' ;	//默认手动模式，通过发指令切换

	  //设置云台相关参数配置初始化
      CurrentPtzSetCom=0;
	  GTZMHDCmdSetStep1=0;
      GTZMHDCmdSetStep2=0;
	  GTZMHDCmdSetOk=0;
	  DoGTZMHDCmdSetRet=0;

	  //获取云台相关参数配置初始化
      GTZMHDCmdGetStep1=0;
      GTZMHDCmdGetStep2=0;
	  GTZMHDCmdGetOk=0;
      DoGTZMHDCmdGetRet=0;

	  //从动轮轮子相关初始化
	  CodeAB_Start=0;
	  CodeZ_Start=0;
      gCodeZ=0;
      CodeAB_Last=0;
	  Whl_CodeZ_init( );
		

	//测试过程 需要默认开启各种功能
	GpioSetBit(GPIO_BAT_CHARGE_EN,0);
    GpioSetBit(GPIO_MOTOR_PWR_EN,0);
    GpioSetBit(GPIO_12V_EN,1);
    GpioSetBit(GPIO_CAM_AV_EN,1);
    GpioSetBit(GPIO_CAM_24V_EN,1);
	GpioSetBit(GPIO_CAM_12V_EN,1);
    GpioSetBit(GPIO_GPS_EN,1);
    GpioSetL(GPIO_CTR_RF433_M0);
    GpioSetL(GPIO_CTR_RF433_M1);



	Boot_HWDT_Feed();
	
    log_sem 	= OSSemCreate(1);
    p_msgBuffer = OSMemCreate(msgBuffer,100,sizeof(DATA_CONTROL_BLOCK),&err);

	mem160ptr	= OSMemCreate(mem_160,MEM160_NUM,220,&err);
	
	mem512_sem  = OSSemCreate(MEM512_NUM);
	mem512ptr   = OSMemCreate(mem_512,MEM512_NUM,512,&err);	

	//队列相关
    GetWeatherQueue = OSQCreate(&weather_Q[0],WEATHER_Q_LEN);
	RemoteRecQueue = OSQCreate(&RemoteRec_Q[0],TIMER_Q_LEN);
	timerQueue = OSQCreate(&timer_Q[0],TIMER_Q_LEN);
	LocalQueue = OSQCreate(&local_Q[0],LOCAL_LENGH);

    PtzSem = OSSemCreate(1);

	
/*
	//硬件测试程序

	while(1)
		
	{
	
    	
		
		GpioSetBit( GPIO_BAT_CHARGE_EN,0);
    	GpioSetBit(GPIO_MOTOR_PWR_EN,0);
    	GpioSetBit(GPIO_12V_EN,1);
    	GpioSetBit(GPIO_CAM_AV_EN,1);
    	GpioSetBit(GPIO_CAM_24V_EN,1);
		GpioSetBit(GPIO_CAM_12V_EN,1);
    	GpioSetBit(GPIO_GPS_EN,1);
		GpioSetBit(GPIO_BREAK_MEN,1);
		//GpioSetL(GPIO_LED_OUT_SHOW1);
		//GpioSetL(GPIO_LED_OUT_SHOW2);

		GpioSetBit(GPIO_CTL_ARM_RST,1);
		
		//SetPwm_Tim8_CH4(50); 
		//SetPwm_Tim8_CH3(50);

		//SET_MASTER_MOTOR_ZZ() ;
		//SET_MASTER_MOTOR_FZ() ;

		//SET_SLAVE_MOTOR_ZZ();
		//SET_SLAVE_MOTOR_FZ();
		
		//for(;;)


	}
*/

	
	App_TaskCreate();
	HWDT_Feed();
	OSTimeDly(OS_TICKS_PER_SEC);

	stprintf(ID_DEBUG,"欢迎使用国网富达公司的驻塔机器人\r\n");

	//UCOS版本号
	OSVersion();
	stprintf(ID_DEBUG,"UCOS版本号:");
	nprintf(ID_DEBUG,OS_VERSION,0,DEC);
	stprintf(ID_DEBUG,"\r\n");
	
	
	for(j=0;j<5;j++)			  //启动妙算前的延时5秒
	{
		HWDT_Feed();
    	OSTimeDly(OS_TICKS_PER_SEC);
	}
	GpioSetL(GPIO_LED_SHOW2);
	GpioSetL(GPIO_LED_OUT_SHOW1);
    GpioSetH(GPIO_OUT_SPARE1);    //启动妙算
    for(j=0;j<3;j++)              //启动妙算时拉低引脚3秒
	{
		HWDT_Feed();
    	OSTimeDly(OS_TICKS_PER_SEC);
	}
	GpioSetH(GPIO_LED_SHOW2);
	GpioSetH(GPIO_LED_OUT_SHOW1);
    GpioSetL(GPIO_OUT_SPARE1);		//释放引脚
	HWDT_Feed();
	j=0;
	for(;;)
   	{
        j=j+1;
		//GpioSetH(GPIO_LED_OUT_SHOW1);
		//GpioSetH(GPIO_LED_OUT_SHOW2);
		//GpioSetH(GPIO_LED_SHOW1);
		//GpioSetH(GPIO_LED_SHOW2);
		HWDT_Feed();
		OSTimeDly(OS_TICKS_PER_SEC/2);
		//GpioSetL(GPIO_LED_OUT_SHOW1);
		//GpioSetL(GPIO_LED_OUT_SHOW2);
		//GpioSetL(GPIO_LED_SHOW1);
		//GpioSetL(GPIO_LED_SHOW2);
		HWDT_Feed();
		OSTimeDly(OS_TICKS_PER_SEC/2);

        DoPTZ();
		
		if((j & 0x01)== 0)//正式程序中此处不运行
		{
			#ifndef MOTER_PERFORM_TEST_EN
			zt_msg_send_real_time_pkg();	
			#endif
		}
    }	
}

//创建巡检机器人的具体任务
static void  App_TaskCreate (void)
{
    CPU_INT08U  os_err;

	os_err = os_err; 

	//时间任务
    os_err = OSTaskCreateExt (Task_Timer,	
					(void *)0,            
					&TaskStk_Timer[TIMER_LENGH - 1],
					TIMER_TASK_PRIO,
					TIMER_TASK_ID,
					&TaskStk_Timer[0],
					TIMER_LENGH,
					(void *)0,
					OS_TASK_OPT_STK_CHK+OS_TASK_OPT_STK_CLR
					);
	#if OS_TASK_NAME_EN > 0
		OSTaskNameSet(TIMER_TASK_PRIO, "Task_Timer", &os_err);
	#endif

	//打印输出任务	
    os_err = OSTaskCreateExt((void (*)(void *)) Task_Weather, 		
						(void 		 * ) 0, 						
						(OS_STK		 * )&TaskStk_Weather[WEATHER_LENGH - 1],		
						(INT8U		   ) WEATHER_TASK_PRIO,
					    (INT16U          ) WEATHER_TASK_ID,
			            (OS_STK        * )&TaskStk_Weather[0],
			            (INT32U          ) WEATHER_LENGH,
			            (void          * )0,
			            (INT16U         )(OS_TASK_OPT_STK_CLR | OS_TASK_OPT_STK_CHK)	);							
		
	#if OS_TASK_NAME_EN > 0
		OSTaskNameSet(WEATHER_TASK_PRIO, "Task_Weather", &os_err);
	#endif


	OSTimeDly(OS_TICKS_PER_SEC);
	
	//命令解析任务
   os_err = OSTaskCreateExt((void (*)(void *)) Task_Remote_Rec, 		
							(void 		 * ) 0, 						
							(OS_STK		 * )&TaskStk_Remote_Rec[REMOTE_REC_LENGH - 1],		
							(INT8U		   ) REMOTE_REC_TASK_PRIO,
							(INT16U        ) REMOTE_REC_TASK_ID,
			                (OS_STK      * )&TaskStk_Remote_Rec[0],
			                (INT32U        ) REMOTE_REC_LENGH,
			                (void        * )0,
			                (INT16U        )(OS_TASK_OPT_STK_CHK+OS_TASK_OPT_STK_CLR)	);							
		
	#if OS_TASK_NAME_EN > 0
		OSTaskNameSet(REMOTE_REC_TASK_PRIO, "Task_Remote_Rec", &os_err);
	#endif  
   
	 //本地任务																													
	 os_err =OSTaskCreateExt(Task_Local,		
					(void *)0,            
					&TaskStk_Local[LOCAL_LENGH - 1],
					LOCAL_TASK_PRIO,
					LOCAL_TASK_ID,
					&TaskStk_Local[0],
					LOCAL_LENGH,
					(void *)0,
					OS_TASK_OPT_STK_CHK+OS_TASK_OPT_STK_CLR
					);
    #if OS_TASK_NAME_EN > 0
		OSTaskNameSet(LOCAL_TASK_PRIO, "Task_Local", &os_err);
	#endif  
					
	/* 
	//远程任务
	os_err =OSTaskCreateExt (Task_Remote,		
					(void *)0,            
					&TaskStk_Remote[REMOTE_LENGH - 1],
					REMOTE_TASK_PRIO,
					REMOTE_TASK_ID,
					&TaskStk_Remote[0],
					REMOTE_LENGH,
					(void *)0,
					OS_TASK_OPT_STK_CHK+OS_TASK_OPT_STK_CLR
					);	
	*/

}							 



#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *   where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif

/*********************************************************************************************************
      END FILE
*********************************************************************************************************/
