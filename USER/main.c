#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "timer.h"
#include "lcd.h"
#include "key.h"
#include "beep.h"
#include "string.h"
#include "malloc.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "dwin.h"





//任务优先级
#define START_TASK_PRIO		1
//任务堆栈大小
#define START_STK_SIZE 		256
//任务句柄
TaskHandle_t StartTask_Handler;
//任务函数
void start_task(void *pvParameters);

//任务优先级
#define DWIN_TASK_PRIO		2
//任务堆栈大小
#define DWIN_TASK_STK_SIZE 		256
//任务句柄
TaskHandle_t DwinTask_Handler;
//任务函数
void dwin_task(void *pvParameters);

//任务优先级
#define DEBUG_TASK_PRIO 3
//任务堆栈大小
#define DEBUG_STK_SIZE  256
//任务句柄
TaskHandle_t Debug_Handler;
//任务函数
void debug_task(void *pvParameters);



int main(void)
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);//设置系统中断优先级分组4
    delay_init(168);					//初始化延时函数
    uart_init(115200);     				//初始化串口

    //串口初始化
    DwinUartInit(115200);
    //清空串口接收缓冲区
    queue_reset();
    //延时等待串口屏初始化完毕,必须等待300ms
    delay_ms(300);


    //创建开始任务
    xTaskCreate((TaskFunction_t )start_task,            //任务函数
                (const char*    )"start_task",          //任务名称
                (uint16_t       )START_STK_SIZE,        //任务堆栈大小
                (void*          )NULL,                  //传递给任务函数的参数
                (UBaseType_t    )START_TASK_PRIO,       //任务优先级
                (TaskHandle_t*  )&StartTask_Handler);   //任务句柄
    vTaskStartScheduler();          //开启任务调度
}

//开始任务任务函数
void start_task(void *pvParameters)
{
    taskENTER_CRITICAL();           //进入临界区


    //创建TASK1任务
    xTaskCreate((TaskFunction_t )dwin_task,
                (const char*    )"dwin_task",
                (uint16_t       )DWIN_TASK_STK_SIZE,
                (void*          )NULL,
                (UBaseType_t    )DWIN_TASK_PRIO,
                (TaskHandle_t*  )&DwinTask_Handler);
    //创建TASK2任务
    xTaskCreate((TaskFunction_t )debug_task,
                (const char*    )"debug_task",
                (uint16_t       )DEBUG_STK_SIZE,
                (void*          )NULL,
                (UBaseType_t    )DEBUG_TASK_PRIO,
                (TaskHandle_t*  )&Debug_Handler);
    vTaskDelete(StartTask_Handler); //删除开始任务
    taskEXIT_CRITICAL();            //退出临界区
}

//迪文屏任务函数
void dwin_task(void *pvParameters)
{
    while(1)
    {
				dwin_ack();						//接收处理	
        vTaskDelay(20);      //延时20ms
    }
}


//调试函数
void debug_task(void *pvParameters)
{
		static u32 cnt=0;
    while(1)
    {

				if(cnt%100==0)
				{
					read_u16_from_dwin(TEST_NUMBER_ADDR);
				}
        vTaskDelay(10);      //延时10ms
    }
}






