//
//  aabb.h
//  opGL_origin
//
//  Created by 颜治文 on 2020/9/21.
//  Copyright © 2020 颜治文. All rights reserved.
//

#ifndef aabb_h
#define aabb_h

class aabb {
public:
    aabb(){}
    aabb(const vec3& mi, const vec3& ma) : minP(mi), maxP(ma) {}
    inline vec3 min() const { return minP; }
    inline vec3 max() const { return maxP; }
    
    bool hit(const ray& r, float tmin, float tmax) const;
    
    vec3 minP;
    vec3 maxP;
};

bool aabb::hit(const ray &r, float tmin, float tmax) const {
    for (int i = 0; i < 3; i++) {
        auto inDir = 1.f / r.direction()[i];
        auto t0 = (min()[i] - r.origin()[i]) * inDir;
        auto t1 = (max()[i] - r.origin()[i]) * inDir;
        if (inDir < 0.f)
            std::swap(t0, t1);
        tmin = t0 > tmin ? t0 : tmin;
        tmax = t1 < tmax ? t1 : tmax;
        
        if (tmin >= tmax)
            return false;
    }
    return true;
}

aabb union_aabb(const aabb& a, const aabb& b) {
    vec3 sMin(fmin(a.min().x(), b.min().x()),
               fmin(a.min().y(), b.min().y()),
               fmin(a.min().z(), b.min().z()));
    vec3 sMax(fmax(a.max().x(), b.max().x()),
              fmax(a.max().y(), b.max().y()),
              fmax(a.max().z(), b.max().z()));
    return aabb(sMin, sMax);
}

#endif /* aabb_h */
