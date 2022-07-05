/*****************************************************************************************
cmd_queue.c�й�5�����������ָ������queue_reset()���Ӵ������ָ������queue_push()��
�Ӷ�����ȡһ������queue_pop().��ȡ��������Ч���ݸ���queue_size()����ָ�������ȡ��һ��������ָ��queue_find_cmd����
----------------------------------------------------------------------------------------*/
#include "cmd_queue.h"

typedef struct _QUEUE
{
    u16 _head;                                                       //����ͷ
    u16 _tail;                                                       //����β
    u8 _data[QUEUE_MAX_SIZE];                                       //�������ݻ�����
} QUEUE;

static QUEUE que = {0, 0, 0};                                          //ָ�����
static u16 cmd_pos = 0;                                              //��ǰָ��ָ��λ��

/*!
*  \brief  ���ָ������
*/
void queue_reset()
{
    que._head = que._tail = 0;
    cmd_pos  = 0;
}
/*!
* \brief  ���ָ������
* \detial ���ڽ��յ����ݣ�ͨ���˺�������ָ�����
*  \param  _data ָ������
*/
void queue_push(u8 _data)
{
    u16 pos = (que._head + 1) % QUEUE_MAX_SIZE;
    if(pos != que._tail)                                              //����״̬
    {
        que._data[que._head] = _data;
        que._head = pos;
    }
}

//�Ӷ�����ȡһ������
static void queue_pop(u8* _data)
{
    if(que._tail != que._head)                                        //�ǿ�״̬
    {
        *_data = que._data[que._tail];
        que._tail = (que._tail + 1) % QUEUE_MAX_SIZE;
    }
}

//��ȡ��������Ч���ݸ���
static u16 queue_size()
{
    return ((que._head + QUEUE_MAX_SIZE - que._tail) % QUEUE_MAX_SIZE);
}
/*!
*  \brief  ��ָ�������ȡ��һ��������ָ��
*  \param  cmd ָ����ջ�����
*  \param  buf_len ָ����ջ�������С
*  \return  ָ��ȣ�0��ʾ������������ָ��
*/
u16 queue_find_cmd(u8 *buffer, u16 buf_len)
{
    u16 cmd_size = 0;
    u8 _data = 0;

    while(queue_size() > 0)
    {
        //ȡһ������
        queue_pop(&_data);

        if(cmd_pos == 0 && _data != CMD_HEAD1)                         //ָ���һ���ֽڱ�����֡ͷ����������
        {
            continue;
        }
        if(cmd_pos == 1 && _data != CMD_HEAD2)                         //ָ���һ���ֽڱ�����֡ͷ����������
        {
            cmd_pos = 0;
            continue;
        }
        if(cmd_pos < buf_len)                                         //��ֹ���������
            buffer[cmd_pos++] = _data;

        if(buffer[3] == cmd_pos - 1 )																		//ָ�����
        {
            return cmd_size;				//ָ���ֽڳ���
        }

    }
    return 0;                                                         //û���γ�������һ֡
}
