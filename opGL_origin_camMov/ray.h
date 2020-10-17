//
//  ray.h
//  opGL_origin
//
//  Created by 颜治文 on 2020/6/19.
//  Copyright © 2020 颜治文. All rights reserved.
//

#ifndef ray_h
#define ray_h

class ray {
public:
    ray() {}
    ray(const vec3& a, const vec3& b, float t = 0.f) {
        A = a;
        //B = unit_vector(b);
        B = b;
        Time = t;
    }
    
    inline vec3 origin() const { return A; }
    inline vec3 direction() const { return B; }
    inline vec3 point_at_parameter(float t) const { return A + t*B; }
    inline float time() const {return Time;}
    
    vec3 A;
    vec3 B;
    float Time;
};

#endif /* ray_h */
