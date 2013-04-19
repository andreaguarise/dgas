// DGAS (DataGrid Accounting System) 
// Client APIs.
// 
// $Id: legacyProducer.h,v 1.1.2.4.2.4 2012/03/15 15:21:20 aguarise Exp $
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
#include <iostream>
#include <sstream>
#include <string>
#include <unistd.h>
#include <vector>

#ifdef NOGSI
#include "glite/dgas/common/tls/SocketClient.h"
#include "glite/dgas/common/tls/SocketAgent.h"
#else
#include "glite/dgas/common/tls/GSISocketClient.h"
#include "glite/dgas/common/tls/GSISocketAgent.h"
#endif


#define ATM_CLIENT_VERSION "glite"

using namespace std;

#ifndef GLITE_SOCKETPP
using namespace glite::wmsutils::tls::socket_pp;
#endif


struct producerConfiguration {
	string configFileName;
	string hlrServer;
	string paServer;
	bool economicAccounting;
};

int ATM_client_toResource(
		string& input, 
		string *server_answer,
		producerConfiguration& conf);


//Parse the XML input, returns 0 if success >1 otherwise, differerntly from
//the other client code, here we will not use Xerces since we want the code
//to be very light and easily portable.
int ATMc_parse_xml(string input_message);

