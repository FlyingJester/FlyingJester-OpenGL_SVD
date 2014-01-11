#ifndef FJ_GL32_CONCURRENT_QUEUE_HEAD
#define FJ_GL32_CONCURRENT_QUEUE_HEAD

#ifdef _WIN32

#include <Windows.h>

#endif

#ifdef USE_NATIVE_CONCURRENCT_QUEUE

#include <concurrent_queue.h>

#endif

#ifdef USE_TBB_CONCURRENT_QUEUE

#include <tbb/concurrent_queue.h>

#endif

namespace FJ {

    class Primitive;

    #ifdef USE_NATIVE_CONCURRENCT_QUEUE

    #ifdef _WIN32
	using namespace Concurrency;
    #endif

    typedef concurrent_queue concurrent_queue;

    #endif

    #ifdef USE_TBB_CONCURRENT_QUEUE

	typedef tbb::strict_ppl::concurrent_queue<FJ::Primitive *> DrawList;

    #endif


}

#endif
