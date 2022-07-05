#include "dwin.h"

u8  cmd_buffer[CMD_MAX_SIZE];   //指令缓存


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
    u8 write_cmd[8] = {0x5A, 0xA5, 0x05, 0x82, addr_h, addr_l, value_h, value_l};
    sendArray(write_cmd, sizeof(write_cmd));
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
    u8 write_cmd[10] = {0x5A, 0xA5, 0x07, 0x82, m.chardata[1], m.chardata[0], n.chardata[3], n.chardata[2], n.chardata[1], n.chardata[0]};
    sendArray(write_cmd, sizeof(write_cmd));
}

void write_float_to_dwin(u16 addr, float data)
{
    short_char_union m;
    m.shortdata = addr;
    float_char_union n;
    n.floatdata = data;
    u8 write_cmd[10] = {0x5A, 0xA5, 0x07, 0x82, m.chardata[1], m.chardata[0], n.chardata[3], n.chardata[2], n.chardata[1], n.chardata[0]};
    sendArray(write_cmd, sizeof(write_cmd));
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
    u8 read_cmd[7] = {0x5A, 0xA5, 0x04, 0x83, addr_h, addr_l, size};
    sendArray(read_cmd, sizeof(read_cmd));
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



/*!
*  \brief  消息处理流程
*  \param msg 待处理消息
*  \param size 消息长度
*/
void ProcessMessage(CTRL_MSG *msg)
{
    static u8 Flag = 0;

    u8 cmd_type = msg->cmd_type;                 	//命令类型(0x83)
    u8 addr_h   = msg->addr_h;										//变量高地址
    u8 addr_l   = msg->addr_l;										//变量低地址
    u8 data_len = msg->data_len;									//数据长度

    u16 addr = (addr_h << 8) + (addr_l);

    u8 data8 = 0;
    u16 data16 = 0;
    u32 data32 = 0;

    if(data_len == 1)
    {
        data8 = msg->data[0];
        printf("recv data %d\r\n", data8);
    }
    else if(data_len == 2)
    {
        short_char_union m;
        m.chardata[1] = msg->data[0];
        m.chardata[0] = msg->data[1];
        data16 = m.shortdata;
        printf("recv data %d\r\n", data16);
    }
    else if(data_len == 4)
    {
        int_char_union n;
        n.chardata[3] = msg->data[0];
        n.chardata[2] = msg->data[1];
        n.chardata[1] = msg->data[2];
        n.chardata[0] = msg->data[3];
        data32 = n.intdata;
        printf("recv data %d\r\n", data32);
    }

    switch(addr)
    {
    case TEST_BTN_ADDR:

        //触摸屏按下
        printf("recv over\r\n");

        if(Flag)
        {
            write_u8_to_dwin(TEST_LED_ADDR, Flag);
            Flag = 0;
        }
        else
        {
            write_u8_to_dwin(TEST_LED_ADDR, Flag);
            Flag = 1;
        }
        break;

    case TEST_NUMBER_ADDR:
        write_u16_to_dwin(TEST_TEXT_ADDR, data16);
        break;


    default:
        break;
    }
}

/*!
*  \brief  外部程序周期调用
*/
void dwin_ack(void)
{
    u16  size = 0;
    size = queue_find_cmd(cmd_buffer, CMD_MAX_SIZE);                             //从缓冲区中获取一条指令
    if(size > 0)                                   //接收到指令
    {
        ProcessMessage((CTRL_MSG*)cmd_buffer);                             //指令处理
    }
}


