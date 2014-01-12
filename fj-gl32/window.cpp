#include "api.h"
#include "resolution.h"
#include "universal/atomic.h"
#include "universal/concurrent_queue.h"
#include "universal/clipping_rectangle.h"
#include "universal/frame_synchro.h"
#include "universal/create_thread.h"

#include "gltexture_state.h"
#include "glblend_state.h"

#include <cstddef>

#include <GL/gl.h>

////////////////////////////////////////////////////////
// State variables
////////////////////////////////////////////////////////

GLuint emptyTexture = 0;

FJ::DrawList drawList;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*                                   *//*   Internal FJ-GL API   *//*                                         */
////////////////////////////////////////////////////////////////////////////////////////////////////////////////

inline void StarveThreadFunction(void){
    while(!((!drawList.unsafe_size())&&FJ::Atomic::getAtomic(render_idling))){
        /////
        // Busy wait.
    }

    /////
    // The render thread is now starved of operations.
}

////////////////////////////////////////////////////////
// Renderer Thread function
////////////////////////////////////////////////////////

void FJ::ThreadFunction(void){

	FJ::Primitive *prim = NULL;

	while(!FJ::Atomic::getAtomic(near_death)){


		while (!drawList.try_pop(prim)){
            FJ::Atomic::setAtomic(render_idling, 1);
			FJ::Sleep(1);
            FJ::Atomic::setAtomic(render_idling, 0);
		}


		prim->setupGL();
		prim->drawGL();

	}

    FJ::Thread::ExitThread();

}

////////////////////////////////////////////////////////
// FJ::Primitive::textureSetup Definitions
////////////////////////////////////////////////////////

void FJ::Operation::UntexturePrimitive::textureSetup(void){

	FJ::GLstate::texture::primitiveSetup();
	FJ::GLstate::blendmode::setBlendMode(FJ::Sphere::BlendMode::bmBlend);

}

unsigned int  FJ::Operation::TexturePrimitive::getTexture(void) const{
    return texture;
}

int  FJ::Operation::TexturePrimitive::getWidth(void) const{
    FJ::GLstate::texture::resetSetup();
    glBindTexture(GL_TEXTURE_2D, getTexture());

    int width = 0;

    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &width);

    return width;
}

int  FJ::Operation::TexturePrimitive::getHeight(void) const{
    FJ::GLstate::texture::resetSetup();
    glBindTexture(GL_TEXTURE_2D, getTexture());

    int height = 0;

    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &height);

    return height;
}

unsigned int  FJ::Operation::Image::getTexture(void) const{
    return im->glname;
}

int  FJ::Operation::Image::getWidth(void) const{
    return im->w;
}

int  FJ::Operation::Image::getHeight(void) const{
    return im->h;
}

void FJ::Operation::TexturePrimitive::textureSetup(void){

    glBindTexture(GL_TEXTURE_2D, texture);
    FJ::GLstate::blendmode::setBlendMode(blendmode);
    FJ::GLstate::texture::textureSetup();


}

////////////////////////////////////////////////////////
// FJ::Primitive::setupGL Definitions
////////////////////////////////////////////////////////

void FJ::Primitive::setupGL(void){

	FJ::GLstate::texture::textureSetup();

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

void FJ::Operation::Triangle::drawGL(){
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 3);
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

	FJ::Atomic::setAtomic(clippingrect_x, coord[0]);
	FJ::Atomic::setAtomic(clippingrect_y, coord[1]);
	FJ::Atomic::setAtomic(clippingrect_w, coord[2]);
	FJ::Atomic::setAtomic(clippingrect_h, coord[3]);

}

void FJ::Operation::ClippingRectangleSet::drawGL(){

}

void FJ::Operation::FlipScreen::drawGL(){

}

////////////////////////////////////////////////////////
// FJ::Image Class
////////////////////////////////////////////////////////

FJ::Image::Image(){
    pixels = NULL;
}

FJ::Image::Image(unsigned int width, unsigned int height, RGBA *px){

	w = width;
	h = height;

    glGenTextures(1, &glname);

    glBindTexture(GL_TEXTURE_2D, emptyTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, px);

    this->pixels = new RGBA[width*height];

    memcpy(pixels, px, width*height*sizeof(RGBA));

	/////
	//We bound a texture, so we must flag the texture setup as unkown.
	FJ::GLstate::texture::resetSetup();
}

FJ::Image::~Image(){
	glDeleteTextures(1, &glname);
	delete[] pixels;
}

