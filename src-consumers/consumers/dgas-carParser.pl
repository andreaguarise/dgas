#!/usr/bin/perl -w

use strict;
use XML::LibXML;
use Term::ANSIColor qw(:constants);
use HTTP::Date qw(str2time);
use Date::Format;
use Getopt::Long;

sub iso8601Duration2Time {
	my $iso8601 = $_[0];
	my $s;
	my ( $Y, $M, $D, $H, $m, $S );
	if ( $iso8601 =~ /^P(.*)T(.*)$/ ) {
		my $datePart = $1;
		my $timePart = $2;
		if ( ( $Y, $M, $D ) = ( $datePart =~ /^(\d*Y)?(\d*M)?(\d*D)?$/ ) ) {
			if ($Y) { ($Y) = ( $Y =~ /(\d*)/ ) }
			if ($M) { ($M) = ( $M =~ /(\d*)/ ) }
			if ($D) { ($D) = ( $D =~ /(\d*)/ ) }
		}
		if ( ( $H, $m, $S ) = ( $timePart =~ /^(\d*H)?(\d*M)?(\d*S)?$/ ) ) {
			if ($H) { ($H) = ( $H =~ /(\d*)/ ) }
			if ($m) { ($m) = ( $m =~ /(\d*)/ ) }
			if ($S) { ($S) = ( $S =~ /(\d*)/ ) }
		}
	}
	if ($Y) { $s += $Y * 86400 * 365 }
	if ($M) { $s += $M * 86400 * 30 }
	if ($D) { $s += $D * 86400 }
	if ($H) { $s += $H * 3600 }
	if ($m) { $s += $m * 60 }
	if ($S) { $s += $S }
	return $s;
}

sub ogfurMem2KB {
	my $ogfurMem = $_[0];
	my $kb;
	if ( ( my $value, my $unit ) = ( $ogfurMem =~ /^(\d*)([A-Za-z]{1,2})$/ ) ) {
		if ( $unit eq "KB" ) { $kb = $value }
		if ( $unit eq "MB" ) { $kb = $value * 1024 }
		if ( $unit eq "GB" ) { $kb = $value * 1024 * 1024 }
		if ( $unit eq "TB" ) { $kb = $value * 1024 * 1024 * 1024 }
		if ( $unit eq "PB" ) { $kb = $value * 1024 * 1024 * 1024 * 1024 }
		if ( $unit eq "EB" ) { $kb = $value * 1024 * 1024 * 1024 * 1024 * 1024 }
	}
	return $kb;
}

my $file;
my $outputType = "list";
my $validate   = "no";

my $help;
my $embed;
my $schemaFile = '/etc/dgas/car_v1.0.xsd';

GetOptions(
	'file=s'       => \$file,
	'f=s'          => \$file,
	'schemaFile=s' => \$schemaFile,
	's=s'          => \$schemaFile,
	
	'outputType=s' => \$outputType,
	'o=s'          => \$outputType,
	'v=s'          => \$validate,
	'validate=s'   => \$validate,
	'e'            => \$embed,
	'embed'        => \$embed,
	'h' => \$help
);

if ( $help )
{
	my $helpMessage = "dgas-carParser.pl\n";
	$helpMessage .= "This is a record parser for EMI CAR v1.0 Usage Records.\n";
	$helpMessage .= "Usage:\n\n";
	$helpMessage .= "[ cat MESSAGEFILE | ] /usr/libexec/dgas-carParser.pl -f FILENAME -o OUTPUT_TYPE [ -s SCHEMAFILE -v ]\n";
	$helpMessage .= "\nCommand OPTIONS:\n\n";
	$helpMessage .= "FILENAME: The name of the file to parse, use 'stdin' to parse a string coming from the standard input.\n";
	$helpMessage .= "OUTPUT_TYPE: Can be one of list,CSV,dgasLegacy,commandLineArgs\n";
	$helpMessage .= "SCHEMAFILE: The (optional) filname of teh .xsd CAR record definition file\n";
	$helpMessage .= "-f --file    		 The file to parse, 'stdin for standard input'\n";
	$helpMessage .= "-s --schemaFile     Localtion of the EMICAR .xsd schema file (optional)\n";
	$helpMessage .= "-v --validate       If specified the incoming record will be validate against record SCHEMAFILE\n";
	$helpMessage .= "-o --outputType     Used to specify the desired output type for the parsed record.\n";
	$helpMessage .= "-e --embed			 Embed original UR in the legacy one. Just for dgasLegacy outputType.\n";
	$helpMessage .= "-h                  This help message.\n";
	$helpMessage .= "\n The command reads a CAR Usage Record from standard input or a given file and print the parsed information, in various formats, on the standard output.\n";
	print $helpMessage . "\n";
	
	exit 1;
}

