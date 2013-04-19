//$Id: hlrTranslateDb.cpp,v 1.1.2.1.2.2 2012/02/09 13:16:42 aguarise Exp $
// -------------------------------------------------------------------------
// Copyright (c) 2001-2002, The DataGrid project, INFN, 
// All rights reserved. See LICENSE file for details.
// -------------------------------------------------------------------------
// Author: Andrea Guarise <andrea.guarise@to.infn.it>
/***************************************************************************
 * Code borrowed from:
 *  authors   : 
 *             
 *  
 ***************************************************************************/


#include <iostream>
#include <fstream>
#include <iomanip>
#include <getopt.h>
#include <unistd.h>
#include <csignal>
#include "glite/dgas/hlr-service/base/hlrGenericQuery.h"
#include "glite/dgas/common/base/dgas_config.h"
#include "glite/dgas/common/base/dgasVersion.h"
#include "glite/dgas/common/base/stringSplit.h"
#include "glite/dgas/common/base/int2string.h"
#include "glite/dgas/common/base/libdgas_log.h"
#include "serviceCommonUtl.h"
#include "dbWaitress.h"

#define OPTION_STRING "C:DmvrhcMT"
#define DGAS_DEF_CONF_FILE "/opt/glite/etc/dgas_hlr.conf"

using namespace std;


const char * hlr_sql_server;
const char * hlr_sql_user;
const char * hlr_sql_password;
const char * hlr_sql_dbname;
const char * hlr_tmp_sql_dbname;
ofstream logStream;
string confFileName = DGAS_DEF_CONF_FILE;
string hlr_logFileName;
string masterLock = "";
int system_log_level = 7;
int queryLenght =100;
bool debug = false;
bool reset = false;
bool mergeReset = false;
bool needs_help = false;
bool checkVo = false;
bool checkDuplicate = false;
bool is2ndLevelHlr = false;
bool translationRules =false;

//int I = 0;
//int J = 0;

int options ( int argc, char **argv )
{
	int option_char;
	int option_index = 0;
	static struct option long_options[] =
	{
		{"Conf",1,0,'C'},
		{"debug",0,0,'D'},
		{"TranslationRules",0,0,'T'},
		{"reset",0,0,'r'},
		{"mergeReset",0,0,'m'},
		{"checkvo",0,0,'v'},
		{"checkDuplicate",0,0,'c'},
		{"help",0,0,'h'},
		{0,0,0,0}
	};
	while (( option_char = getopt_long( argc, argv, OPTION_STRING,
					long_options, &option_index)) != EOF)
		switch (option_char)
		{
			case 'C': confFileName = optarg; break;
			case 'D': debug = true; break;
			case 'T': translationRules = true; break;
			case 'r': reset =true;; break;		  
			case 'm': mergeReset =true;; break;		  
			case 'v': checkVo =true;; break;
			case 'c': checkDuplicate =true;; break;
			case 'h': needs_help =true;; break;		  
			default : break;
		}
	return 0;
}

int help (const char *progname)
{
	cerr << "\n " << progname << endl;
        cerr << " Version: "<< VERSION << endl;
        cerr << " Author: Andrea Guarise " << endl;
	cerr << " Updates a reference table used in queries to the database." << endl;
        cerr << " usage: " << endl;
        cerr << " " << progname << " [OPTIONS] " << endl;
        cerr << setw(30) << left << "-D --debug"<<"Ask for debug output" << endl;
        cerr << setw(30) << left << "-T --TranslationRules"<<"Execute translation rules" << endl;
	cerr << setw(30) << left << "-C --Conf"<<"HLR configuration file, if not the default: " << DGAS_DEF_CONF_FILE << endl;
        cerr << setw(30) << left << "-r --reset"<<"Clean up the query tables and recreates them from raw database info." << endl;
	cerr << setw(30) << left << "-v --checkvo"<<"Try to fix most common source of problems determining user vo." << endl;
	cerr << setw(30) << left << "-c --checkDuplicate"<<"Search for duplicate entries and expunge the one with less information." << endl;
        cerr << setw(30) << left <<"-h --help"<<"This help" << endl;
        cerr << endl;
	return 0;	
}

int upgrade_R_3_4_0_23()
{
	int res = 0;
	bool update = false;
	string queryBuffer = "describe jobTransSummary uniqueChecksum";
	hlrGenericQuery describe(queryBuffer);
	res = describe.query();
	if ( res != 0 )
	{
		cerr << "Error in query:" << queryBuffer << endl;
	}
	else
	{
		if ( describe.errNo == 0 )
	        {
	                vector<resultRow>::const_iterator it = (describe.queryResult).begin();
	                while ( it != (describe.queryResult).end() )
	                {
				if ( (*it)[0] == "uniqueChecksum" )
				{
       		                 	if ( (*it)[3] != "PRI" ) 
					{
						update = true;
					}
				}
                	        it++;
                	}
		}
	}
	if ( update )
	{
		cout << "Table jobTransSummary needs an index update" << endl;
		cout << "Table jobTransSummary: dropping primary index..." << endl;
		string queryBuffer = "alter table jobTransSummary drop primary key";
		hlrGenericQuery dropQuery(queryBuffer);
		res = dropQuery.query();
		if ( res != 0 )
		{
			cerr << "Error in query:" << queryBuffer << endl;
			return res;
		}
		else
		{
			cout << "Table jobTransSummary: creating new primary index..." << endl;
	
			string queryBuffer2 = "alter table jobTransSummary ADD primary key (dgJobId,uniqueChecksum)";
			hlrGenericQuery addQuery(queryBuffer2);
			res = addQuery.query();
			if ( res != 0 )
			{
				cerr << "Error in query:" << queryBuffer2 << endl;
				return res;
			}
		}
		cout << "Table jobTransSummary: Index creation done." << endl;
	}
	return res;
}

int computeIterationsNo (int confIterations, int lastProcessedTid )
{
	string queryBuffer = "SELECT count(dgJobId) FROM trans_in WHERE tid > ";
	queryBuffer += int2string(lastProcessedTid);
	hlrGenericQuery iterQuery(queryBuffer);
	int res = iterQuery.query();
	if ( res != 0 )
	{
		cerr << "Error in query:" << queryBuffer << endl;
	}
	else
	{
			int records = atoi((((iterQuery.queryResult).front())[0]).c_str());
			if ( records < 5000 )
			{
				if ( debug ) cout << "Just one iteration is sufficient." << endl; 
				return 1;
			}
			else
			{
				int iBuff = records/80000;
				if ( debug )
				{
					cout << "From configuration: " << int2string(confIterations) << endl;
					cout << "From number of transactions: " << int2string(iBuff) << endl;
				}
				return ( iBuff >= confIterations ) ? iBuff : confIterations;
			}
	}
	return 0;
	
}

string composeQuery(int first, int last)
{
	string queryBuff = "SELECT * FROM ";
        queryBuff += "trans_in,transInLog,acctdesc";
        queryBuff +=" where trans_in.dgJobId=transInLog.dgJobId "; 
	queryBuff +=" AND trans_in.rid=acctdesc.id";
	queryBuff += " AND trans_in.tid >" +int2string(first);//do not use >=
						//since 'first' is already 
						//present in DB.
	queryBuff += " AND trans_in.tid <=" +int2string(last);
	queryBuff += " AND NOT trans_in.rid AND NOT trans_in.gid";
	if (debug)
	{
		cout << "From >" << first << "to <=" << last << endl;
		cerr << queryBuff << endl;
	}
	return queryBuff;
}

