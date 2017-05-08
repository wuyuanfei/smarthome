#include "smartHome.h"

//回调函数
int callback(void *arg,int n,char **value,char **name)
{
	int i;
	printf("记录包含%d个字段\n",n);
	
	for(i = 0;i < n;i ++){
		printf("字段名：%s\t字段值：%s\n",name[i],value[i]);	
	}
	
	printf("_______________________________________\n");

	return 0;
}

//注册
int Sqlite3::sqlite3_insert(sqlite3 *db)
{
    char str[90] = {0};
    void *arg = NULL;
    char **errmsg = NULL;

    sprintf(str,"select * from users where phoneNumber = '%s';",phoneNumber);
    if(sqlite3_exec(db,str,callback,arg,errmsg) != SQLITE_OK)
    {
        cout << "注册操作失败：" << sqlite3_errmsg(db) << endl;
    }

	if(arg)
    {
        cout << "-\t该手机号已经注册过，如果忘记密码请申请找回-" << endl;
		return 1;
    }
	
	memset(&str,0,sizeof(str));
    sprintf(str,"insert into users values ('%s','%s','%s');",userName,phoneNumber,password); 
    if(sqlite3_exec(db,str,NULL,NULL,errmsg) != SQLITE_OK)
    {
        cout << "注册失败：" << sqlite3_errmsg(db);
		return 1;
    }

	cout << "\t-注册成功-" << endl;
    return 9;
}

//用户登录
int Sqlite3::sqlite3_login(sqlite3 *db)
{
    char **resultp = NULL;
    char str[90] = {0};
    char **errmsg = NULL;
    int row = 0,column = 0;
   
    sprintf(str,"select * from users where userName = '%s' and password = '%s';",userName,password);
    if(sqlite3_get_table(db,str,&resultp,&row,&column,errmsg) != SQLITE_OK)
    {
        cout << "登录失败：" << sqlite3_errmsg(db) << endl;
        return 2;
    }

	printf("&resultp:%s\n",*resultp);
	printf("&row:%d\n",row);
	printf("&column:%d\n",column);

    if(row > 0)//返回行号则查询成功，反之失败
    {
        cout << "\t-登录成功-" << endl;
    }else{
        cout << "\t-用户名或密码错误-" << endl;
		return 2;
    }

    return 9;
}

//忘记密码
int Sqlite3::sqlite3_findback(sqlite3 *db)
{
    char **resultp = NULL;
    char str[90] = {0};
    char **errmsg = NULL;
    int row = 0,column = 0;

    sprintf(str,"select * from users where userName = '%s' and password = '%s';",userName,phoneNumber);
    if(sqlite3_get_table(db,str,&resultp,&row,&column,errmsg) != SQLITE_OK)
    {
        cout << "找回失败：" << sqlite3_errmsg(db) << endl;
        return 4;
    }

    if(row > 0)//返回行号则查询成功，反之失败
    {
        cout << "\t-匹配成功-" << endl;
    }else{
        cout << "\t-用户名或手机号错误-" << endl;
		return 4;
    }
   
    return 9;
}

//修改密码
int Sqlite3::sqlite3_update(sqlite3 *db)
{
    char **errmsg;
    char str[90] = {0};
    int row = 0,column = 0;

    sprintf(str,"update users set password = '%s' where userName = '%s' and password = '%s';",newPassword,userName,password);
    if(sqlite3_exec(db,str,NULL,NULL,errmsg) != SQLITE_OK)
    {
        cout << "密码修改失败：" << sqlite3_errmsg(db) << endl;
        exit(EXIT_FAILURE);
    }
   
    return 9;
}

int Sqlite3::dataBaseDeal(int options)
{
	int key = 0; 
    sqlite3 *db;
    char str[128] = {0};
	int static ret = 0;
	char **errmsg = NULL;

	//创建数据库[UserDataBase.db]
    sqlite3_open("UserDataBase.db",&db);
    if(sqlite3_open("UserDataBase.db",&db) != SQLITE_OK)
    {
        cout << "数据库打开失败：" << sqlite3_errmsg(db);
        exit(EXIT_FAILURE);
    }
 
	//创建表格：用户名-电话号码-密码 
#if 0 
	if(0 == ret){
	    sprintf(str,"create table users (userName,phoneNumber,password);");
	    if(sqlite3_exec(db,str,0,0,errmsg) != SQLITE_OK)
	    {
	        cout << "用户表创建失败：" <<  sqlite3_errmsg(db) << endl;
	        exit(EXIT_FAILURE);
	    }
		ret ++;
	}
#endif	

	switch(options)
	{
	case 1://注册
		{
		key = sqlite3_insert(db);
		break;
		}
	case 2://登录
		{
		key = sqlite3_login(db);
		break;
		}
	case 3://忘记密码
		{
		key = sqlite3_findback(db);
		break;
		}
	case 31://匹配验证码
		{
			if(strncmp(randomCode,"qDtT6H",6) == 0){
				key == 9;
				cout << "密码找回成功" << endl;		
			}else{
				key == 6;
				cout << "验证码错误" << endl;		
			}
		break;
		}
	case 4://修改密码
		{
		key = sqlite3_update(db);
		break;
		}		
	}

    sqlite3_close(db);

    return key;
}
