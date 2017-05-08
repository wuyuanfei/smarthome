#include "smartHome.h"

extern int ufd;

//extern pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;
//extern pthread_cond_t cond1 = PTHREAD_COND_INITIALIZER;

/*****************************************************************************************
*	1-每一个函数都在一条独立的线程下，每条线程实现一个功能
*	2-先对串口进行操作，初始化串口封装在函数中
*	3-读写串口得到数据
*	4-创建新buffer操作数据，并记得memset()
*
*    unsigned char sendTemp[5] = {0xdd,0x00,0x00,0x00,'\0'};
*    unsigned char sendHumi[5] = {0xdd,0x01,0x00,0x00,'\0'};
*    unsigned char sendLight[5] = {0xdd,0x02,0x00,0x00,'\0'};
*    unsigned char sendThreeZ[5] = {0xdd,0x03,0x00,0x00,'\0'};
*    unsigned char sendLED1On[5] = {0xdd,0x04,0x00,0x00,'\0'};
*    unsigned char sendLED2On[5] = {0xdd,0x04,0x01,0x00,'\0'};	
*    unsigned char sendFanOn[5] = {0xdd,0x05,0x00,0x00,'\0'};
*    unsigned char sendDoorOn[5] = {0xdd,0x06,0x00,0x00,'\0'};
*
*    unsigned char sendLed1Off[5] = {0xdd,0x04,0x00,0x01,'\0'};
*    unsigned char sendLed2Off[5] = {0xdd,0x04,0x01,0x01,'\0'};	
*    unsigned char sendFanOff[5] = {0xdd,0x05,0x00,0x01,'\0'};
*    unsigned char sendDoorOff[5] = {0xdd,0x06,0x00,0x01,'\0'};
*
*****************************************************************************************/



void tempGt(int *fd)//请求温度
{
    //******************对APP回发，这对JsonMsg进行填充，为不错乱，独立对象*************************
    JsonMsg tpmsg;
    memset(&tpmsg,0,sizeof(tpmsg));
    json_object *sendBackJson;
    const char *backString;

//===================================//
int ret = 0;
//==================================//

    sendBackJson = json_object_new_object();//创建一个空的Json对象成员
printf("1********************************\n");
    //**************************对串口数据进行读写，然后反馈给App*******************************
    int	cfd = *fd;
    //int ufd;
    //ufd = uartOperate(); 
	printf("cfd:%d;ufd:%d.\n",cfd,ufd);
    unsigned char sendTemp[5] = {0xdd,0x00,0x00,0x00,'\0'};		
    unsigned char recvTemp[5] = {0};	

printf("2********************************\n");

    ret = uart_send(ufd,(char *)sendTemp,5);//请求温度 to M0
printf("========send ret : %d==============\n",ret);


    ret = uart_recv(ufd,(char *)recvTemp,5,NULL);//接受温度 
printf("========recv ret : %d==============\n",ret);
printf("3********************************\n");

    if((sendTemp[1] == recvTemp[1]) && (sendTemp[2] == recvTemp[2]))//功能号和设备号匹配才输入
    {
        printf("温度：%d.%d摄氏度.\n",(int)recvTemp[3],(int)recvTemp[4]);
printf("4********************************\n");
        tpmsg.type = 0xff;
        tpmsg.funNum = 0x04;

        wd;//温度
        zero;//成功-0
        backString = (char *)json_object_to_json_string(sendBackJson);
printf("*****************json to app : %s\n",backString);
        strcpy((char *)&tpmsg.msg,backString);
        tpmsg.length = strlen(tpmsg.msg);	

        printf("tempMsg type:%#x\n",tpmsg.type);
        printf("tempMsg funNum:%#x\n",tpmsg.funNum);
        printf("tempMsg length:%d\n",tpmsg.length);
        printf("tempMsg msg:%s\n",tpmsg.msg);

//		pthread_mutex_lock(&mutex1);

//        send(cfd,&tpmsg,sizeof(tpmsg),0);

//		pthread_cond_signal(&cond1);
//		pthread_mutex_unlock(&mutex1);

//		pthread_mutex_lock(&thread_pool->queue_lock);
printf("=======******client fd : %d\n",cfd);
       ret = send(cfd,&tpmsg,strlen(backString ) + 4,0);

		

printf("send app ret : %d\n",ret);
//printf("send msg to app : %s\n",tpmsg.msg);


//		pthread_mutex_unlock(&thread_pool->queue_lock);
    }else{
        printf("获取温度失败\n");

        tpmsg.type = 0xff;
        tpmsg.funNum = 0x04;

        one;//失败-1
        backString = (char *)json_object_to_json_string(sendBackJson);
        strcpy((char *)&tpmsg.msg,backString);
        tpmsg.length = strlen(tpmsg.msg);	

        printf("tempMsg type:%#x\n",tpmsg.type);
        printf("tempMsg funNum:%#x\n",tpmsg.funNum);
        printf("tempMsg length:%d\n",tpmsg.length);
        printf("tempMsg msg:%s\n",tpmsg.msg);

//		pthread_mutex_lock(&thread_pool->queue_lock);
        send(cfd,&tpmsg,strlen(backString ) + 4,0);
//		pthread_mutex_unlock(&thread_pool->queue_lock);
    }
    //close(ufd);//关闭文件描述符，以免资源消耗
    return;
}