int populateJobTransSummaryTable (table& jobTransSummary, JTSManager& jtsManager, const hlrGenericQuery& q, int& I, int& J )
{
	int res = 0;
	if ((q.queryResult).size() == 0 ) res = 1;
	if ( res != 0 )
	{
		cout<< setw(19) << "writing records: "<< setw(40) << "########################################";
		return 1;
	}
	int i = 0;
	int j = 0;
	int counter = 0;
        int records = (q.queryResult).size();
	I += records;
        int step = records/40;
	string currentTid;
	string dgJobId;
	string thisGridId;
	string remoteGridId;
//	string remoteHlr;
	string acl;
	string amount;
	string date;//start date
	string endDate;
	string queryBuffer;
	string valuesBuffer;
	string uniqueS;
	string indicator = "#";
	cout << setw(19) << "writing records: ";
	vector<string> valuesV;
	vector<string> valuesTransInfoV;
	vector<string> valuesTransInV;
	int valuesCounter = 0;
	db hlrDb (hlr_sql_server,
			hlr_sql_user,
			hlr_sql_password,
			hlr_sql_dbname);
	vector<resultRow>::const_iterator it = (q.queryResult).begin();
	while ( it != (q.queryResult).end() )
	{
		currentTid = (*it)[0];
		dgJobId = (*it)[6];
		thisGridId = (*it)[14];
		remoteGridId = (*it)[3];
		acl = (*it)[15];
		string gid = (*it)[2];
		hlrLogRecords logBuff;
		jtsManager.parseTransLog((*it)[10], logBuff);
		if ( logBuff.start != "" && logBuff.start != "0" )
		{
			// there is a start value in the LRMS record
			// use it for 'date' field.
			date = "FROM_UNIXTIME("+ logBuff.start +")";
		}
		else
		{
			if ( logBuff.qtime != "" 
				&& logBuff.qtime != "0" 
				&& logBuff.qtime != "qtime" )
			{
				//there isn't a start value in the LRMS record
				//but there is a queue value. Use it as the
				//date field.
				date = "FROM_UNIXTIME("+ logBuff.qtime +")";
			}
			else
			{
				if ( logBuff.ctime != "" 
					&& logBuff.ctime != "0" 
					&& logBuff.ctime != "ctime" ) 
				{
					//ok, let's try with ctime (submit time for LSF)
					date = "FROM_UNIXTIME("+ logBuff.ctime +")";	
				}
				else
				{
					//There are not 'start', 'queue' and 'submit' values
					//in the LRMS record. Use the transaction
					//timestamp for the 'date' field
					date = "FROM_UNIXTIME("+ (*it)[5] +")";
				}
			}
		}
		if ( logBuff.end != "" && logBuff.end != "0" )
		{
			endDate = "FROM_UNIXTIME("+ logBuff.end +")";
		}
		else
		{
			if ( logBuff.start != "" && logBuff.start != "0" )
			{
				//assume job end == job start!
				endDate = "FROM_UNIXTIME("+ logBuff.start +")";
			}
			else
			{
				if ( logBuff.qtime != "" 
					&& logBuff.qtime != "0"  
					&& logBuff.qtime != "qtime" )
				{
					//assume job end == job queue time!
					endDate = "FROM_UNIXTIME("+ logBuff.qtime +")";
				}
				else
				{
					//assume job end = job transaction 
					//timestamp
					endDate = "FROM_UNIXTIME("+ (*it)[5] +")";
				}
			}
		}
		//Old record workaround (1).
		if ( (logBuff.cpuTime < 0) && (logBuff.start == "0") ) 
		{
			//this is a record from older version of LSF
			//sensors cancelled in the queue. It has
			//uncorrect values in the transaction log.
			logBuff.cpuTime = 0;
			logBuff.wallTime = 0;
			logBuff.mem ="0";
			logBuff.vMem ="0";
		}
		string cpuTime = int2string(logBuff.cpuTime);
		if ( logBuff.wallTime < 0 ) logBuff.wallTime = 0;//PBS Exit_status=-4 negative wall time workaround.
		string wallTime = int2string(logBuff.wallTime);
		amount = (*it)[4];
		if ( checkVo )
		{
			string buff;
			logBuff.userVo = checkUserVo(logBuff.userVo,
					logBuff.fqan,
					logBuff.localUserId,
					buff);
			if ( debug )
			{
				cout << "userVo:" << buff << endl;
			}
		}
		if ( logBuff.accountingProcedure == "" )
		{
			logBuff.accountingProcedure = "grid";
		}
		//compute unique Strings
		string valuesBuffer = "";
		vector<string> ceIdBuff;
		Split (':', thisGridId, &ceIdBuff);        
		if (ceIdBuff.size() > 0)
		{
			valuesBuffer  = ceIdBuff[0];//FIXME can't we finde something else?
		}
		valuesBuffer += logBuff.lrmsId;
		valuesBuffer += logBuff.start;
		valuesBuffer += wallTime;
		valuesBuffer += cpuTime;
		uniqueS = "";
		makeUniqueString (valuesBuffer,uniqueS);
		/*if ( debug )
		{
			cerr << uniqueS << "::" << valuesBuffer << endl;
		}*/
		queryBuffer = "";
		queryBuffer += "SET uniqueChecksum='";
		queryBuffer += uniqueS;
		queryBuffer += "',accountingProcedure='";
		queryBuffer += logBuff.accountingProcedure;
		queryBuffer += "' WHERE tid=" + currentTid;
		valuesTransInV.push_back(queryBuffer);
		// compute unique strings

		queryBuffer = "('";
		queryBuffer += dgJobId;
		queryBuffer += "',";
		queryBuffer += date;
		queryBuffer += ",'";
		queryBuffer += thisGridId;
		queryBuffer += "','";
		queryBuffer += remoteGridId;
		queryBuffer += "','";
		queryBuffer += logBuff.fqan;
		queryBuffer += "','";
		queryBuffer += logBuff.userVo;
		queryBuffer += "',";
		queryBuffer += cpuTime;
		queryBuffer += ",";
		queryBuffer += wallTime;
		queryBuffer += ",";
		queryBuffer += logBuff.mem;
		queryBuffer += ",";
		queryBuffer += logBuff.vMem;
		queryBuffer += ",";
		queryBuffer += amount;
		queryBuffer += ",";
		queryBuffer += logBuff.start;
		queryBuffer += ",";
		queryBuffer += logBuff.end;
		queryBuffer += ",'";
		queryBuffer += logBuff.iBench;
		queryBuffer += "','";
		queryBuffer += logBuff.iBenchType;
		queryBuffer += "','";
		queryBuffer += logBuff.fBench;
		queryBuffer += "','";
		queryBuffer += logBuff.fBenchType;
		queryBuffer += "','";
		queryBuffer += acl;
		queryBuffer += "',0,'";
		queryBuffer += logBuff.lrmsId;
		queryBuffer += "','";
		queryBuffer += logBuff.localUserId;
		queryBuffer += "','";
		queryBuffer += gid;
		queryBuffer += "','";
		queryBuffer += logBuff.localGroupId;
		queryBuffer += "',";
		queryBuffer += endDate;
		queryBuffer += ",'";
		queryBuffer += logBuff.siteName;
		queryBuffer += "','";
		queryBuffer += "";//sender should be here, but we don't have one
		queryBuffer += "',";
		queryBuffer += currentTid;//trans_in::tid
		queryBuffer += ",'";
		queryBuffer += logBuff.accountingProcedure;
		queryBuffer += "','";
		queryBuffer += logBuff.voOrigin;
		queryBuffer += "','";
		queryBuffer += logBuff.glueCEInfoTotalCPUs;
		queryBuffer += "','";
		queryBuffer += logBuff.executingNodes;
		queryBuffer += "','";
		queryBuffer += uniqueS;
		queryBuffer += "')";
		valuesV.push_back(queryBuffer);
		i++;
		counter++;
		valuesCounter++;
                if (counter >= step )
                {
                        cout << indicator << flush;
                        counter = 0;
			indicator = "#";
                }
		j++;
		if ( valuesCounter == queryLenght )
		{
			queryBuffer = "INSERT INTO jobTransSummary VALUES ";
			vector<string>::const_iterator valuesIt = valuesV.begin();
			queryBuffer += *valuesIt;
			valuesIt++;
			while ( valuesIt != valuesV.end() )
			{
				queryBuffer += ",";
				queryBuffer += *valuesIt;
				valuesIt++;
			}
			dbResult result = hlrDb.query(queryBuffer);
			if ( hlrDb.errNo != 0 )
			{
				if ( debug )
				{
					cerr << "ERROR: " <<queryBuffer << "; errNo:" << int2string(hlrDb.errNo) << endl;
				}
				
				indicator = "E";
				hlr_log(queryBuffer,&logStream,4);
			}
			valuesV.clear();
			vector<string>::iterator queryUTIIT = valuesTransInV.begin();
			while ( queryUTIIT != valuesTransInV.end() )
			{
				queryBuffer = "UPDATE trans_in " + *queryUTIIT;
				dbResult resultUTI = hlrDb.query(queryBuffer);
				if ( hlrDb.errNo != 0 )
                        	{
                                	if ( debug )
                                	{
                                       	 	cerr << "ERROR: " <<queryBuffer << "; errNo:" << int2string(hlrDb.errNo) << endl;
                                	}
					hlr_log(queryBuffer,&logStream,4);
					indicator = "E";
                        	}
				queryUTIIT++;
			}
			valuesTransInV.clear();
			valuesCounter=0;
		}
		it++;
	}
        //manage trailing elements in queryResult vector.	
	if ( valuesCounter != 0 )
	{
		if ( debug )
		{
			cerr << int2string(valuesCounter) << " trailing elements." << endl;
		}
		queryBuffer = "INSERT INTO jobTransSummary VALUES ";
		vector<string>::const_iterator valuesIt = valuesV.begin();
		queryBuffer += *valuesIt;
		valuesIt++;
		while ( valuesIt != valuesV.end() )
		{
			queryBuffer += ",";
			queryBuffer += *valuesIt;
			valuesIt++;
		}
		dbResult result = hlrDb.query(queryBuffer);
		if ( hlrDb.errNo != 0 )
		{
			if ( debug )
			{
				cerr << "ERROR: " <<queryBuffer <<"; errNo:"<< int2string(hlrDb.errNo) << endl;
			}
		}
			//trans_in
			vector<string>::iterator queryUTIIT = valuesTransInV.begin();
			while ( queryUTIIT != valuesTransInV.end() )
			{
				queryBuffer = "UPDATE trans_in " + *queryUTIIT;
				dbResult resultUTI = hlrDb.query(queryBuffer);
				if ( hlrDb.errNo == 0 )
                       	 	{
                               	 	/*if ( debug )
                               	 	{
                                        	cerr <<queryBuffer << endl;
                                	}*/
                        	}
                        	else
                        	{
                                	if ( debug )
                                	{
                                       	 	cerr << "ERROR:" <<queryBuffer << endl;
                                	}
                        	}
				queryUTIIT++;
			}

	}
	
	if ( debug )
	{
		cout << "Found " << i << " transactions, processed:" << j << endl;
	}
	J += j;
	return 0;
}

