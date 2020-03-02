//#include "box_os_user.h"
#include <includes.h>

TIM_TypeDef* const gHardTimerList[MAX_HARD_TIMER_CNT+1] = {
    TIM1 , TIM1 , TIM2 , TIM3 , TIM4 , TIM5 , TIM6 , TIM7 , TIM8
};

//////////////////////////////////////////////////////////////////////////
//�����ʱ���� ���������� IO�����ú���  ��Ҫ����ÿ����ʱ���� 1 2 ͨ��Ϊ��ʱ�����ܸ���
void TimerCode_And_Gpio_Init_TIM1(void)
{
    //BOX_ERROR_HANDLER( 777 );  //δʵ�� ���󱨾�
}

//	�ӵ���������ɼ�	PA15 TIM2_CH1      PB3->TIM2_CH2 
//01������ӳ��(CH1/ETR/PA15��CH2/PB3��CH3/PA2��CH4/PA3)
void TimerCode_And_Gpio_Init_TIM2(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2 ,ENABLE);    /*ʹ��TIM2 ʱ��*/
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB , ENABLE); /*ʹ��GPIOA/GPIOB ʱ��*/

    GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable ,ENABLE);  //�Ƚ�PA15��Ϊ��ͨIO��
    GPIO_PinRemapConfig( GPIO_PartialRemap1_TIM2 ,ENABLE);  //����ʱ���ܽ�ӳ�䵽pd12 13 14 15����

    GPIO_InitStructure.GPIO_Speed  =GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15 ;
    GPIO_Init(GPIOA,&GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Speed  =GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 ;
    GPIO_Init(GPIOB,&GPIO_InitStructure);
}

//	������������ɼ�
//10������ӳ��(CH1/PB4 ��CH2/PB5��CH3/PB0��CH4/PB1)
//PB4 �C 90 �C CK_CODEA_M2 �C TIM3_CH1 ����ӳ�䣩�C �����ֱ�����Aͨ���źŲɼ�
//PB5 �C 91 �C CK_CODEB_M2 �C TIM3_CH2 ����ӳ�䣩�C �����ֱ�����Bͨ���źŲɼ�
void TimerCode_And_Gpio_Init_TIM3(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);    /*ʹ��TIM3ʱ��*/
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);   /*ʹ��GPIOBʱ��*/
    GPIO_PinRemapConfig(GPIO_PartialRemap_TIM3,ENABLE);   //����ʱ���ܽ�ӳ�䵽pd12 13 14 15����

    GPIO_InitStructure.GPIO_Speed  =GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4|GPIO_Pin_5 ; /*timer3 ��ӳ���ͨ��1 2*/
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
    GPIO_Init(GPIOB,&GPIO_InitStructure);
}

//1����ȫӳ��(TIM4_CH1/PD12 ��TIM4_CH2/PD13��TIM4_CH3/PD14��TIM4_CH4/PD15) 
// PD12 �C 59 �C CK_CODEB_CD �C TIM4_CH1����ӳ�䣩�C �Ӷ������ӱ�����Bͨ���źŲɼ�
// PD13 �C 60 �C CK_CODEA_CD �C TIM4_CH2����ӳ�䣩�C �Ӷ������ӱ�����Aͨ���źŲɼ�
// PD14 �C 61 �C CK_CODEZ_CD �C TIM4_CH3����ӳ�䣩�C �Ӷ������ӱ�����Zͨ���źŲɼ�
void TimerCode_And_Gpio_Init_TIM4(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4,ENABLE);    /*ʹ��TIM4ʱ��*/
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD,ENABLE);   /*ʹ��GPIODʱ��*/
    GPIO_PinRemapConfig(GPIO_Remap_TIM4,ENABLE);  		  //����ʱ���ܽ�ӳ�䵽pd12 13 14 15����

    GPIO_InitStructure.GPIO_Speed  =GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12|GPIO_Pin_13; /*timer4 ��ӳ���ͨ��1 2*/
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
    GPIO_Init(GPIOD,&GPIO_InitStructure);
}