void humiGt(int *fd)//请求湿度
{
    //******************对APP回发，这对JsonMsg进行填充，为不错乱，独立对象*************************
    JsonMsg tpmsg;
    memset(&tpmsg,0,sizeof(tpmsg));
    json_object *sendBackJson;
    const char *backString;

    sendBackJson = json_object_new_object();//创建一个空的Json对象成员

    //**************************对串口数据进行读写，然后反馈给App*******************************
    int	cfd = *fd;
    //int ufd;
    //ufd = uartOperate(); 
    unsigned char sendHumi[5] = {0xdd,0x01,0x00,0x00,'\0'};
    unsigned char recvHumi[5] = {0};	

    uart_send(ufd,(char *)&sendHumi,5);//请求温度 to M0
    uart_recv(ufd,(char *)&recvHumi,5,NULL);//接受温度 

    if((sendHumi[1] == recvHumi[1]) && (sendHumi[2] == recvHumi[2]))//功能号和设备号匹配才输入
    {
        printf("湿度：%d.%d克/立方米.\n",(int)recvHumi[3],(int)recvHumi[4]);

        tpmsg.type = 0xff;
        tpmsg.funNum = 0x05;

        sd;//温度
        zero;//成功-0
        backString = (char *)json_object_to_json_string(sendBackJson);
        strcpy((char *)&tpmsg.msg,backString);
        tpmsg.length = strlen(tpmsg.msg);	

        printf("tempMsg type:%#x\n",tpmsg.type);
        printf("tempMsg funNum:%#x\n",tpmsg.funNum);
        printf("tempMsg length:%d\n",tpmsg.length);
        printf("tempMsg msg:%s\n",tpmsg.msg);

//		pthread_mutex_lock(&thread_pool->queue_lock);
        send(cfd,&tpmsg,strlen(backString ) + 4,0);
//		pthread_mutex_unlock(&thread_pool->queue_lock);
    }else{
        printf("获取湿度失败\n");

        tpmsg.type = 0xff;
        tpmsg.funNum = 0x05;

        one;//失败-1
        backString = (char *)json_object_to_json_string(sendBackJson);
        strcpy((char *)&tpmsg.msg,backString);
        tpmsg.length = strlen(tpmsg.msg);	

        printf("tempMsg type:%#x\n",tpmsg.type);
        printf("tempMsg funNum:%#x\n",tpmsg.funNum);
        printf("tempMsg length:%d\n",tpmsg.length);
        printf("tempMsg msg:%s\n",tpmsg.msg);

//		pthread_mutex_lock(&thread_pool->queue_lock);
        send(cfd,&tpmsg,strlen(backString ) + 4,0);
//		pthread_mutex_unlock(&thread_pool->queue_lock);
    }
    //close(ufd);//关闭文件描述符，以免资源消耗
    return;
}


void lightGt(int *fd)//请求光照
{
    //******************对APP回发，这对JsonMsg进行填充，为不错乱，独立对象*************************
    JsonMsg tpmsg;
    memset(&tpmsg,0,sizeof(tpmsg));
    json_object *sendBackJson;
    const char *backString;

    sendBackJson = json_object_new_object();//创建一个空的Json对象成员

    //**************************对串口数据进行读写，然后反馈给App*******************************
    int	cfd = *fd;
    //int ufd;
    //ufd = uartOperate(); 
    unsigned char sendTemp[5] = {0xdd,0x02,0x00,0x00,'\0'};		
    unsigned char recvTemp[5] = {0};	

    uart_send(ufd,(char *)&sendTemp,5);//请求温度 to M0
    uart_recv(ufd,(char *)&recvTemp,5,NULL);//接受温度 

    if((sendTemp[1] == recvTemp[1]) && (sendTemp[2] == recvTemp[2]))//功能号和设备号匹配才输入
    {
        printf("光照：%d.%d勒克斯.\n",(int)recvTemp[3],(int)recvTemp[4]);

        tpmsg.type = 0xff;
        tpmsg.funNum = 0x06;

        gz;//温度
        zero;//成功-0
        backString = (char *)json_object_to_json_string(sendBackJson);
        strcpy((char *)&tpmsg.msg,backString);
        tpmsg.length = strlen(tpmsg.msg);	

        printf("tempMsg type:%#x\n",tpmsg.type);
        printf("tempMsg funNum:%#x\n",tpmsg.funNum);
        printf("tempMsg length:%d\n",tpmsg.length);
        printf("tempMsg msg:%s\n",tpmsg.msg);

//		pthread_mutex_lock(&thread_pool->queue_lock);
        send(cfd,&tpmsg,strlen(backString ) + 4,0);
//		pthread_mutex_unlock(&thread_pool->queue_lock);
    }else{
        printf("获取温度失败\n");

        tpmsg.type = 0xff;
        tpmsg.funNum = 0x06;

        one;//失败-1
        backString = (char *)json_object_to_json_string(sendBackJson);
        strcpy((char *)&tpmsg.msg,backString);
        tpmsg.length = strlen(tpmsg.msg);	

        printf("tempMsg type:%#x\n",tpmsg.type);
        printf("tempMsg funNum:%#x\n",tpmsg.funNum);
        printf("tempMsg length:%d\n",tpmsg.length);
        printf("tempMsg msg:%s\n",tpmsg.msg);

//		pthread_mutex_lock(&thread_pool->queue_lock);
        send(cfd,&tpmsg,strlen(backString ) + 4,0);
//		pthread_mutex_unlock(&thread_pool->queue_lock);
    }
    //close(ufd);//关闭文件描述符，以免资源消耗
    return;
}


