#include "includes.h"

void SysTick_init(void)//系统滴答定时器初始化
{
    SysTick_Config(SystemCoreClock/OS_TICKS_PER_SEC);
}
void ESP8266_Config(void)
{
    ESP8266_CH_ENABLE();//使能ESP8266
    ESP8266_AT_Test ();//测试是否正常
    ESP8266_Net_Mode_Choose ( AP );//选择AP模式，发出wifi信号，稳定
    ESP8266_Rst();//复位有效

    while(!ESP8266_CIPAP(esp8266_ip ));//IP正确与否
    while(!ESP8266_BuildAP(esp8266_user, esp8266_password, WPA_WPA2_PSK) );//设置ID，密码，信道，加密方式

    ESP8266_Enable_MultipleId ( ENABLE );//启动多连接，最多可连接5个客户端
    while(!ESP8266_StartOrShutServer(ENABLE, esp8266_port,esp8266_overTime));//设置端口号和超时时间
    
    /*接收数据区和标志清零*/
	strEsp8266_Fram_Record.InfBit.FramLength = 0;
	strEsp8266_Fram_Record.InfBit.FramFinishFlag = 0;	
    ESP8266_UnvarnishSend();//开启透传模式
}
void BSP_Init(void)
{
    SysTick_init();//延时初始化
    TIM3_GPIO_Config();//TIM两个PWM通道GPIO初始化
    TIM3_PWM_Init();//PWM初始化
    USART1_Init();//PC端串口
    I2C_Configuration();//I2C初始化
    OLED_Init();//OLED初始化
    ESP8266_Init();//引脚配置，串口配置
    ESP8266_Config();//IP地址、密码等相关配置
}
