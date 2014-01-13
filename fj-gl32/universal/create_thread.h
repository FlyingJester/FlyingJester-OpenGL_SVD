#ifndef FJ_GL32_CREATE_THREAD_HEAD
#define FJ_GL32_CREATE_THREAD_HEAD
#include "atomic.h"

namespace FJ{
    namespace Thread{

        typedef FJ::Atomic::atomic_t thread_t;

        /////
        // This can be hard to read, so a little explanation.
        //   It is called as such:
        //   CreateThread<ReturnType, Arg1Type, Arg2Type...>(&function, arg1, arg2, ...);
        //
        //   So, for instance, if you wanted the thread to do, say, a memcpy from
        //   void *a to void *b of size size_t n, and given memcpy is declared as
        //   void *memcpy(void *dest, const void *src, size_t num)
        //   You would write
        // CreateThread<void *, void *, const void *, size_t>(&memcpy, a, b, n);
        //   or even CreateThread(&memcpy, a, b, n);

        template<typename R, typename ...A>
        thread_t CreateThread(R(*ThreadFunc)(A...), A ...args);


        void WaitThread(thread_t);

        /////
        // Quit from the current thread. Only call from a function passed to CreateThread.

        void ExitThread(void);

        /////
        // Shorthand for CreateThread for functions that take no arguments and return void.

        namespace Internal{
            thread_t StartThread(void(*)(void));
        }
    }
}

#endif
