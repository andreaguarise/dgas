// DGAS (DataGrid Accounting System) 
// Client APIs.
// 
// $Id: atmResBankClient.h,v 1.1.2.1.4.2.2.1 2012/03/16 09:58:22 aguarise Exp $
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
#ifndef RESBANK_CLIENT_H
#define RESBANK_CLIENT_H

#include <string.h>
#include <stdlib.h>
#include <vector>
#include <string>
#include <sstream>
#include <fstream>

#include "glite/dgas/hlr-service/base/db.h"
#include "glite/dgas/common/base/comm_struct.h"
#include "glite/dgas/common/base/dgasVersion.h"
#include "glite/dgas/common/hlr/hlr_prot_errcode.h"
#include "glite/dgas/common/base/libdgas_log.h"
#include "glite/dgas/common/base/xmlUtil.h"
#include "glite/dgas/common/base/int2string.h"
#include "glite/dgas/common/base/stringSplit.h"
#include "glite/dgas/hlr-service/engines/engineCmnUtl.h"


#include "glite/dgas/hlr-service/base/hlrTransaction.h"
#include "glite/dgas/hlr-service/base/transInLog.h"

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
//parse XML and return value of STATUS tag

namespace dgasResBankClient
{
	int bankClient(hlrTransaction &t);
};
#endif