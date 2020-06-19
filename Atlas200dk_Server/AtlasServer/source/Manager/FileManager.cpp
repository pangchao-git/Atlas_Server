/*
 * @Description  : file manager
 * @Version      : 1.0.0
 * @Author       : mdf
 * @Date         : 2020-05-11 16:13:13
 * @LastEditors  : mdf
 * @LastEditTime : 2020-05-11 17:14:14
 */
#include "FileManager.h"
#include <boost/filesystem.hpp>
#include <fstream>

using namespace std;
namespace fs = boost::filesystem;

FileManager *FileManager::GetInstance()
{
    static FileManager info;
    return &info;
}

std::string FileManager::GetFileName(const std::string &deviceid, const std::string &picId,
                                     const std::string &taskid, const std::string &dirPath, int alarmType)
{
    std::string stFileName = dirPath.empty()?".":dirPath;
    stFileName.append("/" + std::to_string(alarmType));
    stFileName.append("_" + deviceid);
    if (!picId.empty())
        stFileName.append("_" + picId);
    if (!taskid.empty())
        stFileName.append("_" + taskid);
    stFileName.append(".jpg");
    return stFileName;
}

bool FileManager::FileRead(const std::string &fileName, std::string &fileData)
{
    if (fileName.empty()) {
        return false;
    }

    std::ifstream streamHandle;
    if (!fs::exists(fileName)) {
        return false;
    }

    streamHandle.open(fileName, std::ios::in|std::ios::binary|std::ios::ate);
    if(streamHandle)
    {
        long size = streamHandle.tellg();
        fileData.resize(size);

        streamHandle.seekg(0, std::ios::beg);
        streamHandle.read((char *)fileData.c_str(), size);
        streamHandle.close();
    }

    return true;
}

bool FileManager::FileWrite(const std::string &fileName, const char *fileData, long fileSize)
{
    if (fileName.empty()) {
        return false;
    }

    int pos = fileName.find_last_of("/");
    if (pos != std::string::npos)
    {
        std::string dirName;
        dirName = fileName.substr(0, pos);
        fs::create_directories(dirName);
    }
    std::ofstream streamHandle;
    streamHandle.open(fileName, std::ios::out|std::ios::binary);
    if(streamHandle) {
        streamHandle.write(fileData, fileSize);
    }
    streamHandle.close();
    return true;
}

bool FileManager::CreateDirectory(const string &path)
{
    if (path.empty()) {
        return false;
    }
    return fs::create_directories(path);
}
