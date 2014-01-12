#include "create_thread.h"



#ifdef USE_WINDOWS_THREADS

#endif

#ifdef USE_PTHREAD_THREADS
    #include <pthread.h>
#endif

#ifdef USE_TBB_THREADS

#endif

/////
// TODO: Fallback to something else if <functional> is not available.

#include <functional>

typedef void(*voidFunc)(void);

using namespace FJ::Thread;

/////
// A wrapper to the specified thread backend.

template<typename R, typename ...A>thread_t CreateThread(R(*ThreadFunc)(A...), A ...args){
    std::function<R(A...)>func(ThreadFunc);

    //ThreadFunc(std::forward<A> (args)...);

    void(*f)(void) = std::bind(ThreadFunc, std::forward<A> (args)...);

    return Internal::StartThread(f);
}

void *InternalCall(void *r){

    void(*f)(void) = *reinterpret_cast<void(*)(void)>(r);

    f();

    return NULL;

    delete (voidFunc *)r;

}

thread_t Internal::StartThread(void(*function)(void)){
    pthread_t thread;
    voidFunc *f = new voidFunc;

    *f = function;

    //std::function<void(void *)>passfunc = [](void *r){void(*function)(void) = reinterpret_cast<void(*)(void)>(r); function();};

    //void(*passfunc_p)(void *) = std::bind(passfunc, std::placeholders::_1);

    pthread_create(&thread, NULL, InternalCall, f);

    return thread;

}
