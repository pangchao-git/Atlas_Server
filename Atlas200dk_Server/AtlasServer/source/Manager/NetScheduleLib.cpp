/*
 * @Description  : NetScheduleLib
 * @Version      : 1.0.0
 * @Author       : mdf
 * @Date         : 2020-05-08 14:07:10
 * @LastEditors  : mdf
 * @LastEditTime : 2020-05-11 16:33:11
 */
#include "NetScheduleLib.h"
#include "GlobalDataManager.h"
#include "AnalyseSingleService.h"
#include <log4cpp/Category.hh>
#include <log4cpp/Appender.hh>
#include <log4cpp/Priority.hh>
#include <log4cpp/PatternLayout.hh>
#include <log4cpp/RollingFileAppender.hh>
#define _DEBUG
#ifdef _DEBUG
#include <log4cpp/OstreamAppender.hh>
#endif
#include <sstream>
#include <mutex>
#include <stdarg.h>
#include <unistd.h>

namespace dk
{

void InitializeLog4cpp(const std::string& fileName, int iFleSize)
{
    log4cpp::Category& logCat = log4cpp::Category::getInstance(LOG4CPP_CATEGORY_NAME);
    logCat.setAdditivity(false);
    //
    log4cpp::PatternLayout* pRFLayout = new log4cpp::PatternLayout();
    pRFLayout->setConversionPattern("%d: %p %c%x: %m%n");

    log4cpp::RollingFileAppender* rollfileAppender = new log4cpp::RollingFileAppender("rollfileAppender",
                                                                                      fileName, iFleSize*1024*1024, 1);
    rollfileAppender->setLayout(pRFLayout);
    logCat.addAppender(rollfileAppender);

#ifdef _DEBUG
    log4cpp::PatternLayout* pOSLayout = new log4cpp::PatternLayout();
    pOSLayout->setConversionPattern("%d: %p %c%x: %m%n");

    log4cpp::OstreamAppender* osAppender = new log4cpp::OstreamAppender("osAppender", &std::cout);
    osAppender->setLayout(pOSLayout);
    logCat.addAppender(osAppender);
#endif

    logCat.setPriority(log4cpp::Priority::DEBUG);
}

void ReleaseLog4cpp()
{
    log4cpp::Category::shutdown();
}

}

void NetScheduleLib::Init(LibType usetype)
{
    // 读取配置文件
	theDataManager->ReadFromXml("config.xml");
	//
	std::string fileName = theDataManager->GetParameter("LogConfig", "FileName", "server.log");
    std::string fileSize = theDataManager->GetParameter("LogConfig", "FileSize", "10");
    int iFleSize = std::stoi(fileSize);
    iFleSize = std::min(100, std::max(10, iFleSize));
    // 初始化日志库
    dk::InitializeLog4cpp(fileName, iFleSize);
    // 启动分析服务
    IAnalyseSingleServer::GetInstance()->Start();
    // 依赖目录创建
	std::string rootDir = theDataManager->GetParameter("AnalyseHttp", "root", "Image");
	if (0 != access(rootDir.c_str(), 0))
    {
        // if this folder not exist, create a new one.
        mkdir(rootDir.c_str(), 0755);   // 返回 0 表示创建成功，-1 表示失败
    }
}

void NetScheduleLib::UInit()
{
    dk::ReleaseLog4cpp();

}

int NetScheduleLib::Wait()
{
	//等待所有的线程结束
	return 0;
}

std::mutex g_logmutex;
void NetScheduleLib::WriteLog(LogType logtype,const char * format, ...)
{
    char szText[1025] = {0};
    va_list argp;
    va_start (argp, format);
    vsnprintf(szText, 1024, format, argp);
    va_end (argp);
	std::lock_guard<std::mutex> lock(g_logmutex);
    log4cpp::Category& logCat = log4cpp::Category::getInstance(LOG4CPP_CATEGORY_NAME);
	switch (logtype)
	{
	case mLogDebug:
        logCat.debug(szText);
		break;
	case mLogError:
        logCat.error(szText);
		break;
	case mLogWarning:
        logCat.warn(szText);
		break;
	case mLogInfo:
        logCat.info(szText);
		break;
	case mLogNotice:
        logCat.notice(szText);
		break;
	}
}

std::string NetScheduleLib::GetParameter(const std::string &setkey, const std::string &stSubKey, const std::string &stDefValue)
{
	return theDataManager->GetParameter(setkey, stSubKey, stDefValue);
}
