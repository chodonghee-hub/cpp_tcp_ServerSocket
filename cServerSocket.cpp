#include "cServerSocket.h"

cServerSocket::cServerSocket()
{
	SocketMap.clear();
	b_ServerFlag = true;
	__Init__();	
}

cServerSocket::~cServerSocket()
{
	
	cout << "[ Finish ] " << endl;
}

//##################################################################
//	���� :	cServerSocket ���� 
//##################################################################
void cServerSocket::__Init__()
{
	if (!bCheckWinSock())		
	{ 
		return; 
	}

	_SetSocketInfo();

	if (!bCheckValidSocket(&s))
	{
		return; 
	}

	if (!bCheckBind(8080))			
	{ 
		return; 
	}

	if (!bCheckListen())		
	{ 
		return; 
	}

	thread AcceptThread = thread(_AcceptClient, this);
	AcceptThread.join();

	while (b_ServerFlag)		
	{ 
		Sleep(1); 
	}
	closesocket(s);
	WSACleanup();
}

//##################################################################
//	���� :	���� ���� ����
//##################################################################
void cServerSocket::_SetSocketInfo()
{
	s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
}

//##################################################################
//	���� :	exit ȣ�� �� SOCKET, cClntObj�� map���� ����, mutex ���
//##################################################################
void cServerSocket::_EraseClntSocket(SOCKET * _sock)
{
	mtx.lock();						
	cClntObj* _target = SocketMap.at(*_sock);
	SocketMap.erase(*_sock);
	closesocket(*_sock);
	cout << "\n** Erase Socket " << endl;
	delete _target;
	_target = nullptr;
	mtx.unlock();
}

//##################################################################
//	���� :	Ŭ���̾�Ʈ�� ���� ( thread ���, map�� �����Ͽ� ���� ) 
//##################################################################
void cServerSocket::_AcceptClient(LPVOID lp)
{
	cServerSocket* p = (cServerSocket*)lp;
	
	int clnt_size = sizeof(p->clnt_addr);
	
	while (p->b_AcceptFlag)
	{
		clog << "\n\n========================================" << endl;
		clog << "** _AcceptClient() running " << endl;
		clog << "** Accepted clnt : " << p->SocketMap.size() << endl;
		p->clnt = accept(p->s, (SOCKADDR*)&p->clnt_addr, &clnt_size);
		if (p->bCheckValidSocket(&p->clnt))
		{
			p->SocketMap.insert({ p->clnt, new cClntObj(lp, p->clnt) });
			p->SocketMap.at(p->clnt)->__ClntInit__();
		}

		Sleep(1000);
	}
}

//##################################################################
//	���� :	WSAStartup ���� ���� Ȯ�� �� ����û 
//##################################################################
bool cServerSocket::bCheckWinSock()
{
	if (WSAStartup(WINSOCK_VERSION, &wsaData) != 0)
	{
		cout << "�� bCheckWinSock() �� WSAStartup error : " << WSAGetLastError() << endl;
		WSACleanup();
		return false;
	}
	return true;
}

//##################################################################
//	���� :	���� ���� ���ε� ( ��Ʈ��ȣ : 8080 ) 
//##################################################################
bool cServerSocket::bCheckBind(int _PortNum)
{
	addr.sin_family = AF_INET;
	addr.sin_port = htons(_PortNum);
	addr.sin_addr.S_un.S_addr = htonl(ADDR_ANY);
	if (::bind(s, (SOCKADDR*)&addr, sizeof(addr)) == SOCKET_ERROR)
	{
		cout << "�� bCheckBind() �� Bind error : " << WSAGetLastError() << endl;
		WSACleanup();
		return false;
	}
	return true;
}

//##################################################################
//	���� :	Ŭ���̾�Ʈ ���� ��� listen ( SOMAXCONN : �ִ� ���� ) 
//##################################################################
bool cServerSocket::bCheckListen()
{
	if (listen(s, SOMAXCONN) != 0)
	{
		cout << "�� bCheckListen() �� Listen error : " << WSAGetLastError << endl;
		WSACleanup();
		return false;
	}
	cout << "[ Init Server Client ]" << endl;
	b_AcceptFlag = true;
	return true;
}

//##################################################################
// ���� :	INVALID_SOCKET Ȯ�� 
//##################################################################
bool cServerSocket::bCheckValidSocket(SOCKET * _sock)
{
	if (*_sock == INVALID_SOCKET)
	{
		cout << "�� _CheckValidSocket �� INVALID_SOCKET error : " << WSAGetLastError() << endl;
		WSACleanup();
		return false;
	}
	return true;
}

map<SOCKET, cClntObj*> cServerSocket::_GetSocketMap()
{
	return SocketMap;
}
