/*
 * @Description  : 
 * @Version      : 1.0.0
 * @Author       : mdf
 * @Date         : 2020-05-11 16:12:38
 * @LastEditors  : mdf
 * @LastEditTime : 2020-05-11 16:23:24
 */
#ifndef FILE_MANAGER_H
#define FILE_MANAGER_H
#include <string>

class FileManager
{
private:
    FileManager() = default;
    FileManager(const FileManager &) = delete;
    FileManager& operator=(const FileManager &) = delete;
public:
    static FileManager *GetInstance();

public:
    std::string GetFileName(const std::string &deviceid, const std::string &picId,
                            const std::string &taskid, const std::string &dirPath="", int alarmType = 0);
    bool FileRead(const std::string &fileName, std::string &fileData);
    bool FileWrite(const std::string &fileName, const char *fileData, long fileSize);
    bool CreateDirectory(const std::string &path);
};

#define theFileManager (FileManager::GetInstance())
#endif
