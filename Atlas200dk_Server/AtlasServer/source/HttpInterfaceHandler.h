/*
 * @Description  : 
 * @Version      : 1.0.0
 * @Author       : mdf
 * @Date         : 2020-05-11 15:40:23
 * @LastEditors  : mdf
 * @LastEditTime : 2020-05-11 17:22:38
 */
#ifndef HTTP_INTERFACE_HANDLER_H
#define HTTP_INTERFACE_HANDLER_H

#include "http_basic_handler.hpp"
using namespace web;
using namespace web::http;
using namespace web::http::client;
class HttpInterfaceHandler : public HttpBasicHandler
{
public:
    HttpInterfaceHandler();
    void handleAnalyseRequest(web::http::http_request message) override;
    void handleMeterV2CfgRequest(web::http::http_request message) override;
    void handleLogsInfoQuery(web::http::http_request message, web::json::value &response) override;
    void handleSystemInfoGet(web::http::http_request message, web::json::value &response) override;
    void handleCapabilityGet(web::http::http_request message, web::json::value &response) override;
    void handleSystemTimeGet(web::http::http_request message, web::json::value &response) override;
    void handleSystemTimeSet(web::http::http_request message, web::json::value &response) override;
    void handleNTPSyncRequest(web::http::http_request message, web::json::value &response) override;
    void handleNetworkGet(web::http::http_request message, web::json::value &response) override;
    void handleNetworkSet(web::http::http_request message, web::json::value &response) override;
    void handleSystemUpgrade(web::http::http_request message, web::json::value &response) override;
    void handleModelUpdate(web::http::http_request message, web::json::value &response) override;
    void handleSystemRestart(web::http::http_request message, web::json::value &response) override;
private:

    std::string _SrcImageDir;
    std::string _DstImageDir;
};

#endif
