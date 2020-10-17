//
//  hitable.h
//  opGL_origin
//
//  Created by 颜治文 on 2020/6/19.
//  Copyright © 2020 颜治文. All rights reserved.
//

#ifndef hitable_h
#define hitable_h

class hitable {
public:
    virtual bool hit(float t_min, float t_max, const ray& r, hit_record& hr) const = 0;
    virtual bool bounding_box(float t0, float t1, aabb& box) const = 0;
    virtual ~hitable() {};
};

class sphere : public hitable {
public:
    sphere() {}
    sphere(vec3 center, float radius, material *mat) : center(center), radius(radius), mat(mat) {}
    virtual bool hit(float t_min, float t_max, const ray& r, hit_record& hr) const;
    virtual bool bounding_box(float t0, float t1, aabb& box) const;
    
    vec3 center;
    float radius;
    material *mat;
};

bool sphere::hit(float t_min, float t_max, const ray& r, hit_record& rec) const {
    vec3 oc = r.origin() - center;
    
    float a = dot(r.direction(), r.direction());
    float b = dot(oc, r.direction());
    float c = dot(oc, oc) - radius*radius;
    float discriminant = b*b - a*c;
    
    if (discriminant > 0) {
        float tmp = (-b - sqrt(discriminant)) / a;
        if (tmp > t_min && tmp < t_max) {
            rec.t = tmp;
            rec.p = r.point_at_parameter(tmp);
            rec.normal = (rec.p-center) / radius;
            rec.mat = mat;
            return true;
        }
        
        tmp = (-b + sqrt(discriminant)) / a;
        if (tmp > t_min && tmp < t_max) {
            rec.t = tmp;
            rec.p = r.point_at_parameter(tmp);
            rec.normal = (rec.p-center) / radius;
            rec.mat = mat;
            return true;
        }
    }
    
    return false;
}

bool sphere::bounding_box(float t0, float t1, aabb &box) const {
    box = aabb{
        center - vec3(radius, radius, radius),
        center + vec3(radius, radius, radius)
    };
    return true;
}

class mv_sphere : public hitable{
public:
    mv_sphere() {}
    mv_sphere(float r, material* m, float t0, float t1, vec3 c0, vec3 c1) : radius(r), mat(m), tm0(t0), tm1(t1), ce0(c0), ce1(c1) {}
    virtual bool hit(float t_min, float t_max, const ray& r, hit_record& hr) const;
    virtual bool bounding_box(float t0, float t1, aabb &box) const;
    inline vec3 center(float time) const {return ce0 + (ce1-ce0)*((time-tm0)/(tm1-tm0));}
    
    float radius;
    float tm0, tm1;
    vec3 ce0, ce1;
    material *mat;
};

bool mv_sphere::hit(float t_min, float t_max, const ray& r, hit_record& rec) const {
    vec3 oc = r.origin() - center(r.time());
    
    float a = dot(r.direction(), r.direction());
    float b = dot(oc, r.direction());
    float c = dot(oc, oc) - radius*radius;
    float discriminant = b*b - a*c;
    
    if (discriminant > 0) {
        float tmp = (-b - sqrt(discriminant)) / a;
        if (tmp > t_min && tmp < t_max) {
            rec.t = tmp;
            rec.p = r.point_at_parameter(tmp);
            rec.normal = (rec.p-center(r.time())) / radius;
            rec.mat = mat;
            return true;
        }
        
        tmp = (-b + sqrt(discriminant)) / a;
        if (tmp > t_min && tmp < t_max) {
            rec.t = tmp;
            rec.p = r.point_at_parameter(tmp);
            rec.normal = (rec.p-center(r.time())) / radius;
            rec.mat = mat;
            return true;
        }
    }
    
    return false;
}

bool mv_sphere::bounding_box(float t0, float t1, aabb &box) const {
    aabb a(
        center(t0) - vec3(radius, radius, radius),
        center(t0) + vec3(radius, radius, radius)
    );
    aabb b(
        center(t1) - vec3(radius, radius, radius),
        center(t1) + vec3(radius, radius, radius)
    );
    box = union_aabb(a, b);
    return true;
}

class plane : public hitable {
public:
    plane() {}
    plane(vec3 nor, vec3 p, material *m) : normal(nor), _p(p), mat(m) {}
    virtual bool bounding_box(float t0, float t1, aabb &box) const;
    virtual bool hit(float t_min, float t_max, const ray& r, hit_record& hr) const;
    
    vec3 normal;
    vec3 _p;
    material *mat;
};

bool plane::hit(float t_min, float t_max, const ray& r, hit_record& hr) const {
    float hori = dot(r.direction(), normal);
    const float EPSILON = 0.001;
    if (hori < EPSILON && hori > -EPSILON)
        return false;
    
    float t = dot(normal, (_p - r.origin())) / dot(normal, r.direction());
    if (t <= EPSILON || t <= t_min || t >= t_max)
        return false;
    hr.mat = mat;
    hr.normal = normal;
    hr.t = t;
    hr.p = r.point_at_parameter(t);
    return true;
}

bool plane::bounding_box(float t0, float t1, aabb &box) const {
    return false;
}

class hit_list : public hitable {
public:
    hit_list() {}
    hit_list(hitable **list, int num) : list(list), num(num) {}
    virtual bool hit(float t_min, float t_max, const ray& r, hit_record& rec) const;
    virtual bool bounding_box(float t0, float t1, aabb& box) const;
    ~hit_list();
    
    hitable **list;
    int num;
};

bool hit_list::hit(float t_min, float t_max, const ray& r, hit_record& rec) const {
    hit_record tmp_rec;
    bool isHit = false;
    float closest = t_max;
    
    for (int i = 0; i < num; i++) {
        if (list[i]->hit(t_min, closest, r, tmp_rec)) {
            isHit = true;
            closest = tmp_rec.t;
            rec = tmp_rec;
        }
    }
    
    return isHit;
}

bool hit_list::bounding_box(float t0, float t1, aabb& box) const {
    if (num == 0) return false;
    
    list[0]->bounding_box(t0, t1, box);
    if (num == 1) return true;
    
    aabb tmp;
    for (int i = 1; i < num; i++) {
        list[i]->bounding_box(t0, t1, tmp);
        box = union_aabb(box, tmp);
    }
    return true;
}

hit_list::~hit_list() {
    for (int i = 0; i < num; i++) {
        delete list[i];
        list[i] = nullptr;
    }
    list = nullptr;
}

#endif /* hitable_h */
