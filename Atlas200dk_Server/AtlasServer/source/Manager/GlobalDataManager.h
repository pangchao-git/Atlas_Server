/*
 * @Description  : 
 * @Version      : 1.0.0
 * @Author       : mdf
 * @Date         : 2020-05-08 15:29:13
 * @LastEditors  : mdf
 * @LastEditTime : 2020-05-11 16:23:33
 */
#ifndef GLOBAL_DATA_MANAGER_H
#define GLOBAL_DATA_MANAGER_H
#include <map>

typedef std::map<std::string, std::string> MAP_PARAMETER;
class GlobalDataManager
{
private:
    GlobalDataManager() = default;
    GlobalDataManager(const GlobalDataManager &) = delete;
    GlobalDataManager& operator =(const GlobalDataManager &) = delete;
public:
    static GlobalDataManager *GetInstance();
public:
    std::string GetParameter(const std::string &stKey, const std::string &stSubKey, const std::string &stDefValue);
public:
    bool ReadFromXml(const char *stXml);
    void WriteToXml(const char *stXml);
private:
    void Insert2ParameterMap(const char *szKey, const char *szSubKey, const char *szValue);
private:
    MAP_PARAMETER mapParameter;
};

#define theDataManager (GlobalDataManager::GetInstance())

#endif // GLOBAL_DATA_H
