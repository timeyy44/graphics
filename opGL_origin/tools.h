//
//  tools.h
//  opGL_origin
//
//  Created by 颜治文 on 2020/6/19.
//  Copyright © 2020 颜治文. All rights reserved.
//

#ifndef tools_h
#define tools_h

#define ANTIALIASING 10
#define MAX_REFLECT_DIFFUSE 10

class material;

struct hit_record {
    float t;
    vec3 p;
    vec3 normal;
    material *mat;
};

vec3 random_in_unit_sphere() {
    vec3 p;
    do {
        p = 2.f * vec3(drand48(), drand48(), drand48()) - vec3(1.f, 1.f, 1.f);
    } while (p.squared_length() >= 1.f);
    
    return p;
}

vec3 random_in_unit_circle() {
    vec3 p;
    do {
        p = 2.f * vec3(drand48(), drand48(), 0) - vec3(1.f, 1.f, 0);
    } while (p.squared_length() >= 1.f);
    
    return p;
}

vec3 reflect(const vec3& v, const vec3& n) {
    vec3 uv = unit_vector(v);
    return uv - 2*dot(uv, n)*n;
}

bool refract(const vec3& v, const vec3& n, float v_to_t, vec3& refracted) {
    vec3 uv = unit_vector(v);
    float vn = dot(uv, n);
    float discriminant = 1.f - v_to_t*v_to_t*(1.f-vn*vn);
    
    if (discriminant > 0) {
        refracted = v_to_t*(uv-vn*n) - n*sqrt(discriminant);
        return true;
    }
    return false;
}

float schlick(float cosine, float v_to_t) {
    float r0 = (1.f-v_to_t) / (1.f+v_to_t);
    r0 *= r0;
    return r0 + (1-r0)*pow((1-cosine), 5);
}

#endif /* tools_h */
