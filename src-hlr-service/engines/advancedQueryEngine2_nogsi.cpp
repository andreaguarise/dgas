// DGAS (DataGrid Accounting System) 
// Server Daeomn and protocol engines.
// 
// $Id: advancedQueryEngine2_nogsi.cpp,v 1.1.2.2 2012/03/20 12:42:58 aguarise Exp $
// -------------------------------------------------------------------------
// Copyright (c) 2001-2002, The DataGrid project, INFN, 
// All rights reserved. See LICENSE file for details.
// -------------------------------------------------------------------------
// Author: Andrea Guarise <andrea.guarise@to.infn.it>
/***************************************************************************
 * Code borrowed from:
 *  authors   :
 *  copyright : 
 ***************************************************************************/
//
//    

// ---------------------------------------------------------------------------
//  Includes
// ---------------------------------------------------------------------------
#include "advancedQueryEngine2.h"
#include <sstream>
#include <iomanip>

extern ofstream logStream;
extern string  maxItemsPerQuery;
extern string authQueryTables;
extern bool authUserSqlQueries; 

inline int urlSplit(char delim, string url_string , url_type *url_buff)
{
	size_t pos = 0;
	pos = url_string.find_first_of( delim, 0);
	url_buff->hostname=url_string.substr(0,pos);
	url_buff->port=atoi((url_string.substr(pos+1,url_string.size()).c_str()));

	return 0;
}