void triaGt(int *fd)//请求三轴
{
    //******************对APP回发，这对JsonMsg进行填充，为不错乱，独立对象*************************
    JsonMsg tpmsg;
    memset(&tpmsg,0,sizeof(tpmsg));
    json_object *sendBackJson;
    const char *backString;

    sendBackJson = json_object_new_object();//创建一个空的Json对象成员

    //**************************对串口数据进行读写，然后反馈给App*******************************
    int	cfd = *fd;
    //int ufd;
    //ufd = uartOperate(); 
    unsigned char sendTemp[5] = {0xdd,0x03,0x00,0x00,'\0'};		
    unsigned char recvTemp[6] = {0};	

    uart_send(ufd,(char *)&sendTemp,5);//请求三轴 to M0
    uart_recv(ufd,(char *)&recvTemp,6,NULL);//接受三轴 

    if((sendTemp[1] == recvTemp[1]) && (sendTemp[2] == recvTemp[2]))//功能号和设备号匹配才输入
    {
        printf("三轴：%d-%d-%d度.\n",(int)recvTemp[3],(int)recvTemp[4],(int)recvTemp[4]);

        tpmsg.type = 0xff;
        tpmsg.funNum = 0x07;

        xz;//
		yz;
		zz;
        zero;//成功-0
        backString = (char *)json_object_to_json_string(sendBackJson);
        strcpy((char *)&tpmsg.msg,backString);
        tpmsg.length = strlen(tpmsg.msg);	

        printf("tempMsg type:%#x\n",tpmsg.type);
        printf("tempMsg funNum:%#x\n",tpmsg.funNum);
        printf("tempMsg length:%d\n",tpmsg.length);
        printf("tempMsg msg:%s\n",tpmsg.msg);

//		pthread_mutex_lock(&thread_pool->queue_lock);
        send(cfd,&tpmsg,strlen(backString ) + 4,0);
//		pthread_mutex_unlock(&thread_pool->queue_lock);
    }else{
        printf("获取三轴失败\n");

        tpmsg.type = 0xff;
        tpmsg.funNum = 0x07;

        one;//失败-1
        backString = (char *)json_object_to_json_string(sendBackJson);
        strcpy((char *)&tpmsg.msg,backString);
        tpmsg.length = strlen(tpmsg.msg);	

        printf("tempMsg type:%#x\n",tpmsg.type);
        printf("tempMsg funNum:%#x\n",tpmsg.funNum);
        printf("tempMsg length:%d\n",tpmsg.length);
        printf("tempMsg msg:%s\n",tpmsg.msg);

//		pthread_mutex_lock(&thread_pool->queue_lock);
        send(cfd,&tpmsg,strlen(backString ) + 4,0);
//		pthread_mutex_unlock(&thread_pool->queue_lock);
    }
    //close(ufd);//关闭文件描述符，以免资源消耗
	return;
}


void led1O(int *fd)//开灯1
{

printf("************ open led1*********************\n");
    //******************对APP回发，这对JsonMsg进行填充，为不错乱，独立对象*************************
    JsonMsg tpmsg;
    memset(&tpmsg,0,sizeof(tpmsg));
    json_object *sendBackJson;
    const char *backString;

    sendBackJson = json_object_new_object();//创建一个空的Json对象成员

    //**************************对串口数据进行读写，然后反馈给App*******************************
    int	cfd = *fd;
    //int ufd;
    //ufd = uartOperate(); 
    unsigned char sendTemp[5] = {0xdd,0x04,0x00,0x00,0x00};		
    unsigned char recvTemp[4] = {0};	

    uart_send(ufd,(char *)&sendTemp,5);//请求温度 to M0
    uart_recv(ufd,(char *)&recvTemp,4,NULL);//接受温度 

printf("uart send  type :%x  function: %x  device :%x   state: %x\n",recvTemp[0],recvTemp[1],recvTemp[2],recvTemp[3]);

    if((sendTemp[1] == recvTemp[1]) && (sendTemp[2] == recvTemp[2]))//功能号和设备号匹配才输入
    {
		if(0 == recvTemp[3]){
        	printf("\t-灯1的状态：开-\n");	
  	    	tpmsg.type = 0xff;
        	tpmsg.funNum = 0x08;
		
			czero;//on
        	nzero;//led1
        	zero;//成功-0
        	backString = (char *)json_object_to_json_string(sendBackJson);
        	strcpy((char *)&tpmsg.msg,backString);
        	tpmsg.length = strlen(tpmsg.msg);	

        	printf("tempMsg type:%#x\n",tpmsg.type);
        	printf("tempMsg funNum:%#x\n",tpmsg.funNum);
        	printf("tempMsg length:%d\n",tpmsg.length);
        	printf("tempMsg msg:%s\n",tpmsg.msg);

//		pthread_mutex_lock(&thread_pool->queue_lock);
        send(cfd,&tpmsg,strlen(backString ) + 4,0);
//		pthread_mutex_unlock(&thread_pool->queue_lock);
		}else if(1 == recvTemp[3]){
        	printf("\t-灯1的状态：关-\n");	
  	    	tpmsg.type = 0xff;
        	tpmsg.funNum = 0x08;
		
			cone;//off
        	nzero;//led1
        	zero;//成功-0
        	backString = (char *)json_object_to_json_string(sendBackJson);
        	strcpy((char *)&tpmsg.msg,backString);
        	tpmsg.length = strlen(tpmsg.msg);	

        	printf("tempMsg type:%#x\n",tpmsg.type);
        	printf("tempMsg funNum:%#x\n",tpmsg.funNum);
        	printf("tempMsg length:%d\n",tpmsg.length);
        	printf("tempMsg msg:%s\n",tpmsg.msg);

//		pthread_mutex_lock(&thread_pool->queue_lock);
        send(cfd,&tpmsg,strlen(backString ) + 4,0);
//		pthread_mutex_unlock(&thread_pool->queue_lock);		
		}
    }else{
        printf("获取灯1失败\n");

        tpmsg.type = 0xff;
        tpmsg.funNum = 0x08;

        one;//失败-1
        backString = (char *)json_object_to_json_string(sendBackJson);
        strcpy((char *)&tpmsg.msg,backString);
        tpmsg.length = strlen(tpmsg.msg);	

        printf("tempMsg type:%#x\n",tpmsg.type);
        printf("tempMsg funNum:%#x\n",tpmsg.funNum);
        printf("tempMsg length:%d\n",tpmsg.length);
        printf("tempMsg msg:%s\n",tpmsg.msg);

//		pthread_mutex_lock(&thread_pool->queue_lock);
        send(cfd,&tpmsg,strlen(backString ) + 4,0);
//		pthread_mutex_unlock(&thread_pool->queue_lock);
    }
    //close(ufd);//关闭文件描述符，以免资源消耗
	return;
}


