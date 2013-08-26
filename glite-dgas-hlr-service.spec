Summary: The module containing the server backend for te Distributed Grid Accounting System
Name: glite-dgas-hlr-service
Version: 4.0.0
Release: b16.centos6
License: Apache Software License
Group: System Environment/Libraries
Packager: ETICS
BuildArch: x86_64
BuildRoot: %(mktemp -ud %{_tmppath}/%{name}-%{version}-%{release}-XXXXXX)
AutoReqProv: yes
Source: glite-dgas-hlr-service-4.0.0-b16.tar.gz

%description
emi.dgas.hlr-service

%prep
 

%setup -c

%install
 
 rm -rf %{buildroot}
 mkdir -p %{buildroot}
 cp -rpf * %{buildroot}

%clean
 

%pre
export DGAS_LOCATION=${DGAS_LOCATION:-/usr}



if [ -f /etc/dgas/dgas_hlr.conf ]; then



if [ -f $DGAS_LOCATION/sbin/dgas-hlrd ]; then

	if $DGAS_LOCATION/sbin/dgas-hlrd status >/dev/null; then

		echo "dgas-hlrd running"

		$DGAS_LOCATION/sbin/dgas-hlrd stop

	else echo "dgas-hlrd not running"

	fi

else

	if [ -f $DGAS_LOCATION/sbin/glite-dgas-hlrd ]; then

        	if $DGAS_LOCATION/sbin/glite-dgas-hlrd status >/dev/null; then

                	echo "glite-dgas-hlrd running"

                	$DGAS_LOCATION/sbin/glite-dgas-hlrd stop

        	else echo "glite-dgas-hlrd not running"

        	fi

	fi

fi



fi




%post
ldconfig

export DGAS_LOCATION=${DGAS_LOCATION:-/usr}



if [ -h /etc/rc.d/init.d/dgas-hlrd ] ; then

	rm -f /etc/rc.d/init.d/dgas-hlrd

fi 

if [ -h /etc/rc.d/init.d/glite-dgas-hlrd ] ; then

	rm -f /etc/rc.d/init.d/glite-dgas-hlrd

fi 

ln -s $DGAS_LOCATION/sbin/dgas-hlrd /etc/rc.d/init.d/dgas-hlrd

chkconfig --add dgas-hlrd



if [ -f /opt/glite/etc/dgas_hlr.conf ] ; then

	if [ ! -f /etc/dgas/dgas_hlr.conf ] ; then

		mv /opt/glite/etc/dgas_hlr.conf /etc/dgas/

	fi 

fi



%preun
export DGAS_LOCATION=${DGAS_LOCATION:-/usr}



if [ -f /etc/dgas/dgas_hlr.conf ]; then



if [ -f $DGAS_LOCATION/sbin/dgas-hlrd ]; then

	if $DGAS_LOCATION/sbin/dgas-hlrd status >/dev/null; then

		echo "dgas-hlrd running"

		$DGAS_LOCATION/sbin/dgas-hlrd stop

	else echo "dgas-hlrd not running"

	fi

else

	echo "$DGAS_LOCATION/sbin/dgas-hlrd: file not found"

fi



fi




