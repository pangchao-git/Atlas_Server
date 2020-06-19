/*
 * @Description  : 
 * @Version      : 1.0.0
 * @Author       : mdf
 * @Date         : 2020-05-11 15:41:06
 * @LastEditors  : mdf
 * @LastEditTime : 2020-05-11 17:18:13
 */
#include "HttpInterfaceHandler.h"
#include "AnalyseSingleService.h"
#include "FileManager.h"
#include "NetScheduleLib.h"
#include "Algorithm/MeterV2Object.h"
#include "Algorithm/MeterV2Train.h"
#include <cpprest/filestream.h>
#include <cpprest/http_client.h>
#include "DataBaseInterface.h"
#include "Device/dev_system.h"

#define SERVER_API_URL  (U("http://10.1.1.14:6502/api"))
//NetScheduleLib::GetParameter("RestfulConfig","api_url", "http://127.0.0.1:6502/api")


void Print(std::vector<Bbox> &bbox){
    if(0==bbox.size()){
        return;
    }
    for(auto box: bbox){
        cout<<"score: "<<box.score<<" ,classname: "<<box.classname<<" Rect[ "<<box.Left_up.x<<", "<<box.Left_up.y
            <<", "<<box.Right_down.x<<", "<<box.Right_down.y<<"]"<<endl;
    }
}

void NotifyAnalyseResult(AnalyseObjectPtr info)
{
    //返回检测到的信息给客户端
    printf("reply box_infor to client!\n");
    Print(info->m_yolov3_results);
    bool sfyc = (info->m_yolov3_results.size() > 0);
    json::value response;
    response["deviceid"] = json::value::string(info->_deviceId);
    response["algrtype"] = json::value::number(info->_analyseType);
    response["sfyc"] = json::value::number(sfyc);
    std::vector<json::value> rect_arr;
    for (auto &value : info->m_yolov3_results)
    {
        std::vector<json::value> point_arr = {json::value::number(value.Left_up.x),
                                          json::value::number(value.Left_up.y),
                                          json::value::number(value.Right_down.x),
                                          json::value::number(value.Right_down.y)};
        json::value rectval;
        rectval["class"] = json::value::string(value.classname);
//        rectval["value"] = json::value::number(value.dValue);
        rectval["confidence"] = json::value::number(value.score);
        rectval["coordinate"] = json::value::array(point_arr);
        rect_arr.push_back(rectval);
    }
    response["rect"] = json::value::array(rect_arr);
//    m_response=response;
    //将数据发送到其他服务器

    http_client client(SERVER_API_URL);
    client.request(methods::PUT, U("detections/image"), response)
    .then([](http_response response) -> pplx::task<utility::string_t> {
        if(response.status_code() == status_codes::OK) {
            return response.extract_string();
        }
        return pplx::task_from_result(utility::string_t());
    })
    .then([](pplx::task<utility::string_t> previousTask) {
        try {
            utility::string_t const & v = previousTask.get();
            std::cout << "reply =data"<<v << std::endl;
        } catch (http_exception const & e) {
            std::cout << "Error exception: " << e.what() << std::endl;
        }
    })
    .wait();
}

HttpInterfaceHandler::HttpInterfaceHandler()
{
    printf("%s %d start!",__FUNCTION__,__LINE__);
    _SrcImageDir = NetScheduleLib::GetParameter("AnalyseHttp", "root", "Image");
    printf("%s %d end!",__FUNCTION__,__LINE__);
}

void HttpInterfaceHandler::handleAnalyseRequest(http_request message)
{
    printf(" into put callback  handleAnalyseRequest!\n");
    using concurrency::streams::file_stream;
    using concurrency::streams::basic_ostream;

    message.extract_json()
    .then([this](pplx::task<json::value> previousTask) {
        try {
            printf("start get params config from client!\n");
            json::value const & value = previousTask.get();
            auto taskid = value.at(U("taskid")).as_string();
            auto deviceid = value.at(U("deviceid")).as_string();
            auto algrtypeArr = value.at(U("algrtype")).as_array();
            auto picid = value.at(U("picid")).as_string();
            //auto filename = value.at(U("filename")).as_string();
            auto base64 = value.at(U("filedata")).as_string();
            //auto filesize = value.at(U("filesize")).as_number().to_int64();
            //printf("filesize:%ld\n", filesize);
            auto bytes = utility::conversions::from_base64(base64);
            std::string filedata(bytes.begin(), bytes.end());
            //
            int algrtype = algrtypeArr.at(0).as_number().to_int32();
            auto fileName = theFileManager->GetFileName(deviceid, picid, taskid, _SrcImageDir, algrtype);
            printf("fileName:%s\n", fileName.c_str());
            auto ss = concurrency::streams::stringstream::open_istream(filedata);
            concurrency::streams::fstream::open_ostream(U(fileName))
            .then([ss](pplx::task<basic_ostream<unsigned char>> previousTask)
            {
                auto fileStream = previousTask.get();
                ss.read_to_end(fileStream.streambuf())
                .then([fileStream](size_t bytesRead)
                {
                    fileStream.close();
                    ucout << "bytesRead:" << bytesRead << std::endl;
                });
            })
            .wait();
            //
            ucout << "stream read end" << std::endl;
            for (auto &value : algrtypeArr)
            {
                std::shared_ptr<CAnalyseObject> info;//基类的指针
                auto bind_func = std::bind(NotifyAnalyseResult, std::placeholders::_1);//回调函数，将推理数据传递给客户端
                int anatype = value.as_number().to_int32();
                switch(anatype)
                {
                    case AnalysisType_MeterSf6:
                    case AnalysisType_MeterArrester_2:
                    case AnalysisType_MeterArrester_3:
                    case AnalysisType_MeterMainOilTemp:
                    case AnalysisType_MeterReactor:
                        info.reset(new CMeterV2Object(fileName));
                    default:
                        break;
                }
                info->_pictures.push_back(fileName);
                info->_deviceId = deviceid;
                info->_analyseType = anatype;
                info->Init(bind_func, 1);
                IAnalyseSingleServer::GetInstance()->ProcessAnalyseSingle(info);
            }
            ucout << "ProcessAnalyseSingle end" << std::endl;
        }
        catch (http_exception const &e) {
            LogError("http exception:%s", e.what());
        }
        catch (std::runtime_error &e) {
            LogError("runtime_error:%s", e.what());
        }
    });
}

