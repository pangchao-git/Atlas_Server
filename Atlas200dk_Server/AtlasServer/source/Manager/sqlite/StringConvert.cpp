/*
@author		fyu
@date		2015/12/26
@function	sqlite字符串，时间字符串的转换函数
*/

#include "StringConvert.h"
#include "NetScheduleLib.h"
#include <boost/format.hpp>
#include <boost/date_time.hpp>
#include <iostream>
#include <sstream>

using namespace std;
/// 在字符串前后加单引号
/// 应该注意对数据输入输出的字符检查是否位空串
std::string ConvertStdStringToSqliteText(std::string std_string)
{
	std::string sqllite_text = std_string;
	sqllite_text.insert(sqllite_text.begin(), 1, '\'');
	sqllite_text.insert(sqllite_text.end(), 1, '\'');
	return sqllite_text;
}

//iso_string 20150810T103030
//sqllite_datetime 2015-08-10 10:30:30
std::string ConverIsoTimeStringToSqliteTimeString(std::string iso_string)
{
	std::string ret;
	try
	{
		ret = boost::posix_time::to_iso_extended_string(boost::posix_time::from_iso_string(iso_string));
		ret[10] = ' ';
	}
	catch (...)
	{
		boost::format fmt("ConverIsoTimeStringToSqliteTimeString %s failed");
		fmt % iso_string.c_str();
        LogError(fmt.str());
		ret.clear();
	}	
	return ConvertStdStringToSqliteText(ret);
}

//iso_string 20150810T103030
//sqllite_datetime 2015-08-10 10:30:30
std::string ConverSqliteTimeStringToIsoTimeString(std::string sqllite_datetime)
{

	std::string ret;
	try
	{
		ret = boost::posix_time::to_iso_string(boost::posix_time::time_from_string(sqllite_datetime));
	}
	catch (...)
	{
		boost::format fmt("ConverSqliteTimeStringToIsoTimeString %s failed");
		fmt % sqllite_datetime.c_str();
        LogError(fmt.str());
		ret.clear();
	}
	return ret;
}
