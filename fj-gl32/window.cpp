#include "window.h"
#include <cstddef>
#include <Windows.h>
#include <Wingdi.h>

#define GL_GLEXT_PROTOTYPES 1
#define GL3_PROTOTYPES 1

#include <GL/gl.h>

////////////////////////////////////////////////////////
// State variables
////////////////////////////////////////////////////////

#define ATOMIC int

#define SET_ATOMIC(n, to) n=to
#define GET_ATOMIC(n) n

int lastSetup = FJ::eNone;

ATOMIC screen_frame;
ATOMIC engine_frame;

ATOMIC near_death;

/////
// TODO: Implement whole clipping rectangle locking/atomicity

ATOMIC clippingrect_x;
ATOMIC clippingrect_y;
ATOMIC clippingrect_w;
ATOMIC clippingrect_h;

int resolution[2];
HDC dc;
HGLRC glc;

GLuint emptyTexture = 0;

FJ::DrawList drawList;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*                                   *//*   Internal FJ-GL API   *//*                                         */
////////////////////////////////////////////////////////////////////////////////////////////////////////////////




/////
//Call whenever changing the GL Texture state

inline void resetSetup(void){
	lastSetup = FJ::eNone;
}

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

////////////////////////////////////////////////////////
// Renderer Thread function
////////////////////////////////////////////////////////

void ThreadFunction(void){
	
	FJ::Primitive *prim = NULL;

	while(!GET_ATOMIC(near_death)){

		while (!drawList.try_pop(prim)){
			Sleep(1);
		}

		prim->setupGL();
		prim->drawGL();

	}

}

////////////////////////////////////////////////////////
// FJ::Primitive::textureSetup Definitions
////////////////////////////////////////////////////////


/////
// May be functionally identical to resetSetup().
// Maintained as a hook for future use.

inline void TextureSetup(){
	lastSetup = FJ::eTexture;
}

inline void PrimitiveSetup(){
	
	if(lastSetup == FJ::ePrimitive)
		return;

	lastSetup = FJ::ePrimitive;

    glBindTexture(GL_TEXTURE_2D, emptyTexture);

}

void FJ::Operation::UntexturePrimitive::textureSetup(void){
	PrimitiveSetup();
}

void FJ::Operation::TexturePrimitive::textureSetup(void){
	glBindTexture(GL_TEXTURE_2D, texture);
	TextureSetup();
}

////////////////////////////////////////////////////////
// FJ::Primitive::setupGL Definitions
////////////////////////////////////////////////////////

void FJ::Primitive::setupGL(void){
	
	textureSetup();

	glVertexPointer(2, GL_INT, 0, coord);
	glColorPointer(4, GL_UNSIGNED_BYTE, 0, color);
}

////////////////////////////////////////////////////////
// Direct FJ::Primitive::DrawGL Definitions
////////////////////////////////////////////////////////

void FJ::Operation::Point::drawGL(){
	glDrawArrays(GL_POINTS, 0, 1);
}

void FJ::Operation::Line::drawGL(){
	glDrawArrays(GL_LINES, 0, 2);
}

void FJ::Operation::Polygon::drawGL(){
	glDrawArrays(GL_TRIANGLE_STRIP, 0, n);
}

