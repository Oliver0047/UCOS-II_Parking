#include "includes.h"	   
//定义任务优先级
#define ESP8266_TASK_PRIO 9 
#define HTML_TASK_PRIO 10
//定义栈大小
#define ESP8266_STK_SIZE 256
#define HTML_STK_SIZE 128
//定义栈
static OS_STK TASK_ESP8266_STK[ESP8266_STK_SIZE];
static OS_STK TASK_HTML_STK[HTML_STK_SIZE];
int main(void)
{
    BSP_Init();//外设初始化
    OSInit();//操作系统初始化
    OSTaskCreate(Task_esp8266,(void *)0,
	   &TASK_ESP8266_STK[ESP8266_STK_SIZE-1], ESP8266_TASK_PRIO);//创建ESP8266监听任务
    OSTaskCreate(Task_esp8266_sendHTML,(void *)0,
	   &TASK_HTML_STK[HTML_STK_SIZE-1], HTML_TASK_PRIO);//创建ESP8266发送网页任务
	OSStart();//启动操作系统
    return 0;
}
