#ifndef _SC16SI7X2_H_

#define SC16_RHR 0x00
#define SC16_THR 0x00
#define SC16_IER 0x01

#define SC16_FCR 0x02
#define SC16_IIR 0x02

#define SC16_LCR 0x03
#define SC16_MCR 0x04
#define SC16_LSR 0x05

#define SC16_MSR 0x06
#define SC16_SPR 0x07
#define SC16_TLR 0x07

#define SC16_RXLVL 0x09		
#define SC16_EFCR 0x0F
//------SC16_-------------------
//Special register set
//-------------------------
#define SC16_DLL 0x00
#define SC16_DLH 0x01

//-------------------------
//Enhanced register set
//-------------------------
#define SC16_EFR 0x02

//-------------------------
//Function:
//-------------------------
#define SC16IS752A 0x98//0x22
#define SC16IS752B 0x92//0x22
#define ChannelA 0x00
#define ChannelB 0x02

#define SC16IS752_A_FIFO_LEN 56
#define SC16IS752_RTX_LENGTH 220
typedef struct 
{
	uint8 counter;
	uint8 len;
	uint8  buffer[220];
}SC16IS752_BUF_A_T;

extern void SC16IS752_Reset(void);
//extern void WriteRegToExtUart(uint8 Reg,uint8 Channel,uint8 Value);
extern void RecvChannel1A(uint8 Reg,uint8 Channel,uint8 *dp,uint16 length);
extern void RecvChannel1B(uint8 Reg,uint8 Channel,uint8 *dp,uint16 length);
extern void RecvChannel2A(uint8 Reg,uint8 Channel,uint8 *dp,uint16 length);
extern void RecvChannel2B(uint8 Reg,uint8 Channel,uint8 *dp,uint16 length);
extern void WriteRegisterB(uint8 Reg,uint8 Channel,uint8 Value);
extern void ReadRegisterB(uint8 Reg,uint8 Channel,uint8 *dp);
extern void WriteRegister(uint8 Reg,uint8 Channel,uint8 Value);
extern void ReadRegister(uint8 Reg,uint8 Channel,uint8 *dp);
extern void RS485A_sendst (uint8 *sp,uint8 len);
extern void RS485B_sendst(uint8 *sp,uint8 len);
extern void RS232A_sendst (uint8 *sp,uint8 len);
extern void RS232B_sendst (uint8 *sp,uint8 len);

extern void EINT0_Exception(void);
extern SC16IS752_BUF_A_T SC16IS752_BUF_A0;
extern SC16IS752_BUF_A_T SC16IS752_BUF_A1;
//extern SC16IS752_BUF_A_T SC16IS752_SBUF_A0;
//extern SC16IS752_BUF_A_T SC16IS752_SBUF_A1;

extern SC16IS752_BUF_A_T SC16IS752_BUF_B0;
extern SC16IS752_BUF_A_T SC16IS752_BUF_B1;
//extern SC16IS752_BUF_A_T SC16IS752_SBUF_B0;
//extern SC16IS752_BUF_A_T SC16IS752_SBUF_B1;
#endif
