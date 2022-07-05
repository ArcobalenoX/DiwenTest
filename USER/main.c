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





//�������ȼ�
#define START_TASK_PRIO		1
//�����ջ��С
#define START_STK_SIZE 		256
//������
TaskHandle_t StartTask_Handler;
//������
void start_task(void *pvParameters);

//�������ȼ�
#define DWIN_TASK_PRIO		2
//�����ջ��С
#define DWIN_TASK_STK_SIZE 		256
//������
TaskHandle_t DwinTask_Handler;
//������
void dwin_task(void *pvParameters);

//�������ȼ�
#define DEBUG_TASK_PRIO 3
//�����ջ��С
#define DEBUG_STK_SIZE  256
//������
TaskHandle_t Debug_Handler;
//������
void debug_task(void *pvParameters);



int main(void)
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);//����ϵͳ�ж����ȼ�����4
    delay_init(168);					//��ʼ����ʱ����
    uart_init(115200);     				//��ʼ������

    //���ڳ�ʼ��
    DwinUartInit(115200);
    //��մ��ڽ��ջ�����
    queue_reset();
    //��ʱ�ȴ���������ʼ�����,����ȴ�300ms
    delay_ms(300);


    //������ʼ����
    xTaskCreate((TaskFunction_t )start_task,            //������
                (const char*    )"start_task",          //��������
                (uint16_t       )START_STK_SIZE,        //�����ջ��С
                (void*          )NULL,                  //���ݸ��������Ĳ���
                (UBaseType_t    )START_TASK_PRIO,       //�������ȼ�
                (TaskHandle_t*  )&StartTask_Handler);   //������
    vTaskStartScheduler();          //�����������
}

//��ʼ����������
void start_task(void *pvParameters)
{
    taskENTER_CRITICAL();           //�����ٽ���


    //����TASK1����
    xTaskCreate((TaskFunction_t )dwin_task,
                (const char*    )"dwin_task",
                (uint16_t       )DWIN_TASK_STK_SIZE,
                (void*          )NULL,
                (UBaseType_t    )DWIN_TASK_PRIO,
                (TaskHandle_t*  )&DwinTask_Handler);
    //����TASK2����
    xTaskCreate((TaskFunction_t )debug_task,
                (const char*    )"debug_task",
                (uint16_t       )DEBUG_STK_SIZE,
                (void*          )NULL,
                (UBaseType_t    )DEBUG_TASK_PRIO,
                (TaskHandle_t*  )&Debug_Handler);
    vTaskDelete(StartTask_Handler); //ɾ����ʼ����
    taskEXIT_CRITICAL();            //�˳��ٽ���
}

//������������
void dwin_task(void *pvParameters)
{
    while(1)
    {
				dwin_ack();						//���մ���	
        vTaskDelay(20);      //��ʱ20ms
    }
}


//���Ժ���
void debug_task(void *pvParameters)
{
		static u32 cnt=0;
    while(1)
    {

				if(cnt%100==0)
				{
					read_u16_from_dwin(TEST_NUMBER_ADDR);
				}
        vTaskDelay(10);      //��ʱ10ms
    }
}