void led1F(int *fd)//关灯1
{
printf("************ close led1*********************\n");
    //******************对APP回发，这对JsonMsg进行填充，为不错乱，独立对象*************************
    JsonMsg tpmsg;
    memset(&tpmsg,0,sizeof(tpmsg));
    json_object *sendBackJson;
    const char *backString;

    sendBackJson = json_object_new_object();//创建一个空的Json对象成员

    //**************************对串口数据进行读写，然后反馈给App*******************************
    int	cfd = *fd;
    //int ufd;
    //ufd = uartOperate(); 
    unsigned char sendTemp[5] = {0xdd,0x04,0x00,0x00,0x01};		
    unsigned char recvTemp[4] = {0};	



    uart_send(ufd,(char *)&sendTemp,5);//请求温度 to M0
    uart_recv(ufd,(char *)&recvTemp,4,NULL);//接受 

printf("serv -> M0  type :%x  function: %x  device :%x   state: %x\n",sendTemp[0],sendTemp[1],sendTemp[2],sendTemp[3]);
printf("M0 -> serv  type :%x  function: %x  device :%x   state: %x\n",recvTemp[0],recvTemp[1],recvTemp[2],recvTemp[3]);

    if((sendTemp[1] == recvTemp[1]) && (sendTemp[2] == recvTemp[2]))//功能号和设备号匹配才输入
    {
		if(0 == recvTemp[3]){
        	printf("\t-灯1的状态：开-\n");	
  	    	tpmsg.type = 0xff;
        	tpmsg.funNum = 0x08;
		
			czero;//on
        	nzero;//led1
        	zero;//成功-0
        	backString = (char *)json_object_to_json_string(sendBackJson);
        	strcpy((char *)&tpmsg.msg,backString);
        	tpmsg.length = strlen(tpmsg.msg);	

        	printf("tempMsg type:%#x\n",tpmsg.type);
        	printf("tempMsg funNum:%#x\n",tpmsg.funNum);
        	printf("tempMsg length:%d\n",tpmsg.length);
        	printf("tempMsg msg:%s\n",tpmsg.msg);

//		pthread_mutex_lock(&thread_pool->queue_lock);
        send(cfd,&tpmsg,strlen(backString ) + 4,0);
//		pthread_mutex_unlock(&thread_pool->queue_lock);
		}else if(1 == recvTemp[3]){
        	printf("\t-灯1的状态：关-\n");	
  	    	tpmsg.type = 0xff;
        	tpmsg.funNum = 0x08;
		
			cone;//off
        	nzero;//led1
        	zero;//成功-0
        	backString = (char *)json_object_to_json_string(sendBackJson);
        	strcpy((char *)&tpmsg.msg,backString);
        	tpmsg.length = strlen(tpmsg.msg);	

        	printf("tempMsg type:%#x\n",tpmsg.type);
        	printf("tempMsg funNum:%#x\n",tpmsg.funNum);
        	printf("tempMsg length:%d\n",tpmsg.length);
        	printf("tempMsg msg:%s\n",tpmsg.msg);

//		pthread_mutex_lock(&thread_pool->queue_lock);
        send(cfd,&tpmsg,strlen(backString ) + 4,0);
//		pthread_mutex_unlock(&thread_pool->queue_lock);	
		}
    }else{
        printf("获取灯1失败\n");

        tpmsg.type = 0xff;
        tpmsg.funNum = 0x08;

        one;//失败-1
        backString = (char *)json_object_to_json_string(sendBackJson);
        strcpy((char *)&tpmsg.msg,backString);
        tpmsg.length = strlen(tpmsg.msg);	

        printf("tempMsg type:%#x\n",tpmsg.type);
        printf("tempMsg funNum:%#x\n",tpmsg.funNum);
        printf("tempMsg length:%d\n",tpmsg.length);
        printf("tempMsg msg:%s\n",tpmsg.msg);

//		pthread_mutex_lock(&thread_pool->queue_lock);
        send(cfd,&tpmsg,strlen(backString ) + 4,0);
//		pthread_mutex_unlock(&thread_pool->queue_lock);
    }
    //close(ufd);//关闭文件描述符，以免资源消耗
	return;
}