bool createJobTransSummaryTable(database& DB)
{
	table jobTransSummary(DB, "jobTransSummary");
	JTS jts(DB, "jobTransSummary", is2ndLevelHlr);
	jts.create();
	//if ( !is2ndLevelHlr) jobTransSummary.addIndex( "lrmsId");
	//if ( !is2ndLevelHlr) jobTransSummary.addIndex( "hlrTid");
	return jobTransSummary.exists();
}

int flushTables()
{
	string queryString = "FLUSH TABLES";
	hlrGenericQuery flush(queryString);
	int res = flush.query();
	if ( debug )
        {
                cerr << queryString << endl;
		cerr << "Exited with:" << res << endl;
        }
	return res;
}

bool createUrConcentratorTable(database& DB)
{
	table urConcentratorIndex(DB,"urConcentratorIndex");
	string queryString = "";
	queryString = "CREATE TABLE urConcentratorIndex";
	queryString += " (urSourceServer varchar(255), ";
	 queryString += "urSourceServerDN varchar(255), ";
	 queryString += "remoteRecordId varchar(31), ";
	 queryString += "recordDate datetime, ";
	 queryString += "recordInsertDate datetime, ";
	 queryString += "primary key (urSourceServer))";
	if ( debug )
	{
		cerr << queryString << endl;
	}
	hlrGenericQuery makeTable(queryString);
	makeTable.query();
	return urConcentratorIndex.exists();
}


bool createRolesTable(database & DB)
{
	table roles(DB,"roles");
	string queryString = "";
	queryString = "CREATE TABLE roles";
        queryString += " (id int(11), ";
        queryString += " dn varchar(255), ";
        queryString += " role varchar(160), ";
        queryString += " permission varchar(16), ";
        queryString += " queryType varchar(160), ";
        queryString += " queryAdd varchar(255), ";
        queryString += "primary key (dn,role)) DEFAULT CHARSET=latin1";
        if ( debug )
        {
                cerr << queryString << endl;
        }
        hlrGenericQuery makeTable(queryString);
        makeTable.query();
        return roles.exists();
}

bool createVomsAuthMapTable(database & DB)
{
	table vomsAuthMap(DB,"vomsAuthMap");
	string queryString = "";
        queryString = "CREATE TABLE vomsAuthMap";
        queryString += " (vo_id varchar(255), ";
        queryString += " voRole varchar(255), ";
        queryString += " hlrRole varchar(255), ";
        queryString += "primary key (vo_id,voRole)) DEFAULT CHARSET=latin1";
        if ( debug )
        {
                cerr << queryString << endl;
        }
        hlrGenericQuery makeTable(queryString);
        makeTable.query();
        return vomsAuthMap.exists();
}

int getMaxTid( int &maxTid )
{
	string queryString = "SELECT max(tid) FROM trans_in";
	if ( debug )
	{
		cerr << queryString << endl;
	}
	hlrGenericQuery indexQuery(queryString);
	indexQuery.query();
	if ( indexQuery.errNo == 0 )
	{
		vector<resultRow>::const_iterator it = (indexQuery.queryResult).begin();
		while ( it != (indexQuery.queryResult).end() )
		{
			maxTid = atoi(((*it)[0]).c_str());
			it++;
		}
		if ( debug )
		{
			cerr << "got tid: " << maxTid << endl;	
		}
		return 0;
	}
	return 1;
}

