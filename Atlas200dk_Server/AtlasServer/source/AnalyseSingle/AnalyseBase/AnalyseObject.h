#ifndef ANALYSE_OBJECT_H
#define ANALYSE_OBJECT_H
#include "AnalyseCommon.h"
#include <opencv2/opencv.hpp>
#include "../Algorithm/data_type.h"
// 分析——目标检测基类
class CAnalyseObject : public std::enable_shared_from_this<CAnalyseObject>, public CAnalyseBase
{
public:
    CAnalyseObject();
public:
    // 实现CTask类的DoWork；
    void DoWork() override;
    bool Init(const NotifyCallback& func, int analyseNum = 1);
    //virtual bool GetPictures();
    virtual void NotifyUser() = 0;
protected:
    virtual int invokeInit() {return 0;}
    virtual int invokeDetect(const cv::Mat& imag, RESULT_VECTOR& results) {return 0;}
    virtual int invokeDetect(std::vector<Bbox_t> &result){return 0; };
    virtual int invokeUninit() {return 0;}
    virtual std::string dVal2Str(double dVal);
    virtual std::string dVal2Label(double dVal, double dConfidence=0.0F);
    virtual std::string resVal2Str(const AnalyseResultInfo &resVal);
public:
    bool _bGPUEnable;
    int _streamType;    //码流类型
    int _analyseNum;	//分析次数
    std::vector<std::string> _pictures;	//图片
    std::string	_dirPath;       //增加一级目录名称
    std::string _picOut;
    RESULT_VECTOR _results;     //分析结果
    std::vector<Bbox_t> m_yolov3_results;
private:
    NotifyCallback _userFunc;
};
#endif
