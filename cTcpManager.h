#pragma once
#include "cServerSocket.h"

class cTcpManager
{
private :
	cServerSocket* pServerSocket;
public  :
	cTcpManager();
	~cTcpManager();
};