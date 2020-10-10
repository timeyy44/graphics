#pragma once

#include "glm/glm.hpp"

#include <stdlib.h>
#include <time.h>

#define drand48 rand_f

using namespace glm;

struct Material_s {
    fvec3 center;
    float r;
    fvec3 color;
    float ds;
    float de;
    int m;
};

float rand_f() {
    return (rand() % 100) / (float)101;
}

std::vector<Material_s> random_scene_f() {
    srand(time(0));
    int n = 500;
    std::vector<Material_s> list;
    Material_s material;

    material.center = fvec3(0, -1000.f, 0);
    material.r = 1000.f;
    material.m = 0;
    material.ds = 0;
    material.color = fvec3(0.5f, 0.5f, 0.5f);
    list.push_back(material);

    //list[0] = new plane(vec3(0, 1, 0), vec3(0, -1.f, 0), new dielectric(1.2f));
    //list[1] = new plane(vec3(0, 1, 0), vec3(0, -3.f, 0), new metal(vec3(0.8f, 0.8f, 0), 0.02f));

    int i = 1;
    for (int a = -11; a < 11; a++) {
        for (int b = -11; b < 11; b++) {
            float choose_mat = drand48();
            fvec3 center(a + 0.9f * drand48(), 0.2f, b + 0.9f * drand48());
            material.center = center;
            material.r = 0.2f;

            if ((center - fvec3(4, 0.2f, 0)).length() > 0.9) {
                if (choose_mat < 0.8) {
                    // diffuse
                    material.m = 0;
                    material.ds = 0;
                    material.color = fvec3(drand48() * drand48(), drand48() * drand48(), drand48() * drand48());
                }
                else if (choose_mat < 0.95) {
                    // metal
                    material.m = 1;
                    material.ds = 0.5f*drand48();
                    material.color = vec3(0.5f * (1 + drand48()), 0.5f * (1 + drand48()), 0.5f * (1 + drand48()));
                }
                else {
                    // glass
                    material.m = 2;
                    material.de = 1.2f;
                }
                list.push_back(material);
            }
        }
    }

    material.center = fvec3(0, 1.f, 0);
    material.r = 1.f;
    material.m = 0;
    material.ds = 0;
    material.color = vec3(0.4f, 0.2f, 0.1f);
    list.push_back(material);
    material.center = fvec3(-4.f, 1.f, 0);
    material.r = 1.f;
    material.m = 1;
    material.ds = 1.f;
    material.color = vec3(0.7f, 0.6f, 0.5f);
    list.push_back(material);
    material.center = fvec3(4, 1.f, 0);
    material.r = 1.f;
    material.m = 2;
    material.de = 1.5f;
    list.push_back(material);

    //return new hit_list(list, i);
    return list;
}