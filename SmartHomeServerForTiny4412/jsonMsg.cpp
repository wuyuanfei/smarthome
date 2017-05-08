#include "smartHome.h"
//#include "uart.h"

int ufd;

int uartOperate()
{
    int ufd;//串口的文件描述符
    struct timeval timeout = {5,0};//读取串口超时时间

    char usbPath[] = {"/dev/ttyUSB0"};//串口路径
    ufd = uart_open((char *)usbPath);//打开串口
    if(ufd < 0)
    {
        perror("\t-打开串口失败-");
        exit(1);
    }

    uart_init(ufd,115200,0,8,1,'n');//初始化串口

    return ufd;
}

void JsonMsg::readJson(int num,Sqlite3 *sql)//解析Json字符串
{
	json_object *mainRecvJson;//初始json字符串
	json_object *UserName;//用户名
	json_object *PassWord;//密码
	json_object *PhoneNumber;//电话号码
	json_object *RandomCode;//验证码
	json_object *NewPassword;//新密码

	const char *userName;
	const char *password;
	const char *newPassword;
	const char *phoneNumber;
	const char *randomCode;		
	
	mainRecvJson = json_tokener_parse(msg);//字符串转Json

	UserName = json_object_object_get(mainRecvJson,"userName");//用户名
	PassWord = json_object_object_get(mainRecvJson,"password");//密码
	PhoneNumber = json_object_object_get(mainRecvJson,"phoneNumber");//手机号
	RandomCode = json_object_object_get(mainRecvJson,"randomCode");//验证码
	NewPassword = json_object_object_get(mainRecvJson,"newPassword");//新密码

	switch(num)
    {
    case 1://注册
        {
            userName = us;
            password = pa;
            phoneNumber = ph;

            strcpy((char *)&(*sql).userName,userName);	
            strcpy((char *)&(*sql).password,password);	
            strcpy((char *)&(*sql).phoneNumber,phoneNumber);		

            printf("userName:%s\n",(*sql).userName);	
            printf("password:%s\n",(*sql).password);
            printf("phoneNumber:%s\n",(*sql).phoneNumber);	

            break;
        }
    case 2://登录
        {
            userName = us;
            password = pa;

            strcpy((char *)&(*sql).userName,userName);	
            strcpy((char *)&(*sql).password,password);	

            printf("userName:%s\n",(*sql).userName);	
            printf("password:%s\n",(*sql).password);

            break;
        }
    case 3://找回密码
        {
            userName = us;
            phoneNumber = ph;

            strcpy((char *)&(*sql).userName,userName);	
            strcpy((char *)&(*sql).phoneNumber,phoneNumber);	

            printf("userName:%s\n",(*sql).userName);	
            printf("phoneNumber:%s\n",(*sql).phoneNumber);

            break;
        }
    case 31://匹配验证码
        {
            randomCode = ra;

            strcpy((char *)&(*sql).randomCode,randomCode);		

            printf("randomCode:%s\n",(*sql).randomCode);	

            break;
        }
    case 4://修改密码
        {
            userName = us;
            password = pa;
            newPassword = ne;

            strcpy((char *)&(*sql).userName,userName);	
            strcpy((char *)&(*sql).password,password);	
            strcpy((char *)&(*sql).newPassword,newPassword);

            printf("userName:%s\n",(*sql).userName);	
            printf("phoneNumber:%s\n",(*sql).phoneNumber);
            printf("newPassword:%s\n",(*sql).newPassword);

            break;
        }
    }

    json_object_put(mainRecvJson);
    json_object_put(UserName);
    json_object_put(PassWord);
    json_object_put(PhoneNumber);
    json_object_put(RandomCode);
    json_object_put(NewPassword);

    return;
}

