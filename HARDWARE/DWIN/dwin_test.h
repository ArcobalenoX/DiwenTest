#ifndef _DWIN_TEST_H
#define _DWIN_TEST_H

#include "dwin.h"

#define CMD_MAX_SIZE 16         // 单条指令大小，根据需要调整，尽量设置大一些


#define TEST_BTN_ADDR 0x5000		//点击按键
#define TEST_LED_ADDR 0x5002		//指示按键

#define TEST_TEXT_ADDR 0x5004			//显示数字
#define TEST_NUMBER_ADDR 0x5006		//输入数字


/************************************************************************************
* @name		:	ProcessMessage
* @brief	:	处理迪文屏返回的数据
* @param	: 
* @retval	:
************************************************************************************/
void ProcessMessage( CTRL_MSG *msg);
void dwin_ack(void);



#endif