#ifndef _DWIN_H
#define _DWIN_H

#include "cmd_queue.h"
#include "usart.h"



#define TEST_BTN_ADDR 0x5000		//点击按键
#define TEST_LED_ADDR 0x5002		//指示按键


#define TEST_TEXT_ADDR 0x5004			//显示数字
#define TEST_NUMBER_ADDR 0x5006		//输入数字






#define CMD_MAX_SIZE 16         // 单条指令大小，根据需要调整，尽量设置大一些


//16位short与8位char互转
typedef union
{
    u16 shortdata;
    u8 chardata[2];
} short_char_union;
//32位int与8位char互转
typedef union
{
    u32 intdata;
    u8 chardata[4];
} int_char_union;

//32位float与8位char互转
typedef union
{
    float floatdata;
    u8 chardata[4];
} float_char_union;


#pragma pack(push)
#pragma pack(1)                           //按字节对齐
typedef struct
{
    u8    cmd_head1;                    //帧头5A
    u8		cmd_head2;										//帧头A5
    u8    cmd_len;											//后续长度
    u8    cmd_type;                    //命令类型(0x83)
    u8		addr_h;											//变量高地址
    u8		addr_l;											//变量低地址
    u8		data_len;										//数据长度
    u8		data[4];										//数据值

} CTRL_MSG;
#pragma pack(pop)



/****************************************************************************
* 名    称： Uart2
* 功    能： 串口
* 入口参数： 无
* 出口参数： 无
****************************************************************************/
void DwinUartInit(u32 Baudrate);
void SendChar(u8 t);
void sendArray(void * buf, u8 len);


/************************************************************************************
* @name		:	write_to_dwin
* @brief	:	发送向迪文屏写数据的指令
* @param	: addr 变量地址
						value/data 数据
* @retval	:
************************************************************************************/
/*
写变量存储器指令指令（0x82） 此处以向 1000 变量地址里写数值 2 为例： 5AA5 05 82 1000 0002
5AA5 表示：帧头
05 表示：数据长度
82 表示：写变量存储器指令
1000 表示：变量地址（两个字节）
0002 表示：数据 2（两个字节）
解释：通过指令往 0000 地址里面赋值 2，屏上的显示，数据变量整数类型 2
注：DGUS 屏读、写变量的地址都可以看作为起始地址：例如下面 a、b 指令的效果等于 c
a: 5A A5 05 82 1001 000A
b: 5A A5 05 82 1002 000B
c: 5A A5 07 82 1001 000A 000B
*/

void write_to_dwin(u8 addr_h, u8 addr_l, u8 value_h, u8 value_l);
void write_u8_to_dwin(u16 addr, u8 data);
void write_u16_to_dwin(u16 addr, u16 data);
void write_u32_to_dwin(u16 addr, u32 data);
void write_float_to_dwin(u16 addr, float data);





/************************************************************************************
* @name		:	read_from_dwin
* @brief	:	发送从迪文屏读数据的指令
* @param	: addr 变量地址
* @retval	:
************************************************************************************/
/*
读变量存储器指令指令（0x83） 此处以读 1000 变量地址里的数值（假设当前数值为 2）为例： 5AA5 04 83 1000 01
5AA5 表示：帧头
04 表示：数据长度
83 表示：读数据存储区指令
1000 表示：变量地址（两个字节）
01 表示：从 1000 地址开始读 1 个字长度，数据指令最大容许长度 0x7c
读完以后屏会向串口返回读应答： 5AA5 06 83 1000 01 0002
5AA5 表示：帧头 06 表示：数据长度
83 表示：读变量存储器指令指令
1000 表示：变量地址（两个字节）
01 表示：从 1000 地址开始读 1 个字长度数据
00 02 表示：1000 地址里的数据值是 2
*/


void read_from_dwin(u8 addr_h, u8 addr_l, u8 size);
void read_u8_from_dwin(u16 addr);
void read_u16_from_dwin(u16 addr);
void read_u32_from_dwin(u16 addr);
void read_float_from_dwin(u16 addr);



/************************************************************************************
* @name		:	ProcessMessage
* @brief	:	处理迪文屏返回的数据
* @param	: 
* @retval	:
************************************************************************************/
void ProcessMessage( CTRL_MSG *msg);
void dwin_ack(void);


#endif