int getIndexFromTable( int &lastTid )
{
	string queryString = "SELECT hlrTid FROM jobTransSummary ORDER BY hlrTid DESC LIMIT 1";
	if ( debug )
	{
		cerr << queryString << endl;
	}
	hlrGenericQuery indexQuery(queryString);
	indexQuery.query();
	if ( indexQuery.errNo == 0 )
	{
		vector<resultRow>::const_iterator it = (indexQuery.queryResult).begin();
		while ( it != (indexQuery.queryResult).end() )
		{
			lastTid = atoi(((*it)[0]).c_str());
			it++;
		}
		if ( debug )
		{
			cerr << "got tid: " << lastTid << endl;
		}	
	}
	else
	{
		if ( debug )
		{
			cout << "Warning Error in the index query!, size:"<< (indexQuery.queryResult).size() << endl;
			cout << "It is absolutely normal if this is the firt time the command runs on an HLR!"<< endl;
		}
		return 1;
	}
	return 0;
}

int cleanUpOld(string startDate)
{
	string checkString = "SELECT UNIX_TIMESTAMP(\"" + startDate +"\")";
	if ( debug )
	{
		cerr << checkString << endl;
	}
	hlrGenericQuery check(checkString);
	check.query();
	if ( check.errNo != 0)
	{
		cerr << "Error in query checking for date validity." << endl;
		return 1;
	}
	if ( (check.queryResult).front()[0] == "0" )
	{
		cerr << "startDate is not correctly setup in the conf file. Bailing out from cleanup." << endl;
		return 1;
	}
	string queryString = "DELETE FROM jobTransSummary WHERE date < '";
	queryString += startDate + "'";
	if ( debug )	
	{
		cerr << queryString << endl;
	}	
	hlrGenericQuery q(queryString);
	q.query();
	if ( q.errNo != 0 )
	{
		cerr << "Error cleaning up jobTransSummary for jobs older than:" 			<< startDate << endl;
		return 1; 
	}
	return 0;
}

void doNothing ( int sig )
{
	cerr << "The command can't be killed, Please be patient..." << endl;
	signal (sig, doNothing) ;
}

int upgradeJTSSchema (table& jobTransSummary)
{
	int res = 0;
	jobTransSummary.disableKeys();
	jobTransSummary.dropIndex( "date" );
	jobTransSummary.dropIndex( "endDate" );
	jobTransSummary.dropIndex( "lrmsId" );
	jobTransSummary.dropIndex( "urSourceServer" );
	jobTransSummary.dropIndex( "hlrTid" );
	jobTransSummary.dropIndex( "uniqueChecksum" );
	string upgradeQuery = "ALTER TABLE jobTransSummary ";
	upgradeQuery += "CHANGE thisGridId gridResource varchar(160), ";
	upgradeQuery += "CHANGE remoteGridId gridUser varchar(160), ";
	upgradeQuery += "CHANGE cpuTime cpuTime int(10) unsigned, ";
	upgradeQuery += "CHANGE wallTime wallTime int(10) unsigned, ";
	upgradeQuery += "CHANGE pmem pmem int(10) unsigned, ";
	upgradeQuery += "CHANGE vmem vmem int(10) unsigned, ";
	upgradeQuery += "CHANGE amount amount smallint(5) unsigned, ";
	upgradeQuery += "CHANGE start start int(10) unsigned, ";
	upgradeQuery += "CHANGE end end int(10) unsigned, ";
	upgradeQuery += "CHANGE si2k iBench mediumint(8) unsigned, ";
	upgradeQuery += "CHANGE sf2k fBench mediumint(8) unsigned, ";
	upgradeQuery += "CHANGE id id bigint(20) unsigned auto_increment, ";
	upgradeQuery += "CHANGE localUserId localUserId varchar(32), ";
	upgradeQuery += "CHANGE hlrTid hlrTid bigint(20) unsigned, ";
	upgradeQuery += "CHANGE accountingProcedure accountingProcedure varchar(32), ";
	upgradeQuery += "CHANGE voOrigin voOrigin varchar(16), ";
	upgradeQuery += "CHANGE GlueCEInfoTotalCPUs GlueCEInfoTotalCPUs smallint(5) unsigned, ";
	upgradeQuery += "DROP transType, ";
	upgradeQuery += "DROP remoteHlr, ";
	upgradeQuery += "ADD executingNodes varchar(255) DEFAULT '' AFTER GlueCEInfoTotalCPUs, ";
	upgradeQuery += "ADD uniqueChecksum char(32), ";
	upgradeQuery += "ADD iBenchType varchar(16) DEFAULT 'si2k' AFTER iBench, ";
	upgradeQuery += "ADD fBenchType varchar(16) DEFAULT 'sf2k' AFTER fBench, ";
	upgradeQuery += "ADD INDEX date (date), ";
	upgradeQuery += "ADD INDEX endDate (endDate), ";
	upgradeQuery += "ADD INDEX lrmsId (lrmsId), ";
	upgradeQuery += "ADD INDEX urSourceServer (urSourceServer), ";
	upgradeQuery += "ADD INDEX hlrTid (hlrTid), ";
	upgradeQuery += "ADD INDEX uniqueChecksum (uniqueChecksum)";
	if ( debug )
	{
		cerr << upgradeQuery << endl;
	}
	hlrGenericQuery upgrade1(upgradeQuery);
	upgrade1.query();
	if ( upgrade1.errNo != 0)
	{
		cerr << "Error in query upgrading jobTransSummary (CHANGE,DROP,ADD step 1)." << endl;
		cerr << upgradeQuery << ":" << int2string(upgrade1.errNo) << endl;
		res = 1;
	}
	int percentage = 0;
	int oldPercentage = 0;
	time_t time0;
	time_t time1;
	time_t eta = 0;
	int I =0;
	int J = 0;
	int resLastTid = 0;
	int stepNumber = 0;
	int records = 0;
	string queryBuffer = "SELECT min(id),max(id) FROM jobTransSummary";
	hlrGenericQuery iterQuery(queryBuffer);
	res = iterQuery.query();
	if ( res != 0 )
	{
		cerr << "Error in query:" << queryBuffer << endl;
	}
	else
	{
			resLastTid = atoi((((iterQuery.queryResult).front())[0]).c_str());
			records = atoi((((iterQuery.queryResult).front())[1]).c_str()) - resLastTid;
			if ( records <= 80000 )
			{
				if ( debug ) cout << "Just one iteration is sufficient." << endl; 
				stepNumber = 1;
			}
			else
			{
				int iBuff = records/80000;
				if ( debug )
				{
					cout << "Number of records: " << int2string(records) << endl;
					cout << "First Id: " << int2string(resLastTid) << endl;
					cout << "From configuration: " << int2string(stepNumber) << endl;
					cout << "From number of transactions: " << int2string(iBuff) << endl;
				}
				stepNumber = ( iBuff >= stepNumber ) ? iBuff : stepNumber;
			}
	}
	percentage = 0;
	if ( debug )
	{
		cout << "Dividing operation in " << int2string(stepNumber) << " steps." << endl;
	}
	int step = (records/stepNumber) +1;
	// if (step <= 0 ) step = 1;
	for (int i=0; i<stepNumber; i++)
	{
		time0 = time(NULL);
		percentage = ((i+1)*100)/stepNumber;
		string queryString = "SELECT id,gridResource,lrmsId,start,wallTime,cpuTime from jobTransSummary WHERE id >=" + int2string(resLastTid) + " AND id <" + int2string(resLastTid+step);
		hlrGenericQuery stepQuery(queryString);
		if ( debug )
				cerr << queryString << endl; 
   		int res = stepQuery.query();
		if ( res != 0 )
		{
			cout << "Warning: problem in query.";
			if ( debug )
			{
				cout << queryString << endl; 
			}
		}
		int x = 0;
		int y = 0;
		int counter = 0;
	       	int records = (stepQuery.queryResult).size();
		I += records;
       		int barStep = records/40;
		vector<string> valuesJTSV;
		int valuesCounter = 0;
		string indicator = "#";
		db hlrDb (hlr_sql_server,
				hlr_sql_user,
				hlr_sql_password,
				hlr_sql_dbname);
		vector<resultRow>::const_iterator it = (stepQuery.queryResult).begin();
		while ( it != (stepQuery.queryResult).end() )
		{		//compute unique Strings
			string id = (*it)[0];	
			string gridResource = (*it)[1];	
			string lrmsId = (*it)[2];	
			string start = (*it)[3];	
			string wallTime = (*it)[4];	
			string cpuTime = (*it)[5];	
			string valuesBuffer = "";
			vector<string> ceIdBuff;
			Split (':', gridResource, &ceIdBuff);        
			if (ceIdBuff.size() > 0)
			{
				valuesBuffer  = ceIdBuff[0];//FIXME can't we finde something else?
			}
			valuesBuffer += lrmsId;
			valuesBuffer += start;
			valuesBuffer += wallTime;
			valuesBuffer += cpuTime;
			string uniqueS = "";
			makeUniqueString (valuesBuffer,uniqueS);
			string queryBuffer = "";
			queryBuffer += "UPDATE jobTransSummary SET uniqueChecksum='";
			queryBuffer += uniqueS;
			queryBuffer += "' WHERE id=" + id;
			dbResult resultUTI = hlrDb.query(queryBuffer);
			if ( hlrDb.errNo != 0 )
               	       	{
				indicator = "!";
  	                    	if ( debug )
                               	{
                       	       	 	cerr << "ERROR:" <<queryBuffer << endl;
	                       	}
       	                }
			else
			{
				y++;
			}
			// compute unique strings
			x++;
			counter++;
			valuesCounter++;
       		        if (counter >= barStep )
	       	        {
       		             cout << indicator << flush;
  	                     counter = 0;
			     indicator = "#";
  		        }
			it++;
		}
		if ( debug )
		{
			cout << "Found " << x << " transactions, processed:" << y << endl;
		}
		J += y;
		resLastTid = resLastTid+step;
		if ( x == stepNumber-1 ) percentage = 100;
		time1 = time(NULL);
		if ( (percentage-oldPercentage) !=0 )	
		{
			eta = (time1-time0)*(100-percentage)/((percentage-oldPercentage)*60);
		}
		cout << " [" << setw(3) << int2string(percentage) << "%] E:"<< int2string(time1-time0) << " ETA:"<< int2string(eta) << endl;
		oldPercentage = percentage;
	}
	jobTransSummary.enableKeys();
	if ( debug )
	{
		cout << "Total: found " << I << " transactions, processed:" << J << endl;
	}
	return res;
}

