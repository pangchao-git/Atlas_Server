#ifndef ANALYSE_TRAIN_H
#define ANALYSE_TRAIN_H
#include "AnalyseCommon.h"
#include <opencv2/opencv.hpp>

// 训练——算法配置基类
class CAnalyseTrain : public CAnalyseBase
{
public:
    CAnalyseTrain();
public:
    // 实现CTask类的DoWork；
    void DoWork() override;
protected:
    virtual bool PreProcess() { return true; }
    virtual bool Train(const cv::Mat &imag) { return true; }
    virtual bool Add2DataBase() { return true; }
    //virtual void NotifyUser();
public:
    std::vector<std::string> _pictures;	//图片
    std::string	_dirPath;   // 增加一级目录名称
};

#endif
