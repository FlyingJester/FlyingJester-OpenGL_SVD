#include "clipping_rectangle.h"
#include "atomic.h"

/////
// TODO: Implement whole clipping rectangle locking/atomicity

FJ::Atomic::atomic_t clippingrect_x = 0;
FJ::Atomic::atomic_t clippingrect_y = 0;
FJ::Atomic::atomic_t clippingrect_w = 0;
FJ::Atomic::atomic_t clippingrect_h = 0;
