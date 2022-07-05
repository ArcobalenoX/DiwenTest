#ifndef _CMD_QUEUE
#define _CMD_QUEUE

#include "sys.h"
#include "delay.h"


#define CMD_HEAD1 0X5A            //֡ͷ
#define CMD_HEAD2 0XA5            //֡ͷ
#define CRC16_ENABLE 0            // �����ҪCRC16У�鹦�ܣ��޸Ĵ˺�Ϊ1(��ʱ��Ҫ�ڹ�������CRCУ��) ���� ��֪����������CRC��ʽ

#define QUEUE_MAX_SIZE 512        // ָ����ջ�������С��������Ҫ�������������ô�һЩ


/*! 
*  \brief  ���ָ������
*/
void queue_reset(void);

/*! 
* \brief  ���ָ������
* \detial ���ڽ��յ����ݣ�ͨ���˺�������ָ����� 
*  \param  _data ָ������
*/
void queue_push(u8 _data);

/*! 
*  \brief  ��ָ�������ȡ��һ��������ָ��
*  \param  cmd ָ����ջ�����
*  \param  buf_len ָ����ջ�������С
*  \return  ָ��ȣ�0��ʾ������������ָ��
*/
u16 queue_find_cmd(u8 *cmd,u16 buf_len);

#endif
