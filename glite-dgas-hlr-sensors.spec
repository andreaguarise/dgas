Summary: Emi.dgas.hlr-sensors
Name: glite-dgas-hlr-sensors
Version: %{versionstring}
Release: %{diststring}
License: Apache Software License
Group: System Environment/Libraries
Packager: ETICS
BuildArch: %{platformstring}
Requires: perl-DBD-SQLite
BuildRoot: %(mktemp -ud %{_tmppath}/%{name}-%{version}-%{release}-XXXXXX)
AutoReqProv: yes
Source: %{sourcefile}

%define _unpackaged_files_terminate_build 0

%description
emi.dgas.hlr-sensors

%prep
 

%setup -c

%install
 
 rm -rf %{buildroot}
 mkdir -p %{buildroot}
 cp -rpf * %{buildroot}

%clean
 

%pre
export DGAS_LOCATION=${DGAS_LOCATION:-/usr}



if [ -f $DGAS_LOCATION/sbin/glite-dgas-pushd ]; then

	if $DGAS_LOCATION/sbin/glite-dgas-pushd status >/dev/null; then

		echo "glite-dgas-pushd running"

		$DGAS_LOCATION/sbin/glite-dgas-pushd stop

	else

		echo "glite-dgas-pushd not running"

	fi

	rm -f /etc/rc.d/init.d/glite-dgas-pushd

else

	 echo "$DGAS_LOCATION/sbin/glite-dgas-pushd: file not found"

fi





if [ -f $DGAS_LOCATION/sbin/glite-dgas-urcollector ]; then

	if $DGAS_LOCATION/sbin/glite-dgas-urcollector status >/dev/null; then

		echo "glite-dgas-urcollector running"

		$DGAS_LOCATION/sbin/glite-dgas-urcollector stop

	else

		echo "glite-dgas-urcollector not running"

	fi

	rm -f /etc/rc.d/init.d/glite-dgas-urcollector

else

	 echo "$DGAS_LOCATION/sbin/glite-dgas-urcollector: file not found"

fi





if [ -f /etc/dgas/dgas_sensors.conf ]; then



if [ -f $DGAS_LOCATION/sbin/dgas-pushd ]; then

	if $DGAS_LOCATION/sbin/dgas-pushd status >/dev/null; then

		echo "dgas-pushd running"

		$DGAS_LOCATION/sbin/dgas-pushd stop

	else

		echo "dgas-pushd not running"

	fi

fi





if [ -f $DGAS_LOCATION/sbin/dgas-urcollector ]; then

	if $DGAS_LOCATION/sbin/dgas-urcollector status >/dev/null; then

		echo "dgas-urcollector running"

		$DGAS_LOCATION/sbin/dgas-urcollector stop

	else

		echo "dgas-urcollector not running"

	fi

fi



fi

exit 0




%post
ldconfig

export DGAS_LOCATION=${DGAS_LOCATION:-/usr}



if [ -f $DGAS_LOCATION/sbin/dgas-pushd ]; then

	if [ -h /etc/rc.d/init.d/dgas-pushd ] ; then

		rm -f /etc/rc.d/init.d/dgas-pushd

	fi 

	ln -s $DGAS_LOCATION/sbin/dgas-pushd /etc/rc.d/init.d/dgas-pushd

	chkconfig --add dgas-pushd

fi





if [ -f $DGAS_LOCATION/sbin/dgas-urcollector ]; then

	if [ -h /etc/rc.d/init.d/dgas-urcollector ] ; then

		rm -f /etc/rc.d/init.d/dgas-urcollector

	fi 

	ln -s $DGAS_LOCATION/sbin/dgas-urcollector /etc/rc.d/init.d/dgas-urcollector

	chkconfig --add dgas-urcollector

fi





[ -d /var/spool/dgas ] || mkdir /var/spool/dgas



if [ -d /opt/glite/var ]; then

	mv /opt/glite/var/dgasCollectorBuffer.* /var/spool/dgas/ &> /dev/null

fi

exit 0




%preun
export DGAS_LOCATION=${DGAS_LOCATION:-/usr}



if [ -f /etc/dgas/dgas_sensors.conf ]; then



if [ -f $DGAS_LOCATION/sbin/dgas-pushd ]; then

	if $DGAS_LOCATION/sbin/dgas-pushd status >/dev/null; then

		echo "dgas-pushd running"

		$DGAS_LOCATION/sbin/dgas-pushd stop

	else

		echo "dgas-pushd not running"

	fi

else

	 echo "$DGAS_LOCATION/sbin/dgas-pushd: file not found"

fi





if [ -f $DGAS_LOCATION/sbin/dgas-urcollector ]; then

	if $DGAS_LOCATION/sbin/dgas-urcollector status >/dev/null; then

		echo "dgas-urcollector running"

		$DGAS_LOCATION/sbin/dgas-urcollector stop

	else

		echo "dgas-urcollector not running"

	fi

else

	 echo "$DGAS_LOCATION/sbin/dgas-urcollector: file not found"

fi



fi

exit 0




%files
%defattr(-,root,root)
%dir /etc/dgas/
/etc/dgas/glite-dgas-poolAccountPatterns.conf.template
/etc/dgas/dgas_sensors.conf.template
/usr/sbin/dgas-urcollector
/usr/sbin/dgas-pushd-SQLite.pl
/usr/sbin/dgas-urcollector-SQLite.pl
/usr/sbin/dgas-pushd
/usr/sbin/glite-dgas-ceServerd-had.pl
/usr/sbin/dgas-pushd-fileInput.pl
/usr/share/man/man5/dgas_sensors.conf.5.gz
/usr/lib64/libglite_dgas_clientBase.so.0
/usr/lib64/libglite_dgas_clientBase.a
/usr/lib64/libdgas_atmClient.so.0
/usr/lib64/libdgas_atmClient.so
/usr/lib64/libdgas_atmClient.a
/usr/lib64/libdgas_atmClient.so.0.0.0
/usr/lib64/libglite_dgas_clientBase.so
/usr/lib64/libglite_dgas_clientBase.so.0.0.0
%dir /usr/libexec/
/usr/libexec/dgas-atmClient
/usr/libexec/dgas-sendRecord
/usr/libexec/glite-dgas-voFromPoolAccountPatterns.pl

%changelog
 
