// CMakeServerTCP.cpp: 目标的源文件。
//
#define WIN32_LEAN_AND_MEAN
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include<Windows.h>
#include<WinSock2.h>
#include <iostream>
#include<cstdio>
#include<sstream>
#include<string>
#include<map>
using namespace std;

#pragma comment(lib, "ws2_32.lib")

//login
enum CMD
{
	CMD_LOGIN,
	CMD_LOGOUT,
	CMD_ERROR
};
struct DataHeader
{
	short dataLength;
	short cmd;
};
//DataPackage
struct Login
{
	char userName[32];
	char PassWord[32];
};

struct LoginResult
{
	int flag;
	char AddMsg[128];

};

struct Logout
{
	char userName[32];
};

struct LogoutResult
{
	int flag;
	char AddMsg[128];
};

int main(int argc, const char* argv[])
{
	//启动windows socket 2.x环境
	WORD ver = MAKEWORD(2, 2); //版本号
	WSADATA dat;
	WSAStartup(ver, &dat);

	//建立简易TCP服务端
	//建立socket
	SOCKET _sock = socket(AF_INET, SOCK_STREAM, 0); //面向数据流,协议是tcp

	//bind 绑定用于接收客户端连接的网络端口
	sockaddr_in _sin = {};
	_sin.sin_family = AF_INET;
	_sin.sin_port = htons(4567); //host to net unsigned short
	_sin.sin_addr.S_un.S_addr = INADDR_ANY;//一个主机下面很多ip地址，纯内网用本机的地址inet_addr("127.0.0.1")
	if (SOCKET_ERROR == bind(_sock, (sockaddr*)&_sin, sizeof(_sin))) //socket网络地址
	{
		cout << "绑定失败" << endl;
	}
	else
	{
		cout << "绑定成功" << endl;

	}
	//listen 监听网络端口
	if (SOCKET_ERROR == listen(_sock, 5))
	{
		cout << "监听失败" << endl;
	}
	else
	{
		cout << "监听成功" << endl;
	}

	//accept等待接受客户端连接,send向客户端发送数据
	sockaddr_in clienAddr = {};
	int nAddrLen = sizeof(sockaddr_in);
	SOCKET _cSock = INVALID_SOCKET;
	_cSock = accept(_sock, (sockaddr*)&clienAddr, &nAddrLen); //第三个参数传入结构的长度
	if (INVALID_SOCKET == _cSock)
	{
		cout << "接收到无效客户端socke" << endl;
	}
	cout << "新客户端加入：IP = " << inet_ntoa(clienAddr.sin_addr) << " socket: " << _cSock << endl;

	while (1)
	{
		DataHeader head = {};
		//接收客户端数据
		recv(_cSock, (char*)&head, sizeof(head), 0);
		/*if (rlen <= 0)
		{
			cout << "客户端退出, 再见了您嘞";
			break;
		}*/
		cout << "收到消息内容的长度为：" << head.dataLength << " 收到命令的内容为：" << head.cmd << endl;

		switch (head.cmd)
		{
			case CMD_LOGIN:
			{
				Login login = {};
				recv(_cSock, (char*)&login, sizeof(Login), 0);

				if (login.userName == "Lufter" && login.PassWord == "1884")
				{
					LoginResult loginRes = { 0, "登陆成功" };
					send(_cSock, (char*)&loginRes, sizeof(loginRes), 0);
				}
				else
				{
					LoginResult loginRes = { 1, "登录失败" };
					send(_cSock, (char*)&loginRes, sizeof(loginRes), 0);
				}
			}
				break;
			case CMD_LOGOUT:
			{
				Logout logout = {};
				recv(_cSock, (char*)&logout, sizeof(logout), 0);

				LogoutResult ret;
				ret.flag = 0;
				stringstream t;
				t << logout.userName;
				string msg = t.str();
				msg += "已退出";
				strcpy(ret.AddMsg, msg.c_str());
				send(_cSock, (char*)&ret, sizeof(ret), 0);
			}
				break;
			default:
			{
				head.cmd = CMD_ERROR;
				head.dataLength = 0;
				send(_cSock, (char*)&head, sizeof(head), 0);
			}
				break;
		}
	}


	//关闭socket
	closesocket(_sock);

	//清除windows socket环境
	WSACleanup();

	getchar();

	return 0;
}


