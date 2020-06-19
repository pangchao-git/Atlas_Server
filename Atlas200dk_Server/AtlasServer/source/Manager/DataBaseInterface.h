#ifndef DATA_BASE_INTERFACE_H
#define DATA_BASE_INTERFACE_H
#include "sqlite/StringConvert.h"
#include <boost/serialization/vector.hpp>
#include <boost/serialization/map.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <map>
#include <set>
#include <string>
#include <sstream>
#include <mutex>


typedef struct sqlite3 sqlite3;

typedef struct _log_info {
    int id;
    int major;
    int minor;
    int level;
    std::string logTime;
    std::string logInfo;
} LOG_INFO;

typedef std::vector<LOG_INFO> LOG_RESULTS;

class MeterV2TrainInfo;
/// 
class DataBase
{
public:
	enum DataBaseOper
	{
		DataBase_Add=0,
		DataBase_Delete,
		DataBase_Query,
		DataBase_Update,
		DataBase_UpdateResult,
	};

public:
	DataBase();
	~DataBase();
public:
    static DataBase &GetInstance();
public:
    bool Init(std::string& error_msg);
    bool InsertMeterV2TrainInfo(const std::string &stElemDevId, MeterV2TrainInfo &info, std::string &error_msg);
    bool UpdateMeterV2TrainInfo(const std::string &stElemDevId, MeterV2TrainInfo &info, std::string &error_msg);
    bool QueryMeterV2TrainInfo(const std::string &stElemDevId, MeterV2TrainInfo &info, std::string &error_msg);
    bool DeleteMeterV2TrainInfo(const std::string &stElemDevId, std::string &error_msg);
    bool ExistInMeterV3Table(const std::string &stElemDevId);

    bool InsertLogInfo(int majorid, int minorid, int level, const std::string &logInfo);
    bool QueryLogInfo(int majorid, int minorid, const std::string &startTime, const std::string &endTime,
                                    int offset, int maxSize, LOG_RESULTS &results, int &hasNext);
    int GetLogItemCount();
private:
    /// 打开数据库
    bool OpenSqliteDataBase(const std::string &dbFilePath, std::string& error_msg);
	/// 如果表不存在，创建所有表
	bool CreateTablesIfNotExists(std::string& error_msg);
public:
    std::string _dbDirPath;
private:
    std::string _dbFilePath;
    //std::mutex mutex_;        //互斥锁条件变量
    std::recursive_mutex mutex_;
    sqlite3 *conn_;				/// 数据库的指针

	int _periodDays;
	float _rate;
};

DataBase& GetDataBaseObj();

template<class T>
bool Str2Object(std::string str, T& obj)
{
	if (str.empty())
	{
		return false;
	}	
    std::istringstream is(str);
	boost::archive::text_iarchive ia(is);
	ia & BOOST_SERIALIZATION_NVP(obj);
	return true;
}
template<class T>
std::string Object2Str(T& obj)
{
	std::string ret;
	std::ostringstream os;  
	boost::archive::text_oarchive oa(os);
	oa & BOOST_SERIALIZATION_NVP(obj); 
    ret = os.str();

	return ret;
}
template<class T>
std::string Object2SqliteText(T& obj)
{
	return ConvertStdStringToSqliteText(Object2Str<T>(obj)); 
}

#endif
