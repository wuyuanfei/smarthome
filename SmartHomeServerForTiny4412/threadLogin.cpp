#include "smartHome.h"

//pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;
//pthread_cond_t cond1 = PTHREAD_COND_INITIALIZER;

void JsonMsg::afterLogin(int cfd)//客户端的文件描述符
{
    thread_pool_t *thread_pool;
    thread_pool = create_thread_pool(10,3);//创建一个线程池
    if(!thread_pool){
        printf("Fail to create_thread_pool");
        return;
    }
	
//********************************创建线程池部分***********************************


//**************************解析接收到的JsonMsg，并完成分类处理***********************

    JsonMsg Lmsg;//登录后接收的JsonMsg
    memset(&Lmsg,0,sizeof(Lmsg));//初始化
	
while(1){
//	pthread_mutex_lock(&mutex1);

//	recv(cfd,&Lmsg,sizeof(Lmsg),0);
//
//	pthread_cond_signal(&cond1);
//	pthread_mutex_unlock(&mutex1);
	memset(&Lmsg,0,sizeof(Lmsg));
//	pthread_mutex_lock(&thread_pool->queue_lock);


	recv(cfd,(char *)&Lmsg,4,0);

	recv(cfd,Lmsg.msg,Lmsg.length,0);



		
//	pthread_mutex_unlock(&thread_pool->queue_lock);
//----------------循环接收登录后的所有App消息------每接收一条就加入一条到线程池中-------------

	printf("Lmsg type:%#x\n",Lmsg.type);
	printf("Lmsg funNum:%#x\n",Lmsg.funNum);
	printf("Lmsg length:%d\n",Lmsg.length);
	printf("Lmsg msg:%s\n\n",Lmsg.msg);

//****************************解析接收到的JsonMsg字符串为我们需要的信息*****************************

	json_object *mainRecvJson;//初始json字符串
    json_object *UserName;//用户名
    json_object *UserToken;//服务内容
    json_object *DeviceNumber;//设备号
    json_object *DeviceCode;//状态

    const char *userName;//用户名
    const char *userToken;//服务内容
    int deviceNumber;//设备号
    int deviceCode;//状态

	mainRecvJson = json_tokener_parse(Lmsg.msg);//字符串转Json       

    UserName = json_object_object_get(mainRecvJson,"userName");//用户名
    UserToken = json_object_object_get(mainRecvJson,"userToken");//服务内容
    DeviceNumber = json_object_object_get(mainRecvJson,"deviceNumber");//设备号
    DeviceCode = json_object_object_get(mainRecvJson,"deviceCode");//开关状态

    userName = us;
    userToken = json_object_get_string(UserToken);
    deviceNumber = dn;
	deviceCode = dc;

    printf("userName:%s\n",userName);//用户名                  
    printf("userToken:%s\n",userToken);//服务内容
    printf("deviceNumber:%d\n",deviceNumber);//设备号
    printf("deviceCode:%d\n",deviceCode);//开关状态

//**************************得到客户端要我们做什么的信息****************************

//********************************分类处理**************************************
	if(Lmsg.type != 0xaa){
		cout << "\t-错误请求类型-" << endl;
	}else{
		switch(Lmsg.funNum){
			case 0x04://获取温度
				{
					requst_thread_work(thread_pool,tempGt,&cfd);//通过[回调函数]添加[任务]及[参数-arg]到线程池
					break;
				}
			case 0x05://获取湿度
				{
					requst_thread_work(thread_pool,humiGt,&cfd);//通过[回调函数]添加[任务]及[参数-arg]到线程池
					break;
				}
			case 0x06://获取光照
				{
					requst_thread_work(thread_pool,lightGt,&cfd);//通过[回调函数]添加[任务]及[参数-arg]到线程池
					break;
				}
			case 0x07://获取三轴
				{
					requst_thread_work(thread_pool,triaGt,&cfd);//通过[回调函数]添加[任务]及[参数-arg]到线程池
					break;
				}
			case 0x08://开关灯
				{
					if(0 == deviceNumber){//一号灯
						if(0 == deviceCode){//on
							requst_thread_work(thread_pool,led1O,&cfd);//通过[回调函数]添加[任务]及[参数-arg]到线程池
						}else{
							requst_thread_work(thread_pool,led1F,&cfd);//off
						}
					}else{//二号灯
						if(0 == deviceCode){//on
							requst_thread_work(thread_pool,led2O,&cfd);//通过[回调函数]添加[任务]及[参数-arg]到线程池
						}else{
							requst_thread_work(thread_pool,led2F,&cfd);//off
						}
					}
					break;
				}
			case 0x09://开关风扇
				{	
					if(0 == deviceCode){//on
						requst_thread_work(thread_pool,fanO,&cfd);//通过[回调函数]添加[任务]及[参数-arg]到线程池
					}else{
						requst_thread_work(thread_pool,fanF,&cfd);//off
					}
					break;
				}
			case 0x0a://开关门
				{
					if(0 == deviceCode){//on
						requst_thread_work(thread_pool,doorO,&cfd);//通过[回调函数]添加[任务]及[参数-arg]到线程池
					}else{
						requst_thread_work(thread_pool,doorF,&cfd);//off
					}					
					break;
				}
		}//switch结束
	}//if结束
	
	memset(&Lmsg,0,sizeof(Lmsg));
strcpy(Lmsg.msg,"");
	//memset(Lmsg.msg,0,sizeof(Lmsg.msg));
//	sleep(1);
}//recv大循环
    return;
}
