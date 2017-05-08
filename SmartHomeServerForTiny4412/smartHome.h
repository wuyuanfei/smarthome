#ifndef _SMART_HOME_H_
#define _SMART_HOME_H_

#include <unistd.h>
#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <string>
#include <json-c/json.h>
#include <sqlite3.h>
#include <signal.h>
#include <pthread.h>
#include <stdint.h>
#include <stdio.h>      
#include <stdlib.h>     
#include <unistd.h>     
#include <sys/types.h> 
#include <sys/stat.h>   
#include <fcntl.h>     
#include <termios.h>   
#include <errno.h>     
#include <string.h>

using namespace std;

#define DBG_OUT(args...) do{ \
    char b__[1024];\
    sprintf(b__,args);\
    fprintf(stderr,"[%s,%s,%d] : %s",__FILE__,__FUNCTION__,__LINE__,b__); \
    exit(EXIT_FAILURE);\
}while(0)

#define DBUG_OUT(args...) do{ \
    char b__[1024];\
    sprintf(b__,args);\
    fprintf(stderr,"[%s,%s,%d] : %s",__FILE__,__FUNCTION__,__LINE__,b__); \
    exit(EXIT_FAILURE);\
}while(0)

#define us json_object_get_string(UserName)
#define pa json_object_get_string(PassWord)
#define ph json_object_get_string(PhoneNumber)
#define ne json_object_get_string(NewPassword)
#define ra json_object_get_string(RandomCode)

#define ut json_object_get_string(UserToken)
#define dn json_object_get_int(DeviceNumber)
#define dc json_object_get_int(DeviceCode)


#define qr json_object_object_add(sendBackJson,"userToken",json_object_new_string("Qrt3T4"))//登录-
#define nu json_object_object_add(sendBackJson,"userToken",json_object_new_string("null"))//失败-	
#define zero json_object_object_add(sendBackJson,"stateCode",json_object_new_int(0))//-成功	
#define nzero json_object_object_add(sendBackJson,"deviceNumber",json_object_new_int(0))//-shebeihao 
#define czero json_object_object_add(sendBackJson,"deviceCode",json_object_new_int(0))//-zhuangtai 		
#define one json_object_object_add(sendBackJson,"stateCode",json_object_new_int(1))//-失败	
#define none json_object_object_add(sendBackJson,"deviceNumber",json_object_new_int(1))//-shebeihao 
#define cone json_object_object_add(sendBackJson,"deviceCode",json_object_new_int(1))//-zhuangtai 	
#define two json_object_object_add(sendBackJson,"stateCode",json_object_new_int(2))//-用户名错误
#define three json_object_object_add(sendBackJson,"stateCode",json_object_new_int(3))//-密码错误	
#define four json_object_object_add(sendBackJson,"stateCode",json_object_new_int(4))//手机号错误
#define six json_object_object_add(sendBackJson,"stateCode",json_object_new_int(6))//验证码错误	
#define qd json_object_object_add(sendBackJson,"randomCode",json_object_new_string("qDtT6H"))//随机验证码	
#define mm json_object_object_add(sendBackJson,"passWord",json_object_new_string("******"))//向APP发送密码

#define wd json_object_object_add(sendBackJson,"temperature",json_object_new_int((int)recvTemp[3]))//温度
#define sd json_object_object_add(sendBackJson,"humidity",json_object_new_int((int)recvHumi[3]))//湿度
#define gz json_object_object_add(sendBackJson,"light",json_object_new_int((int)recvTemp[3]))//光照
#define xz json_object_object_add(sendBackJson,"triaxialX",json_object_new_int((int)recvTemp[3]))//三轴
#define yz json_object_object_add(sendBackJson,"triaxialY",json_object_new_int((int)recvTemp[4]))//三轴
#define zz json_object_object_add(sendBackJson,"triaxialZ",json_object_new_int((int)recvTemp[5]))//三轴

class TcpServer//tcp类
{
private:
    int listenSock;
    int communicationSock;
    sockaddr_in servAddr;
    sockaddr_in clntAddr;
public:
    TcpServer(const char*listen_ip,int listen_port);
    bool isAccept();
    void handleEcho();
};

class Sqlite3//数据库类
{
public:
    char userName[20];
    char password[20];
    char randomCode[20];
    char phoneNumber[20];
    char newPassword[20];

    int dataBaseDeal(int options);
    int sqlite3_insert(sqlite3 *db);//用户注册
    int sqlite3_login(sqlite3 *db);//用户登录
    int sqlite3_findback(sqlite3 *db);//找回密码
    int sqlite3_update(sqlite3 *db);//修改密码
};

class JsonMsg//Json类
{
public:
    unsigned char type;
    char funNum;
    short length;
    char msg[128];

	void afterLogin(int cfd);
    void readJsonMsg(int cfd);
    void readJson(int num,Sqlite3 *sql);
    void writeJson(int num,int key,int cfd);	
};


//**************************线程池部分************************************

typedef struct node{
	void *data;
	struct node *next;
}LinkNode;

typedef struct{
	int       n;
	LinkNode *front;
	LinkNode *rear;
}LinkQueue;

extern LinkQueue *create_empty_linkqueue();
extern int is_empty_linkqueue(LinkQueue *); 
extern int enter_linkqueue(LinkQueue *,void *);
extern void *delete_linkqueue(LinkQueue *);
extern void clean_linkqueue(LinkQueue *);
extern int get_linkqueue_nodes(LinkQueue *q);


typedef void(*pthread_work_t)(int *);
typedef int pthread_signal_t;
typedef sigset_t pthread_sigset_t;
struct thread_pool;

typedef enum{
	thread_busy,
	thread_idle,
}pthread_s;

typedef struct {
	pthread_t tid;
	pthread_s status;
	int *work_data;
	pthread_work_t   thread_work;
	pthread_sigset_t *thread_sigset;
	struct thread_pool  *thread_pool;
}thread_description_t;

typedef LinkQueue thread_queue_t;

typedef struct thread_pool{
	int min_threads;
	int max_threads;
	int current_threads;	
	pthread_t clean_tid; 
	pthread_sigset_t thread_sigset;	
	pthread_mutex_t  queue_lock;
	thread_queue_t *idle_thread_queue;
	thread_description_t *thread_objs;
}thread_pool_t;




extern thread_pool_t *create_thread_pool(int max_threads,int min_threads);
extern int requst_thread_work(thread_pool_t *thread_pool,pthread_work_t work,int *work_data);		
extern void destroy_thread_pool(thread_pool_t *thread_pool);


extern int uart_open(char * serial_port);
extern void uart_close(int fd);
extern int uart_init(int fd,int speed,int flow_ctrl,int databits,int stopbits,int parity);
extern int uart_recv(int fd, char *rcv_buf,int data_len,struct timeval *timeout);
extern int uart_send(int fd, char *send_buf,int data_len);


extern void tempGt(int *cfd);
extern void humiGt(int *cfd);
extern void lightGt(int *cfd);
extern void triaGt(int *cfd);
extern void led1O(int *cfd);
extern void led1F(int *cfd);
extern void led2O(int *cfd);
extern void led2F(int *cfd);
extern void fanO(int *cfd);
extern void fanF(int *cfd);
extern void doorO(int *cfd);
extern void doorF(int *cfd);


#ifdef DBUG 
#define THREAD_DBUG(args...) do{ \
	char b__[1024];\
	sprintf(b__,args);\
	fprintf(stderr,"[%s,%s,%d] %s",__FILE__,__FUNCTION__,__LINE__,b__); \
}while(0)
#else 
#define THREAD_DBUG(args...)
#endif 

#endif
