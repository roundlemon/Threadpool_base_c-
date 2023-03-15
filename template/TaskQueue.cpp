#include "TaskQueue.h"
template<class T>
TaskQueue<T>::TaskQueue()
{
    pthread_mutex_init(&taskmutex, nullptr);
}

template<class T>
TaskQueue<T>::~TaskQueue()
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
template<class T>
void TaskQueue<T>::addTask(Task<T> &task)
{
    pthread_mutex_lock(&taskmutex);
    taskQ.push(task);
    pthread_mutex_unlock(&taskmutex);
}

template<class T>
void TaskQueue<T>::addTask(callback func, void *argp)
{
    pthread_mutex_lock(&taskmutex);
    Task<T> t;
    t.function = func;
    t.arg = argp;
    taskQ.push(t);
    pthread_mutex_unlock(&taskmutex);
}
// 取出任务
template<class T>
Task<T> TaskQueue<T>::takeTask()
{
    Task<T> t;
    pthread_mutex_lock(&taskmutex);
    if (taskQ.size() > 0)
    {
        t = taskQ.front();
        taskQ.pop();
    }
    pthread_mutex_unlock(&taskmutex);
    return t;
}