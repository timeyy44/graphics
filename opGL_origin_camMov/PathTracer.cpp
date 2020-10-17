#include <tbb/tbb.h>
#include <time.h>
#include <iostream>
#include <vector>

#include "PathTracer.h"

#include "ray.h"
#include "tools.h"
#include "camera.h"
#include "material.h"
#include "aabb.h"
#include "hitable.h"
#include "bvh.h"
#include "tools2.h"
#include "tools3.h"

#define TBB_USED
float initZ = 100.f;

float getInitAngle();

float PathTracer::mouseWheel = 60.f;
vec3 PathTracer::camPos = vec3(0, 0, -initZ);//vec3(40.f, -180.f, 0.f);
vec3 PathTracer::camDir = unit_vector(vec3(0, 0, 0)-camPos);
vec3 PathTracer::up = vec3(0.f, 1.f, 0.f);

float getInitAngle() {
    float half = atan2(getMaxLen(), initZ);
    return half / M_PI * 360.f;
}

hitable *random_scene() {
    int n = 500;
    hitable **list = new hitable*[n];
    
    //list[0] = new sphere(vec3(0, -1000.f, 0), 1000, new lambertian(vec3(0.5f, 0.5f, 0.5f)));
    hitable** special = new hitable*[2];
    special[0] = new plane(vec3(0, 1, 0), vec3(0, -1.f, 0), new dielectric(1.2f));
    special[1] = new plane(vec3(0, 1, 0), vec3(0, -3.f, 0), new metal(vec3(0.8f, 0.8f, 0), 0.02f));
    
    //list[0] = new plane(vec3(0, 1, 0), vec3(0, -1.f, 0), new dielectric(1.2f));
    //list[1] = new plane(vec3(0, 1, 0), vec3(0, -3.f, 0), new metal(vec3(0.8f, 0.8f, 0), 0.02f));
    
    int i = 0;
    for (int a = -11; a < 11; a++) {
        for (int b = -11; b < 11; b++) {
            float choose_mat = drand48();
            vec3 center(a + 0.9f*drand48(), 0.2f, b + 0.9f*drand48());

            if ((center - vec3(4, 0.2f, 0)).length() > 0.9) {
                if (choose_mat < 0.8) {
                    // diffuse
                    list[i++] = new sphere(center, 0.2f, new lambertian(vec3(drand48()*drand48(), drand48()*drand48(), drand48()*drand48())));
                } else if (choose_mat < 0.95) {
                    // metal
                    list[i++] = new sphere(center, 0.2f, new metal(vec3(0.5f*(1+drand48()), 0.5f*(1+drand48()), 0.5f*(1+drand48())), 0.5*drand48()));
                } else {
                    // glass
                    list[i++] = new sphere(center, 0.2f, new dielectric(1.5f));
                }
            }
        }
    }
    
    list[i++] = new sphere(vec3(0, 1.f, 0), 1.f, new dielectric(1.5f));
    list[i++] = new sphere(vec3(-4.f, 1.f, 0), 1.f, new lambertian(vec3(0.4f, 0.2f, 0.1f)));
    list[i++] = new sphere(vec3(4, 1.f, 0), 1.f, new metal(vec3(0.7f, 0.6f, 0.5f), 0.0));
    
    //return new hit_list(list, i);
    return new bvh(list, special, 2, 0, i, 0, 0);
}

hitable *get_scene() {
    ifstream file("./resources/2q6f.txt");
    vector<vector<ATOM>*> *atomss = new vector<vector<ATOM>*>;
    if (!phraseA(file, atomss))
        return nullptr;
    int i = 0;
    int size = 0;
    for (vector<ATOM> *atoms : *atomss) {
        size += atoms->size();
    }
    
    vec3 basic = getBasic();
    PathTracer::mouseWheel = getInitAngle();
    
    hitable **list = new hitable *[size];
    vec3 color;
    float radius;
    for (vector<ATOM> *atoms : *atomss ) {
        for (struct ATOM atom : *atoms) {
            color = getColor(atom.name);
            radius = getRadius(atom.name);
            list[i++] = new sphere(atom.pos-basic, radius, new lambertian(color));
        }
    }
    
    return new bvh(list, 0, size, 0, 0);
}

PathTracer::PathTracer()
	: m_channel(4), m_width(M_WIDTH), m_height(M_HEIGHT), m_image(nullptr) {}

PathTracer::~PathTracer()
{
	if (m_image != nullptr)
		m_image;
	m_image = nullptr;
}

void PathTracer::initialize(int width, int height)
{
	m_width = width;
	m_height = height;
	if (m_image != nullptr)
		delete m_image;

	// allocate pixel buffer, RGBA format.
	m_image = new unsigned char[width * height * m_channel];
}