my $parser = XML::LibXML->new();
my $doc;
my $docBuffer = "";
if ( $file ne "stdin" ) {
	$doc = $parser->parse_file($file) or die;
}
else {
	binmode(STDIN);
	open( STDIN, "< -" );
	while (<STDIN>) {

		#	s/http:\/\/www.w3.org\//file:\/\/\/etc\/dgas\/ogfurxmlcat\//g;
		#	s/http:\/\/schema.ogf.org\//file:\/\/\/etc\/dgas\/ogfurxmlcat\//g;
		$docBuffer .= $_;
	}
	if ( $docBuffer ne "" ) {
		$doc = $parser->parse_string($docBuffer);
	}
	else {
		exit(1);
	}
	close(STDIN);
}
if ( $validate eq "yes" ) {
	my $xmlschema = XML::LibXML::Schema->new( location => $schemaFile );
	eval { $xmlschema->validate($doc); };
	warn $@ if $@;
}
my $root = $doc->documentElement;

my %record = (
	RecordIdentity_recordId                       => "",
	RecordIdentity_createTime                     => "",
	JobIdentity_GlobalJobId                       => "",
	JobIdentity_LocalJobId                        => "",
	UserIdentity_GlobalUserName                   => "",
	UserIdentity_Group                            => "",
	UserIdentity_GroupAttribute_type__FQAN        => "",
	UserIdentity_GroupAttribute_type__group       => "",
	UserIdentity_GroupAttribute_type__role        => "",
	UserIdentity_GroupAttribute_type__ProjectName => "",
	UserIdentity_LocalUserId                      => "",
	UserIdentity_LocalGroup                       => "",
	JobName                                       => "",
	Charge                                        => "",
	Status                                        => "",
	ExitStatus                                    => "",
	Infrastructure                                => "",
	WallDuration                                  => "",
	CpuDuration_usageType__all                    => "", #i.e. __system + __user
	CpuDuration_usageType__user                   => "",
	CpuDuration_usageType__system                 => "",
	ServiceLevel_type__si2k                       => "",
	ServiceLevel_type__sf2k                       => "",
	ServiceLevel_type__HEPSPEC06                  => "",
	ServiceLevel_type__GlueCEInfoTotalCPUs        => "",
	Memory_metric__average_type__Shared           => "",
	Memory_metric__average_type__Physical         => "",
	Memory_metric__max_type__Shared               => "",
	Memory_metric__max_type__Physical             => "",
	TimeInstant_type__ctime                       => "",
	TimeInstant_type__qtime                       => "",
	TimeInstant_type__etime                       => "",
	NodeCount                                     => "",
	Processors                                    => "",
	EndTime                                       => "",
	StartTime                                     => "",
	MachineName                                   => "",
	SubmitHost_type__CE_ID                        => "", #type = "CE-ID"
	Queue                                         => "",
	Site_type__GOCDB                              => "",
	Site_type__X509DN                             => "",

#Host   is missing since multiple value will be treated in the code with a dedicatd array
	Resource_description__DgasAccountingProcedure => "",
	Resource_description__LrmsServer              => "",
	Resource_description__DgasVoOrigin            => "",
);

my @records;
if ( @records = $doc->getElementsByTagName('urf:UsageRecord') ) {

	#go on
}
else {
	@records = $doc->getElementsByTagName('UsageRecord')
	  or die("Can't Find UsageRecord");
}