typedef void (*VoidFuncPtr) (void); 
const VoidFuncPtr TimerCode_And_Gpio_Inits[MAX_HARD_TIMER_CNT+1] = {
    TimerCode_And_Gpio_Init_TIM1 ,
    TimerCode_And_Gpio_Init_TIM1 ,
    TimerCode_And_Gpio_Init_TIM2 ,
    TimerCode_And_Gpio_Init_TIM3 ,
    TimerCode_And_Gpio_Init_TIM4 ,
    //TimerCode_And_Gpio_Init_TIM5 ,
    //TimerCode_And_Gpio_Init_TIM6 ,
    //TimerCode_And_Gpio_Init_TIM7 ,
    //TimerCode_And_Gpio_Init_TIM8 
};

//��ʱ�� �������ɼ���������  TIM1  ~  TIM8
void TimerCode_DefaultFunction_Init(u32 timer_ID)
{
    TIM_TimeBaseInitTypeDef     TIM_TimeBaseStructure;
    TIM_ICInitTypeDef           TIM4_ICInitStructure;
    
    (*TimerCode_And_Gpio_Inits[timer_ID])();

    TIM_DeInit( gHardTimerList[timer_ID] );
    TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
    TIM_TimeBaseStructure.TIM_Period        = 0xFFFF;				//�趨��������װֵ
    TIM_TimeBaseStructure.TIM_Prescaler     = 0;					//TIM4ʱ��Ԥ��Ƶֵ
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1 ;		//����ʱ�ӷָ� T_dts = T_ck_int	
    TIM_TimeBaseStructure.TIM_CounterMode   = TIM_CounterMode_Up; //TIM���ϼ��� 
    TIM_TimeBaseInit( gHardTimerList[timer_ID] , &TIM_TimeBaseStructure);

    TIM_EncoderInterfaceConfig( gHardTimerList[timer_ID] , TIM_EncoderMode_TI12, TIM_ICPolarity_Rising ,TIM_ICPolarity_Rising);//ʹ�ñ�����ģʽ3����������
    TIM_ICStructInit(&TIM4_ICInitStructure);						//���ṹ���е�����ȱʡ����   ͨ��1����
    TIM4_ICInitStructure.TIM_ICFilter = 0;							//ѡ������Ƚ��˲��� 
    TIM_ICInit( gHardTimerList[timer_ID] , &TIM4_ICInitStructure);						//��TIM_ICInitStructure�е�ָ��������ʼ��TIM4

    TIM_ITConfig( gHardTimerList[timer_ID] , TIM_IT_Update, DISABLE );						//���и����ж� ����

    gHardTimerList[timer_ID]->CNT = 0; //��ʼ��ʱ��ռ���
    TIM_Cmd( gHardTimerList[timer_ID] , ENABLE);											//����TIM?��ʱ��
}



/************************************************* 
*Function:	 PWM_Init_Tim8_CH3_CH4	
*Input:			
*OUTPUT:		    
*Return:		
*DESCRIPTION:���PWM����ʼ�����ú���
*           PC8 �C 65 �C MT_CTL_PWM1 �C TIM8_CH3 �C ������PWM����
*           PC9 �C 66 �C MT_CTL_PWM2 �C TIM8_CH4 �C �Ӷ���PWM����
*************************************************/
void PWM_Init_Tim8_CH3_CH4(void)
{
    GPIO_InitTypeDef            GPIO_InitStructure;
    TIM_TimeBaseInitTypeDef     TIM_TimeBaseStructure;
    TIM_OCInitTypeDef           TIM_OCInitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE); //��C��GPIOʱ��
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM8,ENABLE);  // �򿪶�ʱ��8ʱ��

    GPIO_InitStructure.GPIO_Pin=GPIO_Pin_8|GPIO_Pin_9;//
    GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF_PP;  
    GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;   
    GPIO_Init(GPIOC,&GPIO_InitStructure);


    //   TIM_TimeBaseStructure.TIM_RepetitionCounter=1;  . 
    TIM_TimeBaseStructure.TIM_ClockDivision=0;
    TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up; //���ϼ���
    TIM_TimeBaseStructure.TIM_Period=100;
    TIM_TimeBaseStructure.TIM_Prescaler=360;
    TIM_TimeBaseInit(TIM8,&TIM_TimeBaseStructure);

    TIM_OCInitStructure.TIM_OCMode      = TIM_OCMode_PWM2        ;
    TIM_OCInitStructure.TIM_OCPolarity  = TIM_OCPolarity_Low     ;
    TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_Low    ;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable ;
    TIM_OCInitStructure.TIM_Pulse       = 0                      ;

    TIM_OCInitStructure.TIM_OutputNState    = TIM_OutputNState_Disable;
    TIM_OCInitStructure.TIM_OCNIdleState    = TIM_OCNIdleState_Reset;
    TIM_OCInitStructure.TIM_OCIdleState     = TIM_OCIdleState_Reset ;

    TIM_OC3Init(TIM8,&TIM_OCInitStructure);
    TIM_OC3PreloadConfig(TIM8,TIM_OCPreload_Enable); //TIM_8---->ͨ��3[PC8] 

    TIM_OC4Init(TIM8,&TIM_OCInitStructure);
    TIM_OC4PreloadConfig(TIM8,TIM_OCPreload_Enable); //TIM_8---->ͨ��4[PC9]

    TIM_CtrlPWMOutputs(TIM8,ENABLE);
    TIM_Cmd(TIM8,ENABLE);
}


