#ifndef LOCKER_H
#define LOCKER_H

#include <exception>// 异常处理
#include <pthread.h>// 线程
#include <semaphore.h>// 信号量

///RAII机制
//在C++中，信号量（Semaphore）和条件变量（Condition Variable）是用于线程间同步的两种不同的机制。
// 封装信号量的类
class sem
{
public:
    sem()
    {
        if (sem_init(&m_sem, 0, 0) != 0) // 初始化信号量
        {
            throw std::exception();
        }
    }
    sem(int num)
    {
        if (sem_init(&m_sem, 0, num) != 0) // 初始化信号量
        {
            throw std::exception();
        }
    }
    ~sem()
    {
        sem_destroy(&m_sem);// 销毁信号量
    }
    bool wait()
    {
        return sem_wait(&m_sem) == 0;// 原子操作方式信号量减一，信号量为0时,sem_wait阻塞
    }
    bool post()
    {
        return sem_post(&m_sem) == 0;// 信号量加一，信号量大于0时，唤醒调用sem_post的线程
    }

private:
    sem_t m_sem;
};
// 封装互斥锁的类
class locker
{
public:
    locker()
    {
        if (pthread_mutex_init(&m_mutex, NULL) != 0)// 初始化互斥锁
        {
            throw std::exception();
        }
    }
    ~locker()
    {
        pthread_mutex_destroy(&m_mutex);// 销毁互斥锁
    }
    bool lock()
    {
        return pthread_mutex_lock(&m_mutex) == 0;// 加锁
    }
    bool unlock()
    {
        return pthread_mutex_unlock(&m_mutex) == 0;
    }
    pthread_mutex_t *get()
    {
        return &m_mutex;
    }

private:
    pthread_mutex_t m_mutex;
};
// 封装条件变量的类
class cond
{
public:
    cond()
    {
        if (pthread_cond_init(&m_cond, NULL) != 0)// 初始化条件变量
        {
            //pthread_mutex_destroy(&m_mutex);
            throw std::exception();
        }
    }
    ~cond()
    {
        pthread_cond_destroy(&m_cond);// 销毁条件变量
    }
    bool wait(pthread_mutex_t *m_mutex)
    {
        int ret = 0;
        //pthread_mutex_lock(&m_mutex);
        ret = pthread_cond_wait(&m_cond, m_mutex);/// 
        //pthread_mutex_unlock(&m_mutex);
        return ret == 0;
    }
    bool timewait(pthread_mutex_t *m_mutex, struct timespec t)
    {
        int ret = 0;
        //pthread_mutex_lock(&m_mutex);
        ret = pthread_cond_timedwait(&m_cond, m_mutex, &t);// 
        //pthread_mutex_unlock(&m_mutex);
        return ret == 0;
    }
    bool signal()// 唤醒一个等待目标条件变量的线程
    {
        return pthread_cond_signal(&m_cond) == 0;
    }
    bool broadcast()// 唤醒所有等待目标条件变量的线程
    {
        return pthread_cond_broadcast(&m_cond) == 0;
    }

private:
    //static pthread_mutex_t m_mutex;
    pthread_cond_t m_cond;
};
#endif