foreach my $aRecord (@records) {

	# my $xc = XML::LibXML::XPathContext->new($aRecord);
	$record{RecordIdentity_recordId} =
	  $aRecord->findvalue('urf:RecordIdentity/@urf:recordId');
	$record{RecordIdentity_createTime} =
	  HTTP::Date::str2time(
		$aRecord->findvalue('urf:RecordIdentity/@urf:createTime') );
	$record{RecordIdentity_createTime} =
	  time2str( "%a %b %e %T %Y", $record{RecordIdentity_createTime} );
	$record{JobIdentity_GlobalJobId} =
	  $aRecord->findvalue('urf:JobIdentity/urf:GlobalJobId');
	$record{JobIdentity_LocalJobId} =
	  $aRecord->find('urf:JobIdentity/urf:LocalJobId');
	$record{UserIdentity_LocalUserId} =
	  $aRecord->find('urf:UserIdentity/urf:LocalUserId');
	$record{UserIdentity_LocalGroup} =
	  $aRecord->find('urf:UserIdentity/urf:LocalGroup');
	$record{UserIdentity_GlobalUserName} =
	  $aRecord->find('urf:UserIdentity/urf:GlobalUserName');
	$record{UserIdentity_Group} =
	  $aRecord->find('urf:UserIdentity/urf:Group');    #This is the VO!
	$record{UserIdentity_GroupAttribute_type__FQAN} =
	  $aRecord->findvalue(
		'urf:UserIdentity/urf:GroupAttribute[@urf:type="FQAN"]');
	$record{JobName}                    = $aRecord->find('urf:JobName');
	$record{WallDuration}               = $aRecord->find('urf:WallDuration');
	$record{CpuDuration_usageType__all} =
	  $aRecord->findvalue('urf:CpuDuration[@urf:usageType="all"]');
	$record{CpuDuration_usageType__system} =
	  $aRecord->findvalue('urf:CpuDuration[@urf:usageType="system"]');
	$record{CpuDuration_usageType__user} =
	  $aRecord->findvalue('urf:CpuDuration[@urf:usageType="user"]');

	$record{UserIdentity_GroupAttribute_type__ProjectName} =
	  $aRecord->findvalue(
		'urf:UserIdentity/urf:GroupAttribute[@urf:type="ProjectName"]')
	  ;
	$record{UserIdentity_GroupAttribute_type__role} =
	  $aRecord->findvalue(
		'urf:UserIdentity/urf:GroupAttribute[@urf:type="role"]');
	$record{UserIdentity_GroupAttribute_type__group} =
	  $aRecord->findvalue(
		'urf:UserIdentity/urf:GroupAttribute[@urf:type="group"]');
	$record{MachineName} = $aRecord->find('urf:MachineName');
	$record{Status}      = $aRecord->findvalue('urf:Status');

	$record{ExitStatus}        = $aRecord->find('urf:ExitStatus');
	$record{Charge}            = $aRecord->find('urf:Charge');
	$record{StartTime}         = $aRecord->find('urf:StartTime');
	$record{EndTime}           = $aRecord->find('urf:EndTime');
	$record{Queue}             = $aRecord->findvalue('urf:Queue');
	$record{Site_type__X509DN} =
	  $aRecord->findvalue('urf:Site[@urf:type="X509DN"]');
	$record{Site_type__GOCDB} =
	  $aRecord->findvalue('urf:Site[@urf:type="GOCDB"]');
	$record{TimeInstant_type__etime} =
	  $aRecord->findvalue('urf:TimeInstant[@urf:type="etime"]');
	$record{TimeInstant_type__ctime} =
	  $aRecord->findvalue('urf:TimeInstant[@urf:type="ctime"]');
	$record{TimeInstant_type__qtime} =
	  $aRecord->findvalue('urf:TimeInstant[@urf:type="qtime"]');
	$record{ServiceLevel_type__sf2k} =
	  $aRecord->findvalue('urf:ServiceLevel[@urf:type="sf2k"]');
	$record{ServiceLevel_type__si2k} =
	  $aRecord->findvalue('urf:ServiceLevel[@urf:type="si2k"]');
	$record{ServiceLevel_type__HEPSPEC06} =
	  $aRecord->findvalue('urf:ServiceLevel[@urf:type="HEPSPEC06"]');
	$record{ServiceLevel_type__GlueCEInfoTotalCPUs} =
	  $aRecord->findvalue('urf:ServiceLevel[@urf:type="GlueCEInfoTotalCPUs"]');
	$record{Resource_description__DgasAccountingProcedure} =
	  $aRecord->findvalue(
		'urf:Resource[@urf:description="DgasAccountingProcedure"]');
	$record{Resource_description__LrmsServer} =
	  $aRecord->findvalue('urf:Resource[@urf:description="LrmsServer"]');
	$record{SubmitHost_type__CE_ID} =
	  $aRecord->findvalue('urf:SubmitHost')
	  ;    #note that we use SubmitHost not checking for CE_ID
	$record{Resource_description__DgasVoOrigin} =
	  $aRecord->findvalue('urf:Resource[@urf:description="DgasVoOrigin"]');
	$record{Infrastructure} =
	  $aRecord->findvalue('urf:Infrastructure/@urf:type');
	$record{NodeCount}  = $aRecord->findvalue('urf:NodeCount');
	$record{Processors} = $aRecord->findvalue('urf:Processors');

	my @hosts = $aRecord->getElementsByTagName('urf:Host');
	my $count = 0;

	foreach my $host (@hosts) {
		my $isPrimary;
		if ( !( $host->getAttribute('urf:primary') ) ) {
			if ( $count > "0" ) { $record{Host} .= "+" }
			$record{Host} .= $host->string_value;
			if ( my $wn = ( $host->getAttribute('urf:description') ) ) {
				$wn =~ s/WorkerNodeProcessor//;
				$record{Host} .= "/" . $wn;
			}
			$count++;
		}
	}
	$record{Memory_metric__max_type__Physical} =
	  $aRecord->find('urf:Memory[@urf:type="Physical"]');
	$record{Memory_metric__max_type__Physical} .=
	  $aRecord->findvalue('urf:Memory[@urf:type="Physical"]/@urf:storageUnit');
	$record{Memory_metric__max_type__Physical} =
	  &ogfurMem2KB( $record{Memory_metric__max_type__Physical} ) . "kb";
	$record{Memory_metric__max_type__Shared} =
	  $aRecord->find('urf:Memory[@urf:type="Shared"]');
	$record{Memory_metric__max_type__Shared} .=
	  $aRecord->findvalue('urf:Memory[@urf:type="Shared"]/@urf:storageUnit');
	$record{Memory_metric__max_type__Shared} =
	  &ogfurMem2KB( $record{Memory_metric__max_type__Shared} ) . "kb";
}

