/*
 * @Description  : 
 * @Version      : 1.0.0
 * @Author       : mdf
 * @Date         : 2020-05-08 14:52:01
 * @LastEditors  : mdf
 * @LastEditTime : 2020-05-09 11:47:20
 */
#ifndef NetScheduleLib_H
#define NetScheduleLib_H
#include <string.h>
#include <string>
#include <vector>

#define LOG4CPP_CATEGORY_NAME  "logCat"

namespace dk
{
    void InitializeLog4cpp();
    void ReleaseLog4cpp();
}

//通讯和服务调度库
class NetScheduleLib
{
	
public:
	//该库作为服务端或客户端类型
	enum LibType
	{
		mClient=0,
		mServer=1,
	};
	enum LogType
	{
		mLogNotice=02,	//通知信息
		mLogDebug=04,	//调试信息
		mLogInfo =010,	//打印信息
		mLogWarning=040,//警告信息
		mLogError  =0200,//错误信息
	};
	/************************************************************************/
	/*    
	初始化服务，程序刚启动的时候调用
	*/
	/************************************************************************/	
	static void Init(LibType usetype = mServer);

	/************************************************************************/
	/* 
	退出程序时要调用该函数，该函数会阻塞，直到所有的线程和资源清理完毕
	*/
	/************************************************************************/
	static void UInit();

	static int Wait();

	//写日志
	static void WriteLog(LogType logtype,const char * format,...);

	//得到配置参数
	//servername和key 不区分大小写,servername可为""
	static std::string GetParameter(const std::string &setkey, const std::string &stSubKey, const std::string &stDefValue);

	//
	static std::vector<std::string> GetAllIp();

};

#define FileNameLmt(x)      strrchr(x,'/')?strrchr(x,'/')+1:x
#define MakePrefix(fmt)     std::string(FileNameLmt(__FILE__)).append("::").append(__FUNCTION__).append("() - ").append(fmt).c_str()
#define LogDebug(fmt, ...)  NetScheduleLib::WriteLog(NetScheduleLib::mLogDebug, MakePrefix(fmt), ##__VA_ARGS__)
#define LogInfo(fmt, ...)   NetScheduleLib::WriteLog(NetScheduleLib::mLogInfo, MakePrefix(fmt), ##__VA_ARGS__)
#define LogNotice(fmt, ...) NetScheduleLib::WriteLog(NetScheduleLib::mLogNotice, MakePrefix(fmt), ##__VA_ARGS__)
#define LogWarning(fmt, ...)  NetScheduleLib::WriteLog(NetScheduleLib::mLogWarning, MakePrefix(fmt), ##__VA_ARGS__)
#define LogError(fmt, ...)  NetScheduleLib::WriteLog(NetScheduleLib::mLogError, MakePrefix(fmt), ##__VA_ARGS__)

#endif // IScheduleServerw_H
