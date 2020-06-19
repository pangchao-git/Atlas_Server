#pragma once
#include "TaskContainer.h"
#include <thread>
#include <mutex>
#include <condition_variable>
#include <vector>

class ThreadPool
{
public:
    ThreadPool();
    ThreadPool(int thdNums);
    ~ThreadPool();
    
public:
    void addTask(TaskPtr task);
    void start(int thdNums);
	void stop();
    void runTask();
private:
    ThreadPool(int thdNums, bool bR) : _thread_num(thdNums), isRunning(bR) {}
    void setThreadNums(int thdNums);
private:
    size_t _thread_num;
    //线程数组
    std::vector<std::thread> threads;    

    //互斥锁条件变量
    std::mutex _mutex ;
    std::condition_variable _cond;
    
    //线程池工作时为真
    bool isRunning;
    
	//任务队列
	TaskContainer tasks;
};
