## *********************************************************************
## *
## * Copyright (c) 2002 CERN and INFN on behalf of the EU DataGrid.
## * For license conditions see LICENSE file or
## * http://www.edg.org/license.html
## *
## *********************************************************************

man1_MANS = dgas-hlr-query.man

if GSI
GSIFLAGS = -DGSI
GLOBUS_LIBS= $(GLOBUS_SSL_UTILS_LIBS) $(GLOBUS_GSS_THR_LIBS)
GLOBUS_CFLAGS = $(GLOBUS_THR_CFLAGS)
TLS_LIBS = $(GLITE_DGAS_COMMON_TLS_LIBS) -lglite_dgas_tls_gsisocket_pp
else
GSIFLAGS = -DNOGSI
TLS_LIBS = $(GLITE_DGAS_COMMON_TLS_LIBS)
endif

bin_PROGRAMS = \
	dgas-hlr-query

dgas_hlr_query_SOURCES = hlrAdvancedQueryClient.cpp

LDADD  = \
        $(GLOBUS_LIBS) \
        $(TLS_LIBS) \
        $(GLITE_DGAS_COMMON_CONFIG_LIBS) \
        $(GLITE_DGAS_COMMON_XMLUTIL_LIBS)

AM_CPPFLAGS = \
        -I$(top_srcdir)/interface-hlr-clients \
        -I$(top_srcdir)/interface-common \
        -I$(top_srcdir)/src-hlr-clients \
        $(GLITE_CFLAGS) \
        $(GLOBUS_CFLAGS) \
        $(GSIFLAGS)

MAINTAINERCLEANFILES = Makefile.in
