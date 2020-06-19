#include "AnalyseCommon.h"

////////////////////////////////////////////////////
/// 类CAnalyseBase实现
///

CAnalyseBase::CAnalyseBase()
{
    _modeType = 0;
    _analyseType = 0;
    _comType = 0;
    _resultCode = 200;
}

CAnalyseBase::CAnalyseBase(int mode)
{
    _modeType = mode;
    _analyseType = 0;
    _comType = 0;
    _resultCode = 200;
}

std::string CAnalyseBase::GetElementId()
{
    if (_elementId.empty()) {
        MakeElementId();
    }
    return _elementId;
}

void CAnalyseBase::MakeElementId()
{
    if (_deviceId.empty() || _analyseType < 0)
        return;

    _elementId = _deviceId + "_" + std::to_string(_analyseType);
}
