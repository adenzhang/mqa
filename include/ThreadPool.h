#pragma once
#include <mqa_shared.h>
#include <boost/function.hpp>

class MQA_SHARED ThreadPool
{
    ThreadPool(ThreadPool&){}
    void operator=(ThreadPool&){}
public:
    typedef void (*FUNCTION_T)(void *);

    /// @brief Constructor.
    ThreadPool( size_t thread_size, size_t task_size=0);

    /// @brief Destructor.
    ~ThreadPool();

    bool post(FUNCTION_T pFun, void *pParam=NULL);

    size_t task_count(); // count of pending tasks
    size_t working_count(); // count of current working tasks
protected:
    void *priv;
};
