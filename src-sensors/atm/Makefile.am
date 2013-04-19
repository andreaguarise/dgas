## *********************************************************************
## *
## * Copyright (c) 2002 CERN and INFN on behalf of the EU DataGrid.
## * For license conditions see LICENSE file or
## * http://www.edg.org/license.html
## *
## *********************************************************************

lib_LTLIBRARIES  = libdgas_atmClient.la
libexec_PROGRAMS = dgas-atmClient dgas-sendRecord

if GSI
GSIFLAGS = -DGSI
GLOBUS_LIBS= $(GLOBUS_SSL_UTILS_LIBS) $(GLOBUS_GSS_THR_LIBS)
GLOBUS_CFLAGS = $(GLOBUS_THR_CFLAGS)
TLS_LIBS = $(GLITE_DGAS_COMMON_TLS_LIBS)
else
GSIFLAGS = -DNOGSI
TLS_LIBS = $(GLITE_DGAS_COMMON_TLS_LIBS)
endif

libdgas_atmClient_la_SOURCES = \
	atmClient.cpp 

dgas_atmClient_SOURCES = atmClientExe.cpp
dgas_atmClient_LDADD   = \
	./libdgas_atmClient.la \
	$(GLITE_DGAS_COMMON_CONFIG_LIBS) \
	$(GLITE_DGAS_COMMON_XMLUTIL_LIBS) \
	$(TLS_LIBS) \
	$(GLOBUS_LIBS)

dgas_sendRecord_SOURCES = sendRecordExe.cpp
dgas_sendRecord_LDADD   = \
	$(GLITE_DGAS_COMMON_CONFIG_LIBS) \
	$(GLITE_DGAS_COMMON_XMLUTIL_LIBS) \
	$(TLS_LIBS) \
	$(GLOBUS_LIBS)

AM_CPPFLAGS = \
	-I$(top_srcdir)/interface-sensors \
	-I$(top_srcdir)/src-sensors \
	-I$(top_srcdir)/interface-common \
	$(GLITE_CFLAGS) \
	$(GLOBUS_CFLAGS) \
	$(GSIFLAGS)
	-DVERSION=\"@VERSION@\"
MAINTAINERCLEANFILES = Makefile.in 
