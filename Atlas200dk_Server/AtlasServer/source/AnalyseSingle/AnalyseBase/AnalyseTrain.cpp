#include "AnalyseTrain.h"
#include "NetScheduleLib.h"
#include "DataBaseInterface.h"


////////////////////////////////////////////////////
/// 类CAnalyseTrain实现
///

CAnalyseTrain::CAnalyseTrain() : CAnalyseBase(1)
{
}

void CAnalyseTrain::DoWork()
{
    // 数据预处理
    bool bRet = PreProcess();
    if (!bRet) {
        LogError("PreProcess failed!");
        return;
    }

    for(std::string& picture: _pictures)
    {
        NetScheduleLib::WriteLog(NetScheduleLib::mLogDebug,"%s:%d, load image [%s].", __FUNCTION__, __LINE__, picture.c_str());
        cv::Mat image = cv::imread(picture.c_str());
        if (nullptr == image.data)
        {
            NetScheduleLib::WriteLog(NetScheduleLib::mLogError, "load image failed!!!");
            image.release();
            continue;
        }
        // 处理结果保存到数据库
        bRet = Train(image);
        if (!bRet) {
            NetScheduleLib::WriteLog(NetScheduleLib::mLogError,"Train failed, return %d.", bRet);
        } else {
            bRet = Add2DataBase();
        }
        image.release();
        // 只处理一张图
        break;
    }

    NetScheduleLib::WriteLog(NetScheduleLib::mLogDebug,"%s:%d",__FUNCTION__,__LINE__);

//    //通知用户
//    if(this->_userFunc)
//        this->_userFunc(this->shared_from_this());
}
