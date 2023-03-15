#ifndef _TASK_POOL_HPP_
#define _TASK_POOL_HPP_
#include <pthread.h>
#include <queue>
using namespace std;

using callback = void (*)(void *);
template<class T>
struct Task
{
    Task<T>()
    {
        function = nullptr;
        arg = nullptr;
    }
    Task<T>(callback f, void *argp)
    {
        function = f;
        arg = (T*) argp;
    };
    callback function;
    T *arg;
};

template<class T>
class TaskQueue
{
public:
    TaskQueue();
    ~TaskQueue();

    // 添加任务
    void addTask(Task<T>& task);
    void addTask(callback func, void *argp);
    // 取出任务
    Task<T> takeTask();
    // 获取任务数
    inline int taskNumber()
    {
        return taskQ.size();
    }

private:
    pthread_mutex_t taskmutex;
    queue<Task<T>> taskQ;
};

#endif