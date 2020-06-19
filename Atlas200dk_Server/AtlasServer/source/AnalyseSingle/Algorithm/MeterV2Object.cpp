#include "MeterV2Object.h"
#include "DataBaseInterface.h"
#include "NetScheduleLib.h"
#include "FileManager.h"
#include <cpprest/asyncrt_utils.h>
#include <algorithm>

using namespace std;

typedef struct
{
    MPOINT maskPt[10];
    MLong lMaskPtNum;
} MaskParam;

typedef struct
{
    MLong cx;
    MLong cy;
    MLong r;
} MyCircleParam;

typedef struct
{
    MLong left;
    MLong top;
    MLong right;
    MLong bottom;
} MyRect;

////////////////////////////////////////////////////
/// 类CMeterV2Object实现
///
// 表计分析
CMeterV2Object::CMeterV2Object(std::string &image_path)
{
    _streamType = 0;
    _resultCode = 400;
    hHYMRDHand = MNull;
    hMemMgr = MNull;
    pMem = MNull;
    m_image_path = image_path;
}


CMeterV2Object::CMeterV2Object()
{
    _streamType = 0;
    _resultCode = 400;
    hHYMRDHand = MNull;
    hMemMgr = MNull;
    pMem = MNull;
}
CMeterV2Object::~CMeterV2Object()
{
    invokeUninit();
}

void CMeterV2Object::NotifyUser()
{

}

void CMeterV2Object::OutputTrainInfo(const MeterV2TrainInfo &info)
{
    std::cout << "OutputTrainInfo--------------------------------" << std::endl;
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

void CMeterV2Object::OutputInterParam(const HYAMR_INTERACTIVE_PARA &inPara)
{
    std::cout << "OutputInterParam--------------------------------" << std::endl;
    std::cout << "_deviceId: " << _deviceId << std::endl;
    std::cout << "_analyseType: " << _analyseType << std::endl;
    std::cout << "_points size: " << inPara.lPtNum<< std::endl;
    for (int i=0; i<inPara.lPtNum; i++)
    {
        std::cout << "point: (" << inPara.ptPosList[i].x << "," << inPara.ptPosList[i].y << "), " << inPara.dPtValList[i] << std::endl;
    }
    std::cout << "_circleCenter: (" << inPara.circleCoord.x << "," << inPara.circleCoord.y << ")" << std::endl;

    std::cout << "_isWhiteBackGround:" << inPara.bWhiteBackground << std::endl;
    std::cout << "_blur:" << inPara.picLevel << std::endl;

    std::cout << "_lines size: " << inPara.lLineNum << std::endl;
    for (int i=0; i<inPara.lLineNum; i++)
    {
        std::cout << "Line: _colour: " << inPara.lLineColor[i] << std::endl;    // BLACK = 0,  WHITE,  RED,  YELLOW
    }
    std::cout << "Line: width: " << inPara.lLineWidth << std::endl;

    std::cout << "Dist: " << inPara.Dist << std::endl;
}



int CMeterV2Object::invokeInit()
{
    printf(" init yolov3 *******************");
#if 0
    int res = 0;
    // 从数据库读取信息
    string elemDevId = GetElementId();
    bool bRet = GetDataBaseObj().QueryMeterV2TrainInfo(elemDevId, trainInfo, error_msg);
    if (!bRet) {
        LogError("%s", error_msg.c_str());
        return -1;
    }
    OutputTrainInfo(trainInfo);
    string desDataStr;
    if (!theFileManager->FileRead(GetDataBaseObj()._dbDirPath + trainInfo._desPath, desDataStr))
    {
        LogError("read error fileName:%s", trainInfo._desPath.c_str());
        return -1;
    }

    _curTargetVec = utility::conversions::from_base64(trainInfo._curTarget);
    _trnTargetVec = utility::conversions::from_base64(trainInfo._trainTarget);

    //***********************  alloc memory  *******************************
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
        return -1;
    }

    HYAMR_Init(hMemMgr, &hHYMRDHand);
    if (!hHYMRDHand)
    {
        LogError("HYAMR_Init error!!!\n");
        return -1;
    }

    // HYAMR_GetDesMem
    if(0 != HYAMR_GetDesMem(hHYMRDHand, (unsigned char *)const_cast<char *>(desDataStr.c_str())))
    {
        LogError("HYAMR_GetDesMem error");
        res = -1;
        goto EXT;
    }
    res = 0;
EXT:
    return res;
#else
    MAKE_SHARED_NO_THROW(m_atlas,Atlasdk_process);
    if( nullptr == m_atlas ){
        perror("MAKE_SHARED_NO_THROW failed!\n");
        return -1;
    }
    //1 Init
    HIAI_StatusT status = m_atlas->m_custom->Init_Atlas200dk();
    if(status != HIAI_OK){
        INFO_LOG("atlas->m_custom->Init_Atlas200dk()!,ret=%d\n", status);
        return -1;
    }

    //命令行参数
    input_params.model_width = m_model_weight;
    input_params.model_height= m_model_height;
    input_params.save_path="./out";
    input_params.image_path=m_image_path;
    std::cout<<"m_image_path:"<<m_image_path<<std::endl;
    //设置yolov3后处理的配置项
    input_params.yolov3_cfg_params.label=label;
    input_params.yolov3_cfg_params.numClasses=label.size()-1;
    input_params.yolov3_cfg_params.BoxTensorLabel=input_params.yolov3_cfg_params.numClasses+5;

    //设置传输的engine_id
    engine_id.graph_id = m_atlas->m_custom->kGraphId;
    engine_id.engine_id = m_atlas->m_custom->kFirstEngineId;
    engine_id.port_id = m_atlas->m_custom->kFirstEnginePortId;
    return 0;

#endif
}

