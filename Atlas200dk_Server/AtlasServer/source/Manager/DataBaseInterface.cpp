/*
@author		fyu
@date		2015/12/23
@function	数据库操作的统一接口，sqlite不支持并发操作，使用锁同步操作
*/

#include "DataBaseInterface.h"
#include "NetScheduleLib.h"
#include "sqlite/sqlite3.h"
#include "sqlite/sqliteHelpper.h"
#include <algorithm>
#include <boost/format.hpp>
#include <boost/filesystem.hpp>
#include <boost/date_time.hpp>
#include "Algorithm/MeterV2Train.h"

using namespace std; 
namespace fs = boost::filesystem;

const char *analyse_db_name = "analyse.db";

DataBase& GetDataBaseObj()
{
    return DataBase::GetInstance();
}

DataBase::DataBase() : conn_(NULL)
{
	_periodDays = 0;
	_rate = 0.0f;

	try
	{
        _dbDirPath = NetScheduleLib::GetParameter("DbServer", "path", "AnalyseDB");
	}
	catch (...)
	{
        NetScheduleLib::WriteLog(NetScheduleLib::mLogError, "get dbPath exception");
	}

    if (!_dbDirPath.empty())
	{
        if (*_dbDirPath.rbegin() != '/')
		{
            _dbDirPath.append("/");
		}
	}
	else
	{
        _dbDirPath = "./db/";
	}
    _dbFilePath = _dbDirPath;
    _dbFilePath.append(analyse_db_name);
}

DataBase::~DataBase()
{
    unique_lock<recursive_mutex> lk(mutex_);
    SAFE_CLOSE(conn_);
}

DataBase &DataBase::GetInstance()
{
    static DataBase info;
    return info;
}

bool DataBase::OpenSqliteDataBase(const std::string &dbFilePath, std::string& error_msg)
{
    // check database is open
    if (conn_)
        return true;

    // db file name
    string stDBFileName;
	try{
        fs::path db_system_path = fs::system_complete(dbFilePath);
        fs::path db_branch_path = db_system_path.branch_path();
        if (!fs::exists(db_branch_path)){
            fs::create_directories(db_branch_path);
		}
        stDBFileName = db_system_path.string();
	}
    catch(std::exception& e) {
		error_msg = e.what();
		return false;
	}

	//打开数据库
    int result = SQLITE_ERROR;
    try {
        result = sqlite3_open(stDBFileName.c_str(),  &conn_);
    }
    catch(std::exception& e) {
        error_msg = e.what();
        return false;
    }

	if (result != SQLITE_OK) {
		SAFE_CLOSE(conn_);
		error_msg = "[DataBase] OpenSqliteDataBase failed";
		return false;
	}
	return true;
}

bool DataBase::Init(std::string& error_msg)
{
    _periodDays = std::stoi(NetScheduleLib::GetParameter("AlarmConfig","PeriodDays", "7"));
    _rate = std::stof(NetScheduleLib::GetParameter("AlarmConfig","Rate", "0.5"));
    cout << "_dbFile: " << _dbFilePath << endl;
    if(!OpenSqliteDataBase(_dbFilePath, error_msg))
	{
		return false;
	}

	if(!CreateTablesIfNotExists(error_msg))
	{
		return false;
	}
    return true;
}

bool DataBase::InsertMeterV2TrainInfo(const string &stElemDevId, MeterV2TrainInfo &info, string &error_msg)
{
    boost::format fmt = boost::format("INSERT INTO MeterV2Train VALUES(NULL,'%s',%s,%d,%s,%s,%s,%s,%d,%s,%s,%s,%s);");
    fmt % stElemDevId.c_str()
        % Object2SqliteText<std::vector<MeterV2LineInfo> >(info._lines).c_str()
        % info._backGround
        % Object2SqliteText<std::vector<AnalyseSinglePoint> >(info._points).c_str()
        % Object2SqliteText<AnalyseSinglePoint2D>(info._circleCenter).c_str()
        % Object2SqliteText<std::vector<AnalyseSinglePoint2D> >(info._analyseArea).c_str()
        % Object2SqliteText<std::vector<AnalyseSinglePoint2D> >(info._maskArea).c_str()
        % info._blur
        % ConvertStdStringToSqliteText(info._desPath).c_str()
        % ConvertStdStringToSqliteText(info._curTarget).c_str()
        % ConvertStdStringToSqliteText(info._trainTarget).c_str()
        % Object2SqliteText<AnalyseSize>(info._size).c_str();

    unique_lock<recursive_mutex> lk(mutex_);
    if (!DO_SQL_CMD(fmt.str().c_str()))
    {
        error_msg = "[DataBase] InsertMeterV2TrainInfo failed";
        return false;
    }

    return true;
}

bool DataBase::UpdateMeterV2TrainInfo(const string &stElemDevId, MeterV2TrainInfo &info, string &error_msg)
{
    bool bRet = DeleteMeterV2TrainInfo(stElemDevId, error_msg);
    if (bRet) {
        bRet = InsertMeterV2TrainInfo(stElemDevId, info, error_msg);
    }
    return bRet;
}

