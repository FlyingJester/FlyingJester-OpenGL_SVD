#ifndef FJ_GL32_WINDOW_HEAD
#define FJ_GL32_WINDOW_HEAD

#include <concurrent_queue.h>
#include <Windows.h>

bool  InitVideoDriver(HWND window, int screen_width, int screen_height);

namespace FJ{

class Image;

}

typedef  FJ::Image* IMAGE;

typedef struct
{
  byte red;
  byte green;
  byte blue;
  byte alpha;
} RGBA;

typedef struct
{
  const char* name;
  const char* author;
  const char* date;
  const char* version;
  const char* description;
} DRIVERINFO;

void GetDriverInfo(DRIVERINFO* driverinfo);
void ConfigureDriver(HWND parent);

void CloseVideoDriver(void);
bool ToggleFullscreen(void);

void FlipScreen(void);
void SetClippingRectangle(int x, int y, int w, int h);
void GetClippingRectangle(int* x, int* y, int* w, int* h);

IMAGE CreateImage(int width, int height, RGBA* pixels);
IMAGE CloneImage(IMAGE image);
IMAGE GrabImage(IMAGE image, int x, int y, int width, int height);
void DestroyImage(IMAGE image);
void BlitImage(IMAGE image, int x, int y, int blendmode);
void BlitImageMask(IMAGE image, int x, int y, int blendmode, RGBA mask, int mask_blendmode);
void TransformBlitImage(IMAGE image, int x[4], int y[4], int blendmode);
void TransformBlitImageMask(IMAGE image, int x[4], int y[4], int blendmode, RGBA mask, int mask_blendmode);
int GetImageWidth(IMAGE image);
int GetImageHeight(IMAGE image);
RGBA* LockImage(IMAGE image);
void UnlockImage(IMAGE image);
void DirectBlit(int x, int y, int w, int h, RGBA* pixels);
void DirectTransformBlit(int x[4], int y[4], int w, int h, RGBA* pixels);
void DirectGrab(int x, int y, int w, int h, RGBA* pixels);

void DrawPoint(int x, int y, RGBA color);
void DrawPointSeries(int** points, int length, RGBA color);
void DrawLine(int x[2], int y[2], RGBA color);
void DrawGradientLine(int x[2], int y[2], RGBA colors[2]);
void DrawLineSeries(int** points, int length, RGBA color, int type);
void DrawBezierCurve(int x[4], int y[4], double step, RGBA color, int cubic);
void DrawTriangle(int x[3], int y[3], RGBA color);
void DrawGradientTriangle(int x[3], int y[3], RGBA colors[3]);
void DrawPolygon(int** points, int length, int invert, RGBA color);
void DrawOutlinedRectangle(int x, int y, int w, int h, int size, RGBA color);
void DrawRectangle(int x, int y, int w, int h, RGBA color);
void DrawGradientRectangle(int x, int y, int w, int h, RGBA colors[4]);
void DrawOutlinedComplex(int r_x, int r_y, int r_w, int r_h, int circ_x, int circ_y, int circ_r, RGBA color, int antialias);
void DrawFilledComplex(int r_x, int r_y, int r_w, int r_h, int circ_x, int circ_y, int circ_r, float angle, float frac_size, int fill_empty, RGBA colors[2]);
void DrawGradientComplex(int r_x, int r_y, int r_w, int r_h, int circ_x, int circ_y, int circ_r, float angle, float frac_size, int fill_empty, RGBA colors[3]);
void DrawOutlinedEllipse(int x, int y, int rx, int ry, RGBA color);
void DrawFilledEllipse(int x, int y, int rx, int ry, RGBA color);
void DrawOutlinedCircle(int x, int y, int r, RGBA color, int antialias);
void DrawFilledCircle(int x, int y, int r, RGBA color, int antialias);
void DrawGradientCircle(int x, int y, int r, RGBA colors[2], int antialias);

namespace FJ {
	
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
	
	using namespace Concurrency;

	typedef concurrent_queue<FJ::Primitive *> DrawList;

	enum SetupType {eNone, ePrimitive = 0x01, eTexture = 0xF0};

	static const unsigned int HasOwnTexture = 0x80;

}
#endif
