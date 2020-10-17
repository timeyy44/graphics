#ifndef PATH_TRACER_H
#define PATH_TRACER_H

#include "vec3.h"

#define M_WIDTH 800
#define M_HEIGHT 600

#define WINNAME "test"
#define WHEEL_OFFSET 10.f
#define CAM_SPEED 0.5f

class PathTracer
{
private:
	// RGBA format.
	int m_channel;
	// image's size.
	int m_width, m_height;
	// image's pixel buffer.
	unsigned char *m_image;

public:
    static float mouseWheel;
    static vec3 camDir;
    static vec3 camPos;
    static vec3 up;
    static vec3 target;
    
	// Ctor/Dtor.
	PathTracer();
	~PathTracer();

	// Getter.
	int getWidth() const { return m_width; }
	int getHeight() const { return m_height; }
	int getChanne() const { return m_channel; }
	unsigned char *getImage() const { return m_image; }

	// Must call it for the first of all.
	void initialize(int width, int height);

	// Render a frame.
	unsigned char *render(double &timeConsuming, const vec3 &camPos);

private:
	// Draw one pixel in (y,x).
	void drawPixel(unsigned int x, unsigned int y, const vec3 &color);
};

#endif
