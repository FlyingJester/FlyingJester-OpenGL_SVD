#include "../api.h"
#include "atomic.h"
#include "frame_synchro.h"

FJ::Atomic::atomic_t screen_frame = 0;
FJ::Atomic::atomic_t engine_frame = 0;

FJ::Atomic::atomic_t near_death = 0;

FJ::Atomic::atomic_t render_idling = 0;
