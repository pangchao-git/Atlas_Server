#ifndef ANALYSE_OBJECT_MANAGER_H
#define ANALYSE_OBJECT_MANAGER_H
#include "ThreadPool/ThreadPool.h"

typedef TaskPtr AnalyseTaskPtr;

class AnalyseSingleObjectManager
{
private:
    AnalyseSingleObjectManager();
    AnalyseSingleObjectManager(const AnalyseSingleObjectManager &) = delete;
    AnalyseSingleObjectManager& operator=(const AnalyseSingleObjectManager &) = delete;
    ~AnalyseSingleObjectManager();
public:
    static AnalyseSingleObjectManager *GetInstance();
public:
    bool Start(AnalyseTaskPtr info);
	void Stop();
private:
    int _analyseThreadNum;      //分析线程数配置
    ThreadPool _analyseThdPool; //分析线程池

    //int _captureThreadNum;      //抓图线程数配置
    //ThreadPool _captureThdPool;
    bool _isRunning;		//开始状态
};

typedef AnalyseSingleObjectManager IAnalyseSingleObjectManager;

#endif // ANALYSE_OBJECT_MANAGER_H
