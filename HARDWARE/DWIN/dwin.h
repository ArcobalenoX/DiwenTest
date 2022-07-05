#ifndef _DWIN_H
#define _DWIN_H

#include "cmd_queue.h"
#include "usart.h"



#define TEST_BTN_ADDR 0x5000		//�������
#define TEST_LED_ADDR 0x5002		//ָʾ����


#define TEST_TEXT_ADDR 0x5004			//��ʾ����
#define TEST_NUMBER_ADDR 0x5006		//��������






#define CMD_MAX_SIZE 16         // ����ָ���С��������Ҫ�������������ô�һЩ


//16λshort��8λchar��ת
typedef union
{
    u16 shortdata;
    u8 chardata[2];
} short_char_union;
//32λint��8λchar��ת
typedef union
{
    u32 intdata;
    u8 chardata[4];
} int_char_union;

//32λfloat��8λchar��ת
typedef union
{
    float floatdata;
    u8 chardata[4];
} float_char_union;


#pragma pack(push)
#pragma pack(1)                           //���ֽڶ���
typedef struct
{
    u8    cmd_head1;                    //֡ͷ5A
    u8		cmd_head2;										//֡ͷA5
    u8    cmd_len;											//��������
    u8    cmd_type;                    //��������(0x83)
    u8		addr_h;											//�����ߵ�ַ
    u8		addr_l;											//�����͵�ַ
    u8		data_len;										//���ݳ���
    u8		data[4];										//����ֵ

} CTRL_MSG;
#pragma pack(pop)



/****************************************************************************
* ��    �ƣ� Uart2
* ��    �ܣ� ����
* ��ڲ����� ��
* ���ڲ����� ��
****************************************************************************/
void DwinUartInit(u32 Baudrate);
void SendChar(u8 t);
void sendArray(void * buf, u8 len);


/************************************************************************************
* @name		:	write_to_dwin
* @brief	:	�����������д���ݵ�ָ��
* @param	: addr ������ַ
						value/data ����
* @retval	:
************************************************************************************/
/*
д�����洢��ָ��ָ�0x82�� �˴����� 1000 ������ַ��д��ֵ 2 Ϊ���� 5AA5 05 82 1000 0002
5AA5 ��ʾ��֡ͷ
05 ��ʾ�����ݳ���
82 ��ʾ��д�����洢��ָ��
1000 ��ʾ��������ַ�������ֽڣ�
0002 ��ʾ������ 2�������ֽڣ�
���ͣ�ͨ��ָ���� 0000 ��ַ���渳ֵ 2�����ϵ���ʾ�����ݱ����������� 2
ע��DGUS ������д�����ĵ�ַ�����Կ���Ϊ��ʼ��ַ���������� a��b ָ���Ч������ c
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
* @brief	:	���ʹӵ����������ݵ�ָ��
* @param	: addr ������ַ
* @retval	:
************************************************************************************/
/*
�������洢��ָ��ָ�0x83�� �˴��Զ� 1000 ������ַ�����ֵ�����赱ǰ��ֵΪ 2��Ϊ���� 5AA5 04 83 1000 01
5AA5 ��ʾ��֡ͷ
04 ��ʾ�����ݳ���
83 ��ʾ�������ݴ洢��ָ��
1000 ��ʾ��������ַ�������ֽڣ�
01 ��ʾ���� 1000 ��ַ��ʼ�� 1 ���ֳ��ȣ�����ָ����������� 0x7c
�����Ժ������򴮿ڷ��ض�Ӧ�� 5AA5 06 83 1000 01 0002
5AA5 ��ʾ��֡ͷ 06 ��ʾ�����ݳ���
83 ��ʾ���������洢��ָ��ָ��
1000 ��ʾ��������ַ�������ֽڣ�
01 ��ʾ���� 1000 ��ַ��ʼ�� 1 ���ֳ�������
00 02 ��ʾ��1000 ��ַ�������ֵ�� 2
*/


void read_from_dwin(u8 addr_h, u8 addr_l, u8 size);
void read_u8_from_dwin(u16 addr);
void read_u16_from_dwin(u16 addr);
void read_u32_from_dwin(u16 addr);
void read_float_from_dwin(u16 addr);



/************************************************************************************
* @name		:	ProcessMessage
* @brief	:	������������ص�����
* @param	: 
* @retval	:
************************************************************************************/
void ProcessMessage( CTRL_MSG *msg);
void dwin_ack(void);


#endif



