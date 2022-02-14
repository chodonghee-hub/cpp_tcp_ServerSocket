#include "cSocketInfo.h"

cSocketInfo::cSocketInfo(SOCKET* _pSock, LPVOID _pClntObj)
{
	p_ClntSocket = _pSock;
	p_ClntObj = _pClntObj;
}