int upgradeTINSchema (database& DB)
{
	int res = 0;
	table trans_in(DB, "trans_in");
	trans_in.disableKeys();
	string upgradeQuery = "ALTER TABLE trans_in ";
	upgradeQuery += "CHANGE tid tid bigint(20) unsigned NOT NULL auto_increment, ";
	upgradeQuery += "CHANGE amount amount smallint(5) unsigned, ";
	upgradeQuery += "CHANGE tr_stamp tr_stamp int(10) unsigned, ";
	upgradeQuery += "CHANGE dg_jobid dgJobId varchar(160), ";
	upgradeQuery += "DROP from_url, ";
	upgradeQuery += "ADD uniqueChecksum char(32), ";
	upgradeQuery += "ADD accountingProcedure varchar(32)";
	if ( debug )
	{
		cerr << upgradeQuery << endl;
	}
	hlrGenericQuery upgrade1(upgradeQuery);
	upgrade1.query();
	if ( upgrade1.errNo != 0)
	{
		cerr << "Error in query upgrading trans_in (CHANGE,DROP,ADD step 1)." << endl;
		cerr << upgradeQuery << ":" << int2string(upgrade1.errNo) << endl;
		res = 1;
	}
	trans_in.addIndex( "uniqueChecksum");
	trans_in.enableKeys();
	return res;
}

int upgradeTQSchema (database& DB)
{
	int res = 0;
	table trans_queue(DB, "trans_queue");
	trans_queue.disableKeys();
	string upgradeQuery = "ALTER TABLE trans_queue ";
	upgradeQuery += "CHANGE from_cert_subject gridUser varchar(160), ";
	upgradeQuery += "CHANGE to_cert_subject gridResource varchar(160), ";
	upgradeQuery += "CHANGE to_hlr_url urSource varchar(160), ";
	upgradeQuery += "CHANGE amount amount smallint(5) unsigned, ";
	upgradeQuery += "CHANGE timestamp timestamp int(10) unsigned, ";
	upgradeQuery += "CHANGE priority priority smallint(5) unsigned, ";
	upgradeQuery += "CHANGE status_time status_time int(10) unsigned, ";
	upgradeQuery += "ADD uniqueChecksum char(32), ";
	upgradeQuery += "ADD accountingProcedure varchar(32), ";
	upgradeQuery += "DROP from_hlr_url, DROP type";
	if ( debug )
	{
		cerr << upgradeQuery << endl;
	}
	hlrGenericQuery upgrade1(hlr_tmp_sql_dbname, upgradeQuery);
	upgrade1.query();
	if ( upgrade1.errNo != 0)
	{
		cerr << "Error in query upgrading trans_queue (CHANGE step 1)." << endl;
		cerr << upgradeQuery << ":" << int2string(upgrade1.errNo) << endl;
		res = 1;
	}
	trans_queue.addIndex( "uniqueChecksum", true);
	trans_queue.dropIndex( "PRIMARY" );
	trans_queue.enableKeys();
	return res;
}

int RGV2ACCTDESC ()
{
	int res = 0;
	string query = "SELECT rid,gid from resource_group_vo";
	if ( debug )
	{
		cerr << query << endl;
	}
	hlrGenericQuery select1(hlr_sql_dbname, query);
	select1.query();
	if ( select1.errNo == 0 )
	{
		if ( (select1.queryResult).size() != 0 )
		{
			
			vector<resultRow>::iterator it = (select1.queryResult).begin();
			while ( it != (select1.queryResult).end() )
			{
				query = "UPDATE acctdesc SET gid='";
				query += (*it)[1] + "' WHERE id ='";
				query += (*it)[0] + "'";
				if ( debug )
				{
					cout << query << endl;
				}
				hlrGenericQuery replace1(hlr_sql_dbname, query);
				replace1.query();
				if ( replace1.errNo != 0 )
				{
					cerr << "Warning error in:" << query << endl;
				}
				it++;
			}
		}
		else
		{
			//No results???
			cerr << "Warning: no results in RGV." << endl;
		} 	
	}
	else
	{
		cerr << "Error in query upgrading RGV (SELECT step 1)." << endl;
		cerr << query << ":" << int2string(select1.errNo) << endl;
		res = 1;
	}
	return res;
}