FJ::UnsafeImage::UnsafeImage(unsigned int width, unsigned int height, RGBA *px){

	w = width;
	h = height;

    glGenTextures(1, &glname);

    glBindTexture(GL_TEXTURE_2D, emptyTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, px);

	/////
	//We bound a texture, so we must flag the texture setup as unkown.
	FJ::GLstate::texture::resetSetup();
}

FJ::UnsafeImage::~UnsafeImage(){
	glDeleteTextures(1, &glname);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*                                  *//* Sphere Video Driver API *//*                                         */
////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////
// Driver Info functions
////////////////////////////////////////////////////////

void GetDriverInfo(FJ::Sphere::DriverInfo_t* driverinfo){
  driverinfo->name = "FJ-GL";
  driverinfo->author = "Flying Jester";
  driverinfo->date = __DATE__;
  driverinfo->version = "0.09";
  driverinfo->description =     "A more modern OpenGL plugin built for Sphere 1.5 and 1.6.\n\
                                Designed to work on NVidia 8800 GT, AMD HD 6850, and Intel 965 Express or greater.\n\
                                Uses OpenGL 2.0 or 3.1, depending on the graphics card it is running on.\n\
                                Requires Windows, or Pthreads and Intel Thread Building Blocks.";
}

////////////////////////////////////////////////////////
// Miscellaneous Sphere Video Driver API Functions
////////////////////////////////////////////////////////

bool ToggleFullscreen(void){

    /////
    // TODO: Implement a fullscreen toggle that doesn't crash NVidia's X Server.

	return true;
}

////////////////////////////////////////////////////////
// Utility API Functions
////////////////////////////////////////////////////////

void FlipScreen(void){

	/////
	// Wait for the screen to catch up.

	while(FJ::Atomic::getAtomic(engine_frame)>FJ::Atomic::getAtomic(screen_frame)){}

    FJ::Atomic::incAtomic(engine_frame);

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

	*x = FJ::Atomic::getAtomic(clippingrect_x);
	*y = FJ::Atomic::getAtomic(clippingrect_y);
	*w = FJ::Atomic::getAtomic(clippingrect_w);
	*h = FJ::Atomic::getAtomic(clippingrect_h);

}

////////////////////////////////////////////////////////
// Image Lifetime API Functions
////////////////////////////////////////////////////////

IMAGE CreateImage(int width, int height, RGBA* pixels){
	return new FJ::Image(width, height, pixels);
}

IMAGE CloneImage(IMAGE image){
	glBindTexture(GL_TEXTURE_2D, image->glname);

	FJ::GLstate::texture::resetSetup();
}

IMAGE GrabImage(IMAGE image, int x, int y, int width, int height){
	RGBA *pixels = new RGBA[width*height];
	glReadPixels(x,  y, width, height,  GL_RGBA,  GL_UNSIGNED_BYTE,  pixels);

}
void DestroyImage(IMAGE image){
    delete image;
}

////////////////////////////////////////////////////////
// Image Drawing API Functions
////////////////////////////////////////////////////////

void BlitImage(IMAGE image, int x, int y, FJ::Sphere::BlendMode blendmode){

    BlitImageMask(image, x, y, blendmode, 0xFFFFFFFF, FJ::Sphere::bmBlend);

}

void BlitImageMask(IMAGE image, int x, int y, FJ::Sphere::BlendMode blendmode, RGBA mask, FJ::Sphere::BlendMode mask_blendmode){

    FJ::Operation::Image *blit = new FJ::Operation::Image();
    blit->im = image;
    blit->blendmode = blendmode;
    blit->mask_blendmode = mask_blendmode;

    blit->color = new RGBA[4];

    std::fill(blit->color, blit->color+(sizeof(RGBA)*3), mask);

    blit->coord = new int[8];

    blit->coord[0] = x;
    blit->coord[1] = y;
    blit->coord[2] = x+image->w;
    blit->coord[3] = y;
    blit->coord[4] = x+image->w;
    blit->coord[5] = y+image->h;
    blit->coord[6] = x;
    blit->coord[7] = y+image->h;

    drawList.push(blit);

}

void TransformBlitImage(IMAGE image, int x[4], int y[4], FJ::Sphere::BlendMode blendmode){

    TransformBlitImageMask(image, x, y, blendmode, 0xFFFFFFFF, FJ::Sphere::bmBlend);

}
void TransformBlitImageMask(IMAGE image, int x[4], int y[4], FJ::Sphere::BlendMode blendmode, RGBA mask, FJ::Sphere::BlendMode mask_blendmode){

    FJ::Operation::Image *blit = new FJ::Operation::Image();
    blit->im = image;
    blit->blendmode = blendmode;
    blit->mask_blendmode = mask_blendmode;

    blit->color = new RGBA[4];

    std::fill(blit->color, &(blit->color[3]), mask);

    blit->coord = new int[8];

    blit->coord[0] = x[0];
    blit->coord[1] = y[0];
    blit->coord[2] = x[1];
    blit->coord[3] = y[1];
    blit->coord[4] = x[2];
    blit->coord[5] = y[2];
    blit->coord[6] = x[3];
    blit->coord[7] = y[3];

    drawList.push(blit);

}

////////////////////////////////////////////////////////
// Image Utility API Functions
////////////////////////////////////////////////////////

int GetImageWidth(IMAGE image){
    return image->w;


}

int GetImageHeight(IMAGE image){
    return image->h;
}

RGBA* LockImage(IMAGE image){

    StarveThreadFunction();

    /////
    // By trust of the API, we know that `image` will not be used between this and UnlockImage(image).

    return image->pixels;

}
void UnlockImage(IMAGE image){
    FJ::GLstate::texture::resetSetup();
    glBindTexture(GL_TEXTURE_2D, image->glname);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image->w, image->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, image->pixels);
}

