#ifndef FJ_GL32_API_MAIN_HEAD
#define FJ_GL32_API_MAIN_HEAD

#ifndef FJ_GL32_API_HEAD

	/////
	//Include the platform-specific API code if we don't have it already.

    #ifdef _WIN32
        #include "windows/api.h"
    #endif // _WIN32

    #ifdef __linux__
        #include "linux/api.h"
    #endif // __linux__

	/////
	//Verify we got an API file, and that it at least looks like the right kind of header.

    #ifndef FJ_GL32_API_HEAD
        #error Invalid or no api file included.
    #endif


#endif

#ifndef EXPORT
    #error Invalid or no api file included. No EXPORT macro was defined.
#endif

/////
//Use MSVC concurrent queue on MSVC 2010 or greater, Intel Thread Building Blocks concurrent thread otherwise (unix, older MSVC).

#include "universal/concurrent_queue.h"

namespace FJ{

class Image;

}

typedef  FJ::Image* IMAGE;

typedef int RGBA;

typedef struct
{
  const char* name;
  const char* author;
  const char* date;
  const char* version;
  const char* description;
} DRIVERINFO;
extern "C" {
EXPORT(void GetDriverInfo(DRIVERINFO* driverinfo));

EXPORT(void CloseVideoDriver(void));
EXPORT(bool ToggleFullscreen(void));

EXPORT(void FlipScreen(void));
EXPORT(void SetClippingRectangle(int x, int y, int w, int h));
EXPORT(void GetClippingRectangle(int* x, int* y, int* w, int* h));

EXPORT(IMAGE CreateImage(int width, int height, RGBA* pixels));
EXPORT(IMAGE CloneImage(IMAGE image));
EXPORT(IMAGE GrabImage(IMAGE image, int x, int y, int width, int height));
EXPORT(void DestroyImage(IMAGE image));
EXPORT(void BlitImage(IMAGE image, int x, int y, int blendmode));
EXPORT(void BlitImageMask(IMAGE image, int x, int y, int blendmode, RGBA mask, int mask_blendmode));
EXPORT(void TransformBlitImage(IMAGE image, int x[4], int y[4], int blendmode));
EXPORT(void TransformBlitImageMask(IMAGE image, int x[4], int y[4], int blendmode, RGBA mask, int mask_blendmode));
EXPORT(int GetImageWidth(IMAGE image));
EXPORT(int GetImageHeight(IMAGE image));
EXPORT(RGBA* LockImage(IMAGE image));
EXPORT(void UnlockImage(IMAGE image));
EXPORT(void DirectBlit(int x, int y, int w, int h, RGBA* pixels));
EXPORT(void DirectTransformBlit(int x[4], int y[4], int w, int h, RGBA* pixels));
EXPORT(void DirectGrab(int x, int y, int w, int h, RGBA* pixels));

EXPORT(void DrawPoint(int x, int y, RGBA color));
EXPORT(void DrawPointSeries(int** points, int length, RGBA color));
EXPORT(void DrawLine(int x[2], int y[2], RGBA color));
EXPORT(void DrawGradientLine(int x[2], int y[2], RGBA colors[2]));
EXPORT(void DrawLineSeries(int** points, int length, RGBA color, int type));
EXPORT(void DrawBezierCurve(int x[4], int y[4], double step, RGBA color, int cubic));
EXPORT(void DrawTriangle(int x[3], int y[3], RGBA color));
EXPORT(void DrawGradientTriangle(int x[3], int y[3], RGBA colors[3]));
EXPORT(void DrawPolygon(int** points, int length, int invert, RGBA color));
EXPORT(void DrawOutlinedRectangle(int x, int y, int w, int h, int size, RGBA color));
EXPORT(void DrawRectangle(int x, int y, int w, int h, RGBA color));
EXPORT(void DrawGradientRectangle(int x, int y, int w, int h, RGBA colors[4]));
EXPORT(void DrawOutlinedComplex(int r_x, int r_y, int r_w, int r_h, int circ_x, int circ_y, int circ_r, RGBA color, int antialias));
EXPORT(void DrawFilledComplex(int r_x, int r_y, int r_w, int r_h, int circ_x, int circ_y, int circ_r, float angle, float frac_size, int fill_empty, RGBA colors[2]));
EXPORT(void DrawGradientComplex(int r_x, int r_y, int r_w, int r_h, int circ_x, int circ_y, int circ_r, float angle, float frac_size, int fill_empty, RGBA colors[3]));
EXPORT(void DrawOutlinedEllipse(int x, int y, int rx, int ry, RGBA color));
EXPORT(void DrawFilledEllipse(int x, int y, int rx, int ry, RGBA color));
EXPORT(void DrawOutlinedCircle(int x, int y, int r, RGBA color, int antialias));
EXPORT(void DrawFilledCircle(int x, int y, int r, RGBA color, int antialias));
EXPORT(void DrawGradientCircle(int x, int y, int r, RGBA colors[2], int antialias));
}
namespace FJ {

    void Sleep(unsigned int milliseconds);

	class Image{
	public:
		Image(unsigned int w, unsigned int h, RGBA *pixels);
		~Image();
		unsigned int glname;

		unsigned int w, h;

	};

	enum PrimitiveType {ePoint, eLine, ePolygon, eRect, eEllipse, eImage};

	class Primitive{
	public:

		virtual void drawGL(void) = 0;
		void setupGL(void);
		virtual void textureSetup() = 0;
		PrimitiveType type;
		int *coord;
		RGBA *color;
	};

	namespace Operation {

		struct ClippingRectangleSet : public Primitive{
			void textureSetup();
			void drawGL();
		};

		struct FlipScreen : public Primitive{
			void textureSetup();
			void drawGL();

		};

		class UntexturePrimitive : public Primitive{
		public:
			virtual void drawGL(void) = 0;
			void textureSetup();
		};


		class TexturePrimitive : public Primitive{
		public:
			virtual void drawGL(void) = 0;
			void textureSetup();
			unsigned int texture;
		};

		struct Point : public UntexturePrimitive{
			void drawGL(void);
		};

		struct Line : public UntexturePrimitive{
			void drawGL(void);
		};


		struct Polygon : public UntexturePrimitive{
			void drawGL(void);
			unsigned int n;
			bool outline;
		};

		struct Rect : public UntexturePrimitive{
			void drawGL(void);
			bool outline;
		};

		struct Ellipse : public UntexturePrimitive{
			void drawGL(void);
			int r;
		};

		struct Image : public TexturePrimitive{
			void drawGL(void);
			IMAGE im;
		};

	}


	enum SetupType {eNone, ePrimitive = 0x01, eTexture = 0xF0};

	static const unsigned int HasOwnTexture = 0x80;

}
#endif