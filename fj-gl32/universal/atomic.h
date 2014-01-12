#ifndef FJ_GL32_ATOMIC_HEAD
#define FJ_GL32_ATOMIC_HEAD

#if defined USE_NATIVE_ATOMIC && defined USE_TBB_ATOMIC
    #undef USE_TBB_ATOMIC
    #warning Conflicting atomic preprocessor macros defined
#endif

#if defined USE_NATIVE_ATOMIC && defined NO_ATOMIC_AVAILABLE
    #undef USE_NATIVE_ATOMIC
    #warning Conflicting atomic preprocessor macros defined
#endif

#if defined USE_TBB_ATOMIC && defined NO_ATOMIC_AVAILABLE
    #undef USE_TBB_ATOMIC
    #warning Conflicting atomic preprocessor macros defined
#endif

#if ( ! defined USE_TBB_ATOMIC) && ( ! defined USE_NATIVE_ATOMIC) && ( ! defined NO_ATOMIC_AVAILABLE)
    #warning No atomic API specified. Defaulting to dummy atomic API.
    #define NO_ATOMIC_AVAILABLE
#endif

#ifdef USE_NATIVE_ATOMIC
    #include <atomic.h>
namespace FJ{
    namespace Atomic{

#endif

#ifdef USE_TBB_ATOMIC
    #include <tbb/atomic.h>
namespace FJ{
    namespace Atomic{

        typedef tbb::atomic<int> atomic_t;

        inline void setAtomic(atomic_t n, int to) {n=to;}
        inline int getAtomic(atomic_t n) {return n;}
        inline void incAtomic(atomic_t &n){n.fetch_and_add(1);}
#endif


#ifdef NO_ATOMIC_AVAILABLE

namespace FJ{
    namespace Atomic{

        typedef int atomic_t;

        inline void setAtomic(atomic_t n, int to) {n=to;}
        inline int getAtomic(atomic_t n) {return n;}
        inline void incAtomic(atomic_t &n) {n++;}


#endif
    }
}
#endif
