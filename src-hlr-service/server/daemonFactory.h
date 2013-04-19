#ifndef daemonFactory_h
#define daemonFactory_h

#include "glite/dgas/common/base/int2string.h"
#ifdef NOGSI
#include "glite/dgas/common/tls/SocketServer.h"
#include "glite/dgas/common/tls/SocketAgent.h"
#else
#include "glite/dgas/common/tls/GSISocketServer.h"
#include "glite/dgas/common/tls/GSISocketAgent.h"
#endif

using namespace std;
using namespace glite::wmsutils::tls::socket_pp;

//the structure passed to the thread
struct threadStruct{
#ifdef NOGSI
	SocketAgent* a;
	SocketServer* s;
#else
	GSISocketAgent* a;
	GSISocketServer* s;
#endif

	int tN;
	bool isIn;
};

//the loop implementing the thread communication with the remote peer
void* thrLoop(void *param);

int putLock(string lockFile);

int removeLock( string lockFile);

#endif