static int calcLineWidth(const std::vector<AnalyseSinglePoint2D> &points, float fScale)
{
    int lPtNums = points.size();
    float fHaarWidth = 0;
    for (int j=0;j<lPtNums/2;j++)
    {
        int temp = (points[2*j]._x-points[2*j+1]._x)*(points[2*j]._x-points[2*j+1]._x)+
            (points[2*j]._y-points[2*j+1]._y)*(points[2*j]._y-points[2*j+1]._y);
        fHaarWidth += sqrt(temp);
    }
    fHaarWidth /= (lPtNums/2);
    if (fScale > 0)
        fHaarWidth /= fScale;
    return ((int)fHaarWidth);
}

static void TrainInfo2InterParam(const MeterV2TrainInfo &info, HYAMR_INTERACTIVE_PARA &inPara)
{
    float fScale = info._size._scale;
    ////////////////////
    // points
    inPara.lPtNum = std::min((int)info._points.size(), HYAMR_MAX_PT_LIST);
    for (int i = 0; i<inPara.lPtNum; i++) {
        inPara.ptPosList[i].x = info._points.at(i)._x/fScale;
        inPara.ptPosList[i].y = info._points.at(i)._y/fScale;
        inPara.dPtValList[i]  = info._points.at(i)._value;
    }

    // circleCoord
    inPara.circleCoord.x = info._circleCenter._x/fScale;
    inPara.circleCoord.y = info._circleCenter._y/fScale;

    // lines
    inPara.lLineNum = std::min((int)info._lines.size(), 2);
    for (int i = 0; i<inPara.lLineNum; i++) {
        auto &lineVal = info._lines.at(i);
        inPara.lLineColor[i] = HY_LINE_TYPE((int)lineVal._colour - 1);
        if (i >= 1) {
            continue;
        }
        if (lineVal._lineWidth <= 0) {
            inPara.lLineWidth = lineVal._isThin?2:(calcLineWidth(lineVal._points, fScale));
        }
        else {
            inPara.lLineWidth = lineVal._lineWidth/fScale;
        }
        if (inPara.lLineWidth < 2) {
            inPara.lLineWidth = 2;
        }
    }

    inPara.bWhiteBackground = (MeterColour::White == info._backGround)?MTrue:MFalse;
    inPara.picLevel = (MeterBlurLevel::OtherBlur == info._blur)?HY_OTHER_BLUR:HY_BLUR_LEVEL((int)info._blur - 1);
}