void JsonMsg::writeJson(int num,int key,int cfd)//填充Json字符串
{
    JsonMsg Bmsg;
    memset(&Bmsg.msg,0,sizeof(Bmsg.msg));
    json_object *sendBackJson;
    const char *backString;

    sendBackJson = json_object_new_object();//创建一个空的Json对象成员

    switch(num)
    {
    case 1://注册回发
        {
            if(9 == key){//注册成功
                Bmsg.type = 0xff;
                Bmsg.funNum = 0x00;

                zero;
                backString = (char *)json_object_to_json_string(sendBackJson);
                strcpy((char *)&Bmsg.msg,backString);
                Bmsg.length = strlen(Bmsg.msg);	

                printf("Bmsg type:%#x\n",Bmsg.type);
                printf("Bmsg funNum:%#x\n",Bmsg.funNum);
                printf("Bmsg length:%d\n",Bmsg.length);
                printf("Bmsg msg:%s\n",Bmsg.msg);

                send(cfd,&Bmsg,sizeof(Bmsg),0);	
            }
            else if(1 == key){//注册失败
                Bmsg.type = 0xff;
                Bmsg.funNum = 0x00;

                one;
                backString = (char *)json_object_to_json_string(sendBackJson);
                strcpy((char *)&Bmsg.msg,backString);
                Bmsg.length = strlen(Bmsg.msg);	

                printf("Bmsg type:%#x\n",Bmsg.type);
                printf("Bmsg funNum:%#x\n",Bmsg.funNum);
                printf("Bmsg length:%d\n",Bmsg.length);
                printf("Bmsg msg:%s\n",Bmsg.msg);

                send(cfd,&Bmsg,sizeof(Bmsg),0);
            }
            break;	
        }
    case 2://登录回发
        {
            if(9 == key){//登录成功
                Bmsg.type = 0xff;
                Bmsg.funNum = 0x01;
                qr;
                zero;
                backString = (char *)json_object_to_json_string(sendBackJson);
                strcpy((char *)&Bmsg.msg,backString);
                Bmsg.length = strlen(Bmsg.msg);	

                printf("Bmsg type:%#x\n",Bmsg.type);
                printf("Bmsg funNum:%#x\n",Bmsg.funNum);
                printf("Bmsg length:%d\n",Bmsg.length);
                printf("Bmsg msg:%s\n",Bmsg.msg);

                send(cfd,&Bmsg,strlen(backString) + 4,0);	

				afterLogin(cfd);//登录完成，创建‘登录操作’对象log。
            }
            else if(2 == key){//登录失败
                Bmsg.type = 0xff;
                Bmsg.funNum = 0x01;
                nu;
                two;
                backString = (char *)json_object_to_json_string(sendBackJson);
                strcpy((char *)&Bmsg.msg,backString);
                Bmsg.length = strlen(Bmsg.msg);		

                printf("Bmsg type:%#x\n",Bmsg.type);
                printf("Bmsg funNum:%#x\n",Bmsg.funNum);
                printf("Bmsg length:%d\n",Bmsg.length);
                printf("Bmsg msg:%s\n",Bmsg.msg);

                send(cfd,&Bmsg,sizeof(Bmsg),0);	
            }
            break;	
        }
    case 3://找回密码回发
        {
            if(key == 9){//用户请求成功
                Bmsg.type = 0xff;
                Bmsg.funNum = 0x02;
                zero;
                backString = (char *)json_object_to_json_string(sendBackJson);
                strcpy((char *)&Bmsg.msg,backString);
                Bmsg.length = strlen(Bmsg.msg);

                cout << "Bmsg.type:" << Bmsg.type << endl;
                cout << "Bmsg.funNum:" << Bmsg.funNum << endl;
                cout << "Bmsg.length:" << Bmsg.length << endl;
                cout << "Bmsg.msg:" << Bmsg.msg << endl;

                send(cfd,&Bmsg,sizeof(Bmsg),0);

                //发验证码		
                Bmsg.type = 0xff;
                Bmsg.funNum = 0x02;
                qd;
                backString = (char *)json_object_to_json_string(sendBackJson);
                strcpy((char *)&Bmsg.msg,backString);
                Bmsg.length = strlen(Bmsg.msg);

                cout << "Bmsg.type:" << Bmsg.type << endl;
                cout << "Bmsg.funNum:" << Bmsg.funNum << endl;
                cout << "Bmsg.length:" << Bmsg.length << endl;
                cout << "Bmsg.msg:" << Bmsg.msg << endl;

                send(cfd,&Bmsg,sizeof(Bmsg),0);

                recv(cfd,&Bmsg,sizeof(Bmsg),0);

                Bmsg.readJsonMsg(cfd); 
            }
            if(key == 4){//用户名或手机号错误
                Bmsg.type = 0xff;
                Bmsg.funNum = 0x02;
                four;
                backString = (char *)json_object_to_json_string(sendBackJson);
                strcpy((char *)&Bmsg.msg,backString);
                Bmsg.length = strlen(Bmsg.msg);

                cout << "Bmsg.type:" << Bmsg.type << endl;
                cout << "Bmsg.funNum:" << Bmsg.funNum << endl;
                cout << "Bmsg.length:" << Bmsg.length << endl;
                cout << "Bmsg.msg:" << Bmsg.msg << endl;

                send(cfd,&Bmsg,sizeof(Bmsg),0);
            }
            break;		
        }
    case 31://匹配验证码
        {
            Bmsg.type = 0xff;
            Bmsg.funNum = 0x21;
            if(key == 9){
                mm;//找回密码
            }else{
                six;//验证码错误			
            }
            backString = (char *)json_object_to_json_string(sendBackJson);
            strcpy((char *)&Bmsg.msg,backString);
            Bmsg.length = strlen(Bmsg.msg);

            cout << "Bmsg.type:" << Bmsg.type << endl;
			cout << "Bmsg.funNum:" << Bmsg.funNum << endl;
			cout << "Bmsg.length:" << Bmsg.length << endl;
            cout << "Bmsg.msg:" << Bmsg.msg << endl;

            send(cfd,&Bmsg,sizeof(Bmsg),0);
            break;		
        }	
    case 4://修改密码成功
        {
            Bmsg.type = 0xff;
            Bmsg.funNum = 0x03;
            one;
            backString = (char *)json_object_to_json_string(sendBackJson);
            strcpy((char *)&Bmsg.msg,backString);
            Bmsg.length = strlen(Bmsg.msg);

            cout << "Bmsg.type:" << Bmsg.type << endl;
            cout << "Bmsg.funNum:" << Bmsg.funNum << endl;
            cout << "Bmsg.length:" << Bmsg.length << endl;
            cout << "Bmsg.msg:" << Bmsg.msg << endl;

            send(cfd,&Bmsg,sizeof(Bmsg),0);
            break;		
        }	
    }

    json_object_put(sendBackJson);

    return;
}


