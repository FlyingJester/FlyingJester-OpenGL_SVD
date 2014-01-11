#include "api.h"

void FJ::Operation::FlipScreen::textureSetup(){

	SwapBuffers(dc);

	while(GET_ATOMIC(screen_frame)>GET_ATOMIC(engine_frame)){}

	SET_ATOMIC(screen_frame, GET_ATOMIC(screen_frame)+1);

}
