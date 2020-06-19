/**
 * ============================================================================
 *
 * Copyright (C) 2018, Hisilicon Technologies Co., Ltd. All Rights Reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *   1 Redistributions of source code must retain the above copyright notice,
 *     this list of conditions and the following disclaimer.
 *
 *   2 Redistributions in binary form must reproduce the above copyright notice,
 *     this list of conditions and the following disclaimer in the documentation
 *     and/or other materials provided with the distribution.
 *
 *   3 Neither the names of the copyright holders nor the names of the
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 * ============================================================================
 */

#ifndef MAIN_H_
#define MAIN_H_

#include <string>
#include <unistd.h>
#include <algorithm>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <vector>
//#include "data_type.h"
#include "hiaiengine/api.h"
#include "tool_api.h"
#include "data_type.h"
//#include "error_code.h"
#include "opencv2/opencv.hpp"
#include "opencv2/imgcodecs/legacy/constants_c.h"
using namespace std;
using namespace cv;
#define  sigmoid(x)  (1 / (1 + exp(-1*x)))
class CustomDataRecvInterface : public hiai::DataRecvInterface {
public:
    /**
     * @brief: Constructor
     */
    CustomDataRecvInterface() = default;

    ~CustomDataRecvInterface() = default;

    /**
     * @brief: Receive data from call back engine
     * @param [in]: received message
     * @return: result
     */

    HIAI_StatusT RecvData(const std::shared_ptr<void>& message);
    HIAI_StatusT GetData(std::vector<Bbox> &box_result);
    void SetConsoleParams(shared_ptr<ConsoleParams> &param_ptr,Input_param &input_params);

    HIAI_StatusT Init_Atlas200dk();

    HIAI_StatusT SetCallBack(std::shared_ptr<CustomDataRecvInterface> &customDataRecvInterface);
    HIAI_StatusT SetGrgument(hiai::EnginePortID &engine_id,Input_param &input_params);
    std::shared_ptr<EngineTrans>  m_enginetrans= nullptr;
    std::shared_ptr<BBOX>  m_box_result= nullptr;
    HIAI_StatusT Destory();
    const uint32_t kGraphId = 1676964756;
    // device id
    const uint32_t kDeviceId = 0;
// inference engine id
    const uint32_t kFirstEngineId = 487;
// inference port id
    const uint32_t kFirstEnginePortId = 0;
// post engine id
    const uint32_t kPostEngineId = 641;
    std::shared_ptr<hiai::Graph> m_graph= nullptr;
    static int flag ;
//static mutex for decrease flag
    mutex mt;

private:
    // std::shared_ptr<CustomDataRecvInterface> m_CustomDataRecvInterface= nullptr;
    //yolov3相关


    const uint32_t kSendDataPort = 0;
// sleep interval when queue full (unit:microseconds)
    const __useconds_t kSleepInterval = 5;

// graph.config path
    const string kGraphConfigFilePath = "./graph.config";

// args positon
    const int32_t kModelWidthPos = 1;
    const int32_t kModelHeightPos = 2;
    const int32_t kImagePathPos = 3;
    const int32_t kOutputPathPos = 4;
    const int32_t kOutputNumsPos = 5;
// sleep interval for every image (unit: microseconds)
    // const __useconds_t kSleepInterval = 100000;

};
class Atlasdk_process {
public:
    std::shared_ptr<CustomDataRecvInterface> m_custom=nullptr;
    Atlasdk_process();
    ~Atlasdk_process()= default;

};

#endif /* MAIN_H_ */
