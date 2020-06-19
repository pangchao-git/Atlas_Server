#include "dev_system.h"
#include <chrono>
#include <sys/time.h>
#include <time.h>

using namespace std;

int CurrentDateTime(string &timeStr, string &timeZone)
{
    time_t now = chrono::system_clock::to_time_t(chrono::system_clock::now());
    char buf[100] = {0};
    struct tm tm_now;
    localtime_r(&now, &tm_now);
    std::strftime(buf, sizeof(buf), "%Y-%m-%dT%H:%M:%SZ", &tm_now);
    timeStr = string(buf);

    std::strftime(buf, sizeof(buf), "GMT%z", &tm_now);
    timeZone = string(buf);
    return 0;
}

bool set_local_time(struct tm& t)
{
     //将struct tm结构时间转换成GMT时间time_t
     time_t st = mktime(&t);
     if(st==-1)
         return false;

     if(!stime(&st))
         return true;
     else
         return false;
}

