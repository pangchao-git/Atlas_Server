//
//  Created by Ivan Mejia on 12/24/16.
//
// MIT License
//
// Copyright (c) 2016 ivmeroLabs.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//

#include <std_micro_service.hpp>
#include "microsvc_controller.hpp"
#include "HttpInterfaceHandler.h"
#include "NetScheduleLib.h"

using namespace web;
using namespace http;
using namespace std;

MicroserviceController::MicroserviceController() : BasicController() {
    printf("start MicroserviceController::MicroserviceController()!\n ");
    handler.reset(new HttpInterfaceHandler());
    printf("end MicroserviceController::MicroserviceController()!\n ");
}

void MicroserviceController::initRestOpHandlers() {
    _listener.support(methods::GET, std::bind(&MicroserviceController::handleGet, this, std::placeholders::_1));
    _listener.support(methods::PUT, std::bind(&MicroserviceController::handlePut, this, std::placeholders::_1));
    _listener.support(methods::POST, std::bind(&MicroserviceController::handlePost, this, std::placeholders::_1));
    _listener.support(methods::DEL, std::bind(&MicroserviceController::handleDelete, this, std::placeholders::_1));
    _listener.support(methods::PATCH, std::bind(&MicroserviceController::handlePatch, this, std::placeholders::_1));
}

void MicroserviceController::handleGet(http_request message) {
    auto path = requestPath(message);
    for(auto path_:path){
        std::cout<<"message=="<<path_;
    }

#if 0
    if (!path.empty()) {
        if (path[0] == "service" && path[1] == "test") {
            auto response = json::value::object();
            response["version"] = json::value::string("0.1.1");
            response["status"] = json::value::string("ready!");
            message.reply(status_codes::OK, response);
        }
    }
    else {
        message.reply(status_codes::NotFound);
    }
#else
    auto response = json::value::object();
    response["status"]=json::value::string("OK");
    response["data"] = json::value::string("I am deal your  get requeset");
    message.reply(status_codes::OK,response);
#endif
}

void MicroserviceController::handlePatch(http_request message) {
    message.reply(status_codes::NotImplemented, responseNotImpl(methods::PATCH));
}

void MicroserviceController::handlePut(http_request message) {
    NetScheduleLib::WriteLog(NetScheduleLib::mLogDebug, "request url: %s", message.absolute_uri().to_string().c_str());
    printf("start handlePut!\n ");
    auto path = requestPath(message);
    std::cout<<"path[0]="<<path[0]<<",path[1]="<<path[1]<<endl;
    printf("end handlePut!\n ");
    if (path.empty()) {
        message.reply(status_codes::NotFound);
        return;
    }

    if (path[0]=="detections" && path[1]=="image") {
        //此处调用了处理函数去实现对推理数据的获取
        handler->handleAnalyseRequest(message);
    }
        //增加yollov3的处理逻辑
    else if (path[0] =="yolov3_detections" && path[1]=="image"){

    }
    else if (path[0]=="trainings" && path[1]=="image") {
        handler->handleMeterV2CfgRequest(message);
    }


    else {
        message.reply(status_codes::NotFound);
        return;
    }
    auto response = json::value::object();
    //response=handler->;
    //返回数据给客户端
    response["status"] = json::value::string("OK");
    message.reply(status_codes::OK, response);
}

void MicroserviceController::handlePost(http_request message) {
    NetScheduleLib::WriteLog(NetScheduleLib::mLogDebug, "request url: %s", message.absolute_uri().to_string().c_str());
    auto path = requestPath(message);
    if (path.empty()) {
        message.reply(status_codes::NotFound);
        return;
    }

    json::value response = json::value::object();
    if (path[0]=="logs") {
        handler->handleLogsInfoQuery(message, response);
    }
    else {
        message.reply(status_codes::NotFound);
        return;
    }

    message.reply(status_codes::OK, response);
}

void MicroserviceController::handleDelete(http_request message) {    
    message.reply(status_codes::NotImplemented, responseNotImpl(methods::DEL));
}

void MicroserviceController::handleHead(http_request message) {
    message.reply(status_codes::NotImplemented, responseNotImpl(methods::HEAD));
}

void MicroserviceController::handleOptions(http_request message) {
    message.reply(status_codes::NotImplemented, responseNotImpl(methods::OPTIONS));
}

void MicroserviceController::handleTrace(http_request message) {
    message.reply(status_codes::NotImplemented, responseNotImpl(methods::TRCE));
}

void MicroserviceController::handleConnect(http_request message) {
    message.reply(status_codes::NotImplemented, responseNotImpl(methods::CONNECT));
}

void MicroserviceController::handleMerge(http_request message) {
    message.reply(status_codes::NotImplemented, responseNotImpl(methods::MERGE));
}

json::value MicroserviceController::responseNotImpl(const http::method & method) {
    auto response = json::value::object();
    response["serviceName"] = json::value::string("C++ Mircroservice Sample");
    response["http_method"] = json::value::string(method);
    return response ;
}

