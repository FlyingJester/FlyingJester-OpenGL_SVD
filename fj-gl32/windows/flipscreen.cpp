#include "api.h"
#include "../universal/atomic.h"

void FJ::Operation::FlipScreen::textureSetup(){

	SwapBuffers(dc);

	while(FJ::Atomic::getAtomic(screen_frame)>FJ::Atomic::getAtomic(engine_frame)){}

    FL::Atomic::incAtomic(screen_frame);

}
