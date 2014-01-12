#ifndef FJ_GL32_MATH_HEAD
#define FJ_GL32_MATH_HEAD

namespace FJ{
    namespace math{

        /////
        // Abstracted math functions. These will be selected from different math libraries at runtime.
        //   Always at least as fast as libm's functions.

        float       min(float,      float);
        double      min(double,     double);
        unsigned    min(unsigned,   unsigned);
        int         min(int,        int);
        long long   min(long long,  long long);

        float       max(float,      float);
        double      max(double,     double);
        int         max(int,        int);
        unsigned    max(unsigned,   unsigned);
        long long   max(long long,  long long);
    }
}


#endif
