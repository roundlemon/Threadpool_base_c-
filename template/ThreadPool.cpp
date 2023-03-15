#include "TaskQueue.h"
#include "ThreadPool.h"
#include "TaskQueue.cpp"
#include <unistd.h>
#include <pthread.h>
#include <iostream>
#include <string.h>
const int NUMBER = 2;
template <class T>
ThreadPool<T>::ThreadPool(int minn, int maxx)
{
    m_taskQ = new TaskQueue<T>;
    do
    {

        minnum = minn;
        maxnum = maxx;
        busynum = 0;
        alivenum = minn;
        workerIDs = new pthread_t[maxnum];
        if (workerIDs == nullptr)
        {
            cout << "malloc thread_t[] 失败...." << endl;
            ;
            break;
        }
        memset(workerIDs, 0, sizeof(pthread_t) * maxnum);
        if (pthread_cond_init(&notEmpty, nullptr) != 0 ||
            pthread_mutex_init(&poolmutex, nullptr) != 0)
        {
            cout << "init lock failed..." << endl;
            break;
        }

        for (int i = 0; i < minnum; i++)
        {
            pthread_create(&workerIDs[i], nullptr, worker, this);
            cout << "创建子线程, ID: " << to_string(workerIDs[i]) << endl;
        }

        pthread_create(&managerID, nullptr, manager, this);
        cout << "管理者线程已经创建..." << endl;

    } while (0);
}
template <class T>
ThreadPool<T>::~ThreadPool()
{
    shutdown = true;
    pthread_join(managerID, nullptr);
    for (int i = 0; i < alivenum; i++)
    {
        pthread_cond_signal(&notEmpty);
    }
    if (workerIDs)
        delete[] workerIDs;
    if (m_taskQ)
        delete m_taskQ;
    pthread_cond_destroy(&notEmpty);
    pthread_mutex_destroy(&poolmutex);
}
template <class T>
void ThreadPool<T>::addtask(Task<T> t)
{
    if (shutdown)
        return;
    m_taskQ->addTask(t);
    pthread_cond_signal(&notEmpty);
}
template <class T>
int ThreadPool<T>::getbusynum()
{
    int bn = 0;
    pthread_mutex_lock(&poolmutex);
    bn = busynum;
    pthread_mutex_unlock(&poolmutex);
    return bn;
}
template <class T>
int ThreadPool<T>::getalivenum()
{
    int an = 0;
    pthread_mutex_lock(&poolmutex);
    an = alivenum;
    pthread_mutex_unlock(&poolmutex);
    return an;
}
template <class T>
void *ThreadPool<T>::worker(void *arg)
{
    ThreadPool *pool = static_cast<ThreadPool *>(arg);
    while (1)
    {
        pthread_mutex_lock(&pool->poolmutex);
        while (pool->m_taskQ->taskNumber() == 0 && !pool->shutdown)
        {
            cout << "thread " << to_string(pthread_self()) << " waiting..." << endl;
            // 阻塞线程
            pthread_cond_wait(&pool->notEmpty, &pool->poolmutex);

            if (pool->exitnum > 0)
            {
                pool->exitnum--;
                if (pool->alivenum > pool->minnum)
                {
                    pool->alivenum--;
                    pthread_mutex_unlock(&pool->poolmutex);
                    pool->threadexit();
                }
            }
        }
        if (pool->shutdown)
        {
            pthread_mutex_unlock(&pool->poolmutex);
            pool->threadexit();
        }
        Task<T> t;
        t = pool->m_taskQ->takeTask();
        pool->busynum++;
        pthread_mutex_unlock(&pool->poolmutex);

        cout << "thread " << to_string(pthread_self()) << " start working..." << endl;
        t.function(t.arg);

        delete t.arg;
        t.arg = nullptr;
        cout << "thread " << to_string(pthread_self()) << " end working...";

        pthread_mutex_lock(&pool->poolmutex);
        pool->busynum--;
        pthread_mutex_unlock(&pool->poolmutex);
    }
    return nullptr;
}

template <class T>
void *ThreadPool<T>::manager(void *arg)
{
    ThreadPool *pool = static_cast<ThreadPool *>(arg);
    while (!pool->shutdown)
    {
        sleep(3);
        cout << "manager start work!" << endl;
        
        pthread_mutex_lock(&pool->poolmutex);
        int quesz = pool->m_taskQ->taskNumber();
        pthread_mutex_unlock(&pool->poolmutex);
        int busyn = pool->getbusynum();
        int aliven = pool->getalivenum();
        // 创建
        // aliven<quesz && aliven<maxNum
        if (aliven < quesz && aliven < pool->maxnum)
        {
            cout << "add thread!" << endl;
            pthread_mutex_lock(&pool->poolmutex);
            int counter = 0;
            for (int i = 0; i < pool->maxnum && pool->alivenum < pool->maxnum && counter < NUMBER; i++)
            {
                if (pool->workerIDs[i] == 0)
                {
                    pthread_create(&pool->workerIDs[i], nullptr, worker, pool);
                    counter++;
                    pool->alivenum++;
                    cout << "添加线程：" << to_string(pthread_self()) << " 现在存活线程数：" << pool->alivenum << endl;
                }
            }
            pthread_mutex_unlock(&pool->poolmutex);
        }
        // 销毁
        //
        if (busyn * 2 < aliven && aliven > pool->minnum)
        {
            cout << "destory thread!" << endl;
            pthread_mutex_lock(&pool->poolmutex);
            pool->exitnum = NUMBER;
            pthread_mutex_unlock(&pool->poolmutex);
            for (int i = 0; i < NUMBER; i++)
            {
                pthread_cond_signal(&pool->notEmpty);
            }
        }
    }
    return nullptr;
}

template <class T>
void ThreadPool<T>::threadexit()
{
    pthread_t tid = pthread_self();
    for (int i = 0; i < maxnum; i++)
    {
        if (workerIDs[i] == tid)
        {
            workerIDs[i] = 0;
            cout << "threadExit() function: thread "
                 << to_string(pthread_self()) << " exiting..." << endl;
            break;
        }
    }
    pthread_exit(nullptr);
}