if ( $outputType eq "list" ) {
	foreach my $item ( keys %record ) {
		if ( $record{$item} ) {
			print "$item=$record{$item}\n";
		}

		else {
			print BOLD, RED, "$item\n", RESET;
		}
	}
}

if ( $outputType eq "CSV" ) {
	$record{WallDuration} = &iso8601Duration2Time( $record{WallDuration} );
	$record{CpuDuration_usageType__all} =
	  &iso8601Duration2Time( $record{CpuDuration_usageType__all} );
	$record{StartTime} = HTTP::Date::str2time( $record{StartTime} );
	$record{EndTime}   = HTTP::Date::str2time( $record{EndTime} );
	$record{TimeInstant_type__etime} =
	  HTTP::Date::str2time( $record{TimeInstant_type__etime} );
	$record{TimeInstant_type__ctime} =
	  HTTP::Date::str2time( $record{TimeInstant_type__ctime} );
	$record{TimeInstant_type__qtime} =
	  HTTP::Date::str2time( $record{TimeInstant_type__qtime} );

	if ( $record{Status} eq "completed" ) {
		$record{Status} = 0;
	}
	else {
		$record{Status} = 1;
	}

	my @recordKeys = keys %record;
	foreach my $key (@recordKeys) {
		print "$key";
		if ( $key ne $recordKeys[-1] ) {
			print ";";
		}
	}
	print "\n";
	foreach my $key (@recordKeys) {
		print "$record{$key}" if exists $record{$key};
		if ( $key ne $recordKeys[-1] ) {
			print ";";
		}
	}
	print "\n";
}