void HttpInterfaceHandler::handleMeterV2CfgRequest(http_request message)
{
    using concurrency::streams::file_stream;
    using concurrency::streams::basic_ostream;

    message.extract_json()
    .then([this](pplx::task<json::value> previousTask) {
        try {
            json::value const & value = previousTask.get();
            //auto taskid = value.at(U("taskid")).as_string();
            auto deviceid = value.at(U("deviceid")).as_string();
            auto algrtype = value.at(U("algrtype")).as_number().to_int32();
            auto picid = value.at(U("picid")).as_string();
            //auto filename = value.at(U("filename")).as_string();
            auto base64 = value.at(U("filedata")).as_string();
            //auto filesize = value.at(U("filesize")).as_number().to_int64();
            //printf("filesize:%ld\n", filesize);
            auto bytes = utility::conversions::from_base64(base64);
            std::string filedata(bytes.begin(), bytes.end());
            //
            auto fileName = theFileManager->GetFileName(deviceid, picid, "", _SrcImageDir, algrtype);
            printf("fileName:%s\n", fileName.c_str());
            auto ss = concurrency::streams::stringstream::open_istream(filedata);
            concurrency::streams::fstream::open_ostream(U(fileName))
            .then([ss](pplx::task<basic_ostream<unsigned char>> previousTask)
            {
                auto fileStream = previousTask.get();
                ss.read_to_end(fileStream.streambuf())
                .then([fileStream](size_t bytesRead)
                {
                    fileStream.close();
                    ucout << "bytesRead:" << bytesRead << std::endl;
                });
            })
            .wait();
            //
            auto points = value.at(U("points")).as_array();
            auto circlePoint = value.at(U("circleCenter")).as_array();
            auto analyseArea = value.at(U("analyseArea")).as_array();
            auto maskArea = value.at(U("maskArea")).as_array();
            auto lineArea = value.at(U("lineArea")).as_array();
            auto rectArea = value.at(U("rectangle")).as_array();
            int iBkgnd = value.at(U("backGround")).as_number().to_int32();
            int iBlur = value.at(U("blur")).as_number().to_int32();

            std::shared_ptr<CMeterV2Train> info(new CMeterV2Train());
            info->_analyseType = algrtype;
            info->_deviceId = deviceid;
            info->_pictures.push_back(fileName);
            // _points
            for (auto &jval : points)
            {
                AnalyseSinglePoint pt;
                auto arrPoint = jval.at(U("point")).as_array();
                pt._value = jval.at(U("value")).as_number().to_double();
                pt._x = arrPoint.at(0).as_number().to_int32();
                pt._y = arrPoint.at(1).as_number().to_int32();
                info->trainInfo._points.push_back(pt);
            }
            // _circleCenter
            {
                info->trainInfo._circleCenter._x = circlePoint.at(0).as_number().to_int32();
                info->trainInfo._circleCenter._y = circlePoint.at(1).as_number().to_int32();
            }
            // _analyseArea
            for (auto &jval : analyseArea)
            {
                AnalyseSinglePoint2D pt(jval.at(0).as_number().to_int32(), jval.at(1).as_number().to_int32());
                info->trainInfo._analyseArea.push_back(pt);
            }
            // _maskArea
            for (auto &jval : maskArea)
            {
                AnalyseSinglePoint2D pt(jval.at(0).as_number().to_int32(), jval.at(1).as_number().to_int32());
                info->trainInfo._maskArea.push_back(pt);
            }
            // _lines
            for (auto &jval : lineArea)
            {
                MeterV2LineInfo lineInfo;
                auto lineColor = jval.at(U("color")).as_number().to_int32();
                auto lineWidth = jval.at(U("width")).as_number().to_int32();
                auto arrPoints = jval.at(U("points")).as_array();
                lineInfo._colour = MeterColour(lineColor);
                lineInfo._lineWidth = lineWidth;
                lineInfo._isThin = (lineInfo._lineWidth <= 2)?1:0;
                // point
                for (auto &jPoint : arrPoints)
                {
                    AnalyseSinglePoint2D pt;
                    pt._x = jPoint.at(0).as_number().to_int32();
                    pt._y = jPoint.at(1).as_number().to_int32();
                    lineInfo._points.push_back(pt);
                }
                info->trainInfo._lines.push_back(lineInfo);
            }
            // _matchArea
            {
                info->trainInfo._matchArea._topleft._x = rectArea.at(0).as_number().to_int32();
                info->trainInfo._matchArea._topleft._y = rectArea.at(1).as_number().to_int32();
                info->trainInfo._matchArea._bottomright._x = rectArea.at(2).as_number().to_int32();
                info->trainInfo._matchArea._bottomright._y = rectArea.at(3).as_number().to_int32();
            }
            info->trainInfo._backGround = MeterColour(iBkgnd);
            info->trainInfo._blur = MeterBlurLevel(iBlur);
            info->trainInfo._eleDevId = deviceid + "_" + std::to_string(algrtype);

            // for test
            //info->OutputTrainInfo(info->trainInfo);
            IAnalyseSingleServer::GetInstance()->ProcessTrainingSingle(info);
        }
        catch (http_exception const &e) {
            LogError("http exception:%s", e.what());
        }
        catch (std::runtime_error &e) {
            LogError("runtime_error:%s", e.what());
        }
    });
}