int upgradeADSchema (database& DB)
{
	int res = 0;
	table acctdesc(DB, "acctdesc");
	acctdesc.disableKeys();
	string upgradeQuery = "ALTER TABLE acctdesc ";
	upgradeQuery += "CHANGE cert_subject ceId varchar(160)";
	if ( debug )
	{
		cerr << upgradeQuery << endl;
	}
	hlrGenericQuery upgrade1(hlr_sql_dbname, upgradeQuery);
	upgrade1.query();
	if ( upgrade1.errNo != 0)
	{
		cerr << "Error in query upgrading acctdesc (CHANGE step 1)." << endl;
		cerr << upgradeQuery << ":" << int2string(upgrade1.errNo) << endl;
		res = 1;
	}
	upgradeQuery = "ALTER TABLE acctdesc";
	upgradeQuery += " DROP a_type";
	if ( debug )
	{
		cerr << upgradeQuery << endl;
	}
	hlrGenericQuery upgrade2(hlr_sql_dbname, upgradeQuery);
	upgrade2.query(); 
	if ( upgrade2.errNo != 0)
	{
		cerr << "Error in query upgrading acctdesc (DROP step 1)." << endl;
		cerr << upgradeQuery << ":" << int2string(upgrade2.errNo) << endl;
		res = 2;
	}
	upgradeQuery = "ALTER TABLE acctdesc ";
	upgradeQuery += " ADD gid varchar(128)";
	if ( debug )
	{
		cerr << upgradeQuery << endl;
	}
	hlrGenericQuery upgrade3(hlr_sql_dbname, upgradeQuery);
	upgrade3.query();
	if ( upgrade3.errNo != 0)
	{
		cerr << "Error in query upgrading acctdesc (ADD step 1)." << endl;
		cerr << upgradeQuery << ":" << int2string(upgrade2.errNo) << endl;
		res = 3;
	}
	acctdesc.enableKeys();
	return res;
}

int createVoStorageRecordsTable (database& DB)
{
	table voStorageRecords(DB, "voStorageRecords");
	if ( voStorageRecords.exists() )
	{
		if ( debug )
		{
			cout << " table voStorageRecords already exists." << endl; 
		}
		return 0;
	}
	string queryString = "";
        queryString = "CREATE TABLE voStorageRecords";
        queryString += " (";
	queryString += " id bigint(20) unsigned auto_increment, ";
        queryString += " uniqueChecksum char(32), ";
        queryString += " timestamp int(10) unsigned, ";
	queryString += " siteName varchar(255), ";
        queryString += " vo varchar(255), ";
        queryString += " voDefSubClass varchar(255), ";
        queryString += " storage varchar(255), ";
        queryString += " storageSubClass varchar(255), ";
        queryString += " urSourceServer varchar(255), ";
        queryString += " usedBytes bigint(20) unsigned, ";
        queryString += " freeBytes bigint(20) unsigned, ";
        queryString += "primary key (uniqueChecksum), key(id), key (vo), key(storage))";
        if ( debug )
        {
                cerr << queryString << endl;
        }
        hlrGenericQuery makeTable(queryString);
        makeTable.query();
        return voStorageRecords.exists();
}


void Exit (int exitStatus )
{
	JTSLock MasterLock(masterLock);
	MasterLock.remove();
	exit(exitStatus);
}

