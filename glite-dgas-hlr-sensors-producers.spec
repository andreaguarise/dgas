Summary: Emi.dgas.hlr-sensors-producers
Name: glite-dgas-hlr-sensors-producers
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
emi.dgas.hlr-sensors-producers

%prep
 

%setup -c

%install
 
 rm -rf %{buildroot}
 mkdir -p %{buildroot}
 cp -rpf * %{buildroot}

%clean
 

%files
%defattr(-,root,root)
%dir /etc/dgas/
/etc/dgas/dgasAmqProducer.conf.template
/usr/lib64/libdgas_legacyCpuComposer.so.0
/usr/lib64/libdgas_legacyProducer.so
/usr/lib64/libdgas_legacyProducer.a
/usr/lib64/libdgas_legacyProducer.so.0
/usr/lib64/libdgas_legacyCpuComposer.a
/usr/lib64/libdgas_legacyCpuComposer.so.0.0.0
/usr/lib64/libdgas_legacyCpuComposer.so
/usr/lib64/libdgas_amqProducer.so.0.0.0
/usr/lib64/libdgas_legacyProducer.so.0.0.0
/usr/lib64/libdgas_amqProducer.so.0
/usr/lib64/libdgas_amqProducer.a
/usr/lib64/libdgas_amqProducer.so
%dir /usr/libexec/
/usr/libexec/dgas-record-filter.pl
/usr/libexec/dgas-ogfurComposer
/usr/libexec/dgas-amqProducer
/usr/libexec/dgas-sqlInsertComposer.pl
/usr/libexec/dgas-emicarComposer
/usr/libexec/dgas-legacyCpuComposer
/usr/libexec/dgas-legacyStorageComposer
/usr/libexec/dgas-legacyProducer
/usr/libexec/dgas-stompProducer.pl

%changelog
 
