//
//  tools3.h
//  opGL_origin
//
//  Created by 颜治文 on 2020/10/12.
//  Copyright © 2020 颜治文. All rights reserved.
//

#ifndef tools3_h
#define tools3_h

using namespace std;
using namespace tbb;

vec3 color(const ray& r, hitable *world, int depth) {
    if (depth >= MAX_REFLECT_DIFFUSE)
        return vec3(0, 0, 0);
    
    hit_record rec;
    if (world->hit(0.001f, MAXFLOAT, r, rec)) {
        ray scattered;
        ray refra;
        float prob;
        bool isRefra;
        vec3 attenuation;
        if (rec.mat->scatter(r, rec, attenuation, scattered, refra, prob, isRefra)) {
            depth++;
            if (isRefra)
                if (prob > 0)
                    return attenuation*prob*color(scattered, world, depth) + (1.f-prob)*color(refra, world, depth);
                else
                    return color(refra, world, depth);
            else
                return attenuation * color(scattered, world, depth);
        } else
            return vec3(0, 0, 0);
    }
    
    //渲染背景颜色
    vec3 unit_direction = unit_vector(r.direction());
    float num = 0.5 * (unit_direction.y()+1.0);
    return (1.f-num)*vec3(1.f, 1.f, 1.f) + (num)*vec3(0.5f, 0.7f, 1.f);
}

void getUV(int num, int &u, int &v) {
    u = num / M_HEIGHT;
    v = num % M_HEIGHT;
}

class ApplyColor {
public:
    static camera *cam;
    static hitable *world;
    
    static vec3 colors[M_WIDTH][M_HEIGHT];
    
    void operator () (const blocked_range<size_t> & r) const
    {
        int u, v;
        for (size_t ii = r.begin(); ii != r.end(); ++ii) {
            vec3 Color(0, 0, 0);
            getUV((int)ii, u, v);
            for (int i = 0; i < ANTIALIASING; i++) {
                float ui = static_cast<float>(u+drand48()) / static_cast<float>(M_WIDTH);
                float vi = static_cast<float>(v+drand48()) / static_cast<float>(M_HEIGHT);
                
                Color += color(cam->getRay(ui, vi), world, 0);
            }
            Color /= static_cast<float>(ANTIALIASING);
            Color = vec3(sqrt(Color[0]), sqrt(Color[1]), sqrt(Color[2]));
            //drawPixel(u, v, Color);
            colors[u][v] = Color;
        }
    }
};

camera *ApplyColor::cam;
hitable *ApplyColor::world;

vec3 ApplyColor::colors[M_WIDTH][M_HEIGHT];

#endif /* tools3_h */
