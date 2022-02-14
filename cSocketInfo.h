#pragma once
#include <iostream>
#include <winsock2.h>
#include <thread>

#pragma comment(lib, "ws2_32.lib")

using namespace std;

class cSocketInfo
{
private:
	SOCKET* p_ClntSocket;
	LPVOID	p_ClntObj;

public :
	cSocketInfo(SOCKET* _pSock, LPVOID _pClntObj);
};

