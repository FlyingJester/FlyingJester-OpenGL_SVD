#include "api.h"

HDC dc;
HGLRC glc;

////////////////////////////////////////////////////////
// Initialization function
////////////////////////////////////////////////////////

bool InitVideoDriver(HWND window, int screen_width, int screen_height){


	/////
	//Gather required information from what Sphere gives us

	dc = GetDC(window);
	glc = wglCreateContext(dc);
	wglMakeCurrent(dc, glc);

	/////
	//Initialize internal state

	FJ::DrawList drawList = FJ::DrawList();

	resolution[0] = screen_width;
	resolution[1] = screen_height;


	SET_ATOMIC(screen_frame, 0);
	SET_ATOMIC(engine_frame, 0);

	SET_ATOMIC(near_death, 0);

	SET_ATOMIC(clippingrect_x, 0);
	SET_ATOMIC(clippingrect_y, 0);
	SET_ATOMIC(clippingrect_w, screen_width);
	SET_ATOMIC(clippingrect_h, screen_height);

	/////
	//Initialize and set up OpenGL

    glViewport(0, 0, resolution[0], resolution[1]);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, resolution[0], resolution[1], 0, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
	glEnable(GL_TEXTURE_2D);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);

	glEnable(GL_SCISSOR_TEST);

	/////
	//Set up texture mapping

	unsigned int fullCoord[] = {0, 0, 1, 0, 1, 1, 0, 1};

    glTexCoordPointer(2, GL_UNSIGNED_INT, 0, fullCoord);


	/////
	//Set up the empty texture

    glGenTextures(1, &emptyTexture);

	unsigned int fullMask = 0xFFFFFFFF;

    glBindTexture(GL_TEXTURE_2D, emptyTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, &fullMask);

	/////
	//Mark the GL Texture state as uninitialized.

	resetSetup();
}


void CloseVideoDriver(void){
	wglDeleteContext(glc);
}
