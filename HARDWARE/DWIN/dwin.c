#include "dwin.h"



//串口初始化   BaudRate-波特率设置
void DwinUartInit(u32 BaudRate)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);            // GPIO时钟使能
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);            //USART2时钟使能

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_USART2);         //PA2引脚复用为USART2TX
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_USART2);         //PA3引脚复用为USART2RX

    USART_InitStructure.USART_BaudRate = BaudRate;                   //波特率
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;      //8个数据位
    USART_InitStructure.USART_StopBits = USART_StopBits_1;           //1个停止位
    USART_InitStructure.USART_Parity = USART_Parity_No ;             //无奇偶校验位
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//关闭硬件流控
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;  //收发模式
    USART_Init(USART2, &USART_InitStructure);                         //初始化USART2

    USART_Cmd(USART2, ENABLE);                                        //使能USART2
    USART_ITConfig(USART2, USART_IT_PE, ENABLE);
    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);                    //打开USART2的中断
    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 5;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

//发送字节给串口
void  SendChar(u8 data)
{
    while(!(USART_GetFlagStatus(USART2, USART_FLAG_TC) == 1));         //等待发送数据完成
    USART_SendData(USART2, data); //将数据写入数据寄存器中
}

//发送字节数组给串口
void sendArray(void * buf, u8 len)
{
    char *p = (char *)buf;
    while(len --)
    {
        SendChar(*p);
        p++;
    }
}
//串口接收数据
void USART2_IRQHandler(void)
{
    while(USART_GetFlagStatus(USART2, USART_FLAG_RXNE) == 0);
    u8 data = USART_ReceiveData(USART2);
    queue_push(data);
}





/************************************************************************************
* @name		:	write_to_dwin
* @brief	:	串口向迪文屏写数据
* @param	: 	addr_h 变量地址高字节
				addr_l 变量地址低字节
				value_h 变量值高字节
				value_l 变量值低字节
* @retval	:
************************************************************************************/
void write_to_dwin(u8 addr_h, u8 addr_l, u8 value_h, u8 value_l)
{

#if CRC16_ENABLE
    u8 cmd[10] = {0x5A, 0xA5, 0x07, 0x82, addr_h, addr_l, value_h, value_l, 0, 0};
    u16 crc = CRC16_Check(cmd, cmd[2] - 2);
    cmd[sizeof(cmd) - 2] = (crc >> 8);
    cmd[sizeof(cmd) - 1] = crc & 0xff;
    sendArray(cmd, sizeof(cmd));

#elseif
    u8 cmd[8] = {0x5A, 0xA5, 0x05, 0x82, addr_h, addr_l, value_h, value_l};
    sendArray(cmd, sizeof(cmd));

#endif

}

void write_u8_to_dwin(u16 addr, u8 data)
{
    short_char_union m;
    m.shortdata = addr;
    write_to_dwin(m.chardata[1], m.chardata[0], 0x00, data);
}

void write_u16_to_dwin(u16 addr, u16 data)
{
    short_char_union m;
    m.shortdata = addr;
    short_char_union n;
    n.shortdata = data;
    write_to_dwin(m.chardata[1], m.chardata[0], n.chardata[1], n.chardata[0]);
}

void write_u32_to_dwin(u16 addr, u32 data)
{
    short_char_union m;
    m.shortdata = addr;
    int_char_union n;
    n.intdata = data;

#if CRC16_ENABLE
    u8 cmd[12] = {0x5A, 0xA5, 0x09, 0x82, m.chardata[1], m.chardata[0], n.chardata[3], n.chardata[2], n.chardata[1], n.chardata[0], 0, 0};
    u16 crc = CRC16_Check(cmd, cmd[2] - 2);
    cmd[sizeof(cmd) - 2] = (crc >> 8);
    cmd[sizeof(cmd) - 1] = crc & 0xff;
    sendArray(cmd, sizeof(cmd));
#elseif

    u8 cmd[10] = {0x5A, 0xA5, 0x07, 0x82, m.chardata[1], m.chardata[0], n.chardata[3], n.chardata[2], n.chardata[1], n.chardata[0]};
    sendArray(cmd, sizeof(cmd));

#endif

}

