#include "cClntObj.h"
#include "cServerSocket.h"

cClntObj::cClntObj(LPVOID _pServerSocket, SOCKET _pSocket)
{
	cout << "[ Client Accepted ]" << endl;
	p_ServerSock = _pServerSocket;
	p_Clnt = _pSocket;
	b_ClntObjFlag = true;
	b_MessageFlag = false;
}

cClntObj::~cClntObj()
{
	cout << "** Delete Clnt Object" << endl;
}

//##################################################################
// 설명 :	cClntObj 시작 ( Receive, Send 쓰레드 설정 ) 
//##################################################################
void cClntObj::__ClntInit__()
{

	thread RecvThread = thread(_RecvDataFrom, this);
	//thread SendThread = thread(_SendDataTo, this);

	RecvThread.detach();
	//SendThread.detach();
}

//##################################################################
//	설명 :	p_Clnt로부터 받은 데이터를 cBuffer에 저장 
//##################################################################
void cClntObj::_RecvDataFrom(LPVOID lp)
{
	cClntObj* p = (cClntObj*)lp;
	while (p->b_ClntObjFlag)
	{
		ZeroMemory(p->cBuffer, strlen(p->cBuffer));
		if (recv(p->p_Clnt, p->cBuffer, 1024, 0) <= 0)
		{
			p->b_ClntObjFlag = false;
		}
		cout << "▶ now client message : " << p->cBuffer << endl;
		if (strncmp(p->cBuffer, "exit", strlen("exit")) == 0)
		{
			p->b_ClntObjFlag = false;
		}
		else
		{
			p->qMessageQueue.push(p->cBuffer);
			cout << "▶ qMessageQueue size : " << p->qMessageQueue.size() << endl;
			p->SendDataTo_test();
			
		}		
		Sleep(1);
	}	
	p->_BreakConnection(p);
}

//##################################################################
//	설명 :	입력 받은 데이터를 p_Clnt에 전달 
//##################################################################
void cClntObj::_SendDataTo(LPVOID lp)
{
	cClntObj* p = (cClntObj*)lp;
	while (p->b_ClntObjFlag)
	{
		cout << "\n● Text your message : ";
		cin >> p->cBuffer;
	
		if (strncmp(p->cBuffer, "exit", strlen(p->cBuffer)+1) == 0)
		{
			cout << "[ Break Connection ]" << endl;
			p->b_ClntObjFlag = false;
		}
		else
		{
			send(p->p_Clnt, p->cBuffer, strlen(p->cBuffer), 0);
		}
		ZeroMemory(p->cBuffer, strlen(p->cBuffer));
		Sleep(1);
	}
	
}

//##################################################################
//	설명 :	입력 받은 데이터를 p_Clnt에 전달 
//##################################################################
void cClntObj::SendDataTo_test()
{
	cServerSocket* p = (cServerSocket*)p_ServerSock;
	while(!qMessageQueue.empty() && b_ClntObjFlag)
	{
		strncpy(cBuffer, qMessageQueue.front(), 1024);
		qMessageQueue.pop();
		for (auto iter : p->_GetSocketMap())
		{
			// iter.first == p_Clnt → continue 
			if (iter.first != p_Clnt)
			{
				send(iter.first, cBuffer, strlen(cBuffer), 0);
			}
		}

		Sleep(1);
	}
	
}

//##################################################################
//	설명 :	Receive, Send에서 exit 호출 시, p_Clnt삭제 (cServerSocket)
//##################################################################
void cClntObj::_BreakConnection(LPVOID lp)
{
	cClntObj* pObj = (cClntObj*)lp;
	while (pObj->b_ClntObjFlag) { Sleep(1); }
	pObj->qMessageQueue = queue<char*>();
	cServerSocket* p = (cServerSocket*) pObj->p_ServerSock;
	p->_EraseClntSocket(&pObj->p_Clnt);
}