if ( $outputType eq "commandLineArgs" ) {
	$record{WallDuration} = &iso8601Duration2Time( $record{WallDuration} );
	$record{CpuDuration_usageType__all} =
	  &iso8601Duration2Time( $record{CpuDuration_usageType__all} );
	$record{StartTime} = HTTP::Date::str2time( $record{StartTime} );
	$record{EndTime}   = HTTP::Date::str2time( $record{EndTime} );
	$record{TimeInstant_type__etime} =
	  HTTP::Date::str2time( $record{TimeInstant_type__etime} );
	$record{TimeInstant_type__ctime} =
	  HTTP::Date::str2time( $record{TimeInstant_type__ctime} );
	$record{TimeInstant_type__qtime} =
	  HTTP::Date::str2time( $record{TimeInstant_type__qtime} );
    my $SubmissionTimeBuff = HTTP::Date::str2time( $record{RecordIdentity_createTime} );
	if ( $record{Status} eq "completed" ) {
		$record{Status} = 0;
	}
	else {
		$record{Status} = 1;
	}

	my $output = "";
	$output .= "--jobid " . "\"$record{JobIdentity_GlobalJobId}\"";
	if ( $record{SubmitHost_type__CE_ID} ) {
		$output .= " --resgridid " . "\"$record{SubmitHost_type__CE_ID}\"";
	}
	else {
		$output .= " --resgridid " . "\"$record{MachineName}:$record{Queue}\"";
	}
	$output .= " --usrcert " . "\"$record{UserIdentity_GlobalUserName}\"";
	$output .= " -t " . "\"$SubmissionTimeBuff\"";
	$output .= " \"iBench=$record{ServiceLevel_type__si2k}\"";
	$output .= " \"iBenchType=si2k\"";
	$output .= " \"fBench=$record{ServiceLevel_type__sf2k}\"";
	$output .= " \"fBenchType=sf2k\"";
	$output .=
" \"accountingProcedure=$record{Resource_description__DgasAccountingProcedure}\"";
	$output .= " \"URCREATION=$record{RecordIdentity_createTime}\"";
	$output .= " \"CPU_TIME=$record{CpuDuration_usageType__all}\"";
	$output .= " \"WALL_TIME=$record{WallDuration}\"";
	$output .= " \"PMEM=$record{Memory_metric__max_type__Physical}\"";
	$output .= " \"VMEM=$record{Memory_metric__max_type__Shared}\"";
	$output .= " \"QUEUE=$record{Queue}\"";
	$output .= " \"USER=$record{UserIdentity_LocalUserId}\"";
	$output .= " \"LRMSID=$record{JobIdentity_LocalJobId}\"";
	$output .= " \"group=$record{UserIdentity_LocalGroup}\"";
	$output .= " \"jobName=$record{JobName}\"";
	$output .= " \"start=$record{StartTime}\"";
	$output .= " \"end=$record{EndTime}\"";
	$output .= " \"ctime=$record{TimeInstant_type__ctime}\"";
	$output .= " \"qtime=$record{TimeInstant_type__qtime}\"";
	$output .= " \"etime=$record{TimeInstant_type__etime}\"";
	$output .= " \"exitStatus=$record{Status}\"";
	$output .= " \"execHost=$record{Host}\"";
	$output .= " \"ceHostName=$record{MachineName}\"";
	$output .= " \"execCe=$record{SubmitHost_type__CE_ID}\"";
	$output .= " \"lrmsServer=$record{MachineName}\"";
	$output .= " \"fqan=$record{UserIdentity_GroupAttribute_type__FQAN}\"";
	$output .= " \"userVo=$record{UserIdentity_Group}\"";
	$output .= " \"voOrigin=$record{Resource_description__DgasVoOrigin}\"";
	$output .= " \"SiteName=$record{Site_type__GOCDB}\"";
	$output .= " \"ceCertificateSubject=$record{Site_type__X509DN}\"";
	$output .=
" \"GlueCEInfoTotalCPUs=$record{ServiceLevel_type__GlueCEInfoTotalCPUs}\"";
	$output .= " \"uniqueChecksum=$record{RecordIdentity_recordId}\"";

	print $output;
}

