/*
 * @Description  : RESTFUL算法分析接口
 * @Version      : 1.0.0
 * @Author       : mdf
 * @Date         : 2020-05-11 15:45:47
 * @LastEditors  : mdf
 * @LastEditTime : 2020-05-11 17:02:00
 */
#pragma once
#include <cpprest/http_msg.h>

class HttpBasicHandler
{
public: 
    virtual void handleAnalyseRequest(web::http::http_request message) = 0;         // 算法分析请求(异步)
    virtual void handleMeterV2CfgRequest(web::http::http_request message) = 0;      // 算法配置请求(异步)
    virtual void handleLogsInfoQuery(web::http::http_request message, web::json::value &response) = 0;   // 日志查询
    virtual void handleSystemInfoGet(web::http::http_request message, web::json::value &response) = 0;   // 系统信息获取
    virtual void handleCapabilityGet(web::http::http_request message, web::json::value &response) = 0;   // 系统能力获取
    virtual void handleSystemTimeGet(web::http::http_request message, web::json::value &response) = 0;   // 系统时间获取
    virtual void handleSystemTimeSet(web::http::http_request message, web::json::value &response) = 0;   // 系统时间设置
    virtual void handleNTPSyncRequest(web::http::http_request message, web::json::value &response) = 0;   // NTP同步
    virtual void handleNetworkGet(web::http::http_request message, web::json::value &response) = 0;      // 网络信息获取
    virtual void handleNetworkSet(web::http::http_request message, web::json::value &response) = 0;      // 网络信息设置
    virtual void handleSystemUpgrade(web::http::http_request message, web::json::value &response) = 0;   // 系统更新
    virtual void handleModelUpdate(web::http::http_request message,   web::json::value &response) = 0;     // 模型更新
    virtual void handleSystemRestart(web::http::http_request message, web::json::value &response) = 0;   // 系统重启
};
