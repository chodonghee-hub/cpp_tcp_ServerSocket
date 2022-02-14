#pragma once
#include <iostream>
#include <WinSock2.h>
#include <map>
#include <mutex>
#include <thread>
#include "cClntObj.h"
#include "cSocketInfo.h"

#pragma comment(lib, "ws2_32.lib")
using namespace std;

class cServerSocket
{
private :
	map<SOCKET, cClntObj*> SocketMap;
	WSADATA		wsaData;
	SOCKET		s, clnt;
	SOCKADDR_IN	addr, clnt_addr;
	mutex		mtx;
	bool		b_AcceptFlag;	
	bool		b_ServerFlag;
	
public :
	cServerSocket();
	~cServerSocket();

	void __Init__();
	void _SetSocketInfo();
	void _EraseClntSocket(SOCKET* _sock);
	static void _AcceptClient(LPVOID lp);	
	bool bCheckWinSock();
	bool bCheckBind(int _PortNum);
	bool bCheckListen();
	bool bCheckValidSocket(SOCKET* _sock);	
	map<SOCKET, cClntObj*> _GetSocketMap();
};