static void FittingCircle(MPOINT *pPtList, MLong lPtLen,
                       MLong *xc, MLong *yc, MLong *r)
{
    MLong i;
    MDouble X1=0,Y1=0,X2=0,Y2=0,X3=0,Y3=0,X1Y1=0,X1Y2=0,X2Y1=0;
    MDouble C,D,E,G,H,N;
    MDouble a,b,c;
    if (lPtLen<3)
    {
        return;
    }

    for (i=0;i<lPtLen;i++)
    {
        X1 = X1 + pPtList[i].x;
        Y1 = Y1 + pPtList[i].y;
        X2 = X2 + pPtList[i].x*pPtList[i].x;
        Y2 = Y2 + pPtList[i].y*pPtList[i].y;
        X3 = X3 + pPtList[i].x*pPtList[i].x*pPtList[i].x;
        Y3 = Y3 + pPtList[i].y*pPtList[i].y*pPtList[i].y;
        X1Y1 = X1Y1 + pPtList[i].x*pPtList[i].y;
        X1Y2 = X1Y2 + pPtList[i].x*pPtList[i].y*pPtList[i].y;
        X2Y1 = X2Y1 + pPtList[i].x*pPtList[i].x*pPtList[i].y;
    }

    N = lPtLen;
    C = N*X2 - X1*X1;
    D = N*X1Y1 - X1*Y1;
    E = N*X3 + N*X1Y2 - (X2+Y2)*X1;
    G = N*Y2 - Y1*Y1;
    H = N*X2Y1 + N*Y3 - (X2+Y2)*Y1;
    if (C*G-D*D!=0)
    {
        a = (H*D-E*G)/(C*G-D*D);
        b = (H*C-E*D)/(D*D-G*C);
        c = -(a*X1 + b*Y1 + X2 + Y2)/N;
    }
    else
    {
        //拟合不出来
        a = 0;
        b = 0;
        c = 0;
    }

    if (xc)
    {
        *xc=(MLong)(a/(-2));
    }
    if (yc)
    {
        *yc=(MLong)(b/(-2));
    }
    if (r)
    {
        *r=(MLong)(sqrt(a*a+b*b-4*c)/2);
    }
}

