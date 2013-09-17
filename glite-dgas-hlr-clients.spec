Summary: Emi.dgas.hlr-clients
Name: glite-dgas-hlr-clients
Version: %{versionstring}
Release: %{diststring}
License: Apache Software License
Group: System Environment/Libraries
Packager: ETICS
BuildArch: %{platformstring}
BuildRoot: %(mktemp -ud %{_tmppath}/%{name}-%{version}-%{release}-XXXXXX)
AutoReqProv: yes
Source: %{sourcefile}

%define _unpackaged_files_terminate_build 0

%description
emi.dgas.hlr-clients

%prep
 

%setup -c

%install
 
 rm -rf %{buildroot}
 mkdir -p %{buildroot}
 cp -rpf * %{buildroot}

%clean
 

%files
%defattr(-,root,root)
/usr/bin/dgas-ping
/usr/bin/dgas-hlr-query
/usr/share/man/man1/dgas-hlr-query.1.gz
/usr/share/man/man1/dgas-ping.1.gz
/usr/lib64/libglite_dgas_pingClient.a
/usr/lib64/libglite_dgas_pingClient.so.0
/usr/lib64/libglite_dgas_pingClient.so
/usr/lib64/libglite_dgas_pingClient.so.0.0.0

%changelog
 