void JsonMsg::readJsonMsg(int cfd)
{
    int key = 0;
    Sqlite3 sql;

    if(type != 0xaa){
        throw "\t-JsonMsg 格式错误-";
    }else{
        switch(funNum)
        {
        case 0x00://注册
            {
                readJson(1,&sql);
                key = sql.dataBaseDeal(1);
                writeJson(1,key,cfd);	
                cout << "\t-注册操作已完成-" << endl;
                break;
            }
        case 0x01://登录
            {
                readJson(2,&sql);
                key = sql.dataBaseDeal(2);
				ufd = uartOperate(); 
                writeJson(2,key,cfd);
                cout << "\t-登录操作已完成-" << endl;
				
                break;
            }
        case 0x02://忘记密码
            {
                readJson(3,&sql);
                key = sql.dataBaseDeal(3);
                writeJson(3,key,cfd);
                cout << "\t-密码找回操作完成-" << endl;
                break;
            }
        case 0x21://匹配验证码
            {
                readJson(31,&sql);
                key = sql.dataBaseDeal(31);
                writeJson(31,key,cfd);
                cout << "\t-验证码匹配完成-" << endl;
                break;
            }
        case 0x03://修改密码
            {
                readJson(4,&sql);
                key = sql.dataBaseDeal(4);
                writeJson(4,key,cfd);
                cout << "\t-密码修改成功-" << endl;
                break;
            }
        }
    }
    return;
}

