void led2O(int *fd)//开灯2
{
    //******************对APP回发，这对JsonMsg进行填充，为不错乱，独立对象*************************
    JsonMsg tpmsg;
    memset(&tpmsg,0,sizeof(tpmsg));
    json_object *sendBackJson;
    const char *backString;

    sendBackJson = json_object_new_object();//创建一个空的Json对象成员

    //**************************对串口数据进行读写，然后反馈给App*******************************
    int	cfd = *fd;
    //int ufd;
    //ufd = uartOperate(); 
    unsigned char sendTemp[5] = {0xdd,0x04,0x01,0x00,0x00};		
    unsigned char recvTemp[4] = {0};	

    uart_send(ufd,(char *)&sendTemp,5);//请求温度 to M0
    uart_recv(ufd,(char *)&recvTemp,4,NULL);//接受温度 

    if((sendTemp[1] == recvTemp[1]) && (sendTemp[2] == recvTemp[2]))//功能号和设备号匹配才输入
    {
		if(0 == recvTemp[3]){
        	printf("\t-灯2的状态：开-\n");	
  	    	tpmsg.type = 0xff;
        	tpmsg.funNum = 0x08;
		
			czero;//on
        	none;//led2
        	zero;//成功-0
        	backString = (char *)json_object_to_json_string(sendBackJson);
        	strcpy((char *)&tpmsg.msg,backString);
        	tpmsg.length = strlen(tpmsg.msg);	

        	printf("tempMsg type:%#x\n",tpmsg.type);
        	printf("tempMsg funNum:%#x\n",tpmsg.funNum);
        	printf("tempMsg length:%d\n",tpmsg.length);
        	printf("tempMsg msg:%s\n",tpmsg.msg);

//		pthread_mutex_lock(&thread_pool->queue_lock);
        send(cfd,&tpmsg,strlen(backString ) + 4,0);
//		pthread_mutex_unlock(&thread_pool->queue_lock);
		}else if(1 == recvTemp[3]){
        	printf("\t-灯2的状态：关-\n");	
  	    	tpmsg.type = 0xff;
        	tpmsg.funNum = 0x08;
		
			cone;//off
        	none;//led2
        	zero;//成功-0
        	backString = (char *)json_object_to_json_string(sendBackJson);
        	strcpy((char *)&tpmsg.msg,backString);
        	tpmsg.length = strlen(tpmsg.msg);	

        	printf("tempMsg type:%#x\n",tpmsg.type);
        	printf("tempMsg funNum:%#x\n",tpmsg.funNum);
        	printf("tempMsg length:%d\n",tpmsg.length);
        	printf("tempMsg msg:%s\n",tpmsg.msg);

//		pthread_mutex_lock(&thread_pool->queue_lock);
        send(cfd,&tpmsg,strlen(backString ) + 4,0);
//		pthread_mutex_unlock(&thread_pool->queue_lock);		
		}
    }else{
        printf("获取灯2失败\n");

        tpmsg.type = 0xff;
        tpmsg.funNum = 0x08;

        one;//失败-1
        backString = (char *)json_object_to_json_string(sendBackJson);
        strcpy((char *)&tpmsg.msg,backString);
        tpmsg.length = strlen(tpmsg.msg);	

        printf("tempMsg type:%#x\n",tpmsg.type);
        printf("tempMsg funNum:%#x\n",tpmsg.funNum);
        printf("tempMsg length:%d\n",tpmsg.length);
        printf("tempMsg msg:%s\n",tpmsg.msg);

//		pthread_mutex_lock(&thread_pool->queue_lock);
        send(cfd,&tpmsg,strlen(backString ) + 4,0);
//		pthread_mutex_unlock(&thread_pool->queue_lock);
    }
    //close(ufd);//关闭文件描述符，以免资源消耗
	return;
}



void led2F(int *fd)//关灯2
{
    //******************对APP回发，这对JsonMsg进行填充，为不错乱，独立对象*************************
    JsonMsg tpmsg;
    memset(&tpmsg,0,sizeof(tpmsg));
    json_object *sendBackJson;
    const char *backString;

    sendBackJson = json_object_new_object();//创建一个空的Json对象成员

    //**************************对串口数据进行读写，然后反馈给App*******************************
    int	cfd = *fd;
    //int ufd;
    //ufd = uartOperate(); 
    unsigned char sendTemp[5] = {0xdd,0x04,0x01,0x00,0x01};		
    unsigned char recvTemp[4] = {0};	

    uart_send(ufd,(char *)&sendTemp,5);//请求温度 to M0
    uart_recv(ufd,(char *)&recvTemp,4,NULL);//接受温度 

    if((sendTemp[1] == recvTemp[1]) && (sendTemp[2] == recvTemp[2]))//功能号和设备号匹配才输入
    {
		if(0 == recvTemp[3]){
        	printf("\t-灯2的状态：开-\n");	
  	    	tpmsg.type = 0xff;
        	tpmsg.funNum = 0x08;
		
			czero;//on
        	none;//led2
        	zero;//成功-0
        	backString = (char *)json_object_to_json_string(sendBackJson);
        	strcpy((char *)&tpmsg.msg,backString);
        	tpmsg.length = strlen(tpmsg.msg);	

        	printf("tempMsg type:%#x\n",tpmsg.type);
        	printf("tempMsg funNum:%#x\n",tpmsg.funNum);
        	printf("tempMsg length:%d\n",tpmsg.length);
        	printf("tempMsg msg:%s\n",tpmsg.msg);

//		pthread_mutex_lock(&mutex1);

        send(cfd,&tpmsg,strlen(backString ) + 4,0);

//		pthread_cond_signal(&cond1);
//		pthread_mutex_unlock(&mutex1);
		}else if(1 == recvTemp[3]){
        	printf("\t-灯2的状态：关-\n");	
  	    	tpmsg.type = 0xff;
        	tpmsg.funNum = 0x08;
		
			cone;//off
        	none;//led2
        	zero;//成功-0
        	backString = (char *)json_object_to_json_string(sendBackJson);
        	strcpy((char *)&tpmsg.msg,backString);
        	tpmsg.length = strlen(tpmsg.msg);	

        	printf("tempMsg type:%#x\n",tpmsg.type);
        	printf("tempMsg funNum:%#x\n",tpmsg.funNum);
        	printf("tempMsg length:%d\n",tpmsg.length);
        	printf("tempMsg msg:%s\n",tpmsg.msg);

//		pthread_mutex_lock(&mutex1);

        send(cfd,&tpmsg,strlen(backString ) + 4,0);

//		pthread_cond_signal(&cond1);
//		pthread_mutex_unlock(&mutex1);		
		}
    }else{
        printf("获取灯2失败\n");

        tpmsg.type = 0xff;
        tpmsg.funNum = 0x08;

        one;//失败-1
        backString = (char *)json_object_to_json_string(sendBackJson);
        strcpy((char *)&tpmsg.msg,backString);
        tpmsg.length = strlen(tpmsg.msg);	

        printf("tempMsg type:%#x\n",tpmsg.type);
        printf("tempMsg funNum:%#x\n",tpmsg.funNum);
        printf("tempMsg length:%d\n",tpmsg.length);
        printf("tempMsg msg:%s\n",tpmsg.msg);

//		pthread_mutex_lock(&mutex1);

        send(cfd,&tpmsg,strlen(backString ) + 4,0);

//		pthread_cond_signal(&cond1);
//		pthread_mutex_unlock(&mutex1);
    }
    //close(ufd);//关闭文件描述符，以免资源消耗
	return;
}