unsigned char * PathTracer::render(double & timeConsuming, const vec3 &camPos)
{
	if (m_image == nullptr)
	{
		std::cout << "Must call initialize() before rendering.\n";
		return nullptr;
	}

	// record start time.
	double startFrame = clock();
    
    /*int sNum = 3;
    hitable *list[sNum];
    list[0] = new sphere(vec3(0, 0, -3.f), 1.f, new lambertian(vec3(0.8f, 0.3f, 0.3f)));
    //list[0] = new mv_sphere(1.f, new lambertian(vec3(0.8f, 0.3f, 0.3f)), 0, 1, vec3(0, 0, -3.f), vec3(0, 0.5f, -3.f));
    //list[1] = new sphere(vec3(0, -101.f, -3.f), 100.f, new lambertian(vec3(0.8f, 0.8f, 0)));
    //list[1] = new plane(vec3(0, 1, 0), vec3(0, -1.f, 0), new dielectric(1.2f));
    list[1] = new sphere(vec3(2.f, 0.f, -3.f), 1.f, new metal(vec3(0.8f, 0.6f, 0.2f), 0.3f));
    list[2] = new sphere(vec3(-2.f, 0.f, -3.f), 1.f, new dielectric(3.f));
    //list[4] = new plane(vec3(0, 1, 0), vec3(0, -3.f, 0), new metal(vec3(0.8f, 0.8f, 0), 0.02f));
    
    int rNum = 2;
    hitable *special[rNum];
    special[0] = new plane(vec3(0, 1, 0), vec3(0, -1.f, 0), new dielectric(1.2f));
    special[1] = new plane(vec3(0, 1, 0), vec3(0, -3.f, 0), new metal(vec3(0.8f, 0.8f, 0), 0.02f));
     */
    
    //hitable *world = new hit_list(list, sNum);
    //hitable *world = random_scene();
    //hitable *world = new bvh(list, 0, sNum, 0, 0);
    //hitable *world = new bvh(list, special, rNum, 0, sNum, 0, 0);
    static hitable *world = get_scene();
    
    camera cam(PathTracer::mouseWheel, (float)M_WIDTH/M_HEIGHT, PathTracer::camPos, PathTracer::camPos+PathTracer::camDir, PathTracer::up, 0);
      
#ifdef TBB_USED
    
    ApplyColor::cam = &cam;
    ApplyColor::world = world;
    
    task_scheduler_init init(100);
    parallel_for(blocked_range<size_t>(0, M_WIDTH*M_HEIGHT), ApplyColor());
    for (int row = 0; row < m_width; row++) {
        for (int col = 0; col < m_height; col++) {
            drawPixel(row, col, ApplyColor::colors[row][col]);
        }
    }
    
#else
    
    int u, v;
        ray ra;
        vec3 co;
    vec3 colors[M_WIDTH][M_HEIGHT];
        for (int ii = 0; ii < m_width*m_height; ++ii) {
            vec3 Color(0, 0, 0);
            getUV((int)ii, u, v);
            for (int i = 0; i < ANTIALIASING; i++) {
                float ui = static_cast<float>(u+drand48()) / static_cast<float>(M_WIDTH);
                float vi = static_cast<float>(v+drand48()) / static_cast<float>(M_HEIGHT);
                
                Color += color(cam.getRay(ui, vi), world, 0);
            }
            Color /= static_cast<float>(ANTIALIASING);
            Color = vec3(sqrt(Color[0]), sqrt(Color[1]), sqrt(Color[2]));
            //drawPixel(u, v, Color);
            colors[u][v] = Color;
        }
    
    for (int row = 0; row < m_width; row++) {
        for (int col = 0; col < m_height; col++) {
            drawPixel(row, col, colors[row][col]);
        }
    }
    
#endif

	// render the image pixel by pixel.
	/*for (int row = m_height - 1; row >= 0; --row)
	{
		for (int col = 0; col < m_width; ++col)
		{
			// TODO: implement your ray tracing algorithm by yourself.
            
            vec3 Color(0, 0, 0);
            for (int i = 0; i < ANTIALIASING; i++) {
                float u = static_cast<float>(col+drand48()) / static_cast<float>(m_width);
                float v = static_cast<float>(row+drand48()) / static_cast<float>(m_height);
                
                Color += color(cam.getRay(u, v), world, 0);
            }
            Color /= static_cast<float>(ANTIALIASING);
            Color = vec3(sqrt(Color[0]), sqrt(Color[1]), sqrt(Color[2]));
			drawPixel(col, row, Color);
		}
	}*/

	// record end time.
	double endFrame = clock();

	// calculate time consuming.
	timeConsuming = static_cast<double>(endFrame - startFrame) / CLOCKS_PER_SEC;

	return m_image;
}

void PathTracer::drawPixel(unsigned int x, unsigned int y, const vec3 & color)
{
	// Check out 
	if (x < 0 || x >= m_width || y < 0 || y >= m_height)
		return;
	// x is column's index, y is row's index.
	unsigned int index = (y * m_width + x) * m_channel;
	// store the pixel.
	// red component.
	m_image[index + 0] = static_cast<unsigned char>(255 * color.x());
	// green component.
	m_image[index + 1] = static_cast<unsigned char>(255 * color.y());
	// blue component.
	m_image[index + 2] = static_cast<unsigned char>(255 * color.z());
	// alpha component.
	m_image[index + 3] = static_cast<unsigned char>(255);
}
