/*
 * @Description  : main
 * @Version      : 1.0.0
 * @Author       : mdf
 * @Date         : 2020-05-07 18:08:31
 * @LastEditors  : mdf
 * @LastEditTime : 2020-05-11 14:26:33
 */
#include <iostream>
#include <usr_interrupt_handler.hpp>
#include <runtime_utils.hpp>
#include <network_utils.hpp>
#include "NetScheduleLib.h"
#include "microsvc_controller.hpp"
#include <iostream>
using namespace web;
using namespace cfx;
using namespace std;
int main(int argc, const char * argv[]) {    
	InterruptHandler::hookSIGINT();
    NetScheduleLib::Init();
    std::string eth0Addr = NetworkUtils::hostIPbyEthName("eth0");
    std::string httpHost = NetScheduleLib::GetParameter("AnalyseHttp", "localhost", eth0Addr);
    std::string httpPort = NetScheduleLib::GetParameter("AnalyseHttp", "port", "6502");
    std::string httpRoot = NetScheduleLib::GetParameter("AnalyseHttp", "root", "Image");

    MicroserviceController server;//构造函数内部开启监听，监听客户端的get post ,put
    //server.setEndpoint("http://host_auto_ip4:6502/v1/ivmero/api");
    server.setEndpoint("http://" + httpHost + ":" + httpPort + "/detections/image/");
    //NetScheduleLib::WriteLog(NetScheduleLib::mLogDebug, "service api: %s",  server.endpoint().c_str());
    LogDebug("service api: %s", server.endpoint().c_str());

    try {
        // wait for server initialization...
        server.accept().wait();
        std::cout << "Modern C++ Microservice now listening for requests at: " << server.endpoint() << '\n';
        
        InterruptHandler::waitForUserInterrupt();
        server.shutdown().wait();
    }
    catch(std::exception & e) {
        NetScheduleLib::WriteLog(NetScheduleLib::mLogError, "somehitng wrong happen! %s", e.what());
    }
    catch(...) {
        RuntimeUtils::printStackTrace();
    }
    
    NetScheduleLib::WriteLog(NetScheduleLib::mLogDebug, "%s", "main() exit!");
    NetScheduleLib::UInit();
    return 0;
}
