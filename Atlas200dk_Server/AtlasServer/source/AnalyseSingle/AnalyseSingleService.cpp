#include "AnalyseSingleService.h"
#include "SingleObjectManager.h"
#include "AnalyseBase/AnalyseCommon.h"
#include "Algorithm/MeterV2Train.h"
#include "DataBaseInterface.h"
#include "NetScheduleLib.h"

AnalyseSingleServer *AnalyseSingleServer::GetInstance()
{
    static AnalyseSingleServer info;
    return &info;
}

///
/// \brief AnalyseSingleServer::Start
///
void AnalyseSingleServer::Start()
{
    std::string error_msg;
    bool bRet = DataBase::GetInstance().Init(error_msg);
    if (!bRet) {
        LogError("DataBase error: [%s]", error_msg.c_str());
    }
}

bool AnalyseSingleServer::ProcessAnalyseSingle(AnalyseBasePtr task)
{
    if (!task)
        return false;

    return IAnalyseSingleObjectManager::GetInstance()->Start(task);
}

bool AnalyseSingleServer::ProcessTrainingSingle(AnalyseBasePtr task)
{
    if (!task)
        return false;

    task->DoWork();
    return 0;
}
