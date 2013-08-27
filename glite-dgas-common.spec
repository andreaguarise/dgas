Summary: Common set of utility libraries needed by all DGAS modules
Name: glite-dgas-common
Version: %{versionstring}
Release: centos6
License: Apache Software License
Group: System Environment/Libraries
Packager: ETICS
BuildArch: x86_64
BuildRoot: %(mktemp -ud %{_tmppath}/%{name}-%{version}-%{release}-XXXXXX)
AutoReqProv: yes
Source: %{sourcefile}

%define _unpackaged_files_terminate_build 0

%description
Common set of utility libraries needed by all DGAS modules

%prep
 

%setup -c

%install
 
 rm -rf %{buildroot}
 mkdir -p %{buildroot}
 cp -rpf * %{buildroot}

%clean
 

%files
%defattr(-,root,root)
%dir /usr/include/glite/
%dir /usr/include/glite/dgas/
%dir /usr/include/glite/dgas/tls/
%dir /usr/include/glite/dgas/tls/socket++/
/usr/include/glite/dgas/tls/socket++/errors.h
/usr/include/glite/dgas/tls/socket++/tokens.h
/usr/lib64/libglite_dgas_tls_gsisocket_pp.a
/usr/lib64/libglite_dgas_lock.so.0
/usr/lib64/libglite_dgas_localSecurity.so.0
/usr/lib64/libglite_dgas_config.so.0
/usr/lib64/libglite_dgas_config.so.0.0.0
/usr/lib64/libglite_dgas_config.so
/usr/lib64/libglite_dgas_xmlutil.so
/usr/lib64/libglite_dgas_tls_gsisocket_pp.so
/usr/lib64/libglite_dgas_log.so.0
/usr/lib64/libglite_dgas_lock.a
/usr/lib64/libglite_dgas_log.a
/usr/lib64/libglite_dgas_localSecurity.a
/usr/lib64/libglite_dgas_localSecurity.so
/usr/lib64/libglite_dgas_log.so
/usr/lib64/libglite_dgas_localSecurity.so.0.0.0
/usr/lib64/libglite_dgas_log.so.0.0.0
/usr/lib64/libglite_dgas_tls_gsisocket_pp.so.0
/usr/lib64/libglite_dgas_lock.so.0.0.0
/usr/lib64/libglite_dgas_config.a
/usr/lib64/libglite_dgas_tls_socket_pp.so
/usr/lib64/libglite_dgas_tls_socket_pp.so.0.0.0
/usr/lib64/libglite_dgas_tls_socket_pp.a
/usr/lib64/libglite_dgas_xmlutil.so.0
/usr/lib64/libglite_dgas_lock.so
/usr/lib64/libglite_dgas_xmlutil.a
/usr/lib64/libglite_dgas_xmlutil.so.0.0.0
/usr/lib64/libglite_dgas_tls_gsisocket_pp.so.0.0.0
/usr/lib64/libglite_dgas_tls_socket_pp.so.0

%changelog
 