void fanO(int *fd)//开风扇
{
    //******************对APP回发，这对JsonMsg进行填充，为不错乱，独立对象*************************
    JsonMsg tpmsg;
    memset(&tpmsg,0,sizeof(tpmsg));
    json_object *sendBackJson;
    const char *backString;

    sendBackJson = json_object_new_object();//创建一个空的Json对象成员

    //**************************对串口数据进行读写，然后反馈给App*******************************
    int	cfd = *fd;
    //int ufd;
    //ufd = uartOperate(); 
    unsigned char sendTemp[5] = {0xdd,0x05,0x00,0x00,0x00};		
    unsigned char recvTemp[4] = {0};	

    uart_send(ufd,(char *)&sendTemp,5);//请求温度 to M0
    uart_recv(ufd,(char *)&recvTemp,4,NULL);//接受温度 

    if((sendTemp[1] == recvTemp[1]) && (sendTemp[2] == recvTemp[2]))//功能号和设备号匹配才输入
    {
		if(0 == recvTemp[3]){
        	printf("\t-风扇的状态：开-\n");	
  	    	tpmsg.type = 0xff;
        	tpmsg.funNum = 0x08;
		
			czero;//on
        	nzero;//fan
        	zero;//成功-0
        	backString = (char *)json_object_to_json_string(sendBackJson);
        	strcpy((char *)&tpmsg.msg,backString);
        	tpmsg.length = strlen(tpmsg.msg);	

        	printf("tempMsg type:%#x\n",tpmsg.type);
        	printf("tempMsg funNum:%#x\n",tpmsg.funNum);
        	printf("tempMsg length:%d\n",tpmsg.length);
        	printf("tempMsg msg:%s\n",tpmsg.msg);

//		pthread_mutex_lock(&mutex1);

        send(cfd,&tpmsg,strlen(backString ) + 4,0);

//		pthread_cond_signal(&cond1);
//		pthread_mutex_unlock(&mutex1);
		}else if(1 == recvTemp[3]){
        	printf("\t-风扇的状态：关-\n");	
  	    	tpmsg.type = 0xff;
        	tpmsg.funNum = 0x08;
		
			cone;//off
        	nzero;//fan
        	zero;//成功-0
        	backString = (char *)json_object_to_json_string(sendBackJson);
        	strcpy((char *)&tpmsg.msg,backString);
        	tpmsg.length = strlen(tpmsg.msg);	

        	printf("tempMsg type:%#x\n",tpmsg.type);
        	printf("tempMsg funNum:%#x\n",tpmsg.funNum);
        	printf("tempMsg length:%d\n",tpmsg.length);
        	printf("tempMsg msg:%s\n",tpmsg.msg);

//		pthread_mutex_lock(&mutex1);

        send(cfd,&tpmsg,strlen(backString ) + 4,0);

//		pthread_cond_signal(&cond1);
//		pthread_mutex_unlock(&mutex1);		
		}
    }else{
        printf("获取风扇失败\n");

        tpmsg.type = 0xff;
        tpmsg.funNum = 0x08;

        one;//失败-1
        backString = (char *)json_object_to_json_string(sendBackJson);
        strcpy((char *)&tpmsg.msg,backString);
        tpmsg.length = strlen(tpmsg.msg);	

        printf("tempMsg type:%#x\n",tpmsg.type);
        printf("tempMsg funNum:%#x\n",tpmsg.funNum);
        printf("tempMsg length:%d\n",tpmsg.length);
        printf("tempMsg msg:%s\n",tpmsg.msg);

//		pthread_mutex_lock(&mutex1);

        send(cfd,&tpmsg,strlen(backString ) + 4,0);

//		pthread_cond_signal(&cond1);
//		pthread_mutex_unlock(&mutex1);
    }
    //close(ufd);//关闭文件描述符，以免资源消耗
	return;
}