%files
%defattr(-,root,root)
%dir /etc/dgas/
/etc/dgas/dgas_hlr.conf.template
/etc/dgas/glite-dgas-sumrecords2goc-SumCPU.sql
/etc/dgas/dgas_hlr_tmp.sql
/etc/dgas/dgas_sumrecords2goc.conf.template
/etc/dgas/dgas_hlr.sql
/usr/sbin/dgas-hlr-translatedb
/usr/sbin/dgas-sumrecords2goc.pl
/usr/sbin/dgas-sumrecords2gocSSMAggregate.pl
/usr/sbin/dgas-hlr-delresource
/usr/sbin/dgas-hlr-addadmin
/usr/sbin/dgas-hlr-bdiiresimport.pl
/usr/sbin/dgas-hlr-deladmin
/usr/sbin/dgas-hlr-checkqueue
/usr/sbin/dgas-hlr-archive
/usr/sbin/dgas-hlr-listener-nogsi
/usr/sbin/dgas-hlr-queryresource
/usr/sbin/dgas-hlr-qmgr
/usr/sbin/dgas-hlr-listener
/usr/sbin/dgas-hlr-sqlexec
/usr/sbin/dgas-hlr-populateJobTransSummary
/usr/sbin/dgas-hlr-had.pl
/usr/sbin/dgas-hlr-addresource
/usr/sbin/dgas-hlr-dbcreate
/usr/sbin/dgas-hlr-queryadmin
/usr/sbin/dgas-hlrd
/usr/lib64/libglite_dgas_atmResBankClient2.so.0
/usr/lib64/libdgasAtmResourceEngine.so.0.0.0
/usr/lib64/libglite_dgas_hlrQTransaction.so.0
/usr/lib64/libglite_dgas_hlrQTransaction.so.0.0.0
/usr/lib64/libdgasAtmResourceEngine.a
/usr/lib64/libdgasAtmResourceEngine2.so.0.0.0
/usr/lib64/libglite_dgas_hlr.a
/usr/lib64/libdgasAtmResourceEngine.so.0
/usr/lib64/libglite_dgas_atmResBankClient2.so.0.0.0
/usr/lib64/libglite_dgas_dbhelper.so
/usr/lib64/libglite_dgas_hlrGenericQuery.so.0.0.0
/usr/lib64/libglite_dgas_hlr.so
/usr/lib64/libdgasAtmResourceEngine2.a
/usr/lib64/libglite_dgas_dbhelper.a
/usr/lib64/libglite_dgas_atmResBankClient2.so
/usr/lib64/libdgasAtmResourceEngine.so
/usr/lib64/libglite_dgas_atmResBankClient.a
/usr/lib64/libglite_dgas_hlrTransLog.a
/usr/lib64/libglite_dgas_serviceCommonUtils.so
/usr/lib64/libglite_dgas_hlrGenericQuery.a
/usr/lib64/libdgasEngineCmnUtl.so.0.0.0
/usr/lib64/libglite_dgas_hlrTransLog.so
/usr/lib64/libglite_dgas_hlrGenericQuery.so
/usr/lib64/libdgasEngineCmnUtl.so.0
/usr/lib64/libglite_dgas_serviceCommonUtils.so.0
/usr/lib64/libdgasEngineCmnUtl.so
/usr/lib64/libglite_dgas_hlr.so.0.0.0
/usr/lib64/libglite_dgas_hlr.so.0
/usr/lib64/libglite_dgas_serviceCommonUtils.a
/usr/lib64/libglite_dgas_dbhelper.so.0.0.0
/usr/lib64/libglite_dgas_atmResBankClient.so
/usr/lib64/libglite_dgas_serviceCommonUtils.so.0.0.0
/usr/lib64/libglite_dgas_atmResBankClient2.a
/usr/lib64/libglite_dgas_hlrQTransaction.so
/usr/lib64/libglite_dgas_hlrGenericQuery.so.0
/usr/lib64/libglite_dgas_dbhelper.so.0
/usr/lib64/libdgasAtmResourceEngine2.so.0
/usr/lib64/libglite_dgas_atmResBankClient.so.0.0.0
/usr/lib64/libglite_dgas_hlrTransLog.so.0.0.0
/usr/lib64/libdgasAtmResourceEngine2.so
/usr/lib64/libglite_dgas_hlrTransLog.so.0
/usr/lib64/libdgasEngineCmnUtl.a
/usr/lib64/libglite_dgas_hlrQTransaction.a
/usr/lib64/libglite_dgas_atmResBankClient.so.0
%dir /usr/libexec/
/usr/libexec/dgas-hlr-checkHostCertProxy.sh
/usr/libexec/dgas-hlr-urforward
%dir /usr/libexec/dgastests/
/usr/libexec/dgastests/full.test
/usr/libexec/dgastests/level1.test
/usr/libexec/dgastests/storage.test
%dir /usr/libexec/dgastests/tests/
/usr/libexec/dgastests/tests/dgas_sensors_test.conf.lsf.sed
/usr/libexec/dgastests/tests/waitForJob.sh
/usr/libexec/dgastests/tests/glite-info-dynamic-ce.ldif.4482
/usr/libexec/dgastests/tests/generateJobId.sh
/usr/libexec/dgastests/tests/UR_LRMS_lsf.records
/usr/libexec/dgastests/tests/localJobmap.sed
/usr/libexec/dgastests/tests/wrongcputjob.sh
/usr/libexec/dgastests/tests/dgas_sensors_test_poolPatterns.conf
/usr/libexec/dgastests/tests/test.pbs.UR.SQL
/usr/libexec/dgastests/tests/dgas_sensors_test.conf.template
/usr/libexec/dgastests/tests/testMinus_o.sh
/usr/libexec/dgastests/tests/gridJobFQAN.sed
/usr/libexec/dgastests/tests/gridJobFQAN.lsf.sed
/usr/libexec/dgastests/tests/hlrDBExists.sh
/usr/libexec/dgastests/tests/testJobMap.conf
/usr/libexec/dgastests/tests/gridJobGlideIn.sed
/usr/libexec/dgastests/tests/grid-jobmap.template
/usr/libexec/dgastests/tests/multipleRecords.sh
/usr/libexec/dgastests/tests/delResourceAuth.sh
/usr/libexec/dgastests/tests/mysqldStatus.sh
/usr/libexec/dgastests/tests/missingStartQtimeJob.sh
/usr/libexec/dgastests/tests/hlrServerdIsAlive.sh
/usr/libexec/dgastests/tests/hlrQmgrIsAlive.sh
/usr/libexec/dgastests/tests/grid-jobmap.template.lsf
/usr/libexec/dgastests/tests/checkTable.sh
/usr/libexec/dgastests/tests/static-test.ldif
/usr/libexec/dgastests/tests/dgas_sensors_test.conf
/usr/libexec/dgastests/tests/negativeWCTJob.sh
/usr/libexec/dgastests/tests/hlr_tmpDBExists.sh
/usr/libexec/dgastests/tests/glideInTest.sh
/usr/libexec/dgastests/tests/testResubmission.sh
/usr/libexec/dgastests/tests/hlrHADIsAlive.sh
/usr/libexec/dgastests/tests/mysqldPing.sh
/usr/libexec/dgastests/tests/putURindgasURBox.SQL.sh
/usr/libexec/dgastests/tests/benchmarksTest.sh
/usr/libexec/dgastests/tests/gridJobpool.sed
/usr/libexec/dgastests/tests/putURindgasURBox.sh
/usr/libexec/dgastests/tests/multiplePing.sh
/usr/libexec/dgastests/tests/cleanResubJob.sh
/usr/libexec/dgastests/tests/poolAccountPattern.conf.template
/usr/libexec/dgastests/tests/voproblemjob.sh
/usr/libexec/dgastests/tests/putLSFURindgasURBox.sh
/usr/libexec/dgastests/tests/addResourceAuth.sh
/usr/libexec/dgastests/tests/UR_LRMS_pbs.records
/usr/libexec/dgastests/tests/atmClient.sh
/usr/libexec/dgastests/tests/testMinus_a.sh
/usr/libexec/dgastests/tests/dgas_sensors_test.conf.lsf.ldif.sed
/usr/libexec/dgastests/tests/test.pbs.UR
/usr/libexec/dgastests/tests/test.lsf.UR
/usr/libexec/dgastests/tests/voOriginTest.sh
/usr/libexec/dgastests/tests/createLSFRecords.sh
/usr/libexec/dgastests/tests/cleanUpJob.sh
/usr/libexec/dgastests/level2.test
/usr/libexec/dgastests/joinCover.pl
/usr/libexec/dgastests/sensors_full.test
/usr/libexec/dgastests/test.conf
/usr/libexec/dgastests/glite-dgas-testrun.pl
/usr/libexec/dgas-ml-client
%dir /usr/libexec/dgasmonitor/
/usr/libexec/dgasmonitor/glite-dgas-monitor-run.sh
%dir /usr/libexec/dgasmonitor/plugins/
/usr/libexec/dgasmonitor/plugins/ping_4
/usr/libexec/dgasmonitor/plugins/ping_2
/usr/libexec/dgasmonitor/plugins/ping_3

%changelog
 
