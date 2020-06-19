#ifndef ANALYSE_COMMON_H
#define ANALYSE_COMMON_H
#include "ThreadPool/Task.h"
#include <boost/serialization/vector.hpp>
#include <boost/serialization/map.hpp>
#include <string>
#include <vector>
#include <memory>
#include <functional>

#define OUTPUT_CONFIDANCE_AND_COORDINATE (1)  // 输出置信度与坐标

class CAnalyseObject;
typedef std::shared_ptr<CAnalyseObject> AnalyseObjectPtr;
typedef std::function<void (AnalyseObjectPtr)> NotifyCallback;

enum AnalysisType {
    AnalysisType_unknown = -1,
    AnalysisType_vehicleInvsaion = 0,
    AnalysisType_LineAbnormalDetect = 1,
    AnalysisType_AbnormalAction = 2,
    AnalysisType_natureInvasion = 3,
    AnalysisType_transLineBroken = 4,
    AnalysisType_InsulatorNoIntegrity = 5,
    AnalysisType_SwitchFenHe = 110,
    AnalysisType_SwitchFenHeChar = 111,
    AnalysisType_SwitchFenHeChin = 112,

    AnalysisType_MeterSf6 = 113,            // SF6表
    AnalysisType_MeterArrester_2 = 114,     // 避雷器
    AnalysisType_MeterArrester_3 = 115,     // 避雷器
    AnalysisType_MeterMainOilTemp = 116,    // 主变油温表
    AnalysisType_MeterReactor = 117,        // 电抗器

    AnalysisType_DisconnectorDoublePillar = 118,    // 水平双柱隔离开关
    AnalysisType_DisconnectorVerticalFlex = 119,    // 垂直伸缩隔离开关
    AnalysisType_Disconnector = 120,                // 水平伸缩隔离开关
    AnalysisType_DisconnectorThreePillar = 121,     // 三柱水平隔离开关
    AnalysisType_OilWin2 = 122,             // 室外油位视窗

    AnalysisType_SwitchFenHeLine = 123,
    AnalysisType_MeterBushingOL = 124,      // 套管油位
    AnalysisType_MeterOilLevel = 125,       // 油位计
    AnalysisType_EnergyStorage = 126,       // 储能指示状态
    AnalysisType_CounterNumber = 127,       // 计数器数字
    AnalysisType_LEDNumber = 128,           // LED数字

    AnalysisType_Knob = 131,            // 旋钮
    AnalysisType_Link_1 = 132,          // 柱状压板
    AnalysisType_Link_2 = 133,          // 一字压板
    AnalysisType_AirSwitch = 134,       // 空开
    AnalysisType_Light = 135,        	// 指示灯

    AnalysisType_Dial_Blurred = 10000,
    AnalysisType_Dial_Cracked = 10001,
    AnalysisType_Insulator_Breakup = 10002,
    AnalysisType_Huxiqi_Gjbs = 10003,
    AnalysisType_Huxiqi_Yfps = 10004,
    AnalysisType_Suspension = 10005,
    AnalysisType_Nest = 10006,
    AnalysisType_OilLeak_Ground = 10007,
    AnalysisType_OilLeak_Component = 10008,
    AnalysisType_MetalRust = 10009,
    AnalysisType_BoxDoorClosure = 10010,
    AnalysisType_FrameLadderLock = 10011,
    AnalysisType_DoorWindows = 10012,
    AnalysisType_CoverPlate = 10013,
    AnalysisType_SurfaceDirty = 10014,
    AnalysisType_Intrusion = 10015,
    AnalysisType_NoHelmet = 10016,
    AnalysisType_NoWorkWear = 10017,
    AnalysisType_Smoking = 10018,
    AnalysisType_MeterValue = 10019,
    AnalysisType_Huxiqi_Yfywyc = 10020,
    AnalysisType_YabanFenhe = 10021,
    AnalysisType_Max = 2147483647
};

//通信方式类型
enum ComType
{
    ComType_Unkonw = -1,
    ComType_Ace = 0,
    ComType_Thrift = 1,
    ComType_Restful = 2,
};

//分析结果
typedef struct _analyseresultinfo
{
    _analyseresultinfo():dValue(0),dConfidence(0),flag(0),left(0),top(0),right(0),bottom(0) {}

    float dValue;
    float dConfidence;
    int flag;
    int left;
    int top;
    int right;
    int bottom;
} AnalyseResultInfo;

typedef std::vector<AnalyseResultInfo> RESULT_VECTOR;

class AnalyseSinglePoint
{
public:
    AnalyseSinglePoint(): _x(0), _y(0), _value(0) {}
    AnalyseSinglePoint(int x, int y, double value): _x(x), _y(y), _value(value) {}
private:
    friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive& ar, const unsigned int version)
    {
        ar & _x;
        ar & _y;
        ar & _value;
    }

public:
    int _x;
    int _y;
    double _value;
};

class AnalyseSinglePoint2D
{
public:
    AnalyseSinglePoint2D(): _x(0), _y(0) {}
    AnalyseSinglePoint2D(int x, int y): _x(x), _y(y) {}
private:
    friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive& ar, const unsigned int version)
    {
        ar & _x;
        ar & _y;
    }

public:
    int _x;
    int _y;
};

class AnalyseSingleArea
{
public:
    AnalyseSingleArea(): _topleft(0, 0), _bottomright(0, 0) {}
    AnalyseSingleArea(AnalyseSinglePoint2D lt, AnalyseSinglePoint2D rb): _topleft(lt._x, lt._y), _bottomright(rb._x, rb._y) {}
    AnalyseSingleArea(int l, int t, int r, int b): _topleft(l, t), _bottomright(r, b) {}
private:
    friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive& ar, const unsigned int version)
    {
        ar & _topleft;
        ar & _bottomright;
    }

public:
    AnalyseSinglePoint2D _topleft;
    AnalyseSinglePoint2D _bottomright;
};

class AnalyseSize
{
public:
    AnalyseSize(): _width(0), _height(0), _scale(1) {}
    AnalyseSize(int width, int height): _width(width), _height(height), _scale(1) {}
private:
    friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive& ar, const unsigned int version)
    {
        ar & _width;
        ar & _height;
        ar & _scale;
    }

public:
    int _width;
    int _height;
    float _scale;		//缩放比例
};

// Base Class
class CAnalyseBase : public CTask
{
public:
    CAnalyseBase();
    CAnalyseBase(int mode);

    std::string GetElementId();
    void MakeElementId();
//private:
//    friend class boost::serialization::access;
//    template<class Archive>
//    void save(Archive & ar, const unsigned int version) const
//    {
//        ar & _deviceId;
//        ar & _modeType;
//        ar & _analyseType;
//    }
//    template<class Archive>
//    void load(Archive & ar, const unsigned int version)
//    {
//        ar & _deviceId;
//        ar & _modeType;
//        ar & _analyseType;
//    }
//    BOOST_SERIALIZATION_SPLIT_MEMBER()
public:
    std::string _elementId;
    std::string _deviceId;  // 设备ID(唯一识别码)
    int _modeType;          // 0-分析 1-配置
    int _analyseType;       // 分析类型
    int _comType;           // 通信模式，0-ACE，1-Thrift, 2-Restful
    int _resultCode;
};

typedef std::shared_ptr<CAnalyseBase> AnalyseBasePtr;

#endif
