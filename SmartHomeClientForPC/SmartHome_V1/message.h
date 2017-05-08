#ifndef MESSAGE_H
#define MESSAGE_H

#include <QJsonObject>


class message
{
public:
    unsigned char    type;       //消息类型
    char    funNum;             //功能号
    short   length;             //JSON数据长度
    QJsonObject json;           //JSON语句

public:
    message();
};


/********************************
**************JSON 格式的 KEY 值*******************
userName 				用户名
oldPassword 			旧密码
newPassword 			新密码
password 				密码
phoneNumber 			手机号
userToken 				用户身份识别码
randomCode 				验证码（6 位大小写字母和数字组成）
stateCode 				状态值（int）
temperature 			温度
humidity 				湿度
triaxialX 				三轴 x
triaxialY 				三轴 y
triaxialZ 				三轴 z
light 					光照
deviceNumber 			设备号
deviceCode 				设备操作码
deviceState 			设备状态码
videoList 				获取视频文件列表

**********8服务器返回给客户端的状态码**********
stateCode ：0 	客户端请求成功
stateCode ：1 	客户端请求失败
stateCode ：2 	客户端用户名错误
stateCode ：3 	客户端密码错误
stateCode ：4 	客户端手机号错误
stateCode ：5 	客户端验证码错误
stateCode ：6 	客户端 userToken 错误
stateCode ：7 	客户端 userToken 过期

***********客户端请求设备操作码***************
deviceCode:0 	请求打开设备
deviceCode:1 	请求关闭设备
deciceCode:2 	请求获取当前设备的状态服务器返回给客户端的设备状态：
deviceState:0 	设备处于打开状态
deviceState:1 	设备处于关闭状态
deviceState:2 	设备处于故障状态

**********************************************/

#endif // MESSAGE_H
