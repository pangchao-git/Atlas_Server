#pragma once
#include <queue>
#include <memory>
#include <functional>

#define TASK_CONTAINER_MAX_SIZE	500

class CTask;
typedef std::shared_ptr<CTask> TaskPtr;

class TaskContainer
{
public:
	TaskContainer();
	~TaskContainer();
	void push(TaskPtr task);
	TaskPtr top();
	void pop();
	bool empty();
	bool full();
	void setCapacity(int size);
	std::priority_queue<TaskPtr>::size_type size();
private:
	std::priority_queue<TaskPtr> task_container_;
	int task_max_size_;
};

