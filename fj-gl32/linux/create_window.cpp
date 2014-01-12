#include "api.h"
#include "../universal/atomic.h"
#include "../universal/clipping_rectangle.h"
#include "../universal/frame_synchro.h"
#include "../universal/create_thread.h"
#include "../resolution.h"
#include "../gltexture_state.h"

#include <cstdio>

#include <X11/Xlib.h>
#include <GL/gl.h>
#include <GL/glx.h>

Display *FJ::GLX::display;
Window FJ::GLX::window;
GLXContext FJ::GLX::glc;

using namespace FJ::GLX;

////////////////////////////////////////////////////////
// Initialization function
////////////////////////////////////////////////////////

bool InitVideo(int screen_width, int screen_height, std::string sphere_dir){


	/////
	//Gather required information from what Sphere gives us

    display = XOpenDisplay(":0");
    if(!display){
        fprintf(stderr, "[FJ-GL] Error: Cannot open display :0\n");
        exit(EXIT_FAILURE);
    }

    //////////
    // Create GL/GLX state
    //////////

    /////
    // Set up Window

    int attributes[] = {
        GLX_RGBA, GLX_RED_SIZE, 1, GLX_BLUE_SIZE, 1, GLX_GREEN_SIZE, 1, GLX_ALPHA_SIZE, 1,
        GLX_DRAWABLE_TYPE, GLX_WINDOW_BIT,
        GLX_RENDER_TYPE, GLX_RGBA_BIT,
        GLX_DOUBLEBUFFER, True,
        None
    };

    int screen = DefaultScreen(display);
    Window rootwin = RootWindow(display, screen);

    int element;

    /////
    // Set up GLX

    GLXFBConfig *fbconfig = glXChooseFBConfig(display, screen, NULL, &element);
    if(!fbconfig){
        fprintf(stderr, "[FJ-GL] Error: Could not get GLX framebuffer configuration\n");
        exit(EXIT_FAILURE);
    }

    XVisualInfo *visualinfo = glXChooseVisual(display, screen, attributes);
    if(!visualinfo){
        fprintf(stderr, "[FJ-GL] Error: Could not get GLX visual information\n");
        exit(EXIT_FAILURE);
    }

    XSetWindowAttributes winattributes;

    winattributes.background_pixel = 0;
    winattributes.border_pixel = 0;
    winattributes.colormap = XCreateColormap(display, rootwin, visualinfo->visual, AllocNone);
    winattributes.event_mask = StructureNotifyMask | ExposureMask | KeyPressMask;

    window = XCreateWindow(
                                display, rootwin, 0, 0, screen_width, screen_height, 0,
                                visualinfo->depth, InputOutput, visualinfo->visual, CWBackPixel | CWBorderPixel | CWColormap | CWEventMask, &winattributes
                           );
    /////
    // Setup basic OpenGL context

    int glattributes[] = {
        GLX_CONTEXT_MAJOR_VERSION_ARB, 3,
        GLX_CONTEXT_MAJOR_VERSION_ARB, 1,
        None
    };

    glc = glXCreateNewContext(display, *fbconfig, GLX_RGBA_TYPE, NULL, True);

    XFree(visualinfo);

    XMapWindow(display, window);
    glXMakeCurrent(display, window, glc);

    //////////
    // Initialize internal state
    //////////

	FJ::DrawList drawList = FJ::DrawList();

	FJ::resolution[0] = screen_width;
	FJ::resolution[1] = screen_height;


	FJ::Atomic::setAtomic(screen_frame, 0);
	FJ::Atomic::setAtomic(engine_frame, 0);

	FJ::Atomic::setAtomic(near_death, 0);

	FJ::Atomic::setAtomic(clippingrect_x, 0);
	FJ::Atomic::setAtomic(clippingrect_y, 0);
	FJ::Atomic::setAtomic(clippingrect_w, screen_width);
	FJ::Atomic::setAtomic(clippingrect_h, screen_height);

	/////
	//Initialize and set up OpenGL

    glViewport(0, 0, FJ::resolution[0], FJ::resolution[1]);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, FJ::resolution[0], FJ::resolution[1], 0, -1, 1);
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

    glGenTextures(1, &(FJ::GLstate::emptyTexture));

	unsigned int fullMask = 0xFFFFFFFF;

    glBindTexture(GL_TEXTURE_2D, FJ::GLstate::emptyTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, &fullMask);

	/////
	//Mark the GL Texture state as uninitialized.

	FJ::GLstate::texture::resetSetup();
}


void CloseVideoDriver(void){
    /////
    // Delete GL Context
}