static void setMaskPic(cv::Mat &maskImage, MaskParam *pParam,MLong *Dist)
{
    MByte *pData;
    MLong lWidth, lHeight, lStride;
    MLong lExt;
    MyCircleParam outerCircle, innerCircle, tmpCircle;
    MLong lPoxY;
    MLong i,j;
    MPOINT *tmpPt;
    MPOINT pt1, pt2;
    MLong lDistance1, lDistance2;
    MDouble tmp;

    lWidth = maskImage.cols;
    lHeight = maskImage.rows;
    lStride = maskImage.step;
    lExt = lStride - lWidth;
    pData = (MByte*)maskImage.data;
    tmpPt = pParam->maskPt;

    switch(pParam->lMaskPtNum)
    {
        case 3:
        {
            FittingCircle(tmpPt, 3, &outerCircle.cx, &outerCircle.cy, &outerCircle.r);
            for (j=0; j<lHeight; j++, pData+=lExt)
            {
                for (i=0; i<lWidth; i++, pData++)
                {
                    tmp = (i - outerCircle.cx)*(i - outerCircle.cx)+(j - outerCircle.cy)*(j - outerCircle.cy);
                    lDistance1 = (MLong)(sqrt(tmp));
                    if (lDistance1 <= outerCircle.r)
                    {
                        *pData = 255;
                    }
                    else
                    {
                        *pData = 0;
                    }
                }
            }
            break;
        }
        case 5:
        {
            FittingCircle(tmpPt, 3, &outerCircle.cx, &outerCircle.cy, &outerCircle.r); //前面三点拟合圆
            for (j=0; j<lHeight; j++, pData+=lExt)
            {
                for (i=0; i<lWidth; i++, pData++)
                {
                    tmp = (i - outerCircle.cx)*(i - outerCircle.cx)+(j - outerCircle.cy)*(j - outerCircle.cy);
                    lDistance1 = (MLong)(sqrt(tmp));
                    if (lDistance1 <= outerCircle.r &&((i<tmpPt[3].x || i>tmpPt[4].x) || (j<tmpPt[3].y || j>tmpPt[4].y)))
                    {
                        *pData = 255;
                    }
                    else
                    {
                        *pData = 0;
                    }
                }
            }
            break;
        }
        case 6:
            {
                //为圆环
                FittingCircle(tmpPt, 3, &outerCircle.cx, &outerCircle.cy, &outerCircle.r);
                FittingCircle(tmpPt+3, 3, &innerCircle.cx, &innerCircle.cy, &innerCircle.r);
                if (outerCircle.r < innerCircle.r)
                {
                    tmpCircle = outerCircle;
                    outerCircle = innerCircle;
                    innerCircle = tmpCircle;
                }
                *Dist = outerCircle.r - innerCircle.r ;

                for (j=0; j<lHeight; j++, pData+=lExt)
                {
                    for (i=0; i<lWidth; i++, pData++)
                    {
                        tmp = (i - outerCircle.cx)*(i - outerCircle.cx)+(j - outerCircle.cy)*(j - outerCircle.cy);
                        lDistance1 = (MLong)(sqrt(tmp));
                        tmp = (i - innerCircle.cx)*(i - innerCircle.cx)+(j - innerCircle.cy)*(j - innerCircle.cy);
                        lDistance2 = (MLong)(sqrt(tmp));
                        if (lDistance1<=outerCircle.r && lDistance2>=innerCircle.r)
                        {
                            *pData = 255;
                        }
                        else
                        {
                            *pData = 0;
                        }
                    }
                }
                break;
            }

        case 7:
        {
            FittingCircle(tmpPt, 3, &outerCircle.cx, &outerCircle.cy, &outerCircle.r); //前面三点拟合圆
            for (j=0; j<lHeight; j++, pData+=lExt)
            {
                for (i=0; i<lWidth; i++, pData++)
                {
                    tmp = (i - outerCircle.cx)*(i - outerCircle.cx)+(j - outerCircle.cy)*(j - outerCircle.cy);
                    lDistance1 = (MLong)(sqrt(tmp));
                    if (lDistance1 <= outerCircle.r &&((i<tmpPt[3].x || i>tmpPt[4].x) || (j<tmpPt[3].y || j>tmpPt[4].y)) && ((i<tmpPt[5].x || i>tmpPt[6].x) || (j<tmpPt[5].y || j>tmpPt[6].y)))
                    {
                        *pData = 255;
                    }
                    else
                    {
                        *pData = 0;
                    }
                }
            }
            break;
        }
        case 8:
            {
                FittingCircle(tmpPt, 3, &outerCircle.cx, &outerCircle.cy, &outerCircle.r);
                FittingCircle(tmpPt+3, 3, &innerCircle.cx, &innerCircle.cy, &innerCircle.r);
                if (outerCircle.r < innerCircle.r)
                {
                    tmpCircle = outerCircle;
                    outerCircle = innerCircle;
                    innerCircle = tmpCircle;
                }
                pt1 = tmpPt[6];
                pt2 = tmpPt[7];
                lPoxY = (pt1.y + pt2.y)>>1;
                for (j=0; j<lHeight; j++, pData+=lExt)
                {
                    for (i=0; i<lWidth; i++, pData++)
                    {
                        tmp = (i - outerCircle.cx)*(i - outerCircle.cx)+(j - outerCircle.cy)*(j - outerCircle.cy);
                        lDistance1 = (MLong)(sqrt(tmp));
                        tmp = (i - innerCircle.cx)*(i - innerCircle.cx)+(j - innerCircle.cy)*(j - innerCircle.cy);
                        lDistance2 = (MLong)(sqrt(tmp));
                        if((i>pt1.x && i<pt2.x && j>pt1.y && j<pt2.y)||lDistance1>=outerCircle.r || lDistance2<=innerCircle.r)
                        {
                            *pData = 0;
                        }
                    }
                }

                /*for (j=0; j<lHeight; j++, pData+=lExt)
                {
                    for (i=0; i<lWidth; i++, pData++)
                    {
                        tmp = (i - outerCircle.cx)*(i - outerCircle.cx)+(j - outerCircle.cy)*(j - outerCircle.cy);
                        lDistance1 = (MLong)(sqrt(tmp));
                        tmp = (i - innerCircle.cx)*(i - innerCircle.cx)+(j - innerCircle.cy)*(j - innerCircle.cy);
                        lDistance2 = (MLong)(sqrt(tmp));
                        if (lDistance1>=outerCircle.r || (lDistance2<=innerCircle.r && j<=lPoxY))
                        {
                            *pData = 0;
                        }
                        else
                        {
                            *pData = 255;
                        }
                    }
                }*/

                break;
            }
        case 9:
        {
            FittingCircle(tmpPt, 3, &outerCircle.cx, &outerCircle.cy, &outerCircle.r); //前面三点拟合圆
            for (j=0; j<lHeight; j++, pData+=lExt)
            {
                for (i=0; i<lWidth; i++, pData++)
                {
                    tmp = (i - outerCircle.cx)*(i - outerCircle.cx)+(j - outerCircle.cy)*(j - outerCircle.cy);
                    lDistance1 = (MLong)(sqrt(tmp));
                    if (lDistance1 <= outerCircle.r &&((i<tmpPt[3].x || i>tmpPt[4].x) || (j<tmpPt[3].y || j>tmpPt[4].y))
                        && ((i<tmpPt[5].x || i>tmpPt[6].x) || (j<tmpPt[5].y || j>tmpPt[6].y)) && ((i<tmpPt[7].x || i>tmpPt[8].x) || (j<tmpPt[7].y || j>tmpPt[8].y)))
                    {
                        *pData = 255;
                    }
                    else
                    {
                        *pData = 0;
                    }
                }
            }
            break;
        }
        case 0:
        default:
            {
                for (j=0; j<lHeight; j++, pData+=lExt)
                {
                    for (i=0; i<lWidth; i++, pData++)
                    {
                        *pData = 255;
                    }
                }
                break;
            }
    }
}