void write_float_to_dwin(u16 addr, float data)
{
    short_char_union m;
    m.shortdata = addr;
    float_char_union n;
    n.floatdata = data;

#if CRC16_ENABLE
    u8 cmd[12] = {0x5A, 0xA5, 0x09, 0x82, m.chardata[1], m.chardata[0], n.chardata[3], n.chardata[2], n.chardata[1], n.chardata[0], 0, 0};
    u16 crc = CRC16_Check(cmd, cmd[2] - 2);
    cmd[sizeof(cmd) - 2] = (crc >> 8);
    cmd[sizeof(cmd) - 1] = crc & 0xff;
    sendArray(cmd, sizeof(cmd));

#elseif
    u8 cmd[10] = {0x5A, 0xA5, 0x07, 0x82, m.chardata[1], m.chardata[0], n.chardata[3], n.chardata[2], n.chardata[1], n.chardata[0]};
    sendArray(cmd, sizeof(cmd));

#endif
}

/************************************************************************************
* @name		:	read_from_dwin
* @brief	:	发送从迪文屏读数据的指令
* @param	: addr_h 变量地址高字节
						addr_l 变量地址低字节
						size 数据大小(单位：字/两字节/16位)
* @retval	:
************************************************************************************/
void read_from_dwin(u8 addr_h, u8 addr_l, u8 size)
{
#if CRC16_ENABLE
    u8 cmd[9] = {0x5A, 0xA5, 0x06, 0x83, addr_h, addr_l, size, 0, 0};
    u16 crc = CRC16_Check(cmd, cmd[2] - 2);
    cmd[sizeof(cmd) - 2] = (crc >> 8);
    cmd[sizeof(cmd) - 1] = crc & 0xff;
    sendArray(cmd, sizeof(cmd));

#elseif

    u8 cmd[7] = {0x5A, 0xA5, 0x04, 0x83, addr_h, addr_l, size};
    sendArray(cmd, sizeof(cmd));

#endif
}

void read_u8_from_dwin(u16 addr)
{
    short_char_union m;
    m.shortdata = addr;
    read_from_dwin(m.chardata[1], m.chardata[0], 0x01);
}

void read_u16_from_dwin(u16 addr)
{
    short_char_union m;
    m.shortdata = addr;
    read_from_dwin(m.chardata[1], m.chardata[0], 0x01);
}
void read_u32_from_dwin(u16 addr)
{
    short_char_union m;
    m.shortdata = addr;
    read_from_dwin(m.chardata[1], m.chardata[0], 0x02);
}

void read_float_from_dwin(u16 addr)
{
    short_char_union m;
    m.shortdata = addr;
    read_from_dwin(m.chardata[1], m.chardata[0], 0x02);
}

/************************************************************************************
* @name		:	page_read
* @brief	:	读取当前页面——指令示例：读：5A A5 04 83 0014 01 应答：5A A5 06 83 00 14 01 00 07(0007 为 07 号页面)
* @param	:
* @retval	: 	页面ID
************************************************************************************/
void page_read(void)
{
    read_u8_from_dwin(0x0014);
}
/************************************************************************************
* @name		:	page_change
* @brief	:	页面切换指令——指令示例：写: 5A A5 07 82 0084 5A01 0001,显示背景图片 ICL 文件下的 01 序号图片
* @param	: 	page_id 页面ID
* @retval	:
************************************************************************************/
void page_change(u8 page_id)
{
#if CRC16_ENABLE
    u8 cmd[12] = {0x5A, 0xA5, 0x09, 0x82, 0x00, 0x84, 0x5A, 0x01, 0x00, page_id, 0, 0};
    u16 crc = CRC16_Check(cmd, cmd[2] - 2);
    cmd[sizeof(cmd) - 2] = (crc >> 8);
    cmd[sizeof(cmd) - 1] = crc & 0xff;
    sendArray(cmd, sizeof(cmd));

#elseif

    u8 cmd[10] = {0x5A, 0xA5, 0x07, 0x82, 0x00, 0x84, 0x5A, 0x01, 0x00, page_id};
    sendArray(cmd, sizeof(cmd));

#endif


}