void xml2struct(inputData &out, string &xml)
{
	node tagBuff;
	string logBuff = "";

	tagBuff = parse(&xml, "jobId");
	if(tagBuff.status == 0)
	{
		logBuff = "queryENGINE, got: \"" + tagBuff.text + "\"";
		hlr_log(logBuff, &logStream, 8);
		out.jobId = tagBuff.text;
	}
	tagBuff = parse(&xml, "startTid");
	if(tagBuff.status == 0)
	{
		logBuff = "queryENGINE, got: \"" + tagBuff.text + "\"";
		hlr_log(logBuff, &logStream, 8);
		out.startTid = tagBuff.text;
	}

	tagBuff = parse(&xml, "USERCERTBUFFER");
	if(tagBuff.status == 0)
	{
		logBuff = "queryENGINE, got: \"" + tagBuff.text + "\"";
		hlr_log(logBuff, &logStream, 8);
		out.userCertBuffer = tagBuff.text;
	}


	tagBuff = parse(&xml, "RESOURCEIDBUFFER");
	if(tagBuff.status == 0)
	{
		logBuff = "queryENGINE, got: \"" + tagBuff.text + "\"";
		hlr_log(logBuff, &logStream, 8);
		out.resourceIDBuffer = tagBuff.text;
	}


	tagBuff = parse(&xml, "TIMEBUFFER");
	if(tagBuff.status == 0)
	{
		logBuff = "queryENGINE, got: \"" + tagBuff.text + "\"";
		hlr_log(logBuff, &logStream, 8);
		out.timeBuffer = tagBuff.text;
	}


	tagBuff = parse(&xml, "GROUPIDBUFFER");
	if(tagBuff.status == 0)
	{
		logBuff = "queryENGINE, got: \"" + tagBuff.text + "\"";
		hlr_log(logBuff, &logStream, 8);
		out.groupIDBuffer = tagBuff.text;
	}


	tagBuff = parse(&xml, "VOIDBUFFER");
	if(tagBuff.status == 0)
	{
		logBuff = "queryENGINE, got: \"" + tagBuff.text + "\"";
		hlr_log(logBuff, &logStream, 8);
		out.voIDBuffer = tagBuff.text;
	}

	tagBuff = parse(&xml, "VOMSROLE");
	if(tagBuff.status == 0)
	{
		logBuff = "queryENGINE, got: \"" + tagBuff.text + "\"";
		hlr_log(logBuff, &logStream, 8);
		out.fqanBuffer = tagBuff.text;
	}

	tagBuff = parse(&xml, "FREQUENCYBUFFER");
	if(tagBuff.status == 0)
	{
		logBuff = "queryENGINE, got: \"" + tagBuff.text + "\"";
		hlr_log(logBuff, &logStream, 8);
		out.frequencyBuffer = tagBuff.text;
	}

	tagBuff = parse(&xml, "AGGREGATESTRINGBUFFER");
	if(tagBuff.status == 0)
	{
		logBuff = "queryENGINE, got: \"" + tagBuff.text + "\"";
		hlr_log(logBuff, &logStream, 8);
		out.aggregateStringBuff = tagBuff.text;
	}
	tagBuff = parse(&xml, "TIMEINDEXBUFF");
	if(tagBuff.status == 0)
	{
		logBuff = "queryENGINE, got: \"" + tagBuff.text + "\"";
		hlr_log(logBuff, &logStream, 8);
		out.timeIndexBuff = tagBuff.text;
	}


	tagBuff = parse(&xml, "DEBUG");
	if(tagBuff.status == 0)
	{
		logBuff = "queryENGINE, got: \"" + tagBuff.text + "\"";
		hlr_log(logBuff, &logStream, 8);
		out.debug =  atoi(tagBuff.text.c_str());
	}


	tagBuff = parse(&xml, "AGGREGATEFLAG");
	if(tagBuff.status == 0)
	{
		logBuff = "queryENGINE, got: \"" + tagBuff.text + "\"";
		hlr_log(logBuff, &logStream, 8);
		out.aggregateFlag = atoi(tagBuff.text.c_str());
	}

	tagBuff = parse(&xml, "groupBy");
	if(tagBuff.status == 0)
	{
		logBuff = "queryENGINE, got: \"" + tagBuff.text + "\"";
		hlr_log(logBuff, &logStream, 8);
		out.groupBy = tagBuff.text;
	}

	tagBuff = parse(&xml, "siteName");
	if(tagBuff.status == 0)
	{
		logBuff = "queryENGINE, got: \"" + tagBuff.text + "\"";
		hlr_log(logBuff, &logStream, 8);
		out.siteName = tagBuff.text;
	}

	tagBuff = parse(&xml, "urOrigin");
	if(tagBuff.status == 0)
	{
		logBuff = "queryENGINE, got: \"" + tagBuff.text + "\"";
		hlr_log(logBuff, &logStream, 8);
		out.urOrigin = tagBuff.text;
	}

	tagBuff = parse(&xml, "authoriseAs");
	if(tagBuff.status == 0)
	{
		logBuff = "queryENGINE, got: \"" + tagBuff.text + "\"";
		hlr_log(logBuff, &logStream, 8);
		out.substDn = tagBuff.text;
	}

	tagBuff = parse(&xml, "orderBy");
	if(tagBuff.status == 0)
	{
		logBuff = "queryENGINE, got: \"" + tagBuff.text + "\"";
		hlr_log(logBuff, &logStream, 8);
		out.orderBy = tagBuff.text;
	}

	tagBuff = parse(&xml, "LISTFLAG");
	if(tagBuff.status == 0)
	{
		logBuff = "queryENGINE, got: \"" + tagBuff.text + "\"";
		hlr_log(logBuff, &logStream, 8);
		out.listFlag = atoi(tagBuff.text.c_str());
	}


	tagBuff = parse(&xml, "QUERYTYPEBUFFER");
	if(tagBuff.status == 0)
	{
		logBuff = "queryENGINE, got: \"" + tagBuff.text + "\"";
		hlr_log(logBuff, &logStream, 8);
		out.queryTypeBuffer = tagBuff.text;
	}


	tagBuff = parse(&xml, "ITSHEADING");
	if(tagBuff.status == 0)
	{
		logBuff = "queryENGINE, got: \"" + tagBuff.text + "\"";
		hlr_log(logBuff, &logStream, 8);
		out.itsHeading = atoi(tagBuff.text.c_str());
	}

	tagBuff = parse(&xml, "itsFieldList");
	if(tagBuff.status == 0)
	{
		logBuff = "queryENGINE, got: \"" + tagBuff.text + "\"";
		hlr_log(logBuff, &logStream, 8);
		out.itsFieldList = tagBuff.text;
	}

	return;
}
int advancedQueryEngine_parse_xml (string &doc, inputData &input)
{
	node nodeBuff;
	while ( nodeBuff.status == 0 )
	{
		string tag = "inputData";
		nodeBuff = parse(&doc, tag);
		if ( nodeBuff.status != 0 )
			break;
		xml2struct(input, doc);
		nodeBuff.release();
	}
	return 0;

}//advancedQueryEngine_parse_xml


