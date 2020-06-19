#include "SingleObjectManager.h"
#include "AnalyseSingleService.h"
#include "NetScheduleLib.h"

AnalyseSingleObjectManager::AnalyseSingleObjectManager()
{
    int threadNum = 5;
	int analyseThreadNum = 5;
	try
	{
        threadNum = std::stoi(NetScheduleLib::GetParameter("AnalyseSingle","CapThreadNum", "5"));
        analyseThreadNum = std::stoi(NetScheduleLib::GetParameter("AnalyseSingle","AnalyseThreadNum", "5"));
	}
	catch (...)
	{
        NetScheduleLib::WriteLog(NetScheduleLib::mLogError, "get threadNum or analyseThreadNum exception");
	}
    //配置抓图线程


	//配置分析线程数
	_analyseThreadNum = analyseThreadNum;
    _analyseThdPool.start(_analyseThreadNum);

    //重试线程
    //std::thread(&AnalyseSingleObjectManager::ReTryThread, this);
}

AnalyseSingleObjectManager::~AnalyseSingleObjectManager()
{
    Stop();
    LogDebug("~AnalyseSingleObjectManager");
}

AnalyseSingleObjectManager *AnalyseSingleObjectManager::GetInstance()
{
    static AnalyseSingleObjectManager info;
    return &info;
}

bool AnalyseSingleObjectManager::Start(AnalyseTaskPtr task)
{
    _analyseThdPool.addTask(task);
    return true;
}

void AnalyseSingleObjectManager::Stop()
{
    _analyseThdPool.stop();
}

//void AnalyseSingleObjectManager::CaptureThread()
//{
//}

//void AnalyseSingleObjectManager::ReTryThread()
//{
//}

//void AnalyseSingleObjectManager::ProcessFinish(AnalyseSingleIdPtr ptr)
//{
//	//NotifyUser(ptr);
//	this->_threadpool.schedule(boost::bind(&AnalyseSingleObjectManager::NotifyUser,this,ptr));
//}

//void AnalyseSingleObjectManager::NotifyUser(AnalyseSingleIdPtr obj)
//{
//	if (this->_userFunc)
//		this->_userFunc(obj);
//}
