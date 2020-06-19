#include "sqliteHelpper.h"
#include <sstream>
#include "sqlite3.h"
#include "NetScheduleLib.h"
using namespace std;


void SAFE_CLOSE(sqlite3* conn)
{
	if(conn) {
		sqlite3_close(conn); 
	}
	conn = NULL;
}

bool DoSqlCommandNonReturn(sqlite3* conn, const char *sql)
{
	//日志
	stringstream ss;
	ss	<<"[DataBase]DataBase Operation:\r\n"
		<<sql
		<<"\r\n";

	char* errmsg = NULL;
	if (sqlite3_exec(conn, sql, NULL, NULL, &errmsg )){		
		//日志
		ss	<<"DataBase Result: failed\r\n"
			<<"Error Message:\t";
		if (errmsg == NULL)
		{
			ss << errmsg;
		}
		ss <<"\r\n";
        LogError(ss.str());
		sqlite3_free(errmsg);
		return false;
	}
	//日志
    ss << "DataBase Result: success\r\n";
    LogDebug(ss.str());
	sqlite3_free(errmsg);
	return true;
}

bool DoSqlCommandANSIToUTF8(sqlite3* conn, std::string sql)
{
    std::string utf = sql/*ANSIToUTF8(sql)*/;
	return DoSqlCommandNonReturn(conn, utf.c_str());
}

void SAFE_FINALIZE( sqlite3_stmt* stmt )
{
	if(stmt) {
		sqlite3_finalize(stmt); 
	}
	stmt = NULL;
}

bool BeginTransaction(sqlite3* conn)
{
	/// 日志
	stringstream ss;
	char* errmsg = NULL;
	int result = sqlite3_exec ( conn , "begin transaction" , 0 , 0 , & errmsg );
	if(result != SQLITE_OK ){
		ss<< "[DataBase]begin transaction failed, errormsg: "<<errmsg;
        LogError(ss.str());
		sqlite3_free(errmsg);
		return false;
	} else{
        LogDebug("[DataBase]begin transaction success");
		sqlite3_free(errmsg);
		return true;
	}
}

bool EndTransaction( sqlite3* conn,bool sqlResult )
{
	if (sqlResult)
	{
		/// 日志
		stringstream ss;
		char* errmsg = NULL;
		int result = sqlite3_exec ( conn , "commit transaction" , 0 , 0 , & errmsg );
		if(result != SQLITE_OK ){
			ss<< "[DataBase]commit transaction failed, errormsg: "<<errmsg;
            LogError(ss.str());
			sqlite3_free(errmsg);
			return false;
		} else{
            LogDebug("[DataBase]commit transaction success");
			sqlite3_free(errmsg);
			return true;
		}
	}
	else
	{
		/// 日志
		stringstream ss;
		char* errmsg = NULL;
		int result = sqlite3_exec ( conn , "rollback transaction" , 0 , 0 , & errmsg );
		if(result != SQLITE_OK ){
			ss<< "rollback transaction failed, errormsg: "<<errmsg;
            LogError(ss.str());
			sqlite3_free(errmsg);
			return false;
		} else{
            LogDebug("rollback transaction success");
			sqlite3_free(errmsg);
			return true;
		}
	}
}
