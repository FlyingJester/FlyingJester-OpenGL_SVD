#include "api.h"
#include "../universal/atomic.h"
#include "../universal/frame_synchro.h"


void FJ::Operation::FlipScreen::textureSetup(){

    glXSwapBuffers(FJ::GLX::display, FJ::GLX::window);

	while(FJ::Atomic::getAtomic(screen_frame)>FJ::Atomic::getAtomic(engine_frame)){}

    FJ::Atomic::incAtomic(screen_frame);

}