int advancedQueryEngine_compose_xml(vector<string> &queryResult ,string *output, int status, inputData& d)
{
	*output = "<HLR type=\"advancedQuery_answer\">\n";
	*output += "<BODY>\n";
	*output += "<queryResult>";
	vector<string>::iterator it = queryResult.begin();
	vector<string>::iterator it_end = queryResult.end();
	while ( it != it_end )
	{	
		*output += "\n<lineString>";
		*output += *it;
		*output += "</lineString>\n";
		it++;
	}
	*output += "</queryResult>\n";
	*output += "<lastTid>\n";
	*output += d.lastTid;
	*output += "\n</lastTid>\n";
	*output += "<STATUS>";
	*output += int2string(status);
	*output += "</STATUS>";
	*output += "</BODY>\n";
	*output += "</HLR>\n";
	return 0;
}//advancedQueryEngine_compose_xml


bool isHlrAdmin (connInfo& connectionInfo)
{
	//check if the request comes from a user mapped as hlrAdmin
	hlrAdmin a;
	a.acl = connectionInfo.contactString;
	if ( a.get() == 0 )
	{
		return true;
	}
	else
	{
		a.acl = connectionInfo.hostName;
		if ( a.get() == 0 )
		{
			return true;
		}
		return false;
	}
}//request_is_authorized (ATM_job_data &job_data)

bool isVoAdmin (connInfo& connectionInfo, string &requestedVo)
{
	//if user is mapped as voAdmion for the vo requested in the conn
	//he will not be flagged as normal user but there will be 
	//a coinstraint to the requested vo.
	if ( connectionInfo.contactString == "voAdmin" )
	{
		return true;
	}
	hlrVoAcl v;
	v.voId = requestedVo;
	v.acl = connectionInfo.contactString;
	return v.exists(); 
}

bool isSiteAdmin (connInfo& connectionInfo, string &requestedSite)
{
	//if user is mapped as voAdmion for the vo requested in the conn
	//he will not be flagged as normal user but there will be 
	//a coinstraint to the requested SiteName.
	if ( connectionInfo.contactString == "siteAdmin" )
	{
		return true;
	}
	return false;
}


inline string stripWhite ( string &input )
{
	size_t startPos = input.find_first_not_of(" \n\0");
	if ( startPos == string::npos )
		return "";
	size_t endPos = input.find_last_not_of(" \n\0");
	if ( endPos == string::npos )
		return "";
	return input.substr( startPos, endPos - startPos +1 );
}

int getSySDefTables(vector<string>& tables)
{
	string queryString = "show tables like \"sysDef%\"";
	hlrGenericQuery genericQuery(queryString);
	int res = genericQuery.query();
	if ( res == 0 )
	{
		vector<resultRow>::const_iterator it = (genericQuery.queryResult).begin();
		while ( it != (genericQuery.queryResult).end() )
		{
			string logBuff = "Adding " + (*it)[0] + " to tables.";	
			hlr_log(logBuff, &logStream,8); 
			tables.push_back((*it)[0]);
			it++;
		}
	}	
	return 0;
}

