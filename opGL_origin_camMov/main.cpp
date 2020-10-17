#include <iostream>
#include <opencv2/opencv.hpp>

// do not modify it.
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include "PathTracer.h"

bool firstMouse = true;
float lastX, lastY;
float yaw = 90.f;
float pitch = 0.f;

void handleKeyInput(int key);
void my_callBack(int event, int x, int y, int flags, void *param);

int main(int argc, char *argv[])
{
    //opencv callback
    cv::namedWindow(WINNAME,cv::WINDOW_AUTOSIZE);
    cv::setMouseCallback(WINNAME, my_callBack);
    
	// initialization.
	PathTracer tracer;
	tracer.initialize(M_WIDTH, M_HEIGHT);
	
    int count = 0;
    int maxCount = 100;
    float angle = 2*M_PI/maxCount;
    int key = -1;
    while(key != 27) {
	// rendering.
	double timeConsuming = 0.0f;
        //vec3 camPos = vec3(24.f, -180.f, 60.f);
        tracer.render(timeConsuming, vec3(40.f*cos(angle*count), -180.f, 40.f*sin(angle*count)));

	// save pixels to .png file using stb_image.
	/*stbi_flip_vertically_on_write(1);
	stbi_write_png("./result/result2.png",
		tracer.getWidth(),
		tracer.getHeight(),
		4,
		static_cast<void*>(tracer.getImage()),
		tracer.getWidth() * 4);*/
        
        cv::Mat image(M_HEIGHT, M_WIDTH, CV_8UC4, tracer.getImage());
        cv::imshow(WINNAME, image);
        
        key = cv::waitKey(10);
        
        handleKeyInput(key);

	//std::cout << count << " Rendering Finished.\n";
	//std::cout << "Time consuming: " << timeConsuming << " secs.\n";
    }

	return 0;
}

void handleKeyInput(int key) {
    switch (key) {
        case 119:
            PathTracer::camPos += CAM_SPEED * PathTracer::camDir;
            break;
        case 115:
            PathTracer::camPos -= CAM_SPEED * PathTracer::camDir;
            break;
        case 97:
            PathTracer::camPos -= CAM_SPEED * unit_vector(cross(PathTracer::camDir, PathTracer::up));
            break;
        case 100:
            PathTracer::camPos += CAM_SPEED * unit_vector(cross(PathTracer::camDir, PathTracer::up));
            break;
        default:
            break;
    }
}

float getRadians(float angle) {
    return angle / 180.0 * M_PI;
}

void mouseMove(int x, int y) {
    if(firstMouse)
    {
        lastX = x;
        lastY = y;
        firstMouse = false;
    }
    
    float xoffset = x - lastX;
    float yoffset = lastY - y;
    lastX = x;
    lastY = y;

    float sensitivity = 0.05;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw   += xoffset;
    pitch -= yoffset;

    if(pitch > 89.0f)
        pitch = 89.0f;
    if(pitch < -89.0f)
        pitch = -89.0f;
    
    //std::cout << yaw << " " << pitch << std::endl;

    vec3 front = vec3(cos(getRadians(yaw)) * cos(getRadians(pitch)),
                      sin(getRadians(pitch)),
                      sin(getRadians(yaw)) * cos(getRadians(pitch)));
    PathTracer::camDir = unit_vector(front);
}

void my_callBack(int event, int x, int y, int flags, void *param) {
    float value;
    switch (event) {
        case cv::EVENT_MOUSEWHEEL:
            value = cv::getMouseWheelDelta(flags);
            PathTracer::mouseWheel += (value > 0) ? WHEEL_OFFSET : -WHEEL_OFFSET;
            std::cout << "get This!\n";
            break;
        case cv::EVENT_LBUTTONDOWN:
            PathTracer::mouseWheel += -WHEEL_OFFSET;
            std::cout << "left button down!\n";
            break;
        case cv::EVENT_RBUTTONDOWN:
            PathTracer::mouseWheel -= -WHEEL_OFFSET;
            std::cout << "right button down!\n";
            break;
        default:
            break;
    }
    mouseMove(x, y);
}
