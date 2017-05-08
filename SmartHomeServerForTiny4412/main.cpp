#include "smartHome.h"

void sleep_work(void *work_data)
{
	sleep(*(int *)work_data);
	return;
}

int echo_server(int argc,const char *argv[])
{
    int port;
    const char *ip;

    if(argc == 3){
        ip = argv[1];
        port = atoi(argv[2]);
    }else{
        cout << "\t-请输入ip和端口号-" << endl;
    }
    
    TcpServer myServ(ip,port);

    while(true){
        if(myServ.isAccept() == true){           
			myServ.handleEcho();
        }
    }

    return 0;
}

int main(int argc, const char *argv[])
{
    int mainRtn = 0;

    try{
        mainRtn = echo_server(argc,argv);
    }
	
    catch(const char *s){
        perror(s);
        exit(EXIT_FAILURE);
    }
  
    return mainRtn;
}
