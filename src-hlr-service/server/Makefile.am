## *********************************************************************
## *
## * Copyright (c) 2002 CERN and INFN on behalf of the EU DataGrid.
## * For license conditions see LICENSE file or
## * http://www.edg.org/license.html
## *
## *********************************************************************

noinst_LTLIBRARIES = libdgasXmlHlrHelper.la

noinst_HEADERS = xmlHlrHelper.h

sbin_PROGRAMS = \
	dgas-hlr-listener \
	dgas-hlr-qmgr

libexec_PROGRAMS = \
	dgas-ml-client \
	dgas-hlr-urforward

sbin_SCRIPTS = \
	dgas-hlr-had.pl

if UNIXODBC
DBFLAGS = $(UNIXODBC_CFLAGS) -DWITH_UNIXODBC
DBLIBS = $(UNIXODBC_LIBS)
else
DBFLAGS = $(MYSQL_CFLAGS)
DBLIBS = $(MYSQL_LIBS)
endif

if VOMS
VOMSFLAGS = -DWITH_VOMS
VOMSLIBS = -lvomsapi
endif

if GSI
GSIFLAGS = -DGSI
GLOBUS_LIBS= $(GLOBUS_SSL_UTILS_LIBS) $(GLOBUS_GSS_THR_LIBS)
GLOBUS_CFLAGS = $(GLOBUS_THR_CFLAGS)
TLS_LIBS = $(GLITE_DGAS_COMMON_TLS_LIBS)
else
GSIFLAGS = -DNOGSI
TLS_LIBS = $(GLITE_DGAS_COMMON_TLS_LIBS) -lssl
endif



libdgasXmlHlrHelper_la_SOURCES = \
	xmlHlrHelper.h \
	xmlHlrHelper.cpp

dgas_hlr_urforward_SOURCES = urForwardFactory.cpp urForwardFactory.h \
	urForward.cpp 

dgas_hlr_urforward_LDADD = \
	$(GLOBUS_LIBS) \
	$(TLS_LIBS) \
	$(GLITE_DGAS_COMMON_XMLUTIL_LIBS) \
	$(GLITE_DGAS_COMMON_LOG_LIBS) \
	$(GLITE_DGAS_COMMON_CONFIG_LIBS) \
	$(GLITE_DGAS_COMMON_LOCK_LIBS) \
	$(top_builddir)/src-hlr-service/base/libglite_dgas_dbhelper.la \
	$(top_builddir)/src-hlr-service/base/libglite_dgas_hlrGenericQuery.la \
	$(top_builddir)/src-hlr-service/base/libglite_dgas_dbWaitress.la \
	$(top_builddir)/src-hlr-service/base/libdgas_serviceVersion.la \
	$(DBLIBS)
	
dgas_hlr_listener_SOURCES  = hlrServerd.cpp \
	daemonFactory.cpp \
	daemonFactory.h 

dgas_hlr_listener_LDADD =  \
	$(GLITE_DGAS_COMMON_XMLUTIL_LIBS) \
	$(GLITE_DGAS_COMMON_CONFIG_LIBS) \
	$(GLITE_DGAS_COMMON_LOCK_LIBS) \
	$(GLITE_DGAS_COMMON_LOG_LIBS) \
	$(TLS_LIBS) \
	$(GLOBUS_LIBS) \
	$(SERVICE_BASE_LIBS) \
	./libdgasXmlHlrHelper.la \
	$(top_builddir)/src-hlr-service/engines/libdgasAtmResourceEngine.la \
	$(top_builddir)/src-hlr-service/engines/libdgasAtmResourceEngine2.la \
	$(top_builddir)/src-hlr-service/engines/libdgasEngineCmnUtl.la \
	$(top_builddir)/src-hlr-service/engines/libdgasPingEngine.la \
	$(top_builddir)/src-hlr-service/engines/libdgasAdvancedQueryEngine2.la \
	$(top_builddir)/src-hlr-service/engines/libdgasUrConcentratorEngine.la \
	$(top_builddir)/src-hlr-service/engines/libdgasGetRecordEngine.la \
	$(top_builddir)/src-hlr-service/base/libglite_dgas_serviceCommonUtils.la \
	$(top_builddir)/src-hlr-service/base/libglite_dgas_dbhelper.la \
	$(top_builddir)/src-hlr-service/base/libglite_dgas_dbWaitress.la \
	$(top_builddir)/src-hlr-service/base/libdgas_serviceVersion.la \
	-lglite_dgas_localSecurity \
	-lglite_dgas_hlrGenericQuery \
	$(DBLIBS) $(VOMSLIBS) -lpthread

dgas_ml_client_SOURCES = mlClient.cpp
dgas_ml_client_LDADD   = \
	$(GLITE_DGAS_COMMON_LOCK_LIBS) \
	$(TLS_LIBS) \
	$(GLOBUS_LIBS)

dgas_hlr_qmgr_SOURCES = hlrQtransMgrd.cpp
dgas_hlr_qmgr_LDADD = \
	$(GLITE_DGAS_COMMON_XMLUTIL_LIBS) \
	$(GLITE_DGAS_COMMON_CONFIG_LIBS) \
	$(GLITE_DGAS_COMMON_LOCK_LIBS) \
	$(GLITE_DGAS_COMMON_LOG_LIBS) \
	$(TLS_LIBS) \
	$(GLOBUS_LIBS) \
	$(SERVICE_BASE_LIBS) \
	$(top_builddir)/src-hlr-service/engines/libglite_dgas_atmResBankClient.la \
	$(top_builddir)/src-hlr-service/engines/libglite_dgas_atmResBankClient2.la \
	$(top_builddir)/src-hlr-service/engines/libdgasEngineCmnUtl.la \
	$(top_builddir)/src-hlr-service/base/libglite_dgas_serviceCommonUtils.la \
	$(top_builddir)/src-hlr-service/base/libglite_dgas_dbhelper.la \
	$(top_builddir)/src-hlr-service/base/libdgas_serviceVersion.la \
	$(DBLIBS)

SERVICE_BASE_LIBS = \
	$(top_builddir)/src-hlr-service/base/libglite_dgas_hlr.la \
	$(top_builddir)/src-hlr-service/base/libglite_dgas_hlrQTransaction.la \
	$(top_builddir)/src-hlr-service/base/libglite_dgas_hlrTransLog.la 

AM_CPPFLAGS = \
	-I$(top_srcdir)/src-hlr-service/engines \
	-I$(top_srcdir)/src-hlr-service/base \
	-I$(top_srcdir)/interface-hlr-service \
	-I$(top_srcdir)/interface-pa-clients \
	-I$(top_srcdir)/interface-common \
	-I$(top_srcdir)/src-hlr-service \
	$(DBFLAGS) \
	$(GLITE_CFLAGS) \
	$(GLOBUS_CFLAGS) \
	$(VOMSFLAGS) \
	$(GSIFLAGS) \
	-DMERGE 

MAINTAINERCLEANFILES = Makefile.in
