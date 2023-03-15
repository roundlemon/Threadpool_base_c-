#include "TaskQueue.h"
TaskQueue::TaskQueue()
{
    pthread_mutex_init(&taskmutex, nullptr);
}

TaskQueue::~TaskQueue()
{
    // for (int i = 0; i < taskQ.size(); i++)
    // {
    //     Task<T> *t = taskQ.front();
    //     taskQ.pop();
    //     if (t)
    //         delete t;
    // }
    pthread_mutex_destroy(&taskmutex);
}

// 添加任务
void TaskQueue::addTask(Task &task)
{
    pthread_mutex_lock(&taskmutex);
    taskQ.push(task);
    pthread_mutex_unlock(&taskmutex);
}

void TaskQueue::addTask(callback func, void *argp)
{
    pthread_mutex_lock(&taskmutex);
    Task t;
    t.function = func;
    t.arg = argp;
    taskQ.push(t);
    pthread_mutex_unlock(&taskmutex);
}
// 取出任务
Task TaskQueue::takeTask()
{
    Task t;
    pthread_mutex_lock(&taskmutex);
    if (taskQ.size() > 0)
    {
        t = taskQ.front();
        taskQ.pop();
    }
    pthread_mutex_unlock(&taskmutex);
    return t;
}