void fanF(int *fd)//关风扇
{
    //******************对APP回发，这对JsonMsg进行填充，为不错乱，独立对象*************************
    JsonMsg tpmsg;
    memset(&tpmsg,0,sizeof(tpmsg));
    json_object *sendBackJson;
    const char *backString;

    sendBackJson = json_object_new_object();//创建一个空的Json对象成员

    //**************************对串口数据进行读写，然后反馈给App*******************************
    int	cfd = *fd;
    //int ufd;
    //ufd = uartOperate(); 
    unsigned char sendTemp[5] = {0xdd,0x05,0x00,0x00,0x01};		
    unsigned char recvTemp[4] = {0};	

    uart_send(ufd,(char *)&sendTemp,5);//请求 to M0
    uart_recv(ufd,(char *)&recvTemp,4,NULL);//接受温度 

    if((sendTemp[1] == recvTemp[1]) && (sendTemp[2] == recvTemp[2]))//功能号和设备号匹配才输入
    {
		if(0 == recvTemp[3]){
        	printf("\t-风扇的状态：开-\n");	
  	    	tpmsg.type = 0xff;
        	tpmsg.funNum = 0x08;
		
			czero;//on
        	nzero;//
        	zero;//成功-0
        	backString = (char *)json_object_to_json_string(sendBackJson);
        	strcpy((char *)&tpmsg.msg,backString);
        	tpmsg.length = strlen(tpmsg.msg);	

        	printf("tempMsg type:%#x\n",tpmsg.type);
        	printf("tempMsg funNum:%#x\n",tpmsg.funNum);
        	printf("tempMsg length:%d\n",tpmsg.length);
        	printf("tempMsg msg:%s\n",tpmsg.msg);

//		pthread_mutex_lock(&mutex1);

        send(cfd,&tpmsg,strlen(backString ) + 4,0);

//		pthread_cond_signal(&cond1);
//		pthread_mutex_unlock(&mutex1);
		}else if(1 == recvTemp[3]){
        	printf("\t-风扇的状态：关-\n");	
  	    	tpmsg.type = 0xff;
        	tpmsg.funNum = 0x08;
		
			cone;//off
        	nzero;//
        	zero;//成功-0
        	backString = (char *)json_object_to_json_string(sendBackJson);
        	strcpy((char *)&tpmsg.msg,backString);
        	tpmsg.length = strlen(tpmsg.msg);	

        	printf("tempMsg type:%#x\n",tpmsg.type);
        	printf("tempMsg funNum:%#x\n",tpmsg.funNum);
        	printf("tempMsg length:%d\n",tpmsg.length);
        	printf("tempMsg msg:%s\n",tpmsg.msg);

//		pthread_mutex_lock(&mutex1);

        send(cfd,&tpmsg,strlen(backString ) + 4,0);

//		pthread_cond_signal(&cond1);
//		pthread_mutex_unlock(&mutex1);		
		}
    }else{
        printf("获取风扇失败\n");

        tpmsg.type = 0xff;
        tpmsg.funNum = 0x08;

        one;//失败-1
        backString = (char *)json_object_to_json_string(sendBackJson);
        strcpy((char *)&tpmsg.msg,backString);
        tpmsg.length = strlen(tpmsg.msg);	

        printf("tempMsg type:%#x\n",tpmsg.type);
        printf("tempMsg funNum:%#x\n",tpmsg.funNum);
        printf("tempMsg length:%d\n",tpmsg.length);
        printf("tempMsg msg:%s\n",tpmsg.msg);

//		pthread_mutex_lock(&mutex1);

        send(cfd,&tpmsg,strlen(backString ) + 4,0);

//		pthread_cond_signal(&cond1);
//		pthread_mutex_unlock(&mutex1);
    }
    //close(ufd);//关闭文件描述符，以免资源消耗
	return;
}



void doorO(int *fd)//开门
{
    //******************对APP回发，这对JsonMsg进行填充，为不错乱，独立对象*************************
    JsonMsg tpmsg;
    memset(&tpmsg,0,sizeof(tpmsg));
    json_object *sendBackJson;
    const char *backString;

    sendBackJson = json_object_new_object();//创建一个空的Json对象成员

    //**************************对串口数据进行读写，然后反馈给App*******************************
    int	cfd = *fd;
    //int ufd;
    //ufd = uartOperate(); 
    unsigned char sendTemp[5] = {0xdd,0x06,0x00,0x00,0x00};		
    unsigned char recvTemp[5] = {0};	

    uart_send(ufd,(char *)&sendTemp,5);//请求门 to M0
    uart_recv(ufd,(char *)&recvTemp,4,NULL);//接受门 

    if((sendTemp[1] == recvTemp[1]) && (sendTemp[2] == recvTemp[2]))//功能号和设备号匹配才输入
    {
		if(0 == recvTemp[3]){
        	printf("\t-门的状态：开-\n");	
  	    	tpmsg.type = 0xff;
        	tpmsg.funNum = 0x08;
		
			czero;//on
        	nzero;//
        	zero;//成功-0
        	backString = (char *)json_object_to_json_string(sendBackJson);
        	strcpy((char *)&tpmsg.msg,backString);
        	tpmsg.length = strlen(tpmsg.msg);	

        	printf("tempMsg type:%#x\n",tpmsg.type);
        	printf("tempMsg funNum:%#x\n",tpmsg.funNum);
        	printf("tempMsg length:%d\n",tpmsg.length);
        	printf("tempMsg msg:%s\n",tpmsg.msg);

//		pthread_mutex_lock(&mutex1);

        send(cfd,&tpmsg,strlen(backString ) + 4,0);

//		pthread_cond_signal(&cond1);
//		pthread_mutex_unlock(&mutex1);
		}else if(1 == recvTemp[3]){
        	printf("\t-门的状态：关-\n");	
  	    	tpmsg.type = 0xff;
        	tpmsg.funNum = 0x08;
		
			cone;//off
        	nzero;//
        	zero;//成功-0
        	backString = (char *)json_object_to_json_string(sendBackJson);
        	strcpy((char *)&tpmsg.msg,backString);
        	tpmsg.length = strlen(tpmsg.msg);	

        	printf("tempMsg type:%#x\n",tpmsg.type);
        	printf("tempMsg funNum:%#x\n",tpmsg.funNum);
        	printf("tempMsg length:%d\n",tpmsg.length);
        	printf("tempMsg msg:%s\n",tpmsg.msg);

//		pthread_mutex_lock(&mutex1);

        send(cfd,&tpmsg,strlen(backString ) + 4,0);

//		pthread_cond_signal(&cond1);
//		pthread_mutex_unlock(&mutex1);			
		}
    }else{
        printf("获取门失败\n");

        tpmsg.type = 0xff;
        tpmsg.funNum = 0x08;

        one;//失败-1
        backString = (char *)json_object_to_json_string(sendBackJson);
        strcpy((char *)&tpmsg.msg,backString);
        tpmsg.length = strlen(tpmsg.msg);	

        printf("tempMsg type:%#x\n",tpmsg.type);
        printf("tempMsg funNum:%#x\n",tpmsg.funNum);
        printf("tempMsg length:%d\n",tpmsg.length);
        printf("tempMsg msg:%s\n",tpmsg.msg);

//		pthread_mutex_lock(&mutex1);

        send(cfd,&tpmsg,strlen(backString ) + 4,0);

//		pthread_cond_signal(&cond1);
//		pthread_mutex_unlock(&mutex1);
    }
    //close(ufd);//关闭文件描述符，以免资源消耗
	return;
}



