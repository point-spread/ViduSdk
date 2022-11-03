#pragma once
#include <chrono>
#include <functional>
#include <mutex>

class scheduleTask
{
  private:
    std::mutex mtx;
    std::function<bool(void)> taskFunc;
    std::chrono::time_point<std::chrono::system_clock> nextTp;
    std::chrono::duration<int, std::milli> duration;

  public:
    enum TASK_ERR_CODE
    {
        UNTRIGERED = 0,
        SUCCESS = 1,
        ERROR = 2,
    };
    scheduleTask(const std::function<bool(void)> &_taskFunc, const std::chrono::duration<int, std::milli> &_duration)
        : taskFunc(std::move(_taskFunc)), nextTp(std::chrono::system_clock::now()), duration(_duration)
    {
    }

    bool op()
    {
        return taskFunc();
    }

    TASK_ERR_CODE operator()()
    {
        std::unique_lock<std::mutex> lock(mtx, std::try_to_lock);
        if (lock.owns_lock())
        {
            auto tp = std::chrono::system_clock::now();
            if (tp > nextTp)
            {
                bool ret = op();
                nextTp = tp + duration;
                return ret ? SUCCESS : ERROR;
            }
        }
        return UNTRIGERED;
    }
};
