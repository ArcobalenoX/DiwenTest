/*****************************************************************************************
cmd_queue.c中共5个函数：清空指令数据queue_reset()、从串口添加指令数据queue_push()、
从队列中取一个数据queue_pop().获取队列中有效数据个数queue_size()、从指令队列中取出一条完整的指令queue_find_cmd（）
----------------------------------------------------------------------------------------*/
#include "cmd_queue.h"

typedef struct _QUEUE
{
    u16 _head;                                                       //队列头
    u16 _tail;                                                       //队列尾
    u8 _data[QUEUE_MAX_SIZE];                                       //队列数据缓存区
} QUEUE;

static QUEUE que = {0, 0, 0};                                          //指令队列
static u16 cmd_pos = 0;                                              //当前指令指针位置

/*!
*  \brief  清空指令数据
*/
void queue_reset()
{
    que._head = que._tail = 0;
    cmd_pos  = 0;
}
/*!
* \brief  添加指令数据
* \detial 串口接收的数据，通过此函数放入指令队列
*  \param  _data 指令数据
*/
void queue_push(u8 _data)
{
    u16 pos = (que._head + 1) % QUEUE_MAX_SIZE;
    if(pos != que._tail)                                              //非满状态
    {
        que._data[que._head] = _data;
        que._head = pos;
    }
}

//从队列中取一个数据
static void queue_pop(u8* _data)
{
    if(que._tail != que._head)                                        //非空状态
    {
        *_data = que._data[que._tail];
        que._tail = (que._tail + 1) % QUEUE_MAX_SIZE;
    }
}

//获取队列中有效数据个数
static u16 queue_size()
{
    return ((que._head + QUEUE_MAX_SIZE - que._tail) % QUEUE_MAX_SIZE);
}
/*!
*  \brief  从指令队列中取出一条完整的指令
*  \param  cmd 指令接收缓存区
*  \param  buf_len 指令接收缓存区大小
*  \return  指令长度，0表示队列中无完整指令
*/
u16 queue_find_cmd(u8 *buffer, u16 buf_len)
{
    u16 cmd_size = 0;
    u8 _data = 0;

    while(queue_size() > 0)
    {
        //取一个数据
        queue_pop(&_data);

        if(cmd_pos == 0 && _data != CMD_HEAD1)                         //指令第一个字节必须是帧头，否则跳过
        {
            continue;
        }
        if(cmd_pos == 1 && _data != CMD_HEAD2)                         //指令第一个字节必须是帧头，否则跳过
        {
            cmd_pos = 0;
            continue;
        }
        if(cmd_pos < buf_len)                                         //防止缓冲区溢出
            buffer[cmd_pos++] = _data;

        if(buffer[3] == cmd_pos - 1 )																		//指令结束
        {
            return cmd_size;				//指令字节长度
        }

    }
    return 0;                                                         //没有形成完整的一帧
}
