#ifndef _CMD_QUEUE
#define _CMD_QUEUE

#include "sys.h"
#include "delay.h"


#define CMD_HEAD1 0X5A            //帧头
#define CMD_HEAD2 0XA5            //帧头
#define CRC16_ENABLE 0            // 如果需要CRC16校验功能，修改此宏为1(此时需要在工程中配CRC校验)

#define QUEUE_MAX_SIZE 512        // 指令接收缓冲区大小，根据需要调整，尽量设置大一些


/*! 
*  \brief  清空指令数据
*/
void queue_reset(void);

/*! 
* \brief  添加指令数据
* \detial 串口接收的数据，通过此函数放入指令队列 
*  \param  _data 指令数据
*/
void queue_push(u8 _data);

/*! 
*  \brief  从指令队列中取出一条完整的指令
*  \param  cmd 指令接收缓存区
*  \param  buf_len 指令接收缓存区大小
*  \return  指令长度，0表示队列中无完整指令
*/
u16 queue_find_cmd(u8 *cmd,u16 buf_len);

/*! 
*  \brief  crc16校验
*  \param  str 待校验数据
*  \param  len 待校验数据长度
*  \return  校验码
*/
uint16_t CRC16_Check(uint8_t *str, uint8_t len);

#endif
