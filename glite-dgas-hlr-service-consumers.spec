Summary: Emi.dgas.hlr-service-consumers
Name: glite-dgas-hlr-service-consumers
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
emi.dgas.hlr-service-consumers

%prep
 

%setup -c

%install
 
 rm -rf %{buildroot}
 mkdir -p %{buildroot}
 cp -rpf * %{buildroot}

%clean
 

%post
ldconfig

export DGAS_LOCATION=${DGAS_LOCATION:-/usr}



if [ -f $DGAS_LOCATION/sbin/dgas-hlr-amq-consumer ]; then

	if [ -h /etc/rc.d/init.d/dgas-hlr-amq-consumer ] ; then

		rm -f /etc/rc.d/init.d/dgas-hlr-amq-consumer

	fi 

	ln -s $DGAS_LOCATION/sbin/dgas-hlr-amq-consumer /etc/rc.d/init.d/dgas-hlr-amq-consumer

	chkconfig --add dgas-hlr-amq-consumer

fi




%files
%defattr(-,root,root)
%dir /etc/dgas/
/etc/dgas/car_aggregated_v1.2.xsd
/etc/dgas/ogfur_v1.xsd.xml
/etc/dgas/car_v1.2.xsd
/etc/dgas/dgas_amq_consumer.conf.template
/usr/sbin/dgas-hlr-amq-consumer
/usr/lib64/libglite_dgas_legacyRecordManager.so.0.0.0
/usr/lib64/libglite_dgas_legacyRecordManager.so.0
/usr/lib64/libglite_dgas_AMQConsumer.so.0
/usr/lib64/libglite_dgas_AMQConsumer.so
/usr/lib64/libglite_dgas_legacyRecordManager.so
/usr/lib64/libglite_dgas_legacyRecordManager.a
/usr/lib64/libglite_dgas_AMQConsumer.so.0.0.0
/usr/lib64/libglite_dgas_AMQConsumer.a
%dir /usr/libexec/
/usr/libexec/dgas-AMQConsumer
/usr/libexec/glite-dgas-ogfurParser
/usr/libexec/dgas-legacyRecordManager

%changelog
 
