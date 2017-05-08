#include "smartHome.h"

/*******************************************************************
打开串口：
	传参-USB0串口路径；返回-文件描述符。
 *******************************************************************/
int uart_open(char *serial_port)
{
	int fd;

	fd = open(serial_port, O_RDWR | O_NOCTTY | O_NDELAY);
	if (fd < 0){
		return -1;
	}

                              
	if(fcntl(fd, F_SETFL, 0) < 0){
		printf("fcntl failed!\n");
		return -1;
	}else{
//		printf("fcntl=%d\n",fcntl(fd, F_SETFL,0));
	}

    
	if(0 == isatty(STDIN_FILENO)){
		printf("standard input is not a terminal device\n");
		return -1;
	}else{
		printf("\t-串口打开成功-\n");
	}   

	return fd;
}

/*****************************************************************************
关闭串口：
	传参-文件描述符
 *****************************************************************************/
void uart_close(int fd)
{
	close(fd);
	
	return;
}

/************************************************************************************
初始化串口：
	传参-(int fd,int speed,int flow_ctrl,int databits,int stopbits,int parity)
		文件描述符-fd;波特率-115200;控制位-0	;	数据位-8;		停止位-1;		校对位-'n'
	
	返回值：bool-真假
 *************************************************************************************/
int uart_init(int fd,int speed,int flow_ctrl,int databits,int stopbits,int parity)
{

	int   i;
	int   speed_arr[] = { B115200, B19200, B9600, B4800, B2400, B1200, B300};
	int   name_arr[] = {115200,  19200,  9600,  4800,  2400,  1200,  300};

	struct termios options;

	if(tcgetattr(fd,&options)  !=  0){
		perror("SetupSerial 1");    
		return(FALSE); 
	}

	for (i= 0; i < sizeof(speed_arr)/sizeof(int);  i++){
		if(speed == name_arr[i]){             
			cfsetispeed(&options, speed_arr[i]); 
			cfsetospeed(&options, speed_arr[i]);
			break;
		}
	}

	if(i == sizeof(speed_arr)/sizeof(int)){
		printf("Error,Baudrate %d not support!\n",speed);
		return FALSE;
	}


	options.c_cflag |= CLOCAL;

	options.c_cflag |= CREAD;


	switch(flow_ctrl){
	case 0 :
		options.c_cflag &= ~CRTSCTS;
		break;   
	case 1 :
		options.c_cflag |= CRTSCTS;
		break;
	case 2 :
		options.c_cflag |= IXON | IXOFF | IXANY;
		break;
	}

	options.c_cflag &= ~CSIZE;
	switch (databits){  
	case 5    :
		options.c_cflag |= CS5;
		break;
	case 6    :
		options.c_cflag |= CS6;
		break;
	case 7    :    
		options.c_cflag |= CS7;
		break;
	case 8:    
		options.c_cflag |= CS8;
		break;  
	default:   
		fprintf(stderr,"Unsupported data size\n");
		return -1; 
	}


	switch (parity){  
	case 'n':
	case 'N':
		options.c_cflag &= ~PARENB; 
		options.c_iflag &= ~INPCK;    
		break; 
	case 'o':  
	case 'O':   
		options.c_cflag |= (PARODD | PARENB); 
		options.c_iflag |= INPCK;             
		break; 
	case 'e': 
	case 'E': 
		options.c_cflag |= PARENB;       
		options.c_cflag &= ~PARODD;       
		options.c_iflag |= INPCK;      
		break;
	case 's':
	case 'S': 
		options.c_cflag &= ~PARENB;
		options.c_cflag &= ~CSTOPB;
		break; 
	default:  
		fprintf(stderr,"Unsupported parity\n");    
		return -1; 
	} 

	
	switch (stopbits){  
	case 1:   
		options.c_cflag &= ~CSTOPB; 
		break; 
	case 2:   
		options.c_cflag |= CSTOPB; 
		break;
	default:   
		fprintf(stderr,"Unsupported stop bits\n"); 
		return -1;
	}


	options.c_oflag &= ~OPOST;
	options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);

	options.c_iflag &= ~(ICRNL | IGNCR |IXON); 


	options.c_cc[VTIME] = 1; 
	options.c_cc[VMIN] = 1; 

	
	tcflush(fd,TCIFLUSH);

	if (tcsetattr(fd,TCSANOW,&options) != 0)  {
		perror("com set error!\n");  
		return -1; 
	}

	return 0; 
}

