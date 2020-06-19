#include "AnalyseObject.h"
#include "NetScheduleLib.h"
#include <stdio.h>

using namespace std;

////////////////////////////////////////////////////
/// 类CAnalyseObject实现
///
CAnalyseObject::CAnalyseObject() : CAnalyseBase(0)
{
    _bGPUEnable = false;
    _streamType = 0;
    _analyseNum = 1;
}

//bool CAnalyseObject::GetPictures()
//{
//    return true;
//}

string CAnalyseObject::dVal2Str(double dVal)
{
    char szVal[10] = {0};
    sprintf(szVal, "%.2f", dVal);
    return szVal;
}

string CAnalyseObject::dVal2Label(double dVal, double dConfidence)
{
    string stValue = dVal2Str(dVal);
    if (dConfidence > 0.000001)
    {
        char szVal[32] = {0};
        sprintf(szVal, "%s %.3f", stValue.c_str(), dConfidence);
        return szVal;
    }
    return stValue;
}

string CAnalyseObject::resVal2Str(const AnalyseResultInfo &resVal)
{
    string stValue = dVal2Str(resVal.dValue);
#if OUTPUT_CONFIDANCE_AND_COORDINATE
    char szVal[64] = {0};
    sprintf(szVal, "%s,%.3f,%d:%d:%d:%d", stValue.c_str(), resVal.dConfidence, resVal.left, resVal.top, resVal.right, resVal.bottom);
    return szVal;
#else
    return stValue;
#endif
}

void CAnalyseObject::DoWork()
{
    int iRet = 0;
    string filename;

    bool bRet = false;
    _bGPUEnable = (NetScheduleLib::GetParameter("GPUConfig","IsGPUStart", "false") == "true");
    LogError("DoWork(), IsGPUStart=%d", _bGPUEnable);
    // 初始化
    //真正处理数据的地方
    printf("*******************CAnalyseObject::DoWork()*****************!\n");
    iRet = invokeInit();
    if (0 != iRet) {
        NetScheduleLib::WriteLog(NetScheduleLib::mLogError,"Init Detect Interface error!!!");
        goto NOTIFY;
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

        _results.clear();
        m_yolov3_results.clear();
        // 检测
        iRet = invokeDetect(m_yolov3_results);
        if(0 != iRet)
        {
            NetScheduleLib::WriteLog(NetScheduleLib::mLogError,"Detect failed, return %d.", iRet);
        }
#ifdef HYL_PRINT_RECTANGLE
        else
        {
            filename = GetAnalyseFileMgtServerObj().GetFileName(_cameraId, _ptzIndex, _guideIndex, count++, "", _channelNo, _analysisType);
            NetScheduleLib::WriteLog(mLogDebug, "save filename %s.", filename.c_str());
            cvSaveImage(filename.c_str(), RectImg);
        }
#endif
        image.release();
    }

    NetScheduleLib::WriteLog(NetScheduleLib::mLogDebug,"%s:%d",__FUNCTION__,__LINE__);
    bRet = true;

NOTIFY:
    invokeUninit();

    if (!filename.empty())
        _picOut = filename;
    else if (_pictures.size() > 0)
        _picOut = _pictures.back();

    //通知用户
    if(this->_userFunc)
        this->_userFunc(this->shared_from_this());
}

bool CAnalyseObject::Init(const NotifyCallback &func, int analyseNum)
{
    _analyseNum = analyseNum;
    _userFunc = func;
}