bool DataBase::QueryMeterV2TrainInfo(const string &stElemDevId, MeterV2TrainInfo &info, string &error_msg)
{
    bool ret = false;
    sqlite3_stmt* stmt = NULL;
    //
    boost::format fmt = boost::format("SELECT * FROM MeterV2Train where eleDevId='%s';");
    fmt % stElemDevId.c_str();

    if (sqlite3_prepare_v2(conn_,fmt.str().c_str(),fmt.str().size(),&stmt,NULL) != SQLITE_OK)
    {
        SAFE_FINALIZE(stmt);
        error_msg = "[DataBase] QueryRealTimeRecordsWhichCompletedInPeriod failed";
        return false;
    }
    do
    {
        int r = sqlite3_step(stmt);
        if (r == SQLITE_ROW)
        {
            int i=1;
            ret = true;
            info._eleDevId = (std::string)(const char*)sqlite3_column_text(stmt,i++);
            Str2Object<vector<MeterV2LineInfo> >((std::string)(const char*)sqlite3_column_text(stmt,i++), info._lines);
            info._backGround = MeterColour(sqlite3_column_int(stmt,i++));
            Str2Object<vector<AnalyseSinglePoint> >((std::string)(const char*)sqlite3_column_text(stmt,i++), info._points);
            Str2Object<AnalyseSinglePoint2D>((std::string)(const char*)sqlite3_column_text(stmt,i++), info._circleCenter);
            Str2Object<vector<AnalyseSinglePoint2D> >((std::string)(const char*)sqlite3_column_text(stmt,i++), info._analyseArea);
            Str2Object<vector<AnalyseSinglePoint2D> >((std::string)(const char*)sqlite3_column_text(stmt,i++), info._maskArea);
            info._blur = MeterBlurLevel(sqlite3_column_int(stmt,i++));
            info._desPath = (string)(const char*)sqlite3_column_text(stmt,i++);
            info._curTarget = (string)(const char*)sqlite3_column_text(stmt,i++);
            info._trainTarget = (string)(const char*)sqlite3_column_text(stmt,i++);
            Str2Object<AnalyseSize>((string)(const char*)sqlite3_column_text(stmt,i++), info._size);
            info._dbFlag = 1;
        }
        else if (r == SQLITE_DONE)
        {
            break;
        }
        else
        {
            break;
        }
    } while (true);
    SAFE_FINALIZE(stmt);

    return ret;
}

bool DataBase::DeleteMeterV2TrainInfo(const string &stElemDevId, string &error_msg)
{
    boost::format fmt = boost::format("DELETE FROM MeterV2Train where eleDevId='%s';");
    fmt % stElemDevId.c_str();

    unique_lock<recursive_mutex> lk(mutex_);
    if (!DO_SQL_CMD(fmt.str().c_str()))
    {
        error_msg = "[DataBase] DeleteMeterV2TrainInfo failed";
        return false;
    }

    return true;
}

bool DataBase::CreateTablesIfNotExists(std::string& error_msg)
{
    string createSQL;
    //////////////////////////////MeterV2Train////////////////////////////////////////////
    // 表计配置参数表
    createSQL =
        "CREATE TABLE IF NOT EXISTS	MeterV2Train \
        (id	INTEGER PRIMARY KEY	AUTOINCREMENT \
        ,eleDevId	TEXT \
        ,lineInfo	TEXT \
        ,backColour	INT \
        ,points	TEXT \
        ,circleCenter	TEXT \
        ,analyseArea TEXT \
        ,maskArea TEXT \
        ,blur	INT \
        ,desPath TEXT \
        ,curTarget TEXT \
        ,trainTarget TEXT \
        ,resolution TEXT \
        )";

	if (!DO_SQL_CMD(createSQL.c_str()))
	{
        error_msg = "[DataBase] MeterV2Train table failed";
		return false;
	}

    // 表计V3(细指针)配置参数表
    createSQL =
        "CREATE TABLE IF NOT EXISTS	MeterV3Train \
        (id	INTEGER PRIMARY KEY	AUTOINCREMENT \
        ,eleDevId	TEXT \
        ,points	TEXT \
        ,circleCenter	TEXT \
        ,outerRadius	INT \
        ,innerRadius	INT \
        ,startAngle	INT \
        ,endAngle	INT \
        ,resolution TEXT \
        )";

    if (!DO_SQL_CMD(createSQL.c_str()))
    {
        error_msg = "[DataBase] MeterV3Train table failed";
        return false;
    }

    // 日志表
    createSQL =
        "CREATE TABLE IF NOT EXISTS	LogData \
        (id	INTEGER PRIMARY KEY	AUTOINCREMENT \
        ,major INTEGER \
        ,minor INTEGER \
        ,level INTEGER \
        ,logTime	DATETIME \
        ,logInfo	TEXT \
        )";

    if (!DO_SQL_CMD(createSQL.c_str()))
    {
        error_msg = "[DataBase] MeterV3Train table failed";
        return false;
    }

	return true;
}

