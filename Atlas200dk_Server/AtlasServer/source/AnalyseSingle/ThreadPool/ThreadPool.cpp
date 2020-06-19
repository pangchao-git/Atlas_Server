#include "ThreadPool.h"
#include "Task.h"
#include <iostream>
using namespace std;

ThreadPool::ThreadPool(int thdNums) : ThreadPool(thdNums, false) {
}

ThreadPool::ThreadPool() : ThreadPool(5, false) {
}

//添加任务
void ThreadPool::addTask(TaskPtr f){

    std::cout << "addTask" << std::endl;
    if(isRunning){
        //保护共享资源
        unique_lock<mutex> lk(_mutex);

        //给队列中添加任务
        tasks.push(f);

        std::cout<<"tasks size:"<< tasks.size() << std::endl;
        _cond.notify_one();
    }
}

void ThreadPool::stop(){

    {
        // stop thread pool, should notify all threads to wake
        unique_lock<mutex> lk(_mutex);
        isRunning = false;
        _cond.notify_all(); // must do this to avoid thread block
    }
    // terminate every thread job
    for (std::thread& t : threads)
    {
        if (t.joinable())
            t.join();
    }		
}

void ThreadPool::runTask(){

    //不断遍历队列,判断要是有任务的话,就执行
    while(isRunning){

        TaskPtr task;
        {
            unique_lock<mutex> lk(_mutex);
            if (!tasks.empty()) {
                std::cout << "get a task" << std::endl;
                task = move(tasks.top());                
                tasks.pop();
            }
            else if (isRunning && tasks.empty()) {
                _cond.wait(lk);
            }
            // cond.wait(lk, [=]{return (!isRunning || !tasks.empty());});
            // if(!isRunning && tasks.empty())   
            //     return;

            // task = std::move(this->tasks.top());   
            // tasks.pop();
        }
        if (task) {
            task->DoWork();
        }
    }
}

void ThreadPool::setThreadNums(int thdNums)
{
    if (thdNums > 0 && thdNums < 100)
        _thread_num = thdNums;
}

void ThreadPool::start(int thdNums){
    if (isRunning)
        stop();
    //
    {
        unique_lock<mutex> lk(_mutex);
        setThreadNums(thdNums);
        isRunning = true;
    }
    std::cout<<"ThreadPool start, num: "<< _thread_num << std::endl;
    for(int i=0;i<_thread_num;i++){
        threads.emplace_back(thread(&ThreadPool::runTask,this));
    }
}

ThreadPool::~ThreadPool(){
    if (isRunning)
		stop();
}