void HttpInterfaceHandler::handleLogsInfoQuery(http_request message, json::value &response)
{
    message.extract_json()
    .then([&response](pplx::task<json::value> previousTask) {
        try {
            json::value const & value = previousTask.get();
            int majorid = value.at(U("major")).as_number().to_int32();
            int minorid = value.at(U("minor")).as_number().to_int32();
            auto startTime = value.at(U("startTime")).as_string();
            auto endTime = value.at(U("endTime")).as_string();
            int pageIndex = value.at(U("page")).as_number().to_int32();
            int rowSize = value.at(U("count")).as_number().to_int32();
            // 在数据库查询日志
            LOG_RESULTS results;
            int hasNext = 0;
            GetDataBaseObj().QueryLogInfo(majorid, minorid, startTime, endTime, pageIndex, rowSize, results, hasNext);
            response["major"] = json::value::number(majorid);
            response["minor"] = json::value::number(minorid);
            std::vector<json::value> data_arr;
            for (auto &item : results)
            {
                json::value dataval;
                dataval["id"] = json::value::number(item.id);
                dataval["time"] = json::value::string(item.logTime);
                dataval["level"] = json::value::number(item.level);
                dataval["info"] = json::value::string(item.logInfo);
                data_arr.push_back(dataval);
            }
            response["data"] = json::value::array(data_arr);
            response["page"] = json::value::number(pageIndex);
            response["hasNext"] = json::value::number(hasNext);
        }
        catch (http_exception const &e) {
            LogError("http exception:%s", e.what());
        }
        catch (std::runtime_error &e) {
            LogError("runtime_error:%s", e.what());
        }
    })
    .wait();
}

void HttpInterfaceHandler::handleSystemInfoGet(http_request message, json::value &response)
{

}

void HttpInterfaceHandler::handleCapabilityGet(http_request message, json::value &response)
{

}

void HttpInterfaceHandler::handleSystemTimeGet(http_request message, json::value &response)
{
    std::string timeStr, timeZone;
    CurrentDateTime(timeStr, timeZone);
    response["timeZone"] = json::value::string(timeZone);
    response["time"] = json::value::string(timeStr);
}

void HttpInterfaceHandler::handleSystemTimeSet(http_request message, json::value &response)
{
    message.extract_json()
    .then([&response](pplx::task<json::value> previousTask) {
        try {
            // 请求解析
            json::value const & value = previousTask.get();
            auto timeStr = value.at(U("time")).as_string();
            auto timeZone= value.at(U("timeZone")).as_string();
            // 响应
            response["status"] = json::value::string("OK");
        }
        catch (http_exception const &e) {
            LogError("http exception:%s", e.what());
        }
        catch (std::runtime_error &e) {
            LogError("runtime_error:%s", e.what());
        }
    })
    .wait();
}

void HttpInterfaceHandler::handleNTPSyncRequest(http_request message, json::value &response)
{

}

void HttpInterfaceHandler::handleNetworkGet(http_request message, json::value &response)
{

}

void HttpInterfaceHandler::handleNetworkSet(http_request message, json::value &response)
{

}

void HttpInterfaceHandler::handleSystemUpgrade(http_request message, json::value &response)
{

}

void HttpInterfaceHandler::handleModelUpdate(http_request message, json::value &response)
{

}

void HttpInterfaceHandler::handleSystemRestart(http_request message, json::value &response)
{

}
