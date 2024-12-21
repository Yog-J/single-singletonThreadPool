//
// Created by Lenovo on 2024/12/21.
//

#ifndef THREADPOOL_THREADPOOL_H
#define THREADPOOL_THREADPOOL_H

#include <condition_variable>
#include "thread"
#include "queue"
#include "vector"
#include "functional"
#include "iostream"
#include "mutex"

class ThreadPool{
public:
    ThreadPool() {
        this->m_threadsum = 0;
        this->m_threadflag = false;
    }
    ThreadPool(int mThreadsum, bool mThreadflag = true) : m_threadsum(mThreadsum), m_threadflag(mThreadflag) {
        for (int i = 0; i < m_threadsum; ++i) {
            threads.emplace_back(std::thread([this, &i]{
                while(true)
                {
                    std::unique_lock<std::mutex> uniqueLock(mtx);
                    conditionVariable.wait(uniqueLock,[this]{return !tasks.empty() || !m_threadflag; });
                    if(tasks.empty() || m_threadflag == false)
                    {
                        return ;
                    }
                    std::function<void()> task(std::move(tasks.front()));
                    task();
                    printf("Thread %d has completed task\n", std::this_thread::get_id());
                    uniqueLock.unlock();
                    tasks.pop();
                }
            }));
        }
        std::cout << "Thread pool created with " << m_threadsum << " threads." << std::endl;
    }

    virtual ~ThreadPool() {
        {
            std::unique_lock<std::mutex> uniqueLock(mtx);
            m_threadflag = false;
        }
        conditionVariable.notify_all();
        for (auto& t: threads) {
                t.join();
        }

    }

    template<class T, class...Args>
    void enqueue(T&& t,Args... args)
    {
        std::function<void()> task(std::bind(std::forward<T>(t),std::forward<Args>(args)...));
        {
            std::unique_lock<std::mutex>uniqueLock(mtx);
            tasks.emplace(std::move(task));
        }
        conditionVariable.notify_one();
    }
private:
    int m_threadsum;
    bool m_threadflag;
    std::vector<std::thread> threads;
    std::queue<std::function<void()>> tasks;
    std::mutex mtx;
    std::condition_variable conditionVariable;
};
#endif //THREADPOOL_THREADPOOL_H
