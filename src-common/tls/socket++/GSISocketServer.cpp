/***************************************************************************
 *  filename  : GSISocketServer.cpp
 *  authors   : Salvatore Monforte <salvatore.monforte@ct.infn.it>
 *  copyright : (C) 2001 by INFN
 ***************************************************************************/

// $Id:

/** The globus secure shell API definitions. */
#include <gssapi.h>
#include <memory.h>
#include <time.h>
#include <stdio.h>

/** Functionalities for transmitting and receiveing tokens. */
#include "tokens.h"
/** The secure Socket Agent used for communication. */
#include "glite/dgas/common/tls/GSISocketAgent.h"
/** This class header file. */
#include "glite/dgas/common/tls//GSISocketServer.h"

//extern ofstream logStream;

namespace glite {   
namespace wmsutils { 
namespace tls {
namespace socket_pp {

/** The data struct containing the authentication context. */
struct GSIAuthenticationContext
{
	std::string	certificate_subject;         /**< The certificate subject. */
	gss_cred_id_t  credential;                 /**< The credential identifier. */

	/**
	 * Constructor.
	 */
	GSIAuthenticationContext() {
		certificate_subject="";
		credential = GSS_C_NO_CREDENTIAL;
	}
};

void GSISocketServer::set_auth_timeout(int to)
{
	m_auth_timeout = to;
}
/**
 * Constructor.
 * @param p the secure server port.
 * @param b the backlog, that is the maximum number of outstanding connection requests.
 */
GSISocketServer::GSISocketServer(int p, int b) : SocketServer(p,b)
{
	gsi_logfile = stdout;
	limited_proxy_mode = normal;
	m_auth_timeout = -1 ;
          pthread_mutex_init(&GSISocketServerAuthMutex,NULL);
          pthread_mutex_init(&GSISocketServerAcquireMutex,NULL);
}

/**
 * Destructor.
 */
GSISocketServer::~GSISocketServer()
{
	Close();
	  pthread_mutex_destroy(&GSISocketServerAuthMutex);
	  pthread_mutex_destroy(&GSISocketServerAcquireMutex);
}

/**
 * Close the connection.
 */
void GSISocketServer::Close()
{
	SocketServer::Close();
}

/**
 * Accept the GSI Authentication.
 * @param sock the socket for communication.
 * @param ctx the authorization context.
 * @return the context identifier.
 */
gss_ctx_id_t GSISocketServer::AcceptGSIAuthentication(int sock, GSIAuthenticationContext& ctx)
{
	OM_uint32      major_status, minor_status;
	OM_uint32      ret_flags = 0;
	int            user_to_user = 0;
	int            token_status = 0;
	gss_ctx_id_t   context = GSS_C_NO_CONTEXT;
	char           *name = NULL;
	gss_cred_id_t  delegated_cred = GSS_C_NO_CREDENTIAL;

	ret_flags =  limited_proxy_mode == normal ? GSS_C_GLOBUS_LIMITED_PROXY_FLAG:GSS_C_GLOBUS_LIMITED_PROXY_MANY_FLAG;
	std::pair<int,int> arg(sock, m_auth_timeout);
    pthread_mutex_lock(&GSISocketServerAcquireMutex);
	major_status = globus_gss_assist_accept_sec_context(&minor_status,
			&context,
			ctx.credential,
			&name,
			&ret_flags,
			&user_to_user,
			&token_status,
			// &delegated_cred,
			GLOBUS_NULL,
			&get_token,
			(void *) &arg,
			&send_token,
			(void *) &arg);

	if( GSS_ERROR(major_status) ) 
	{
		char *gssmsg;
		globus_gss_assist_display_status_str( &gssmsg, 
				NULL,
				major_status,
				minor_status,
				token_status);
		if( context != GSS_C_NO_CONTEXT ) 
		{ 
			gss_delete_sec_context(&minor_status, &context, GSS_C_NO_BUFFER); 
			context = GSS_C_NO_CONTEXT; 
		}
		if (delegated_cred != GSS_C_NO_CREDENTIAL) 
		{
			gss_release_cred(&minor_status, &delegated_cred);
			delegated_cred = GSS_C_NO_CREDENTIAL;
		}
		if( name ) 
		{		
			free(name);
		}
		//std::string source(gssmsg);//FIXME log this
		//cerr << source << endl;
		free(gssmsg);	
	}
	else
	{
		ctx.certificate_subject = name; 
		if( name ) free(name);
	}
	if (delegated_cred != GSS_C_NO_CREDENTIAL)
        {
		gss_release_cred(&minor_status, &delegated_cred);
	}
	pthread_mutex_unlock(&GSISocketServerAcquireMutex);
	return context;
}

/**
 * Listen for incoming connection requests.
 * Accept incoming requests and redirect communication on a dedicated port.
 * @param a a reference to the secure GSI Socket Agent sent by Client.
 * @return the GSI Socket Agent redirecting communication on a dedicated port.
 */
GSISocketAgent* GSISocketServer::Listen()
{
	GSISocketAgent*  sa;
	sa = static_cast<GSISocketAgent*>(SocketServer::Listen(new GSISocketAgent));
	return sa;
}

bool GSISocketServer::AuthenticateAgent(GSISocketAgent* sa)
{
	gss_ctx_id_t   context = GSS_C_NO_CONTEXT;
	OM_uint32      major_status, minor_status;
	int sd = 0;
	GSIAuthenticationContext ctx;
	if (sa)
	{
		sd = sa -> SocketDescriptor();
			//mutex here
		pthread_mutex_lock(&GSISocketServerAcquireMutex);
		major_status =
				globus_gss_assist_acquire_cred(&minor_status,
						GSS_C_BOTH,
						&(ctx.credential));
		pthread_mutex_unlock(&GSISocketServerAcquireMutex);
		if(GSS_ERROR(major_status))
		{
			SocketServer::KillAgent(sa);
			sa = NULL;
			char *gssmsg;
			globus_gss_assist_display_status_str( &gssmsg,
					NULL,
					major_status,
					minor_status,
					0);

			errMsg = (std::string)gssmsg;
			free(gssmsg);
			return false;
		}

		context = AcceptGSIAuthentication(sa->sck, ctx);
		if( context == GSS_C_NO_CONTEXT)
		{
			gss_release_cred(&minor_status, &(ctx.credential));
			SocketServer::KillAgent(sa);
			sa = NULL;
			return false;
		}
		else
		{
			sa -> gss_context = context;
			sa -> credential = ctx.credential;
			sa -> _certificate_subject = ctx.certificate_subject;
			int prevTimeOut = sa->m_send_timeout;
			sa -> SetSndTimeout(m_auth_timeout);
			bool res = sa -> Send(1);
			sa -> SetSndTimeout(prevTimeOut);//take back previous timeout;
			return res;
		}
	}
	return sa != NULL;
}

} // namespace socket_pp
} // namespace tls
} // namespace wmsutils
} // namespace glite


