// DGAS (DataGrid Accounting System) 
// Client APIs.
// 
// $Id: pingClient.h,v 1.1.2.1.4.1.2.2 2012/03/16 09:58:21 aguarise Exp $
// -------------------------------------------------------------------------
// Copyright (c) 2001-2002, The DataGrid project, INFN, 
// All rights reserved. See LICENSE file for details.
// -------------------------------------------------------------------------
// Author: Andrea Guarise <andrea.guarise@to.infn.it>
 /***************************************************************************
 * Code borrowed from:
 *  authors   :
 *  copyright : 
 ***************************************************************************/
//
//    
// ---------------------------------------------------------------------------
//  Includes
// ---------------------------------------------------------------------------

#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#ifdef NOGSI
#include "glite/dgas/common/tls/SocketClient.h"
#include "glite/dgas/common/tls/SocketAgent.h"
#else
#include "glite/dgas/common/tls/GSISocketClient.h"
#include "glite/dgas/common/tls/GSISocketAgent.h"
#endif

using namespace std;
#ifndef GLITE_SOCKETPP
using namespace glite::wmsutils::tls::socket_pp;
#endif



struct statusInfo {
        string engines;
        int uiRequests;
        int ATMRequests;
        int paRequests;
        int pingRequests;
        int authErrors;
	string serverMessage;
};

struct errorInfo {
        string engines;
        int uiErrors;
        int ATMErrors;
        int bankErrors;
        int paErrors;
        int pingErrors;
};

//0 on success > 0 on failure
int dgas_ping_client(string &acct_id,
	        int pingType,	
		statusInfo *status,
		errorInfo *errors,
		string *server_answer);


