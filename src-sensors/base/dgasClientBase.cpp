/*
 * dgasClient.cpp
 * 
 * Copyright (c) 2002 EU DataGrid.
 * For license conditions see http://www.eu-datagrid.org/license.html
 */

#include <string>
#include <iostream>
#include "glite/dgas/hlr-clients/base/dgasClientBase.h"

#ifdef NOGSI
#include "glite/dgas/common/tls/SocketClient.h"
#include "glite/dgas/common/tls/SocketAgent.h"
#else
#include "glite/dgas/common/tls/GSISocketClient.h"
#include "glite/dgas/common/tls/GSISocketAgent.h"
#endif

namespace glite {
	namespace socket_pp = wmsutils::tls::socket_pp;
namespace workload {
namespace dgas {
namespace clients {

dgasClient::dgasClient(const std::string& h, int p, const std::string& c)
{
	connection = NULL;
#ifdef NOGSI
	connection = new socket_pp::SocketClient(h,p);
	connection->getAgent();
#else
	connection = new socket_pp::GSISocketClient(h,p);
	if (connection->getAgent())
		connection->ServerContact(c);
#endif
}

dgasClient::~dgasClient()
{
	if (connection) delete connection;
}

bool dgasClient::connect()
{
	if ( connection == NULL ) return false;
	return ( connection -> Open());
}

bool dgasClient::disconnect()
{
	if( !connection ) return false;
	return connection -> Close();
}

bool dgasClient::send(const std::string& xml)
{
	return ( connection->Send(xml) );
}

bool dgasClient::receive(std::string& xml)
{
	return ( connection->Receive(xml) );
}

}// namespace clients
}// namespace dgas
}// namespace workload
}// namespace edg