int main (int argc, char **argv)
{
	options ( argc, argv );
	if (needs_help)
	{
		help(argv[0]);
		Exit(0);
	}
        map <string,string> confMap;
        if ( dgas_conf_read ( confFileName, &confMap ) != 0 )
	{
		cerr << "Couldn't open configuration file: " <<
			confFileName << endl;
	}
	signal (SIGTERM, doNothing);
	signal (SIGINT, doNothing);
        hlr_sql_server = (confMap["hlr_sql_server"]).c_str();
        hlr_sql_user = (confMap["hlr_sql_user"]).c_str();
        hlr_sql_password = (confMap["hlr_sql_password"]).c_str();
        hlr_sql_dbname = (confMap["hlr_sql_dbname"]).c_str();
        hlr_tmp_sql_dbname = (confMap["hlr_tmp_sql_dbname"]).c_str();
	hlr_logFileName = confMap["hlr_def_log"];
	int stepNumber = 5;
	string stepNumberStr = (confMap["translateStepNumber"]);
	string acceptRecordsStartDate = "";
	string removeDuplicatedWhereClause = "";
	bool autoDeleteOldRecords = false;
	string rulesFile;
	int res = bootstrapLog(hlr_logFileName, &logStream);
	if ( res != 0 )
	{
		cerr << "Error bootstrapping the Log file:" << endl;
		cerr << hlr_logFileName<< ",Error Code:" << int2string(res)<< endl;
		Exit(1);
	}
	database JTSdb(hlr_sql_server,
                        hlr_sql_user,
                        hlr_sql_password,
                        hlr_sql_dbname);
	database TMPdb(hlr_sql_server,
                        hlr_sql_user,
                        hlr_sql_password,
                        hlr_tmp_sql_dbname);
	if ( ( confMap["autoDeleteOldRecords"] == "true" ) || 
		( confMap["autoDeleteOldRecords"] == "yes" )  )
	{
		autoDeleteOldRecords = true;
	}
	if ( confMap["masterLock"] != "" )
	{
		masterLock = (confMap["masterLock"]).c_str();
	}
	else
	{
		masterLock = "/opt/glite/var/hlrTranslateDb.lock";
	}
	if ( confMap["systemLogLevel"] != "" )
	{
		system_log_level = atoi((confMap["systemLogLevel"]).c_str());
	}
	JTSLock MasterLock(masterLock);
	if ( MasterLock.exists() )
	{
		cout << "Another instance of hlr-translatedb put a lock. Exiting." << endl;
		exit(0);
	}
	cout << "Locking other instances out." << endl;
	MasterLock.create();
	if ( confMap["acceptRecordsStartDate"] != "" )
	{
		if ( autoDeleteOldRecords)
		{
			acceptRecordsStartDate = confMap["acceptRecordsStartDate"];
		}
	}
	if ( stepNumberStr != "" )
	{
		stepNumber = atoi(stepNumberStr.c_str());
	}
	else
	{
		stepNumber = 5;
	}
	if ( confMap["translateQueryLenght"] != "" )
	{
		queryLenght = atoi((confMap["translateQueryLenght"]).c_str());
	}
	if ( confMap["removeDuplicatedWhereClause"] != "" )
	{
		removeDuplicatedWhereClause = atoi((confMap["removeDuplicatedWhereClause"]).c_str());
	}
	is2ndLevelHlr = false;
	if ( confMap["is2ndLevelHlr"] == "true" )
	{
		is2ndLevelHlr =true;
	}
	#ifdef MERGE
	bool useMergeTables = false;
	string mergeTablesFile = "";
	string mergeTablesDefinitions = "";
	int mergeTablesPastMonths = 3;
	database DB(hlr_sql_server,
			hlr_sql_user,
			hlr_sql_password,
			hlr_sql_dbname);
	if ( confMap["rulesFile"] != "" )
	{
		rulesFile = confMap["rulesFile"];
	}
	if ( confMap["useMergeTables"] == "true" )
	{
		useMergeTables = true;
	}
	if ( confMap["mergeTablesDefinitions"] != "" )
	{
		mergeTablesDefinitions = confMap["mergeTablesDefinitions"];
	}
	if ( confMap["mergeTablesFile"] != "" )
	{
		mergeTablesFile = confMap["mergeTablesFile"];
	}
	if ( confMap["mergeTablesPastMonths"] != "" )
	{
		mergeTablesPastMonths = atoi((confMap["mergeTablesPastMonths"]).c_str());
	}
	mergeTables mt(DB,
			mergeTablesDefinitions,
			is2ndLevelHlr,
			mergeTablesFile,
			mergeTablesPastMonths);
	if ( mergeReset || reset ) mt.reset = true;
	#endif
	/*END merge tables definition*/
	JTSManager jtsManager(JTSdb, "jobTransSummary");
	table jobTransSummary(JTSdb, "jobTransSummary");
	table trans_in(JTSdb, "trans_in");
	
	cout << "Initializing database, this operation can take several minutes." << endl;
	if ( !jobTransSummary.checkIndex( "date") ) jobTransSummary.addIndex("date");
	if ( !jobTransSummary.checkIndex( "id") ) jobTransSummary.addIndex("id");
	if ( !jobTransSummary.checkIndex( "endDate") ) jobTransSummary.addIndex("endDate");
	if ( !jobTransSummary.checkIndex( "urSourceServer") ) jobTransSummary.addIndex("urSourceServer");
	if ( !jobTransSummary.checkIndex( "uniqueChecksum") ) jobTransSummary.addIndex("uniqueChecksum");
	if ( !jobTransSummary.checkIndex( "rid" ) ) trans_in.addIndex("rid");
	if ( !jobTransSummary.checkIndex( "dgJobId" ) ) trans_in.addIndex("dgJobId");
	if ( !jobTransSummary.checkIndex( "uniqueChecksum" ) ) trans_in.addIndex("uniqueChecksum");
	if ( (!jobTransSummary.checkIndex( "lrmsId" ) ) && ( !is2ndLevelHlr ) ) jobTransSummary.addIndex("lrmsId");
	if ( (!jobTransSummary.checkIndex( "hlrTid" ) ) && ( !is2ndLevelHlr ) ) jobTransSummary.addIndex("hlrTid");
	//Remove unused 'jobAuth' table (>3.1.8)
	table jobAuth(JTSdb, "jobAuth");
	if ( jobAuth.exists() )
	{
		if ( jobAuth.drop() != 0) 
		{
			if ( debug )
			{
				cerr << "Warning: Error dropping jobAuth!" << endl;
			}
		}
		
	}
	table jobTransSummaryIndex(JTSdb, "jobTransSummaryIndex");
	table roles(JTSdb, "roles");
	table vomsAuthMap(JTSdb, "vomsAuthMap");
	//create storage records table if it doesn't exists yet.
	createVoStorageRecordsTable(JTSdb);
	//try to update the table withuot recreating it 
	//(applies just if updating 
	//from a database already containing previous updates otherwise
	//a reset will occur)
	string jobTransSummaryFields = "dgJobId;date;transType;thisGridId;remoteGridId;userFqan;userVo;remoteHlr;cpuTime;wallTime;pmem;vmem;amount;start;end;si2k;sf2k;acl;id;lrmsId;localUserId;hlrGroup;localGroup;endDate;siteName;urSourceServer;hlrTid;accountingProcedure;voOrigin;GlueCEInfoTotalCPUs";
	if ( !reset && jobTransSummary.checkAgainst(jobTransSummaryFields) )
	{
		if ( !reset && is2ndLevelHlr )
		{
			cout << "Updating jobTransSummary. This operation requires several minutes." << endl;
			//add the field
			res = upgradeJTSSchema(jobTransSummary);
			if ( res != 0 )
			{
				cerr << "Error upgrading database! Please contact dgas-support@to.infn.it" << endl;
				Exit(1);
			}
		}
		else
		{
			reset = true;
		}
		if ( jobTransSummaryIndex.exists())
		{
			jobTransSummaryIndex.drop();
		}
	}
	string trans_inFields = "tid;rid;gid;from_dn;from_url;amount;tr_stamp;dg_jobid";
	if ( trans_in.checkAgainst( trans_inFields ) )
	{
		cout << "Updating trans_in. This operation requires several minutes, no progress bar." << endl;
		cout << "This operation is performed just once." << endl;
		//add the field
		res = upgradeTINSchema(JTSdb);
		if ( res != 0 )
		{
			cerr << "Error upgrading database! Please contact dgas-support@to.infn.it" << endl;
			Exit(1);
		}
	}
	table trans_queue(TMPdb, "trans_queue");
	string trans_queueFields = "transaction_id;from_cert_subject;to_cert_subject;from_hlr_url;to_hlr_url;amount;timestamp;log_data;priority;status_time;type";
	if ( trans_queue.checkAgainst( trans_queueFields ) )
	{
		cout << "Updating trans_queue schema. This operation requires several minutes, no progress bar will be showed during the upgrade." << endl;
		cout << "This operation is performed just once." << endl;
		//add the field
		res = upgradeTQSchema(TMPdb);
		if ( res != 0 )
		{
			cerr << "Error upgrading database! Please contact dgas-support@to.infn.it" << endl;
			Exit(1);
		}
	}
	table acctdesc (JTSdb, "acctdesc");
	string acctdescFields = "id;a_type;email;descr;cert_subject;acl";
	if ( acctdesc.checkAgainst( acctdescFields ) )
	{
		cout << "Updating acctdesc schema." << endl;
		cout << "This operation is performed just once." << endl;
		res = upgradeADSchema(JTSdb);
		if ( res != 0 )
		{
			cerr << "Error upgrading database! Please contact dgas-support@to.infn.it" << endl;
			Exit(1);
		}
		else
		{
			table resource_group_vo (JTSdb, "resource_group_vo");
			if (resource_group_vo.exists())
			{
				res = RGV2ACCTDESC();
				if ( res != 0 )
				{
					cerr << "Error upgrading database! Please contact dgas-support@to.infn.it" << endl;
					Exit(1);
				}
				else
				{
					resource_group_vo.drop();
				}	
			}
		}
	}
	jobTransSummaryFields = "dgJobId;date;gridResource;gridUser;userFqan;userVo;cpuTime;wallTime;pmem;vmem;amount;start;end;iBench;iBenchType;fBench;fBenchType;acl;id;lrmsId;localUserId;hlrGroup;localGroup;endDate;siteName;urSourceServer;hlrTid;accountingProcedure;voOrigin;GlueCEInfoTotalCPUs;executingNodes;uniqueChecksum";
	if ( (!is2ndLevelHlr) && (!jobTransSummary.checkAgainst(jobTransSummaryFields )) )
	{
		reset = true;
	}
	if ( reset ) 
	{
		#ifdef MERGE
		if ( useMergeTables ) mt.drop();
		if ( useMergeTables ) mt.dropAll();
		#endif
		jobTransSummary.drop();
		if ( jobTransSummaryIndex.exists() ) 
			jobTransSummaryIndex.drop();
	}
	if (!jobTransSummary.exists() )
	{
		flushTables();
		if ( !createJobTransSummaryTable(JTSdb) )
		{
			cerr << "Error creating the table jobTransSummary!" << endl;
			Exit(1);
		}
	}
	if ( !roles.exists() )
        {
                if ( !createRolesTable(JTSdb) )
                {
                        cerr << "Error creating the  table roles!" << endl;
                        Exit(1);
                }
        }
	if (!vomsAuthMap.exists())
	{
		if (!createVomsAuthMapTable(JTSdb))
		{
			cerr << "Error creating the  table vomsAuthMap!" << endl;
                        Exit(1);
		}
	}
	//drop transInInfo table. Not used anymore.
	table transInInfo(JTSdb, "transInInfo"); 
	if ( transInInfo.exists()) transInInfo.drop();
	//drop vodesc table. Not used anymore.
	table vodesc(JTSdb, "vodesc"); 
	if (vodesc.exists()) vodesc.drop();
	//drop grdesc table. Not used anymore.
	table grdesc(JTSdb, "grdesc"); 
	if (grdesc.exists()) grdesc.drop();
	//dropping tables related to User HLR (remvoved >=3.2.0)
	table transOutInfo(JTSdb, "transOutInfo"); 
	if (transOutInfo.exists()) transOutInfo.drop();
	table transOutLog(JTSdb, "transOutLog"); 
	if (transOutLog.exists()) transOutLog.drop();
	table trans_out(JTSdb, "trans_out"); 
	if ( trans_out.exists()) trans_out.drop();
	table user_group_vo(JTSdb, "user_group_vo"); 
	if ( user_group_vo.exists()) user_group_vo.drop();
	table group_vo(JTSdb, "group_vo"); 
	if ( group_vo.exists() ) group_vo.drop();
	table groupAdmin(JTSdb, "groupAdmin"); 
	if ( groupAdmin.exists()) groupAdmin.drop();
	upgrade_R_3_4_0_23();
	if ( is2ndLevelHlr )
	{
		cout << "2ndLevelHlr is set to \"true\" in the conf file." << endl;
		table urConcentratorIndex (JTSdb, "urConcentratorIndex");
		string urConcentratorIndexFields = "urSourceServer;urSourceServerDN;remoteRecordId;recordDate;recordInsertDate";
		if ( !urConcentratorIndex.checkAgainst( urConcentratorIndexFields ) )
		{
			cout << "Resetting urConcentratorIndex." << endl;
			reset = true;
		}
		if ( reset ) 
		{
			urConcentratorIndex.drop();
		}
		if (urConcentratorIndex.exists())
		{
			if ( !createUrConcentratorTable(JTSdb) )
			{
				cerr << "Error creating the  table urConcentratorIndex!" << endl;
				Exit(1);
			}
		}
		//cleanup entries older than the expected period.
		if ( acceptRecordsStartDate != "" )
		{
			if ( cleanUpOld(acceptRecordsStartDate) != 0 )
			{
				cerr << "Error cleaning up old records!" << endl;
				Exit(1);
			}
		}
		//clean up database from tables not needed on 2lhlr.
		if (acctdesc.exists()) acctdesc.drop();
		table resource_group_vo (JTSdb, "resource_group_vo");
		if (resource_group_vo.exists()) resource_group_vo.drop();
		table transInInfo (JTSdb, "transInInfo");
		if (transInInfo.exists()) transInInfo.drop();
		table transInLog (JTSdb, "transInLog");
		if (transInLog.exists()) transInLog.drop();
		table trans_in (JTSdb, "trans_in");
		if (trans_in.exists()) trans_in.drop();
		#ifdef MERGE
		if ( useMergeTables )
		{
			cout << "Updating merge tables status." << endl;
			mt.exec();
			//now add recordDate index on date field to records_*
			//tables
			mt.addIndex("date","recordDate");
		}
		#endif
		if ( checkDuplicate ) jtsManager.removeDuplicated(removeDuplicatedWhereClause);
		if ( translationRules ) execTranslationRules(JTSdb, rulesFile);
		//cleanup Indexes that are not useful on 2L hlr
		if ( jobTransSummary.checkIndex( "lrmsId" ) )
			jobTransSummary.dropIndex( "lrmsId" );
		if ( jobTransSummary.checkIndex( "hlrTid") )
			jobTransSummary.dropIndex( "hlrTid" );
		cout << "We can safely exit here." << endl;
		Exit(0);
	//if this is a 2nd level HLR we can bail out here...
	}
	//otherwise we must go on...
	if (reset) jobTransSummary.disableKeys();
	int percentage = 0;
	int oldPercentage = 0;
	time_t time0;
	time_t time1;
	time_t eta = 0;
	int I =0;
	int J = 0;
	int resLastTid = 0;
	int maxResTid;
	getIndexFromTable ( resLastTid );	
	getMaxTid( maxResTid );
	if ( maxResTid == resLastTid )
	{
		//nothing to do.
		if ( debug ) cout << "Nothing new to process." << endl;		
	}
	else
	{
		maxResTid++;
		stepNumber = computeIterationsNo ( stepNumber, resLastTid );
		percentage = 0;
		if ( debug )
		{
			cout << "Dividing operation in " << int2string(stepNumber) << " steps." << endl;
		}
		int step = 1;
		if ( stepNumber != 0 )
		{
			step = (maxResTid-resLastTid)/stepNumber;
		}
		if (step <= 0 ) step = 1;
		for (int i=0; i<stepNumber; i++)
		{
			time0 = time(NULL);
			percentage = ((i+1)*100)/stepNumber;
			string queryString_res = composeQuery(resLastTid,resLastTid+step);
			hlrGenericQuery genericQuery_res(queryString_res);
   			int res = genericQuery_res.query();
			if ( res != 0 )
			{
				cout << "Warning: problem in query.";
				if ( debug )
				{
					cout << int2string(res) << ":" << queryString_res << endl;
				}
			}
			populateJobTransSummaryTable (jobTransSummary, jtsManager, genericQuery_res, I, J );
			resLastTid = resLastTid+step;
			if ( i == stepNumber-1 ) percentage = 100;
			time1 = time(NULL);
			if ( (percentage-oldPercentage) !=0 )	
			{
				eta = (time1-time0)*(100-percentage)/((percentage-oldPercentage)*60);
			}
			cout << " [" << setw(3) << int2string(percentage) << "%] E:"<< int2string(time1-time0) << " ETA:"<< int2string(eta) << endl;
			oldPercentage = percentage;
		}
	}
	if ( reset ) jobTransSummary.enableKeys ();
	cout << "Found " << I << " transactions, processed:" << J << endl;
	#ifdef MERGE
	if ( useMergeTables )
	{
		mt.exec();
		mt.addIndex("date","recordDate");
	}
	#endif
	if (checkDuplicate )
	{
		jtsManager.removeDuplicated(removeDuplicatedWhereClause);
	}
	if ( translationRules ) execTranslationRules(JTSdb, rulesFile);
	/*merge tables exec end*/
	cout << "Done." << endl;
	Exit(0);
}
