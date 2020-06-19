#ifndef ANALYSE_SINGLE_SERVER_H
#define ANALYSE_SINGLE_SERVER_H
#include "AnalyseBase/AnalyseCommon.h"

class AnalyseSingleServer
{
private:
    AnalyseSingleServer() = default;
    AnalyseSingleServer(const AnalyseSingleServer &) = delete;
    AnalyseSingleServer& operator=(const AnalyseSingleServer &) = delete;
public:
    static AnalyseSingleServer *GetInstance();

public:
    // 启动服务
    void Start();
    // 处理算法
    bool ProcessAnalyseSingle(AnalyseBasePtr task);
    // 处理算法配置
    bool ProcessTrainingSingle(AnalyseBasePtr task);
    // 处理抓图
    // 处理视频
};

typedef AnalyseSingleServer IAnalyseSingleServer;

#endif  // ANALYSE_SINGLE_SERVER_H