////////////////////////////////////////////////////////
// Image Direct API Functions
////////////////////////////////////////////////////////

void DirectBlit(int x, int y, int w, int h, RGBA* pixels){

    StarveThreadFunction();

    glRasterPos2i(x, y);
    glDrawPixels(w, h, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

}
void DirectTransformBlit(int x[4], int y[4], int w, int h, RGBA* pixels){

    IMAGE image = new FJ::UnsafeImage(w, h, pixels);

    FJ::Operation::Image *blit = new FJ::Operation::Image();
    blit->im = image;
    blit->blendmode = FJ::Sphere::bmBlend;
    blit->mask_blendmode = FJ::Sphere::bmBlend;

    blit->color = new RGBA[4];

    unsigned int mask = 0xFFFFFF;

    std::fill(blit->color, &(blit->color[3]), mask);

    blit->coord = new int[8];

    blit->coord[0] = x[0];
    blit->coord[1] = y[0];
    blit->coord[2] = x[1];
    blit->coord[3] = y[1];
    blit->coord[4] = x[2];
    blit->coord[5] = y[2];
    blit->coord[6] = x[3];
    blit->coord[7] = y[3];

    drawList.push(blit);

    StarveThreadFunction();

    delete image;

}
void DirectGrab(int x, int y, int w, int h, RGBA* pixels){

    StarveThreadFunction();

    glReadPixels(x, y, w, h, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
}

////////////////////////////////////////////////////////
// Simple Primitive API Functions
////////////////////////////////////////////////////////

void DrawPoint(int x, int y, RGBA color){

    if(!(color&0x000000FF))
        return;

    FJ::Operation::Point *point = new FJ::Operation::Point();

    point->color = new RGBA[1];
    point->color[0] = color;
    point->coord = new int[2];
    point->coord[0] = x;
    point->coord[1] = y;

    drawList.push(point);

}

void DrawLine(int x[2], int y[2], RGBA color){

    if(!(color&0x000000FF))
        return;

    RGBA colors[] = {color, color};
    DrawGradientLine(x, y, colors);

}
void DrawGradientLine(int x[2], int y[2], RGBA colors[2]){

    if(!((colors[0]|colors[1])&0x000000FF))
        return;

    FJ::Operation::Line *line = new FJ::Operation::Line();

    line->color = new RGBA[2];

    line->color[0] = colors[0];
    line->color[1] = colors[1];

    line->coord = new int[4];

    line->coord[0] = x[0];
    line->coord[1] = y[0];
    line->coord[2] = x[1];
    line->coord[3] = y[1];

    drawList.push(line);
}

/////
// TODO: Implement BezierCurve

void DrawBezierCurve(int x[4], int y[4], double step, RGBA color, int cubic){
    return;
}

void DrawTriangle(int x[3], int y[3], RGBA color){

    if(!(color&0x000000FF))
        return;

    RGBA colors[] = {color, color, color};

    DrawGradientTriangle(x, y, colors);

}

void DrawGradientTriangle(int x[3], int y[3], RGBA colors[3]){

    if(!((colors[0]|colors[1]|colors[2])&0x000000FF))
        return;

    FJ::Operation::Triangle *triangle = new FJ::Operation::Triangle();

    triangle->color = new RGBA[3];

    memcpy(triangle->color, colors, 3*sizeof(RGBA));

    triangle->coord = new int[6];

    triangle->coord[0] = x[0];
    triangle->coord[1] = y[0];
    triangle->coord[2] = x[1];
    triangle->coord[3] = y[1];
    triangle->coord[4] = x[2];
    triangle->coord[5] = y[2];

    drawList.push(triangle);
}

void DrawPolygon(int** points, int length, int invert, RGBA color){

    if(!(color&0x000000FF))
        return;

    FJ::Operation::Polygon *poly = new FJ::Operation::Polygon();

    poly->n = length;

    poly->color = new RGBA[length];

    std::fill(poly->color, poly->color+(length*sizeof(RGBA)), color);

    if(!invert){

        poly->coord = new int[length];

        memcpy(poly->coord, points, length*sizeof(int)<<1);

    }
    else{


    }

    drawList.push(poly);


}

void DrawRectangle(int x, int y, int w, int h, RGBA color){

    if(!(color&0x000000FF))
        return;

    RGBA colors[] = {color, color, color, color};

    DrawGradientRectangle(x, y, w, h, colors);

}

void DrawOutlinedRectangle(int x, int y, int w, int h, int size, RGBA color){

    FJ::Operation::Rect *rect = new FJ::Operation::Rect();

    rect->color = new RGBA[4];

    rect->outline = true;

    std::fill(rect->color, rect->color+(3*sizeof(RGBA)), color);

    rect->coord = new int[8];

    rect->coord[0] = x;
    rect->coord[1] = y;
    rect->coord[2] = x+w;
    rect->coord[3] = y;
    rect->coord[4] = x+w;
    rect->coord[5] = y+h;
    rect->coord[6] = x;
    rect->coord[7] = y+h;

    drawList.push(rect);

}

void DrawGradientRectangle(int x, int y, int w, int h, RGBA colors[4]){

    if(!((colors[0]|colors[1]|colors[2]|colors[3])&0x000000FF))
        return;

    FJ::Operation::Rect *rect = new FJ::Operation::Rect();

    rect->color = new RGBA[4];

    memcpy(rect->color, colors, 4*sizeof(RGBA));

    rect->outline = false;

    rect->coord = new int[8];

    rect->coord[0] = x;
    rect->coord[1] = y;
    rect->coord[2] = x+w;
    rect->coord[3] = y;
    rect->coord[4] = x+w;
    rect->coord[5] = y+h;
    rect->coord[6] = x;
    rect->coord[7] = y+h;

    drawList.push(rect);
}

////////////////////////////////////////////////////////
// Simple Primitive API Functions
////////////////////////////////////////////////////////

void DrawOutlinedComplex(int r_x, int r_y, int r_w, int r_h, int circ_x, int circ_y, int circ_r, RGBA color, int antialias){
    return;
}

void DrawFilledComplex(int r_x, int r_y, int r_w, int r_h, int circ_x, int circ_y, int circ_r, float angle, float frac_size, int fill_empty, RGBA colors[2]){
    return;
}

void DrawGradientComplex(int r_x, int r_y, int r_w, int r_h, int circ_x, int circ_y, int circ_r, float angle, float frac_size, int fill_empty, RGBA colors[3]){
    return;
}

void DrawOutlinedEllipse(int x, int y, int rx, int ry, RGBA color){
    return;
}

void DrawFilledEllipse(int x, int y, int rx, int ry, RGBA color){
    return;
}

void DrawOutlinedCircle(int x, int y, int r, RGBA color, int antialias){
    return;
}

void DrawFilledCircle(int x, int y, int r, RGBA color, int antialias){
    return;
}

void DrawGradientCircle(int x, int y, int r, RGBA colors[2], int antialias){
    return;
}

////////////////////////////////////////////////////////
// Array Primitive API Functions
////////////////////////////////////////////////////////

void DrawPointSeries(int** points, int length, RGBA color){
    for(int i = 0; i<length; i++){
        DrawPoint(points[i][0], points[i][1], color);
    }
}

void DrawLineSeries(int** points, int length, RGBA color, int type){
    //for(int i = 0; i<length; i++){
    //    DrawLine(points[i][0], points[i][1], points[i][2], points[i][3], color);
    //}
}