/***********************************************************************************
读串口：
	传参-	-文件描述符，接受buf，数据长度，超时检测时间（秒，毫妙）
	
	返回值-select返回值-负值：select错误
				正值：某些文件可读写或出错
				0：等待超时，没有可读写或错误的文件


int select(int maxfdp,fd_set *readfds,fd_set *writefds,fd_set *errorfds,struct timeval*timeout); 

先说明两个结构体： 
（1）struct fd_set可以理解为一个集合，这个集合中存放的是文件描述符(filedescriptor)，即文件句柄，这可以是我们所说的普通意义的文件，当然Unix下任何设备、管道、FIFO等都是文件形式，全部包括在内，所以毫无疑问一个socket就是一个文件，socket句柄就是一个文件描述符。fd_set集合可以通过一些宏由人为来操作，比如清空集合FD_ZERO(fd_set *)，将一个给定的文件描述符加入集合之中FD_SET(int ,fd_set*)，将一个给定的文件描述符从集合中删除FD_CLR(int,fd_set*)，检查集合中指定的文件描述符是否可以读写FD_ISSET(int ,fd_set* )。

（2）struct timeval是一个大家常用的结构，用来代表时间值，有两个成员，一个是秒数，另一个是毫秒数。 

具体解释select的参数： 
【1】int maxfdp是一个整数值，是指集合中所有文件描述符的范围，即所有文件描述符的最大值加1，不能错！在Windows中这个参数的值无所谓，可以设置不正确。 

【2】fd_set * readfds是指向fd_set结构的指针，这个集合中应该包括文件描述符，我们是要监视这些文件描述符的读变化的，即我们关心是否可以从这些文件中读取数据了，如果这个集合中有一个文件可读，select就会返回一个大于0的值，表示有文件可读，如果没有可读的文件，则根据timeout参数再判断是否超时，若超出timeout的时间，select返回0，若发生错误返回负值。可以传入NULL值，表示不关心任何文件的读变化。 

【3】fd_set * writefds是指向fd_set结构的指针，这个集合中应该包括文件描述符，我们是要监视这些文件描述符的写变化的，即我们关心是否可以向这些文件中写入数据了，如果这个集合中有一个文件可写，select就会返回一个大于0的值，表示有文件可写，如果没有可写的文件，则根据timeout参数再判断是否超时，若超出timeout的时间，select返回0，若发生错误返回负值。可以传入NULL值，表示不关心任何文件的写变化。 

【4】fd_set * errorfds同上面两个参数的意图，用来监视文件错误异常。 

【5】struct timeval * timeout是select的超时时间，这个参数至关重要，它可以使select处于三种状态，第一，若将NULL以形参传入，即不传入时间结构，就是将select置于阻塞状态，一定等到监视文件描述符集合中某个文件描述符发生变化为止；第二，若将时间值设为0秒0毫秒，就变成一个纯粹的非阻塞函数，不管文件描述符是否有变化，都立刻返回继续执行，文件无变化返回0，有变化返回一个正值；第三，timeout的值大于0，这就是等待的超时时间，即select在timeout时间内阻塞，超时时间之内有事件到来就返回了，否则在超时后不管怎样一定返回，返回值同上述。 

 **********************************************************************************/
int uart_recv(int fd, char *rcv_buf,int data_len,struct timeval *timeout)
{
	int count = 0;
	int len = 0,ret;
	fd_set fs_read;
	
	while(count < data_len){
		FD_ZERO(&fs_read);//用select函数之前先把集合清零
		FD_SET(fd,&fs_read);//把要检测的句柄-fd，加入到集合里

printf("===================start to recv ===============\n");

		ret = select(fd+1,&fs_read,NULL,NULL,timeout);
		if(ret == -1){
			printf("fail to select : %s\n",strerror(errno));
			break;
		}else if(ret){
			len = read(fd,rcv_buf + count,data_len - count);
			count += len;
            printf("\t-读到字符长度: %d-\n",len);
		}else{
			printf("\t-USB串口读取超时-\n");
			break;
		}
	}

	return ret;
}

/********************************************************************
写串口：
	传参-文件描述符，发送buf，数据长度
	
	返回值-
 *******************************************************************/
int uart_send(int fd, char *send_buf,int data_len)
{
	int len = 0;

	len = write(fd,send_buf,data_len);
	if (len == data_len ){
		return len;
	}else{
		tcflush(fd,TCOFLUSH);
		return -1;
	}
}