int getAuthorizedTables(vector<string>& tables)
{
	vector<string> buffer;
	Split(';',authQueryTables, &buffer );
	vector<string>::iterator it = buffer.begin();
	vector<string>::iterator it_end = buffer.end();
	while ( it != it_end )
	{
		tables.push_back(stripWhite(*it));
		it++;
	}
	//FIXME complete getAuthorizedTables
	//This method retrieves the list of "user@db.table:password" that can be queried.
	return 0;
}


int parseQueryType(inputData& iD)
{
	if ( iD.queryTypeBuffer == "sql" || iD.queryTypeBuffer == "sqlCsv" )
	{
		return 0;
	}
	if ( iD.queryTypeBuffer == "fieldList" )
	{
		return 0;
	}
	if ( iD.queryTypeBuffer == "showTables" )
	{
		return 0;
	}
	return 1;
}

string composeError(string message, int status)
{
	string output;
	output = "<HLR type=\"advancedQuery_answer\">\n";
	output += "<BODY>\n";
	output += "<Error>";
	output += message;
	output += "</Error>";
	output += "<STATUS>";
	output += int2string(status);
	output += "</STATUS>";
	output += "</BODY>\n";
	output += "</HLR>\n";
	return output;
}

//get the xml object from the daemon, parse the information 
int advancedQueryEngine( string &inputXML, connInfo &connectionInfo, string *output )
{
	hlr_log ("advancedQueryEngine: Entering.", &logStream,5);
	int exitCode = 0;
	inputData inputStruct;
	if ( advancedQueryEngine_parse_xml (inputXML, inputStruct) != 0)
	{
		hlr_log ("advancedQueryEngine: ERROR parsing XML.", &logStream,1);
		exitCode =1;
	}
	if ( inputStruct.substDn != "" )
	{
		if ( isHlrAdmin(connectionInfo) )
		{
			string logBuff = "Request received from authorised peer ";
			hlr_log(logBuff,&logStream,3);
			connectionInfo.contactString = inputStruct.substDn;
			logBuff = "requested to be authorised as:";
			logBuff += connectionInfo.contactString;
			hlr_log(logBuff,&logStream,3);
		}
		else
		{
			*output = composeError("You are not authorised to issue an authoriseAs request. Your certificate is not mapped as hlr admin, sorry.",1);
			return 4;
		}
	}
	if ( 
			inputStruct.queryTypeBuffer != "sql" &&
			inputStruct.queryTypeBuffer != "sqlCsv" &&
			inputStruct.queryTypeBuffer != "fieldList" &&
			inputStruct.queryTypeBuffer != "showTables" )
	{
		*output = composeError("Unknown Query Type",1);
		return 1;
	}
	vector<string> outputVector;
	if ( parseQueryType(inputStruct) != 0 )
	{
		hlr_log ("advancedQueryEngine: Error composing the query",
				&logStream,1);
		*output = composeError("HLR Internal error:Error composing the query",2);
		return 2;
	}
	else	
	{
		string logBuff = "advancedQueryEngine: Query Type identified:";
		logBuff += inputStruct.queryTypeBuffer;
		hlr_log (logBuff,
				&logStream,6);
	}
	if ( inputStruct.queryTypeBuffer == "fieldList"  )
	{
		if ( inputStruct.itsFieldList != "1" )
		{
			*output = composeError("Command not supported, client too old.",3);
			return 3;
		}
		//returns help info here.
		string queryString = "DESCRIBE jobTransSummary";
		hlr_log("Performing query...", &logStream,8);
		hlrGenericQuery genericQuery(queryString);
		int res = genericQuery.query();
		hlr_log("... query performed", &logStream,8);
		if ( res != 0 )
		{
			hlr_log ("advancedQueryEngine.Error in query.",
					&logStream,3);
			*output = composeError("Info not Found",3);
			return 3;
		}
		else
		{
			hlr_log("Formatting query output...", &logStream,8);
			*output = "<HLR type=\"advancedQuery_answer\">\n";
			*output += "<BODY>\n";
			*output += "<queryResult>";
			vector<resultRow>::const_iterator it = (genericQuery.queryResult).begin();
			while ( it != (genericQuery.queryResult).end() )
			{
				*output += "\n<lineString>";
				*output += (*it)[0] + " - " + (*it)[1] + " - " +(*it)[3];
				*output += "</lineString>\n";
				it++;
			}
			*output += "</queryResult>\n";
			*output += "<STATUS>0</STATUS>";
			*output += "</BODY>\n";
			*output += "</HLR>\n";
			hlr_log("...done.", &logStream,8);
		}
		return 0;
	}
	if ( (inputStruct.queryTypeBuffer == "sql") || 
			(inputStruct.queryTypeBuffer == "sqlCsv") )
	{
		bool authz = false;
		string appendQuery = "";
		string authRoleLogBuff = "";
		if (!isHlrAdmin(connectionInfo) )
		{
			if (isVoAdmin(connectionInfo,inputStruct.voIDBuffer))
			{
				authz = true;
				authRoleLogBuff = "voAdmin";
				appendQuery = " userVo=\"" + inputStruct.voIDBuffer + "\" ";
			}
			else if(isSiteAdmin(connectionInfo,inputStruct.siteName))
			{
				authz = true;
				authRoleLogBuff = "siteAdmin";
				appendQuery = " siteName=\"" + inputStruct.siteName + "\" ";
			}
			else if ( authUserSqlQueries )
			{

				//FIXME insert a configuration parameter to
				//allow the administrator to disable normal 
				//user queries.
				authz = true;
				authRoleLogBuff = "normalUser";
				appendQuery = " gridUser=\"" + connectionInfo.contactString + "\" ";
			}
		}
		else
		{
			authz = true;
			authRoleLogBuff = "hlrAdmin";
		}
		if ( !authz )
		{
			*output = composeError("User not authorized to this query type!",3);
			return 3;
		}
		string queryString = inputStruct.aggregateStringBuff;
		string queryStringBuffer = queryString;
		for (size_t j=0; j<queryStringBuffer.length(); ++j)
		{
			queryStringBuffer[j]=toupper(queryStringBuffer[j]);
		}
		string::size_type pos = 0;
		pos = queryStringBuffer.find("SELECT");
		if ( (pos != 0) || ( pos == string::npos ) )
		{	
			pos = queryStringBuffer.find("EXPLAIN");
			if ( (pos != 0) || ( pos == string::npos ) )
			{
				*output = composeError("Command not allowed: queries MUST begin with SELECT or EXPLAIN ",3);
				return 3;
			}
		}
		//manage LIMIT clause
		string maxItemsBuff = maxItemsPerQuery;
		if ( maxItemsBuff != "" )
		{
			string Buff = "maxItemsPerQuery =" + maxItemsBuff;
			Buff += ", searching for:" + authRoleLogBuff;
			hlr_log (Buff, &logStream,8);
			if ( maxItemsBuff.find_first_not_of("0123456789") != string::npos )
			{
				//not just a number. try searching for authRoleLogBuff:number string and use number as the limit value, otherwise search for "default:number" and use this as limit
				size_t startPos = maxItemsBuff.find(authRoleLogBuff);
				if ( startPos != string::npos )
				{
					//authRoleLogBuff string found
					startPos = maxItemsBuff.find(":(",startPos) + 2;
					size_t endPos = maxItemsBuff.find_first_of(")", startPos);
					if ( endPos != string::npos )
					{
						maxItemsBuff = maxItemsBuff.substr(startPos, endPos-startPos);
					}
				}
			}
			if ( maxItemsBuff.find_first_not_of("0123456789") != string::npos )
			{
				hlr_log ("Configuration error for maxItemsPerQuery variable, please check, using default value", &logStream, 3);
				maxItemsBuff = "2500";
			}
			Buff = "maxItemsPerQuery =" + maxItemsBuff;
			hlr_log (Buff, &logStream,7);
		}
		string currentLimit = maxItemsBuff;
		pos = queryStringBuffer.find("LIMIT");
		if ( pos != string::npos )
		{
			//user specified his own limit value, if it is lower 
			//than maxItemsPerQuery use the user's limit.
			string::size_type limitStart = queryStringBuffer.find_first_of("0123456789",pos);
			string::size_type limitEnd = queryStringBuffer.find_first_not_of("0123456789",limitStart);
			if ( limitStart != string::npos ) 
			{
				string limitBuff = queryStringBuffer.substr(limitStart,limitEnd-limitStart);
				if (atoi(limitBuff.c_str()) < atoi(maxItemsBuff.c_str()))
					currentLimit = limitBuff;

				queryString.erase(pos);
			}
			else	
			{
				*output = composeError("Syntax error",3);
				return 3;
			}	
		}
		//manage ORDER clause
		string orderBuff;
		pos = queryStringBuffer.rfind("ORDER BY");
		if ( pos == string::npos )
			pos = queryStringBuffer.rfind("order by");
		if ( pos != string::npos )
		{
			orderBuff = queryString.substr(pos);
			queryString.erase(pos);
			string logBuff = "ORDER BY:" + orderBuff;
			hlr_log(logBuff, &logStream,6);
		}
		//manage GROUP clause
		string groupBuff;
		pos = queryStringBuffer.rfind("GROUP BY");
		if ( pos == string::npos )
			pos = queryStringBuffer.rfind("group by");
		if ( pos != string::npos )
		{
			groupBuff = queryString.substr(pos);
			queryString.erase(pos);
			string logBuff = "GROUP BY:" + groupBuff;
			hlr_log(logBuff, &logStream,6);
		}
		//find FROM statement
		string tablesBuffer;
		//manage WHERE clause
		pos = queryString.rfind("WHERE ");
		if ( pos == string::npos )
			pos = queryString.rfind("where ");
		if ( pos != string::npos )
		{	
			size_t fromPos = queryString.rfind("FROM ",pos);
			if ( fromPos == string::npos )
				fromPos = queryString.rfind("from ",pos);
			tablesBuffer = queryString.substr(fromPos+5,pos-fromPos-6);
			if ( appendQuery != "" )
				appendQuery = " AND " + appendQuery;
		}
		else
		{
			size_t fromPos = queryString.rfind("FROM ");
			if ( fromPos == string::npos )
				fromPos = queryString.rfind("from ");
			tablesBuffer = queryString.substr(fromPos+5);
			if ( appendQuery != "" )
				appendQuery = " WHERE " + appendQuery;
		}
		string logBuff = "tables:-" + tablesBuffer + "-";
		hlr_log(logBuff, &logStream,8);
		string tableList;
		vector<string> tables;
		getSySDefTables(tables);
		getAuthorizedTables(tables);
		if ( tables.size() != 0 )
		{
			vector<string>::iterator it = tables.begin();
			vector<string>::iterator it_end = tables.end();
			while ( it != it_end )
			{
				tableList += *it + " ; ";
				it++;
			}
		}
		if ( tableList.find("jobTransSummary") == string::npos )
		{
			tableList += " jobTransSummary ;";
		}
		if ( tableList.find("voStorageRecords") == string::npos )
		{
			tableList += " voStorageRecords ;";
		}
		hlr_log (tableList,&logStream,8);
		vector<string> queryTables;
		Split(',',tablesBuffer,&queryTables);
		vector<string>::iterator it = queryTables.begin();
		while ( it != queryTables.end() )
		{	
			string buff = stripWhite(*it);
			if ( tableList.find(buff) == string::npos )
			{
				string logBuff = "Queries on table: "+ buff + " are not allowed!. use -Q showTables to get available tables.";
				*output = composeError(logBuff,3);
				return 3;
			}
			it++;
		}
		//recompose query
		queryString += appendQuery;
		queryString += groupBuff;
		queryString += orderBuff;
		queryString += " LIMIT " + currentLimit; 
		hlr_log("Performing query...", &logStream,8);
		hlr_log(queryString, &logStream,6);
		hlrGenericQuery genericQuery(queryString);
		int res = genericQuery.query();
		hlr_log("... query performed", &logStream,8);
		if ( res != 0 )
		{
			hlr_log ("advancedQueryEngine.Error in query.",
					&logStream,3);
			string error = "Info not found, DB error:" + genericQuery.errMsg;
			*output = composeError(error,3);
			return 3;
		}
		else
		{
			hlr_log("Formatting query output...", &logStream,8);
			*output = "<HLR type=\"advancedQuery_answer\">\n";
			*output += "<BODY>\n";
			*output += "<queryResult>";
			vector<size_t> fieldsSize;
			vector<resultRow>::const_iterator lenght_it = (genericQuery.queryResult).begin();
			vector<resultRow>::const_iterator lenght_it_end = (genericQuery.queryResult).end();
			size_t buff;
			while (lenght_it != lenght_it_end)
			{
				for (size_t i=0; i<(*lenght_it).size(); i++)
				{
					if ( lenght_it == (genericQuery.queryResult).begin())
					{
						fieldsSize.push_back(0);
					}
					buff = ((*lenght_it)[i]).size();
					if ( fieldsSize[i] < buff ) fieldsSize[i] = buff;
				}
				lenght_it++;
			}
			size_t lineSize = 0;
			vector<size_t>::iterator fieldsSizeIt = fieldsSize.begin();
			vector<size_t>::iterator fieldsSizeIt_end = fieldsSize.end();
			while ( fieldsSizeIt != fieldsSizeIt_end )
			{
				lineSize+=*fieldsSizeIt;
				fieldsSizeIt++;
			}
			stringstream buf2;
			bool csv = false;
			if ( (inputStruct.queryTypeBuffer == "sql") )
			{
				vector<string>::const_iterator h_it= (genericQuery.fieldNames).begin();
				vector<string>::const_iterator h_it_end= (genericQuery.fieldNames).end();
				stringstream buf;
				buf2.fill('-');
				int i =0;
				while ( h_it != h_it_end )
				{
					if ( fieldsSize[i] < (*h_it).size() ) fieldsSize[i] = (*h_it).size();
					buf << left << setw(fieldsSize[i]) << *h_it << right << "|";
					buf2 << left << setw(fieldsSize[i]+1) << right << "+";
					h_it++;
					i++;
				}
				*output += "\n<lineString>+";
				*output += buf2.str();
				*output += "</lineString>\n";
				*output += "\n<lineString>|";
				*output += buf.str();
				*output += "</lineString>\n";
				*output += "\n<lineString>+";
				*output += buf2.str();
				*output += "</lineString>\n";
			}
			else
			{
				csv = true;
			}
			output->reserve( ((genericQuery.queryResult).size())*lineSize );
			vector<resultRow>::const_iterator it = (genericQuery.queryResult).begin();
			vector<resultRow>::const_iterator it_end = (genericQuery.queryResult).end();
			while ( it != it_end )
			{
				*output += "\n<lineString>|";
				vector<string>::const_iterator l_it=(*it).begin();
				vector<string>::const_iterator l_it_end=(*it).end();
				stringstream buf;
				int i =0;
				while ( l_it != l_it_end )
				{
					if ( csv )
					{
						buf << *l_it << "|";
					}
					else
					{
						buf << left << setw(fieldsSize[i]) << *l_it << right << "|";
					}
					l_it++;
					i++;
				}
				*output += buf.str();
				*output += "</lineString>\n";
				it++;
			}
			if ( !csv )
			{
				*output += "\n<lineString>+";
				*output += buf2.str();
				*output += "</lineString>\n";
			}
			*output += "</queryResult>\n";
			*output += "<STATUS>0</STATUS>";
			*output += "</BODY>\n";
			*output += "</HLR>\n";
			hlr_log("...done.", &logStream,8);
		}
		return 0;
	}
	hlr_log ("advancedQueryEngine: Exiting.", &logStream,4);	
	return exitCode;
}







