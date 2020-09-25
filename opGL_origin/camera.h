//
//  camera.h
//  opGL_origin
//
//  Created by 颜治文 on 2020/6/19.
//  Copyright © 2020 颜治文. All rights reserved.
//

#ifndef camera_h
#define camera_h

class camera {
public:
    camera(float vfov, float aspect, vec3 lookFrom, vec3 lookAt, vec3 up, float sperture, float t0 = 0, float t1 = 0) {
        //设置射线方向
        float theta = vfov / 180.f * M_PI;
        float h_height = tan(theta/2.f);
        float h_width = h_height * aspect;
        float disk_to_focus = (lookFrom-lookAt).length();
        
        origin = lookFrom;
        lens_radius = sperture / 2.f;
        tm0 = t0;
        tm1 = t1;
        
        w = unit_vector(lookFrom - lookAt);
        u = unit_vector(cross(up, w));
        v = cross(w, u);
        
        lower_left_corner = lookFrom - disk_to_focus*(w + h_width*u + h_height*v);
        horizontal = 2.f * disk_to_focus * h_width * u;
        vertical = 2.f * disk_to_focus * h_height * v;
    }
    
    ray getRay(float s, float t) {
        vec3 rd = lens_radius*random_in_unit_circle();
        vec3 offset = u * rd.x() + v * rd.y();
        float rd_time = tm0 + (tm1-tm0)*drand48();
        return ray(origin+offset, lower_left_corner+s*horizontal+t*vertical-origin-offset, rd_time);
    }
    
    vec3 origin;
    vec3 lower_left_corner;
    vec3 horizontal;
    vec3 vertical;
    
    vec3 u;
    vec3 v;
    vec3 w;
    float lens_radius;
    float tm0, tm1;
};

#endif /* camera_h */
