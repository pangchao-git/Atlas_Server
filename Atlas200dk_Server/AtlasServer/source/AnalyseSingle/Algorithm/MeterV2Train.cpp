#include "MeterV2Train.h"
#include "DataBaseInterface.h"
#include "NetScheduleLib.h"
#include "meterReg/HYAMR_meterReg.h"
#include <iostream>
#include <opencv2/opencv.hpp>
#include <cpprest/asyncrt_utils.h>
#include <FileManager.h>
////////////////////////////////////////////////////
/// 类CMeterV2Train实现
///

// 表记配置
CMeterV2Train::CMeterV2Train()
{
}

bool CMeterV2Train::PreProcess()
{
    if (trainInfo._eleDevId.empty())
    {
        trainInfo._eleDevId = _deviceId + "_" + std::to_string(_analyseType);
    }
    trainInfo._desPath = "Train/MeterV2/" + trainInfo._eleDevId + ".dat";
    //
    _dirPath = GetDataBaseObj()._dbDirPath + "Train/MeterV2/"+ trainInfo._eleDevId +"/";
    theFileManager->CreateDirectory(_dirPath);
    //
    _desmem_path = GetDataBaseObj()._dbDirPath + "Train/MeterV2/"+ trainInfo._eleDevId +".dat";
    _test_image_path = _dirPath + "orignal.jpg";
    _mask_image_path = _dirPath + "mask.jpg";
    return true;
}

bool CMeterV2Train::Train(const cv::Mat &image)
{
    int res = 0;
    MHandle hMemMgr = MNull;
    MHandle hHYMRDHand = MNull;
    MVoid *pMem = MNull;

    cv::Mat testImage;
    cv::Mat maskImage;
    cv::Size ImgSize;

    HYAMR_IMAGES wy_testImage = {0};
    HYAMR_IMAGES wy_maskImage = {0};
    MLong lMemSize = 0;
    MLong lDesSize = 0;
    float fScale = 1.0;
    int iRect[4] = { 0 };
    unsigned char szTrainTarget[32] = { 0 };
    unsigned char szCurTarget[32]  = { 0 };
    unsigned char *pData = MNull;
    unsigned char *szMeterDes = MNull;
    std::vector<unsigned char> strArray;

    pMem = malloc(MR_MEM_SIZE);
    if (!pMem)
    {
        LogError("malloc 50*1024*1024 error!!!\n");
        return -1;
    }

    hMemMgr = HYAMR_MemMgrCreate(pMem, MR_MEM_SIZE);
    if (!hMemMgr)
    {
        LogError("HYAMR_MemMgrCreate error!!!\n");
        res = -1;
        goto EXT;
    }

    HYAMR_Init(hMemMgr, &hHYMRDHand);
    if (!hHYMRDHand)
    {
        LogError("HYAMR_Init error!!!\n");
        res = -1;
        goto EXT;
    }

    trainInfo._size._scale = fScale;
    trainInfo._size._height= image.rows;
    trainInfo._size._width = image.cols;
    trainInfo._matchArea._topleft._x /= fScale;
    trainInfo._matchArea._topleft._y /= fScale;
    trainInfo._matchArea._bottomright._x /= fScale;
    trainInfo._matchArea._bottomright._y /= fScale;

    ImgSize.height = image.rows / fScale;
    ImgSize.width = image.cols / fScale;

    iRect[0] = trainInfo._matchArea._topleft._x;
    iRect[1] = trainInfo._matchArea._topleft._y;
    iRect[2] = trainInfo._matchArea._bottomright._x;
    iRect[3] = trainInfo._matchArea._bottomright._y;

    testImage = cv::Mat(ImgSize, CV_8UC3);
    cv::resize(image, testImage, ImgSize);
    cv::imwrite(_test_image_path, testImage);

    maskImage = cv::Mat(ImgSize, CV_8UC1);
    pData = (unsigned char *)(maskImage.data);
    for (int j=0;j<ImgSize.height;j++) {
        for (int i=0;i<ImgSize.width;i++) {
            pData[j*maskImage.step+i] = (j>=iRect[1]&&j<=iRect[3]&&i>=iRect[0]&&i<=iRect[2])?255:0;
        }
    }
    cv::imwrite(_mask_image_path, maskImage);

    //test
    wy_testImage.lWidth = testImage.cols;
    wy_testImage.lHeight = testImage.rows;
    wy_testImage.pixelArray.chunky.lLineBytes = testImage.step;
    wy_testImage.pixelArray.chunky.pPixel = testImage.data;
    wy_testImage.lPixelArrayFormat = HYAMR_IMAGE_BGR;
    //mask
    wy_maskImage.lWidth = maskImage.cols;
    wy_maskImage.lHeight = maskImage.rows;
    wy_maskImage.pixelArray.chunky.lLineBytes = maskImage.step;
    wy_maskImage.pixelArray.chunky.pPixel = maskImage.data;
    wy_maskImage.lPixelArrayFormat = HYAMR_IMAGE_GRAY;

    if (0 != HYAMR_TrainTemplateFromMask(hHYMRDHand, &wy_testImage, &wy_maskImage, const_cast<char *>("OK"), 0))
    {
        LogError("HYAMR_TrainTemplateFromMask error!\n");
        res = -1;
        goto EXT;
    }

    lMemSize = testImage.cols * testImage.rows;
    szMeterDes = (unsigned char *)malloc(lMemSize * sizeof(unsigned char));
    if(0 != HYAMR_SaveDesMem(hHYMRDHand, szMeterDes, lMemSize, &lDesSize))
    {
        LogError("HYAMR_SaveTDescriptorsGroup error!\n");
        res = -1;
        goto EXT;
    }

    HYAMR_GetObjRect(hHYMRDHand, &wy_testImage, const_cast<char *>("OK"), 0.75);
    memset(szCurTarget, 0, 32 * sizeof(unsigned char));
    memset(szTrainTarget, 0, 32 * sizeof(unsigned char));

    //printf("%d, %d, %d, %d\n", iRect[0], iRect[1], iRect[2], iRect[3]);
    if(0 != HYAMR_SaveTargetInfoToMemTmp(hHYMRDHand, szCurTarget, szTrainTarget, 1, iRect))
    {
        LogError("HYAMR_SaveTargetInfo error!\n");
        res = -1;
        goto EXT;
    }
    theFileManager->FileWrite(_desmem_path, (const char *)szMeterDes, lDesSize);
    //
    strArray.assign(szCurTarget, szCurTarget + 32);
    trainInfo._curTarget = utility::conversions::to_base64(strArray);
    strArray.assign(szTrainTarget, szTrainTarget + 32);
    trainInfo._trainTarget = utility::conversions::to_base64(strArray);

    res = 0;
EXT:
    HYAMR_Uninit(hHYMRDHand);
    HYAMR_MemMgrDestroy(hMemMgr);
    if(MNull!=pMem)
    {
        free(pMem);
        pMem = MNull;
    }
    if (szMeterDes)
    {
        free(szMeterDes);
        szMeterDes = MNull;
    }
    return (0 == res);
}