void FJ::Operation::Rect::drawGL(){
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

void FJ::Operation::Ellipse::drawGL(){

}

void FJ::Operation::Image::drawGL(){
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

////////////////////////////////////////////////////////
// Indirect FJ::Primitive::DrawGL Definitions
////////////////////////////////////////////////////////

void FJ::Operation::ClippingRectangleSet::textureSetup(){
	glScissor(coord[0], coord[1], coord[2], coord[3]);

	SET_ATOMIC(clippingrect_x, coord[0]);
	SET_ATOMIC(clippingrect_y, coord[1]);
	SET_ATOMIC(clippingrect_w, coord[2]);
	SET_ATOMIC(clippingrect_h, coord[3]);

}

void FJ::Operation::ClippingRectangleSet::drawGL(){

}

void FJ::Operation::FlipScreen::textureSetup(){

	SwapBuffers(dc);

	while(GET_ATOMIC(screen_frame)>GET_ATOMIC(engine_frame)){}

	SET_ATOMIC(screen_frame, GET_ATOMIC(screen_frame)+1);

}

////////////////////////////////////////////////////////
// FJ::Image Class
////////////////////////////////////////////////////////

FJ::Image::Image(unsigned int width, unsigned int height, RGBA *pixels){
	
	w = width;
	h = height;

    glGenTextures(1, &glname);

    glBindTexture(GL_TEXTURE_2D, emptyTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
	
	/////
	//We bound a texture, so we must flag the texture setup as unkown.
	resetSetup();
}


FJ::Image::~Image(){
	glDeleteTextures(1, &glname);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*                                  *//* Sphere Video Driver API *//*                                         */
////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////
// Driver Info functions
////////////////////////////////////////////////////////

void GetDriverInfo(DRIVERINFO* driverinfo){
  driverinfo->name = "Flying Jester OpenGL";
  driverinfo->author = "Flying Jester";
  driverinfo->date = "2014";
  driverinfo->version = "0.9";
  driverinfo->description = "A more modern OpenGL plugin for Sphere 1.5 and 1.6. Designed to work on NVidia 8800 GT, AMD HD 6850, and Intel 965 Express or greater.";
}

////////////////////////////////////////////////////////
// Miscellaneous Sphere Video Driver API Functions
////////////////////////////////////////////////////////

void ConfigureDriver(HWND parent){
	return;
}

void CloseVideoDriver(void){
	wglDeleteContext(glc);
}

bool ToggleFullscreen(void){
	return true;
}

////////////////////////////////////////////////////////
// Utility API Functions
////////////////////////////////////////////////////////

void FlipScreen(void){
	
	/////
	// Wait for the screen to catch up.
	
	while(GET_ATOMIC(engine_frame)>GET_ATOMIC(screen_frame)){}

	SET_ATOMIC(engine_frame, GET_ATOMIC(engine_frame)+1);

}

void SetClippingRectangle(int x, int y, int w, int h){

	 FJ::Operation::ClippingRectangleSet *cliprect = new FJ::Operation::ClippingRectangleSet();

	 cliprect->coord = new int[4];
	 cliprect->coord[0] = x;
	 cliprect->coord[0] = y;
	 cliprect->coord[0] = w;
	 cliprect->coord[0] = h;

	 drawList.push(cliprect);

}
void GetClippingRectangle(int* x, int* y, int* w, int* h){
	
	/////
	// This is an innaccurate result. 
	// The result may be desynchronized if the clipping rectangle has 
	//   been changed since the last FlipScreen synchronization, and
	//   the engine is ahead of the screen.
	
	/////
	// TODO: Make this return an accurate value.
	
	*x = GET_ATOMIC(clippingrect_x);
	*y = GET_ATOMIC(clippingrect_y);
	*w = GET_ATOMIC(clippingrect_w);
	*h = GET_ATOMIC(clippingrect_h);

}

////////////////////////////////////////////////////////
// Image Lifetime API Functions
////////////////////////////////////////////////////////

IMAGE CreateImage(int width, int height, RGBA* pixels){
	return new FJ::Image(width, height, pixels);
}

IMAGE CloneImage(IMAGE image){
	glBindTexture(GL_TEXTURE_2D, image->glname);
	
	resetSetup();
}

IMAGE GrabImage(IMAGE image, int x, int y, int width, int height){
	RGBA *pixels = new RGBA[width*height];
	glReadPixels(x,  y, width, height,  GL_RGBA,  GL_UNSIGNED_BYTE,  pixels);

}
void DestroyImage(IMAGE image);

////////////////////////////////////////////////////////
// Image Drawing API Functions
////////////////////////////////////////////////////////

void BlitImage(IMAGE image, int x, int y, int blendmode);
void BlitImageMask(IMAGE image, int x, int y, int blendmode, RGBA mask, int mask_blendmode);
void TransformBlitImage(IMAGE image, int x[4], int y[4], int blendmode);
void TransformBlitImageMask(IMAGE image, int x[4], int y[4], int blendmode, RGBA mask, int mask_blendmode);

////////////////////////////////////////////////////////
// Image Utility API Functions
////////////////////////////////////////////////////////

int GetImageWidth(IMAGE image);
int GetImageHeight(IMAGE image);
RGBA* LockImage(IMAGE image);
void UnlockImage(IMAGE image);

////////////////////////////////////////////////////////
// Image Direct API Functions
////////////////////////////////////////////////////////

void DirectBlit(int x, int y, int w, int h, RGBA* pixels);
void DirectTransformBlit(int x[4], int y[4], int w, int h, RGBA* pixels);
void DirectGrab(int x, int y, int w, int h, RGBA* pixels);

////////////////////////////////////////////////////////
// Simple Primitive API Functions
////////////////////////////////////////////////////////

void DrawPoint(int x, int y, RGBA color);

void DrawLine(int x[2], int y[2], RGBA color);
void DrawGradientLine(int x[2], int y[2], RGBA colors[2]);

void DrawBezierCurve(int x[4], int y[4], double step, RGBA color, int cubic);

void DrawTriangle(int x[3], int y[3], RGBA color);
void DrawGradientTriangle(int x[3], int y[3], RGBA colors[3]);

void DrawPolygon(int** points, int length, int invert, RGBA color);

void DrawRectangle(int x, int y, int w, int h, RGBA color);
void DrawOutlinedRectangle(int x, int y, int w, int h, int size, RGBA color);
void DrawGradientRectangle(int x, int y, int w, int h, RGBA colors[4]);

////////////////////////////////////////////////////////
// Simple Primitive API Functions
////////////////////////////////////////////////////////

void DrawOutlinedComplex(int r_x, int r_y, int r_w, int r_h, int circ_x, int circ_y, int circ_r, RGBA color, int antialias);
void DrawFilledComplex(int r_x, int r_y, int r_w, int r_h, int circ_x, int circ_y, int circ_r, float angle, float frac_size, int fill_empty, RGBA colors[2]);
void DrawGradientComplex(int r_x, int r_y, int r_w, int r_h, int circ_x, int circ_y, int circ_r, float angle, float frac_size, int fill_empty, RGBA colors[3]);
void DrawOutlinedEllipse(int x, int y, int rx, int ry, RGBA color);
void DrawFilledEllipse(int x, int y, int rx, int ry, RGBA color);
void DrawOutlinedCircle(int x, int y, int r, RGBA color, int antialias);
void DrawFilledCircle(int x, int y, int r, RGBA color, int antialias);
void DrawGradientCircle(int x, int y, int r, RGBA colors[2], int antialias);

////////////////////////////////////////////////////////
// Array Primitive API Functions
////////////////////////////////////////////////////////

void DrawPointSeries(int** points, int length, RGBA color);
void DrawLineSeries(int** points, int length, RGBA color, int type);
