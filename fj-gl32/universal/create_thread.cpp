#include "create_thread.h"

#ifdef USE_WINDOWS_THREADS
    #ifndef _WIN32
        #error Using Windows threads on a non-Windows platform
    #endif

    #ifdef USE_PTHREAD_THREADS
        #pragma warning Both Windows and Pthreads threads have been requested. Defaulting to Windows threads on Windows platform.
        #undef USE_PTHREAD_THREADS
    #endif

    #ifdef USE_TBB_THREADS
        #pragma warning Both Windows and Intel TBB threads have been requested. Defaulting to Windows threads on Windows platform.
        #undef USE_TBB_THREADS
    #endif
#endif

#ifdef USE_TBB_THREADS
    #ifndef _WIN32
        #ifdef USE_PTHREAD_THREADS
            #pragma warning Both Pthread and Intel TBB threads have been requested. Defaulting to Pthread threads on a Unix platform.
            #undef USE_TBB_THREADS
        #endif
    #else
        #ifdef USE_PTHREAD_THREADS
            #pragma warning Both Pthread and Intel TBB threads have been requested. Defaulting to Intel TBB threads on Windows platform.
            #undef USE_PTHREAD_THREADS
        #endif
    #endif
#endif

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

#ifdef USE_PTHREAD_THREADS
thread_t Internal::StartThread(void(*function)(void)){
    pthread_t thread;
    voidFunc *f = new voidFunc;

    *f = function;

    //std::function<void(void *)>passfunc = [](void *r){void(*function)(void) = reinterpret_cast<void(*)(void)>(r); function();};

    //void(*passfunc_p)(void *) = std::bind(passfunc, std::placeholders::_1);

    pthread_create(&thread, NULL, InternalCall, f);

    return thread;

}

void ExitThread(void){
    pthread_exit(NULL);
}

void WaitThread(thread_t thread){
    pthread_join(thread, NULL);
}
#endif
