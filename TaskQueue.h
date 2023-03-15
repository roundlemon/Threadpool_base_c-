#ifndef _TASK_POOL_HPP_
#define _TASK_POOL_HPP_
#include <pthread.h>
#include <queue>
using namespace std;

using callback = void (*)(void *);
struct Task
{
    Task()
    {
        function = nullptr;
        arg = nullptr;
    }
    Task(callback f, void *argp)
    {
        function = f;
        this->arg = argp;
    }
    callback function;
    void *arg;
};

class TaskQueue
{
public:
    TaskQueue();
    ~TaskQueue();

    // 添加任务
    void addTask(Task &task);
    void addTask(callback func, void *argp);
    // 取出任务
    Task takeTask();
    // 获取任务数
    inline int taskNumber()
    {
        return taskQ.size();
    }

private:
    pthread_mutex_t taskmutex;
    queue<Task> taskQ;
};

#endif