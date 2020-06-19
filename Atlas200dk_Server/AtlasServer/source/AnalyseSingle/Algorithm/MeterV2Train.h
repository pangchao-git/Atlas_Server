#ifndef METER_V2_TRAIN_H
#define METER_V2_TRAIN_H
#include "AnalyseBase/AnalyseTrain.h"

#define MR_MEM_SIZE (50*1024*1024)

enum MeterColour
{
    Other = 0,
    Black,
    White,
    Red,
    Yellow,
};

enum MeterBlurLevel
{
    OtherBlur = 0,
    LowBlur,
    HighBlur,
};

class MeterV2LineInfo
{
public:
    MeterV2LineInfo(): _colour(MeterColour::Black), _isThin(false), _lineWidth(0) {}
public:
    MeterColour _colour;    // 指针颜色
    bool _isThin;           // 指针很细
    int _lineWidth;         // 指针宽度
    std::vector<AnalyseSinglePoint2D> _points;	//如果不是细指针，则需要三组（6个点）计算出线宽

private:
    friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive& ar, const unsigned int version)
    {
        ar & _colour;
        ar & _isThin;
        ar & _lineWidth;
        ar & _points;
    }
};


class MeterV2TrainInfo
{
public:
    MeterV2TrainInfo():_backGround(MeterColour::White),
        _blur(MeterBlurLevel::LowBlur), _dbFlag(0)
    {
        _size._scale = 1.0;
        _size._height= 0;
        _size._width = 0;
    }
public:
    std::vector<MeterV2LineInfo>	_lines;     // 指针信息
    std::vector<AnalyseSinglePoint>	_points;    // 刻度信息
    std::vector<AnalyseSinglePoint2D> _analyseArea; //分析区域
    std::vector<AnalyseSinglePoint2D> _maskArea;    //屏蔽区域
    AnalyseSinglePoint2D _circleCenter;     //圆心坐标
    AnalyseSingleArea _matchArea;   // 缸体区域
    MeterColour _backGround;        // 背景颜色
    MeterBlurLevel _blur;           // 清晰度
    std::string _desPath;           // 描述信息路径
    std::string _curTarget;
    std::string _trainTarget;
    std::string _eleDevId;          //
    AnalyseSize _size;
    int _dbFlag;
};

// 表记配置
class CMeterV2Train : public CAnalyseTrain
{
public:
    CMeterV2Train();
public:
    // 实现CAnalyseTrain类的虚函数；
    bool PreProcess() override;
    bool Train(const cv::Mat& image) override;
    bool Add2DataBase() override;
public:
    void OutputTrainInfo(const MeterV2TrainInfo &info);
public:
    MeterV2TrainInfo trainInfo;
private:
    std::string _test_image_path;
    std::string _mask_image_path;
    std::string _desmem_path;
    std::string error_msg;
};
#endif
