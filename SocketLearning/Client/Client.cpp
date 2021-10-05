// CMakeClientTCP.cpp: 目标的源文件。
//
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define WIN32_LEAN_AND_MEAN

#include<Windows.h>
#include<WinSock2.h>
#include<iostream>
#include<cstdio>
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
	WORD ver = MAKEWORD(2, 2);
	WSADATA dat;
	WSAStartup(ver, &dat);

	//建立简易TCP客户端
	//SOCKET 建立一个socket
	SOCKET _sock = socket(AF_INET, SOCK_STREAM, 0);  //客户端不用声明TCP
	if (SOCKET_ERROR == _sock)
	{
		cout << "建立失败" << endl;
	}
	else
	{
		cout << "建立成功" << endl;
	}

	//connect 连接服务器
	SOCKADDR_IN _sin = {};
	_sin.sin_family = AF_INET;
	_sin.sin_port = htons(4567);
	_sin.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	int ret = connect(_sock, (SOCKADDR*)&_sin, sizeof(SOCKADDR_IN));
	if (SOCKET_ERROR == ret)
	{
		cout << "连接失败" << endl;
	}
	else
	{
		cout << "连接成功" << endl;
	}

	//接收数据
	char _cmdBuf[256] = {};
	while (true)
	{
		//输入请求
		cout << "请输入您的请求：";
		cin >> _cmdBuf;

		//处理请求，判断发不发送
		if (0 == strcmp(_cmdBuf, "exit"))
		{
			break;
		}
		else if (0 == strcmp(_cmdBuf, "login"))
		{
			cout << "请输入用户名：";
			string usrName;
			cin >> usrName;
			cout << "请输入用户密码：";
			string passW;
			cin >> passW;

			Login login;

			strcpy(login.userName, usrName.c_str());
			strcpy(login.PassWord, passW.c_str());

			DataHeader header;
			header.dataLength = sizeof(login);
			header.cmd = CMD_LOGIN;

			//发送命令
			send(_sock, (char*)&header, sizeof(header), 0);
			Sleep(10);
			send(_sock, (char*)&login, sizeof(login), 0);

			//接收回收的命令
			LoginResult loginRes = {};
			int nlen = recv(_sock, (char*)&loginRes, sizeof(loginRes), 0);
			if (nlen > 0)
			{

				cout << "登录结果：<" << loginRes.flag << "> " << loginRes.AddMsg << endl;
			}
			else
			{
				cout << "接收失败" << endl;
			}
		}
		else if (0 == strcmp(_cmdBuf, "logout"))
		{
			cout << "请输入用户名：";
			string usrName;
			cin >> usrName;
			Logout logout;
			strcpy(logout.userName, usrName.c_str());

			DataHeader header;
			header.dataLength = sizeof(logout);
			header.cmd = CMD_LOGOUT;
			send(_sock, (char*)&header, sizeof(header), 0);
			send(_sock, (char*)&logout, sizeof(logout), 0);

			//接收回收的命令
			LogoutResult logoutRes = {};
			int nlen = recv(_sock, (char*)&logoutRes, sizeof(logoutRes), 0);
			if (nlen > 0)
			{

				cout << "退出结果：<" << logoutRes.flag << "> " << logoutRes.AddMsg << endl;
			}
			else
			{
				cout << "接收失败" << endl;
			}
		}
		else
		{
			cout << "请求姿势不对 T_T" << endl;
			//continue;
		}

	}

	//关闭socke
	closesocket(_sock);

	WSACleanup();

	getchar();
	return 0;
}