int CMeterV2Object::invokeDetect(std::vector<Bbox_t> &result) {
    m_atlas->m_custom->SetCallBack(m_atlas->m_custom);
    m_atlas->m_custom->SetGrgument(engine_id,input_params);
    m_atlas->m_custom->GetData(result);
    Print(result);

    HIAI_StatusT status=m_atlas->m_custom->Destory();
    if (status != HIAI_OK) {
        ERROR_LOG("Failed to call atlas->m_custom->Destory(), ret=%d.", status);
        return kHiaiInitFailed;
    }

    return 0;
}
int CMeterV2Object::invokeDetect(const cv::Mat &image, RESULT_VECTOR &results)
{
#if 1
    int res = 0;
    int maskIndex = 0;
    cv::Mat testImage;
    cv::Mat maskImage;
    HYAMR_IMAGES wy_testImage = {0};
    HYAMR_IMAGES wy_maskImage = {0};
    cv::Size ImgSize;

    HYAMR_INTERACTIVE_PARA inPara;
    memset(&inPara, 0, sizeof(inPara));
    inPara.Dist = 0xFFFF;

    MaskParam maskParam;
    HYAMR_READ_PARA pReadPara[2] = {0};
    MDouble matchRate = 0.45;
    MDouble dResult = 0.0;

    // check data
    if (!hHYMRDHand || !trainInfo._dbFlag)
        return -1;

    float fScale = trainInfo._size._scale;
    // check resolution
    if (std::abs(trainInfo._size._width - image.cols) > 10 ||
            std::abs(trainInfo._size._height - image.rows) > 10)
    {
        LogError("resolution error, train[%d*%d], image[%d*%d]\n",
                    trainInfo._size._width, trainInfo._size._height,
                    image.cols, image.rows);
        res = -1;
        goto EXT;
    }

    ImgSize.height = image.rows / fScale;
    ImgSize.width = image.cols / fScale;

    // Get Tareget Info From memory
    if(0 != HYAMR_GetTaregetInfoFromMem(hHYMRDHand, &_curTargetVec[0], &_trnTargetVec[0], ImgSize.width, ImgSize.height))
    {
        LogError("HYAMR_GetTargetInfo error!");
        res = -1;
        goto EXT;
    }

    // maskImage
    maskImage = cv::Mat(ImgSize, CV_8UC1);
    maskParam.lMaskPtNum = trainInfo._analyseArea.size() + trainInfo._maskArea.size();
    maskIndex = 0;
    for (int i = 0; i<trainInfo._analyseArea.size(); i++)
    {
        maskParam.maskPt[maskIndex].x = trainInfo._analyseArea[i]._x/fScale;
        maskParam.maskPt[maskIndex].y = trainInfo._analyseArea[i]._y/fScale;
        maskIndex++;
    }
    for (int i = 0; i<trainInfo._analyseArea.size(); i++)
    {
        maskParam.maskPt[maskIndex].x = trainInfo._maskArea[i]._x/fScale;
        maskParam.maskPt[maskIndex].y = trainInfo._maskArea[i]._x/fScale;
        maskIndex++;
    }
    setMaskPic(maskImage, &maskParam, &inPara.Dist);

    wy_maskImage.lWidth = maskImage.cols;
    wy_maskImage.lHeight = maskImage.rows;
    wy_maskImage.pixelArray.chunky.lLineBytes = maskImage.step;
    wy_maskImage.pixelArray.chunky.pPixel = maskImage.data;
    wy_maskImage.lPixelArrayFormat = HYAMR_IMAGE_GRAY;

    //////////////////
    TrainInfo2InterParam(trainInfo, inPara);
    OutputInterParam(inPara);
    res = HYAMR_SetParam(hHYMRDHand, &wy_maskImage, &inPara);
    if (0 != res)
    {
        LogError("HYAMR_SetParam error: %d!", res);
        res = -1;
        goto EXT;
    }

    // testImage
    testImage = cv::Mat(ImgSize, CV_8UC3);
    cv::resize(image, testImage, ImgSize);
    //
    wy_testImage.lWidth = testImage.cols;
    wy_testImage.lHeight = testImage.rows;
    wy_testImage.pixelArray.chunky.lLineBytes = testImage.step;
    wy_testImage.pixelArray.chunky.pPixel = testImage.data;
    wy_testImage.lPixelArrayFormat = HYAMR_IMAGE_BGR;

    //t1 = clock();
    // Recognise the pointer line
    if (0 != HYAMR_GetLineParam(hHYMRDHand, &wy_testImage, &inPara, const_cast<char *>("OK"), matchRate, &_curTargetVec[0], &_trnTargetVec[0], pReadPara))
    {
        LogError("HYAMR_GetLineParam error!");
        res = -1;
        goto EXT;
    }
    // printf("Time1 cost(ms): %ld.\n", ((long)(clock() - t1) * 1000 / CLOCKS_PER_SEC));
    for (int i=0; i<inPara.lPtNum; i++)
    {
        pReadPara[0].ptInfo[i].ptVal = pReadPara[1].ptInfo[i].ptVal = inPara.dPtValList[i];
    }
    //t1 = clock();
    for (int i=0; i<inPara.lLineNum; i++)
    {
        HYAMR_GetMeterResult(hHYMRDHand, *(pReadPara+i), &dResult);
        LogDebug("result=%.4f", dResult);
        AnalyseResultInfo resInfo;
        resInfo.dValue = dResult;
        results.push_back(resInfo);
    }
    //printf("Time2 cost(ms): %ld.\n", ((long)(clock() - t1) * 1000 / CLOCKS_PER_SEC));
    res = 0;
EXT:
    return res;
#else


#endif
}
void CMeterV2Object::Print(std::vector<Bbox> &bbox){
    if(0==bbox.size()){
        return;
    }
    for(auto box: bbox){
        cout<<"score: "<<box.score<<" ,classname: "<<box.classname<<" Rect[ "<<box.Left_up.x<<", "<<box.Left_up.y
            <<", "<<box.Right_down.x<<", "<<box.Right_down.y<<"]"<<endl;
    }
}
int CMeterV2Object::invokeUninit()
{
#if 0
    if (hHYMRDHand) {
        HYAMR_Uninit(hHYMRDHand);
        hHYMRDHand = MNull;
    }
    if (hMemMgr) {
        HYAMR_MemMgrDestroy(hMemMgr);
        hMemMgr = MNull;
    }
    if (pMem) {
        free(pMem);
        pMem = MNull;
    }
    return 0;
#else
//    4 Destory
//    HIAI_StatusT status=m_atlas->m_custom->Destory();
//    if (status != HIAI_OK) {
//        ERROR_LOG("Failed to call atlas->m_custom->Destory(), ret=%d.", status);
//        return kHiaiInitFailed;
//    }
    return 0;
#endif

}

