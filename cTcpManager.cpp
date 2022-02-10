#include "cTcpManager.h"

cTcpManager::cTcpManager()
{
	pServerSocket = new cServerSocket;
}

cTcpManager::~cTcpManager()
{
	delete[] pServerSocket;
	pServerSocket = nullptr;
}
