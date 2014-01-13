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

#include <pthread.h>

Display *FJ::GLX::display;
Window FJ::GLX::window;
GLXContext FJ::GLX::glc;
GLXContext FJ::GLX::glcMain;
GLXWindow FJ::GLX::glxwindow;

using namespace FJ::GLX;

FJ::Thread::thread_t rendererthread;

////////////////////////////////////////////////////////
// Initialization function
////////////////////////////////////////////////////////

void *CallThreadFunction(void *o){
    FJ::ThreadFunction();
    return NULL;
}

bool InitVideo(int screen_width, int screen_height, std::string sphere_dir){


	/////
	//Gather required information from what Sphere gives us

    display = XOpenDisplay(NULL);
    if(!display){
        fprintf(stderr, "[FJ-GL] Error: Cannot open display :0\n");
        return false;
    }

    //////////
    // Create GL/GLX state
    //////////


    /////
    // Set up Window

    int attributes[] = {
        GLX_RGBA, GLX_RED_SIZE, 8, GLX_BLUE_SIZE, 8, GLX_GREEN_SIZE, 8, GLX_ALPHA_SIZE, 0,
        GLX_DRAWABLE_TYPE, GLX_WINDOW_BIT,
        GLX_RENDER_TYPE, GLX_RGBA_BIT,
        GLX_DOUBLEBUFFER, True,
        GLX_LEVEL, 0,
        None
    };

    int screen = DefaultScreen(display);
    Window rootwin = RootWindow(display, screen);

    int element;

    /////
    // Set up GLX

    GLXFBConfig *fbconfig = glXChooseFBConfig(display, screen, attributes, &element);
    if(!fbconfig){
        fprintf(stderr, "[FJ-GL] Error: Could not get GLX framebuffer configuration\n");
        return false;
    }

    XVisualInfo *visualinfo = glXChooseVisual(display, screen, attributes);
    if(!visualinfo){
        fprintf(stderr, "[FJ-GL] Error: Could not get GLX visual information\n");
        return false;
    }

    GLXContext temp_context = glXCreateContext(display, visualinfo, NULL, True);
    GLXContext (*glXCreateContextAttribsARB)(Display*, GLXFBConfig, GLXContext, Bool, const int*) = (GLXContext (*)(Display*, GLXFBConfig, GLXContext, Bool, const int*))glXGetProcAddressARB((const GLubyte *)"glXCreateContextAttribsARB");

    glXDestroyContext(display, temp_context);

    if(!glXCreateContextAttribsARB)
        printf("[FJ-GL] glXCreateContextAttribsARB was not found. Assuming GLX version 1.3 or earlier, and OpenGL 3.1 or earlier.\n");

    const char *extstring = glXQueryExtensionsString(display, screen);

    if(!strstr(extstring, "GLX_ARB_create_context"))
        printf("[FJ-GL] glXCreateContextAttribsARB was not found. Assuming GLX version 1.3 or earlier, and OpenGL 3.1 or earlier.\n");

    XSetWindowAttributes winattributes;

    //winattributes.background_pixel = 0;
    winattributes.border_pixel = 0;
    winattributes.colormap = XCreateColormap(FJ::GLX::display, rootwin, visualinfo->visual, AllocNone);
    winattributes.event_mask = StructureNotifyMask | ExposureMask | KeyPressMask;

    window = XCreateWindow(
                                FJ::GLX::display, rootwin, 0, 0, screen_width, screen_height, 0,
                                visualinfo->depth, InputOutput, visualinfo->visual, CWBackPixel | CWBorderPixel | CWColormap | CWEventMask, &winattributes
                           );
    /////
    // Setup basic OpenGL context


    int glattributes[] = {
        GLX_CONTEXT_MAJOR_VERSION_ARB, 3,
        GLX_CONTEXT_MINOR_VERSION_ARB, 1,
        None
    };


    /////
    // If we have glXCreateContextAttribsARB, that means we need to force an earlier version of OpenGL.

    if(glXCreateContextAttribsARB){
        FJ::GLX::glc = glXCreateContextAttribsARB(FJ::GLX::display, *fbconfig, 0, True, glattributes);
        FJ::GLX::glcMain = glXCreateContextAttribsARB(FJ::GLX::display, *fbconfig, FJ::GLX::glc, True, glattributes);
    }
    else{
        FJ::GLX::glc = glXCreateNewContext(FJ::GLX::display, *fbconfig, GLX_RGBA_TYPE, NULL, True);
        FJ::GLX::glcMain = glXCreateNewContext(FJ::GLX::display, *fbconfig, GLX_RGBA_TYPE, FJ::GLX::glc, True);
        printf("[FJ-GL] Falling back to GLX 1.3 functions.\n");
    }
    XSync(FJ::GLX::display, False);

    FJ::GLX::glxwindow = glXCreateWindow(FJ::GLX::display, *fbconfig, window, glattributes);

    XFree(visualinfo);

    XEvent event;
    XStoreName(FJ::GLX::display, window, "Sphere Game Engine");

    XMapWindow(FJ::GLX::display, window);
    //XIfEvent(display, &event, WaitForNotify, (XPointer) window);

//    glXMakeContextCurrent(display, glxwindow, glxwindow, glc);
    glXMakeCurrent(FJ::GLX::display, FJ::GLX::window, FJ::GLX::glc);

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

    glXMakeCurrent(FJ::GLX::display, FJ::GLX::window, FJ::GLX::glcMain);

    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);
	/////
	//Set up the empty texture

    glGenTextures(1, &(FJ::GLstate::emptyTexture));

    if(!FJ::GLstate::emptyTexture){
        printf("[FJ-GL] Could not create empty texture.\n");
        return false;
    }
    else{
        printf("[FJ-GL] Empty texture\t%i\n", FJ::GLstate::emptyTexture);

    }

	unsigned int fullMask = 0xFFFFFFFF;

    glBindTexture(GL_TEXTURE_2D, FJ::GLstate::emptyTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, &fullMask);

	/////
	//Mark the GL Texture state as uninitialized.

	FJ::GLstate::texture::resetSetup();

	/////
	// Create OpenGL rendering threads

    const char *GLX_Vendor = glXGetClientString(FJ::GLX::display, GLX_VENDOR);

    int MV;
    int mV;

    glGetIntegerv(GL_MAJOR_VERSION, &MV);
    glGetIntegerv(GL_MINOR_VERSION, &mV);

    printf("[FJ-GL] Using GLX %s\n", GLX_Vendor);
    printf("[FJ-GL] Using GL version %i.%i\n", MV, mV);

    const char * version2 = (const char *)glGetString(GL_VERSION);

    printf("[FJ-GL] Extended GL version is %s\n", version2);

    pthread_t thread;

    glFinish();

    pthread_create(&thread, NULL, CallThreadFunction, NULL);

    rendererthread = thread;

	//rendererthread = FJ::Thread::Internal::StartThread(&FJ::ThreadFunction);

    printf("[FJ-GL] Setup succeeded.\n");

    return true;

}


void CloseVideoDriver(void){

    /////
    // Signal the renderer thread to exit

	FJ::Atomic::setAtomic(near_death, 1);

    /////
    // Wait for thread to exit

    FJ::Thread::WaitThread(rendererthread);

    /////
    // Delete GL Context

    XSync(FJ::GLX::display, False);

    glXMakeCurrent(display, 0, 0);
    glXDestroyContext(display, glc);

    XDestroyWindow(FJ::GLX::display, FJ::GLX::window);

    XCloseDisplay(display);

}
