#include "TaskContainer.h"
#include "Task.h"

TaskContainer::TaskContainer()
{
	task_max_size_ = TASK_CONTAINER_MAX_SIZE;
}

TaskContainer::~TaskContainer()
{
}

void TaskContainer::push(TaskPtr t)
{
	task_container_.emplace(t);
}

TaskPtr TaskContainer::top()
{
	return task_container_.top();
}

void TaskContainer::pop()
{
	task_container_.pop();
}

bool TaskContainer::empty()
{
    return task_container_.empty();
}

bool TaskContainer::full()
{
	return (task_container_.size() >= task_max_size_);
}

void TaskContainer::setCapacity(int size)
{
	task_max_size_ = size;
}

std::priority_queue<TaskPtr>::size_type TaskContainer::size()
{
	return task_container_.size();
}
