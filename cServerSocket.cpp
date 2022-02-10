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
//	설명 :	cServerSocket 시작 
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
//	설명 :	서버 소켓 설정
//##################################################################
void cServerSocket::_SetSocketInfo()
{
	s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
}

//##################################################################
//	설명 :	exit 호출 시 SOCKET, cClntObj를 map에서 삭제, mutex 사용
//##################################################################
void cServerSocket::_EraseClntSocket(SOCKET * _sock)
{
	mtx.lock();						
	cClntObj* _target = SocketMap.at(_sock);
	SocketMap.erase(_sock);
	closesocket(*_sock);
	cout << "** Erase Socket " << endl;
	delete _target;
	_target = nullptr;
	mtx.unlock();
}

//##################################################################
//	설명 :	클라이언트와 연결 ( thread 사용, map에 저장하여 관리 ) 
//##################################################################
void cServerSocket::_AcceptClient(LPVOID lp)
{
	cServerSocket* p = (cServerSocket*)lp;
	
	int clnt_size = sizeof(p->clnt_addr);
	
	while (p->b_AcceptFlag)
	{
		clog << "** _AcceptClient() running " << endl;
		p->clnt = accept(p->s, (SOCKADDR*)&p->clnt_addr, &clnt_size);
		if (p->bCheckValidSocket(&p->clnt))
		{
			p->SocketMap.insert({ &p->clnt, new cClntObj(lp, &p->clnt) });
			p->SocketMap.at(&p->clnt)->__ClntInit__();
		}

		Sleep(1000);
	}
}

//##################################################################
//	설명 :	WSAStartup 윈속 버전 확인 및 사용요청 
//##################################################################
bool cServerSocket::bCheckWinSock()
{
	if (WSAStartup(WINSOCK_VERSION, &wsaData) != 0)
	{
		cout << "● bCheckWinSock() → WSAStartup error : " << WSAGetLastError() << endl;
		WSACleanup();
		return false;
	}
	return true;
}

//##################################################################
//	설명 :	서버 소켓 바인드 ( 포트번호 : 8080 ) 
//##################################################################
bool cServerSocket::bCheckBind(int _PortNum)
{
	addr.sin_family = AF_INET;
	addr.sin_port = htons(_PortNum);
	addr.sin_addr.S_un.S_addr = htonl(ADDR_ANY);
	if (::bind(s, (SOCKADDR*)&addr, sizeof(addr)) == SOCKET_ERROR)
	{
		cout << "● bCheckBind() → Bind error : " << WSAGetLastError() << endl;
		WSACleanup();
		return false;
	}
	return true;
}

//##################################################################
//	설명 :	클라이언트 접속 대기 listen ( SOMAXCONN : 최대 접속 ) 
//##################################################################
bool cServerSocket::bCheckListen()
{
	if (listen(s, SOMAXCONN) != 0)
	{
		cout << "● bCheckListen() → Listen error : " << WSAGetLastError << endl;
		WSACleanup();
		return false;
	}
	cout << "[ Init Server Client ]" << endl;
	b_AcceptFlag = true;
	return true;
}

//##################################################################
// 설명 :	INVALID_SOCKET 확인 
//##################################################################
bool cServerSocket::bCheckValidSocket(SOCKET * _sock)
{
	if (*_sock == INVALID_SOCKET)
	{
		cout << "● _CheckValidSocket → INVALID_SOCKET error : " << WSAGetLastError() << endl;
		return false;
	}
	return true;
}