bool DataBase::ExistInMeterV3Table(const string &stElemDevId)
{
    bool ret = false;
    unique_lock<recursive_mutex> lk(mutex_);

    sqlite3_stmt* stmt = NULL;
    if (stElemDevId.empty())
        return false;

    boost::format fmt = boost::format("SELECT 1 FROM MeterV3Train where eleDevId='%s' limit 1;");
    fmt % stElemDevId.c_str();
    //
    if (SQLITE_OK == sqlite3_prepare_v2(conn_,fmt.str().c_str(),fmt.str().size(),&stmt,NULL))
    {
        do
        {
            ret = (SQLITE_ROW == sqlite3_step(stmt));
            break;
        } while (true);
    }
    //
    SAFE_FINALIZE(stmt);
    return ret;
}

bool DataBase::InsertLogInfo(int majorid, int minorid, int level, const string &logInfo)
{
    string error_msg;
    boost::format fmt = boost::format("INSERT INTO LogData VALUES(NULL,%d,%d,%d,datetime('now','localtime'),%s);");
    fmt % majorid
        % minorid
        % level
        % logInfo.c_str();

    unique_lock<recursive_mutex> lk(mutex_);
    if (!DO_SQL_CMD(fmt.str().c_str()))
    {
        error_msg = "[DataBase] AddLogInfo failed";
        return false;
    }

    return true;
}

bool DataBase::QueryLogInfo(int majorid, int minorid, const string &startTime, const string &endTime, int offset, int maxSize, LOG_RESULTS &results, int &hasNext)
{
    string error_msg;
    sqlite3_stmt* stmt = NULL;
    //
    boost::format fmt = boost::format("SELECT id, level, strftime('%Y-%m-%d %H:%M:%S', logTime) as logTime, logInfo FROM LogData \
                                      where majorid=%d and minorid=%d and logTime >= datetime(%s) and logTime < datetime(%s) LIMIT %d OFFSET %d;");
    fmt % majorid
        % minorid
        % startTime.c_str()
        % endTime.c_str()
#if 0
        % (maxSize + 1) // 这里加1是为了判断是否取到最后一条数据
#else
        % maxSize
#endif
        % (offset * maxSize);

    results.clear();
    if (sqlite3_prepare_v2(conn_,fmt.str().c_str(),fmt.str().size(),&stmt,NULL) != SQLITE_OK)
    {
        SAFE_FINALIZE(stmt);
        error_msg = "[DataBase] QueryRealTimeRecordsWhichCompletedInPeriod failed";
        return false;
    }
    do
    {
        int r = sqlite3_step(stmt);
        if (r == SQLITE_ROW)
        {
            LOG_INFO info;
            info.major = majorid;
            info.minor = minorid;
            info.id = sqlite3_column_int(stmt, 0);
            info.level = sqlite3_column_int(stmt, 1);
            info.logTime = (string)(const char*)sqlite3_column_text(stmt, 2);
            info.logInfo = (string)(const char*)sqlite3_column_text(stmt, 3);
            results.push_back(info);
        }
        else if (r == SQLITE_DONE)
        {
            break;
        }
        else
        {
            break;
        }
    } while (true);
    SAFE_FINALIZE(stmt);

    // 获取日志条目数量
#if 0
    hasNext = results.size() >= maxSize?0:1;
    results.pop_back();
#else
    int totalSize = GetLogItemCount();
    hasNext = (results.size() + offset * maxSize >= totalSize)?0:1;
#endif
    return (!results.empty());
}

int DataBase::GetLogItemCount()
{
    int iRet = 0;
    string error_msg;
    string stSql = "select count(id) as count FROM LogData;";
#if 0
    char *szErrMsg = NULL;
    char **pResult;
    int nRow = 0;
    int nCol = 0;
    int ret = sqlite3_get_table(conn_, stSql.c_str(), &pResult, &nRow, &nCol, &szErrMsg);
    if (ret != SQLITE_OK) {
        error_msg = szErrMsg?szErrMsg"Query LogData total size failed!";
    } else if (nRow > 0 && nCol > 0){
        iRet = std::stoi(pResult[1][0]);
    }

    if (szErrMsg) {
        sqlite3_free(szErrMsg);
    }
    sqlite3_free_table(pResult);
#else
    sqlite3_stmt* stmt = NULL;
    int ret = sqlite3_prepare_v2(conn_, stSql.c_str(), stSql.size(), &stmt, NULL);
    if (ret != SQLITE_OK) {
        error_msg = "[DataBase] QueryRealTimeRecordsWhichCompletedInPeriod failed";
   } else {
        int r = sqlite3_step(stmt);
        if (r == SQLITE_ROW) {
            iRet = sqlite3_column_int(stmt, 0);
        }
    }
    SAFE_FINALIZE(stmt);
#endif
    return iRet;
}

