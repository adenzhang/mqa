#pragma once
#include <mqa/mqa_shared.h>
#include <cstddef>

class MQA_SHARED ThreadPool
{
    ThreadPool(ThreadPool&){}
    void operator=(ThreadPool&){}
public:
    typedef void (*FUNCTION_T)(void *);

    /// @brief Constructor.
    ThreadPool( std::size_t thread_size, std::size_t task_size=0);

    /// @brief Destructor.
    ~ThreadPool();

    template<typename FUNCTOR_T>
    bool post(FUNCTOR_T& functor){
        typedef void(FUNCTOR_T::* MEMBER_FUNC_T)();
        #define CALL_MEMBER_FN(object,ptrToMember)  ((object).*(ptrToMember))

        struct My {
            FUNCTOR_T *pObj;
            MEMBER_FUNC_T pFunc; 

            My(FUNCTOR_T *obj, MEMBER_FUNC_T func): pObj(obj), pFunc(func) {}
            static void dowork(void *p) {
                //printf("My::dowork\n");
                CALL_MEMBER_FN(*(((My*)p)->pObj), ((My*)p)->pFunc)();
                delete p;
            }
        };
        return post( My::dowork, new My(&functor, &FUNCTOR_T::operator()) );
    }

    bool post(FUNCTION_T pFun, void *pParam);


    std::size_t task_count(); // count of pending tasks
    std::size_t working_count(); // count of current working tasks
protected:
    void *priv;
};
