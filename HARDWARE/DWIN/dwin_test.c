#include "dwin_test.h"


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
		u8  cmd_buffer[CMD_MAX_SIZE];   //指令缓存

    u16  size = 0;
    size = queue_find_cmd(cmd_buffer, CMD_MAX_SIZE);                             //从缓冲区中获取一条指令
    if(size > 0)                                   //接收到指令
    {
        ProcessMessage((CTRL_MSG*)cmd_buffer);                             //指令处理
    }
}