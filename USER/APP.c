#include "includes.h"
#include "bmp.h"
char cStr[100]={0};
char *p,*pCh;//wifi接收首地址，匹配首地址
char oledcar[50],oledinfo[50];//OLED车辆信息
u8 ucId;//信道
int carnum=0,carin=0,carout=0;//停车场车辆数，进出汽车数量
u8 link_status;//连接状态
/*客户端网页*/
char page[]="<html><body><a href=\"/ENTER\"><input type=\"Button\" value=\"ENTER\" style=\"position:absolute;top:50%;left:40%\"></input></a><a href=\"/PLAY\"><input type=\"Button\" value=\"PLAY\" style=\"position:absolute;top:50%;left:50%\"></input></a><a href=\"/LEAVE\"><input type=\"Button\" value=\"LEAVE\" style=\"position:absolute;top:50%;left:60%\"></input></a></body></html>";
/*标题显示*/
void printInfo(void)
{
    OLED_Fill(0x00);//清屏
    OLED_ShowStr(10,0,(unsigned char*)"Oliver-parking lot!",1);//显示标题
    OLED_ShowStr(20,2,(unsigned char*)"Welcome home!",1);//显示信息
}
void Task_esp8266_sendHTML(void *p_arg)
{
     u8 i;
    (void)p_arg;
    while(1)
    {
        link_status=ESP8266_Get_IdLinkStatus();//获取当下的连接情况
        for(i=0;i<5;i++)
        {
            if(((link_status>>i) & 0x01)!=0)
            {
                ESP8266_SendString ( DISABLE, page, strlen ( page ), ( ENUM_ID_NO_TypeDef ) i );//给每个客户端发送网页
            }
        }
        OSTimeDlyHMSM(0,0,1,0);
    }
}
void Task_esp8266(void *p_arg)
{ 
    (void)p_arg;
    printInfo();//显示标题
    sprintf(oledinfo,"      <Start> ");
    OLED_ShowStr(0,4,(unsigned char*)oledinfo,1);//显示车辆进出情况
    sprintf(oledcar,"Car number: %d",carnum);
    OLED_ShowStr(20,6,(unsigned char*)oledcar,1);//显示停车场情况
    ESP8266_Inquire_ApIp(cStr, 20 );//获取本机IP
    while(1)
    {
        p=ESP8266_ReceiveString(DISABLE);//接收数据，p为接收到的数据的首地址
        if(strEsp8266_Fram_Record .InfBit .FramFinishFlag )//如果接收到了数据
		{
			USART_ITConfig (USART2, USART_IT_RXNE, DISABLE ); //禁用串口接收中断
            //printf("接收到的数据: %s\r\n",strEsp8266_Fram_Record.Data_RX_BUF);
			if ( ((bool)strstr (strEsp8266_Fram_Record.Data_RX_BUF, "ENTER" ) ) != 0 ) //如果是进车信号
            {
                if(carnum==5)//车位已经满了
                {
                    sprintf(oledinfo,"Filled!Filled!Filled!");
                }
                else
                {
                    Gate1Open();//打开进车栏杆
                    carin++;//进车数加1
                    carnum++;//车位减1
                    sprintf(oledinfo,"  %d cars has entered!",carin);
                    
                } 
            }
            else if ( ((bool)strstr (strEsp8266_Fram_Record.Data_RX_BUF, "LEAVE" ) ) != 0 ) //如果是出车信号
            {
                if(carnum==0)//没车，就不能出车
                {
                    sprintf(oledinfo,"  Are you kidding?");
                }
                else
                {
                    
                    carnum--;//车位加1
                    Gate2Open();//打开出车栏杆
                    carout++;//出车数加1
                    sprintf(oledinfo,"  %d cars has leaved!",carout);
                }
            }
            else if( ((bool)strstr (strEsp8266_Fram_Record.Data_RX_BUF, "PLAY" ) ) != 0 )
            {
                OLED_DrawBMP(0,0,128,8,BMP3);
                OSTimeDlyHMSM(0,0,2,0);
            }
            if ( ( pCh = strstr ( strEsp8266_Fram_Record .Data_RX_BUF, "+IPD," ) ) != 0 ) //发送反馈信息——本机IP
			{
				ucId = * ( pCh + strlen ( "+IPD," ) ) - '0';//多连接容易导致信道切换，所以需要实时获取信道
				ESP8266_SendString ( DISABLE, cStr, strlen ( cStr ), ( ENUM_ID_NO_TypeDef ) ucId );//发送数据
			}
            OSTimeDlyHMSM(0,0,1,0);//延时1s后关闭栏杆
            Gate1Close();
            Gate2Close();
            printInfo();//显示标题
            OLED_ShowStr(0,4,(unsigned char*)oledinfo,1);//显示车辆进出情况
            sprintf(oledcar,"Car number: %d",carnum);
            OLED_ShowStr(20,6,(unsigned char*)oledcar,1);//显示停车场情况
        }
        strEsp8266_Fram_Record .InfBit .FramLength = 0;
	    strEsp8266_Fram_Record .InfBit .FramFinishFlag = 0;		
		USART_ITConfig (USART2, USART_IT_RXNE, ENABLE ); //使能串口接收中断
        OSTimeDlyHMSM(0,0,1,0);
    }
}
