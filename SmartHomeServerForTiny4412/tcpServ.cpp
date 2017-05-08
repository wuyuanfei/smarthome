#include "smartHome.h"

TcpServer::TcpServer(const char *listen_ip,int listen_port)
{
    int ret;

    listenSock = socket(AF_INET,SOCK_STREAM,0);
    if(listenSock < 0){
        throw "socket()failed";
    }

    memset(&servAddr,0,sizeof(servAddr));
    servAddr.sin_family = AF_INET;
    servAddr.sin_addr.s_addr = inet_addr(listen_ip);
    servAddr.sin_port = htons(listen_port);

    ret = bind(listenSock,(struct sockaddr *)&servAddr,sizeof(servAddr));
    if(ret < 0){
        throw "bind()failed";
    }

    ret = listen(listenSock,10);
    if(ret < 0){
        throw "listen()failed";
    }
}

bool TcpServer::isAccept()
{
    unsigned int clntAddrLen = sizeof(clntAddr);

    cout << "等待连接中... ..." << endl;

    communicationSock = accept(listenSock,(struct sockaddr *)&clntAddr,&clntAddrLen);
    if(communicationSock < 0){
        return false;
    }else{
        cout << "客户端-ip：" << inet_ntoa(clntAddr.sin_addr) << "-已连接。" << endl;
        return true;
    }
}

void TcpServer::handleEcho()
{
    JsonMsg Msg;
	memset(&Msg.msg,0,sizeof(Msg.msg));

    recv(communicationSock,&Msg,sizeof(Msg),0);
	
	printf("Msg type:%#x\n",Msg.type);
	printf("Msg funNum:%#x\n",Msg.funNum);
	printf("Msg length:%d\n",Msg.length);
	printf("Msg msg:%s\n\n",Msg.msg);

	Msg.readJsonMsg(communicationSock);   
	
    //close(communicationSock);
	return;
}
