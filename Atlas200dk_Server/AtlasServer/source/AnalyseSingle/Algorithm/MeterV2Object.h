#ifndef METER_V2_OBJECT_H
#define METER_V2_OBJECT_H
#include "AnalyseBase/AnalyseObject.h"
#include "Algorithm/MeterV2Train.h"
#include "meterReg/HYAMR_meterReg.h"
#include "Algorithm/Main.h"

// 表记分析
class CMeterV2Object : public CAnalyseObject
{
public:
    CMeterV2Object(std::string &image_path);
    CMeterV2Object();
    ~CMeterV2Object();
public:
    void NotifyUser() override;
    // 实现CAnalyseObject类的虚函数；
    int invokeInit() override;
    int invokeDetect(const cv::Mat& image, RESULT_VECTOR& results) override;
    int invokeDetect(std::vector<Bbox_t> & result) override;
    int invokeUninit() override;
private:
    void Print(std::vector<Bbox> &bbox);
    void OutputTrainInfo(const MeterV2TrainInfo &info);
    void OutputInterParam(const HYAMR_INTERACTIVE_PARA &inPara);
private:
    MeterV2TrainInfo trainInfo;
    std::string error_msg;
    MVoid *pMem;
    MHandle hHYMRDHand;
    MHandle hMemMgr;

    std::vector<unsigned char> _curTargetVec;
    std::vector<unsigned char> _trnTargetVec;

    //yolov3相关配置
    std::shared_ptr<Atlasdk_process> m_atlas= nullptr;
    std::vector<string> label = {"background", "defect_product"};

    std::vector<Bbox> m_bbox;//post_process out
    Input_param input_params;
    hiai::EnginePortID engine_id;
    int m_model_weight = 416;
    int m_model_height = 416;
    std::string m_image_path="";
};
#endif
