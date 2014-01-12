#ifndef FJ_GL32_BLENDMODE_HEAD
#define FJ_GL32_BLENDMODE_HEAD
#include "api.h"

namespace FJ{
    namespace blendmode{
        RGBA Replace(RGBA a, RGBA b);
        RGBA RGB(RGBA a, RGBA b);
        RGBA Alpha(RGBA a, RGBA b);
        RGBA Add(RGBA a, RGBA b);
        RGBA Subtract(RGBA a, RGBA b);
        RGBA Multiply(RGBA a, RGBA b);
        RGBA Average(RGBA a, RGBA b);
        RGBA Invert(RGBA a, RGBA b);
        RGBA Blend(RGBA a, RGBA b);
    }
}


#endif