void timer1_8_pwm_init(TIM_TypeDef* TIMx,uint16_t Prescaler,uint16_t Period)
{
    TIM_TimeBaseInitTypeDef     TIM_TimeBaseStructure;
    TIM_DeInit(TIMx);
    //   TIM_TimeBaseStructure.TIM_RepetitionCounter=1;
    TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseStructure.TIM_CounterMode   = TIM_CounterMode_Up; //���ϼ���
    TIM_TimeBaseStructure.TIM_Period   = Period;
    TIM_TimeBaseStructure.TIM_Prescaler= Prescaler ;
    TIM_TimeBaseInit(TIMx,&TIM_TimeBaseStructure);
    TIM_CtrlPWMOutputs(TIMx,ENABLE);
}

void timer2_3_4_5_pwm_init(TIM_TypeDef* TIMx,uint16_t Prescaler,uint16_t Period)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure; 
    //RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
    TIM_DeInit(TIMx);
    ////TIM_InternalClockConfig(TIMx);
    TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
    TIM_TimeBaseStructure.TIM_ClockDivision=0;
    TIM_TimeBaseStructure.TIM_Period   = Period ;//;//ARR��ֵ����10K
    TIM_TimeBaseStructure.TIM_Prescaler= Prescaler ;
    TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up; //���ϼ���ģʽ
    TIM_TimeBaseInit(TIMx, &TIM_TimeBaseStructure);
}


void timer2_3_4_5_pwm_channel_init(TIM_TypeDef* TIMx,tim_ocX_init ocX_init)
{
    TIM_OCInitTypeDef TIMOCInitStructure;
    TIMOCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; //PWMģʽ1���
    TIMOCInitStructure.TIM_Pulse = TIMx->ARR/2 ; //  0;//ռ�ձ�=(CCRx/ARR)*100%
    TIMOCInitStructure.TIM_OCPolarity  = TIM_OCPolarity_High;//TIM����Ƚϼ��Ը�
    TIMOCInitStructure.TIM_OutputState = TIM_OutputState_Enable;//ʹ�����״̬
    ocX_init(TIMx, &TIMOCInitStructure);//TIM3��CH2��� TIM_OC2Init
}
void timer1_8_pwm_channel_init(
    TIM_TypeDef*            TIMx,
    tim_ocX_init            ocX_init,
    tim_ocX_preload_init    ocX_preload_init
)
{
    TIM_OCInitTypeDef TIMOCInitStructure;
        
    //TIMOCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; //PWMģʽ1���
    TIMOCInitStructure.TIM_Pulse       = TIMx->ARR/2            ; //  0;//ռ�ձ�=(CCRx/ARR)*100%
    TIMOCInitStructure.TIM_OCMode      = TIM_OCMode_PWM1        ;
    TIMOCInitStructure.TIM_OCPolarity  = TIM_OCPolarity_Low     ;
    TIMOCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_Low    ;
    TIMOCInitStructure.TIM_OutputState = TIM_OutputState_Enable ;

    TIMOCInitStructure.TIM_OutputNState    = TIM_OutputNState_Disable;
    TIMOCInitStructure.TIM_OCNIdleState    = TIM_OCNIdleState_Reset;
    TIMOCInitStructure.TIM_OCIdleState     = TIM_OCIdleState_Reset ;

    ocX_init( TIMx ,&TIMOCInitStructure);
    ocX_preload_init( TIMx , TIM_OCPreload_Enable ); //TIM_8---->ͨ��3[PC8] 
}