bool CMeterV2Train::Add2DataBase()
{
    bool bRet = GetDataBaseObj().UpdateMeterV2TrainInfo(trainInfo._eleDevId, trainInfo, error_msg);
    OutputTrainInfo(trainInfo);
    return bRet;
}

void CMeterV2Train::OutputTrainInfo(const MeterV2TrainInfo &info)
{
    std::cout << "_deviceId: " << _deviceId << std::endl;
    std::cout << "_analyseType: " << _analyseType << std::endl;

    std::cout << "_topleft: (" << info._matchArea._topleft._x << ", " << info._matchArea._topleft._y << ")" << std::endl;
    std::cout << "_bottomright: (" << info._matchArea._bottomright._x << ", " << info._matchArea._bottomright._y << ")" << std::endl;

    std::cout << "_points size: " << info._points.size() << std::endl;
    for (auto &point : info._points)
    {
        std::cout << "point: (" << point._x << "," << point._y << "), " << point._value << std::endl;
    }
    std::cout << "_circleCenter: (" << info._circleCenter._x << "," << info._circleCenter._y << ")" << std::endl;

    std::cout << "_backGround:" << info._backGround << std::endl;
    std::cout << "_blur:" << info._blur << std::endl;

    std::cout << "_lines size: " << info._lines.size() << std::endl;
    for (auto &value : info._lines)
    {
        std::cout << "Line: _colour: " << value._colour << ", width: " << value._lineWidth << std::endl;
        for (auto &point : value._points)
        {
            std::cout << "point: (" << point._x << "," << point._y << ")" << std::endl;
        }
    }
    std::cout << "_analyseArea size: " << info._analyseArea.size() << std::endl;
    for (auto &point : info._analyseArea)
    {
        std::cout << "point: (" << point._x << "," << point._y << ")" << std::endl;
    }

    std::cout << "_maskArea size: " << info._maskArea.size() << std::endl;
    for (auto &point : info._maskArea)
    {
        std::cout << "point: (" << point._x << "," << point._y << ")" << std::endl;
    }
}


