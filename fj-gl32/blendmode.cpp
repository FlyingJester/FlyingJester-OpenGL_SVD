#include "blendmode.h"
#include "universal/math.h"

RGBA FJ::blendmode::Replace(RGBA a, RGBA b){
    return b;
}

RGBA FJ::blendmode::RGB(RGBA a, RGBA b){
    return ((a&0xFFFFFF00)|(b&0x000000FF));
}

RGBA FJ::blendmode::Alpha(RGBA a, RGBA b){
    return ((a&0x000000FF)|(b&0xFFFFFF00));
}
RGBA FJ::blendmode::Add(RGBA a, RGBA b){
    return (
            FJ::math::min((a&0x000000FF)+(b&0x000000FF), 0x000000FFu) &
            FJ::math::min((a&0x0000FF00)+(b&0x0000FF00), 0x0000FF00u) &
            FJ::math::min((a&0x00FF0000)+(b&0x00FF0000), 0x00FF0000u) &
            FJ::math::min((a&0xFF000000)+(b&0xFF000000), 0xFF000000u)
    );
}
RGBA FJ::blendmode::Subtract(RGBA a, RGBA b){
    return (
            FJ::math::min((a&0x000000FF)-(b&0x000000FF), 0x000000FFu) &
            FJ::math::min((a&0x0000FF00)-(b&0x0000FF00), 0x0000FF00u) &
            FJ::math::min((a&0x00FF0000)-(b&0x00FF0000), 0x00FF0000u) &
            FJ::math::min((a&0xFF000000)-(b&0xFF000000), 0xFF000000u)
    );
}

RGBA FJ::blendmode::Multiply(RGBA a, RGBA b){
    return (
            (FJ::math::min(((a    )&0x000000FF)*((b    )&0x000000FF), 0x000000FFu)    ) &
            (FJ::math::min(((a>>8 )&0x000000FF)*((b>>8 )&0x000000FF), 0x000000FFu)<<8 ) &
            (FJ::math::min(((a>>16)&0x000000FF)*((b>>16)&0x000000FF), 0x000000FFu)<<16) &
            (FJ::math::min(((a>>24)&0x000000FF)*((b>>24)&0x000000FF), 0x000000FFu)<<24)
    );
}

RGBA FJ::blendmode::Average(RGBA a, RGBA b){
    return (
            (((((a    )&0x000000FF)+((b    )&0x000000FF))>>1)    ) &
            (((((a>>8 )&0x000000FF)+((b>>8 )&0x000000FF))>>1)<<8 ) &
            (((((a>>16)&0x000000FF)+((b>>16)&0x000000FF))>>1)<<16) &
            (((((a>>24)&0x000000FF)+((b>>24)&0x000000FF))>>1)<<24)
    );
}

/////
// Both equations dummied out for now.
//   Due to current implementation, this still produces expected results for Blend().

RGBA FJ::blendmode::Invert(RGBA a, RGBA b){
    return a;
}

RGBA FJ::blendmode::Blend(RGBA a, RGBA b){
    return a;
}
