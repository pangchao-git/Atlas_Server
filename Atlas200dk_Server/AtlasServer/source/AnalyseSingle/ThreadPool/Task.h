#pragma once

namespace
{
	enum PRIORITY
	{
		MIN = 1, NORMAL = 25, MAX = 50
	};
}

class CTask
{
public:
        CTask()
	{
		priority_ = PRIORITY::NORMAL;
	}
        CTask(int priority)
	{
		SetPriority(priority);
	}
        bool operator<(const CTask& obA) const
	{
		return priority_ < obA.priority_; //大顶堆
	}
	void SetPriority(int priority)
	{
        if (priority>(PRIORITY::MAX))
        {
			priority = (PRIORITY::MAX);
		}
        else if (priority<(PRIORITY::MIN))
		{
			priority = (PRIORITY::MIN);
		}
	}	
        virtual void DoWork() = 0;
protected:
	int priority_;
};