if ( $outputType eq "dgasLegacy" ) {

$record{WallDuration} = &iso8601Duration2Time( $record{WallDuration} );
	$record{CpuDuration_usageType__all} =
	  &iso8601Duration2Time( $record{CpuDuration_usageType__all} );
	$record{StartTime} = HTTP::Date::str2time( $record{StartTime} );
	$record{EndTime}   = HTTP::Date::str2time( $record{EndTime} );
	$record{TimeInstant_type__etime} =
	  HTTP::Date::str2time( $record{TimeInstant_type__etime} );
	$record{TimeInstant_type__ctime} =
	  HTTP::Date::str2time( $record{TimeInstant_type__ctime} );
	$record{TimeInstant_type__qtime} =
	  HTTP::Date::str2time( $record{TimeInstant_type__qtime} );
	my $SubmissionTimeBuff = HTTP::Date::str2time( $record{RecordIdentity_createTime} );
	my $res_grid_id;
	if ( $record{SubmitHost_type__CE_ID} ) {
		$res_grid_id = "$record{SubmitHost_type__CE_ID}";
	}
	else {
		$res_grid_id = "$record{MachineName}:$record{Queue}";
	}
	
	if ( $record{Status} eq "completed" )
	{
		$record{Status} = "0";
	}
	else
	{
		$record{Status} = "1";
	}
	my $embeddedUR = "";
	if ( $embed )
	{
		$embeddedUR = "<OriginalURType>EMICAR1.0</OriginalURType>\n";
		$embeddedUR .= "<OriginalUR>\n";
		$embeddedUR .= "$docBuffer\n";
		$embeddedUR .= "</OriginalUR>\n";
	}

	my $output = "<HLR type=\"ATM_request_toResource\">
<BODY>
<JOB_PAYMENT>
<EDG_ECONOMIC_ACCOUNTING>no</EDG_ECONOMIC_ACCOUNTING>
<DG_JOBID>$record{JobIdentity_GlobalJobId}</DG_JOBID>
<SUBMISSION_TIME>$SubmissionTimeBuff</SUBMISSION_TIME>
<RES_ACCT_PA_ID></RES_ACCT_PA_ID>
<RES_ACCT_BANK_ID></RES_ACCT_BANK_ID>
<USER_CERT_SUBJECT>$record{UserIdentity_GlobalUserName}</USER_CERT_SUBJECT>
<RES_GRID_ID>$res_grid_id</RES_GRID_ID>
<forceResourceHlrOnly>yes</forceResourceHlrOnly>
<JOB_INFO>
<CPU_TIME>$record{CpuDuration_usageType__all}</CPU_TIME>
<WALL_TIME>$record{WallDuration}</WALL_TIME>
<MEM>$record{Memory_metric__max_type__Physical}</MEM>
<VMEM>$record{Memory_metric__max_type__Shared}</VMEM>
</JOB_INFO>
</JOB_PAYMENT>
<AdditionalUR>
<dgas:item iBench=\"$record{ServiceLevel_type__si2k}\"\\>
<dgas:item iBenchType=\"si2k\"\\>
<dgas:item fBench=\"$record{ServiceLevel_type__sf2k}\"\\>
<dgas:item fBenchType=\"sf2k\"\\>
<dgas:item accountingProcedure=\"$record{Resource_description__DgasAccountingProcedure}\"\\>
<dgas:item URCREATION=\"$record{RecordIdentity_createTime}\"\\>
<dgas:item CPU_TIME=\"$record{CpuDuration_usageType__all}\"\\>
<dgas:item WALL_TIME=\"$record{WallDuration}\"\\>
<dgas:item PMEM=\"$record{Memory_metric__max_type__Physical}\"\\>
<dgas:item VMEM=\"$record{Memory_metric__max_type__Shared}\"\\>
<dgas:item QUEUE=\"$record{Queue}\"\\>
<dgas:item USER=\"$record{UserIdentity_LocalUserId}\"\\>
<dgas:item LRMSID=\"$record{JobIdentity_LocalJobId}\"\\>
<dgas:item group=\"$record{UserIdentity_LocalGroup}\"\\>
<dgas:item jobName=\"$record{JobName}\"\\>
<dgas:item start=\"$record{StartTime}\"\\>
<dgas:item end=\"$record{EndTime}\"\\>
<dgas:item ctime=\"$record{TimeInstant_type__ctime}\"\\>
<dgas:item qtime=\"$record{TimeInstant_type__qtime}\"\\>
<dgas:item etime=\"$record{TimeInstant_type__etime}\"\\>
<dgas:item exitStatus=\"$record{Status}\"\\>
<dgas:item execHost=\"$record{Host}\"\\>
<dgas:item ceHostName=\"$record{MachineName}\"\\>
<dgas:item execCe=\"$record{SubmitHost_type__CE_ID}\"\\>
<dgas:item lrmsServer=\"$record{MachineName}\"\\>
<dgas:item fqan=\"$record{UserIdentity_GroupAttribute_type__FQAN}\"\\>
<dgas:item userVo=\"$record{UserIdentity_Group}\"\\>
<dgas:item voOrigin=\"$record{Resource_description__DgasVoOrigin}\"\\>
<dgas:item SiteName=\"$record{Site_type__GOCDB}\"\\>
<dgas:item ceCertificateSubject=\"$record{Site_type__X509DN}\"\\>
<dgas:item uniqueChecksum=\"$record{RecordIdentity_recordId}\"\\>
<dgas:item atmClientVersion=\"emicar1.0_to_dgas4.1.0_v1\"\\>
</AdditionalUR>
$embeddedUR
</BODY>
</HLR>";

	print $output;

}