void doorF(int *fd)//关门
{
    //******************对APP回发，这对JsonMsg进行填充，为不错乱，独立对象*************************
    JsonMsg tpmsg;
    memset(&tpmsg,0,sizeof(tpmsg));
    json_object *sendBackJson;
    const char *backString;

    sendBackJson = json_object_new_object();//创建一个空的Json对象成员

    //**************************对串口数据进行读写，然后反馈给App*******************************
    int	cfd = *fd;
    //int ufd;
    //ufd = uartOperate(); 
    unsigned char sendTemp[5] = {0xdd,0x06,0x00,0x00,0x01};		
    unsigned char recvTemp[4] = {0};	

    uart_send(ufd,(char *)&sendTemp,5);//请求温度 to M0
    uart_recv(ufd,(char *)&recvTemp,4,NULL);//接受温度 

    if((sendTemp[1] == recvTemp[1]) && (sendTemp[2] == recvTemp[2]))//功能号和设备号匹配才输入
    {
		if(0 == recvTemp[3]){
        	printf("\t-门的状态：开-\n");	
  	    	tpmsg.type = 0xff;
        	tpmsg.funNum = 0x08;
		
			czero;//on
        	nzero;//
        	zero;//成功-0
        	backString = (char *)json_object_to_json_string(sendBackJson);
        	strcpy((char *)&tpmsg.msg,backString);
        	tpmsg.length = strlen(tpmsg.msg);	

        	printf("tempMsg type:%#x\n",tpmsg.type);
        	printf("tempMsg funNum:%#x\n",tpmsg.funNum);
        	printf("tempMsg length:%d\n",tpmsg.length);
        	printf("tempMsg msg:%s\n",tpmsg.msg);

//		pthread_mutex_lock(&mutex1);

        send(cfd,&tpmsg,strlen(backString ) + 4,0);

//		pthread_cond_signal(&cond1);
//		pthread_mutex_unlock(&mutex1);
		}else if(1 == recvTemp[3]){
        	printf("\t-门的状态：关-\n");	
  	    	tpmsg.type = 0xff;
        	tpmsg.funNum = 0x08;
		
			cone;//off
        	nzero;//led1
        	zero;//成功-0
        	backString = (char *)json_object_to_json_string(sendBackJson);
        	strcpy((char *)&tpmsg.msg,backString);
        	tpmsg.length = strlen(tpmsg.msg);	

        	printf("tempMsg type:%#x\n",tpmsg.type);
        	printf("tempMsg funNum:%#x\n",tpmsg.funNum);
        	printf("tempMsg length:%d\n",tpmsg.length);
        	printf("tempMsg msg:%s\n",tpmsg.msg);

//		pthread_mutex_lock(&mutex1);

        send(cfd,&tpmsg,strlen(backString ) + 4,0);

//		pthread_cond_signal(&cond1);
//		pthread_mutex_unlock(&mutex1);			
		}
    }else{
        printf("获取门失败\n");

        tpmsg.type = 0xff;
        tpmsg.funNum = 0x08;

        one;//失败-1
        backString = (char *)json_object_to_json_string(sendBackJson);
        strcpy((char *)&tpmsg.msg,backString);
        tpmsg.length = strlen(tpmsg.msg);	

        printf("tempMsg type:%#x\n",tpmsg.type);
        printf("tempMsg funNum:%#x\n",tpmsg.funNum);
        printf("tempMsg length:%d\n",tpmsg.length);
        printf("tempMsg msg:%s\n",tpmsg.msg);

//		pthread_mutex_lock(&mutex1);

        send(cfd,&tpmsg,strlen(backString ) + 4,0);

//		pthread_cond_signal(&cond1);
//		pthread_mutex_unlock(&mutex1);
    }
    //close(ufd);//关闭文件描述符，以免资源消耗
	return;
}
