//
//  bvh.h
//  opGL_origin
//
//  Created by 颜治文 on 2020/9/23.
//  Copyright © 2020 颜治文. All rights reserved.
//

#ifndef bvh_h
#define bvh_h

#include <algorithm>

class bvh : public hitable {
public:
    bvh();
    bvh(hitable** list, int start, int end, float t0, float t1);
    bvh(hit_list* world, float t0, float t1) : bvh(world->list, 0, world->num, t0, t1) {}
    bvh(hitable** list, hitable** s, int sNum, int start, int end, float t0, float t1);
    
    bool _hit(float t_min, float t_max, const ray &r, hit_record &hr) const;
    
    virtual bool hit(float t_min, float t_max, const ray& r, hit_record& hr) const;
    virtual bool bounding_box(float t0, float t1, aabb& box) const;
    
    hitable *left, *right;
    hitable** special;
    int sNum;
    aabb box;
};

bool bvh_compare(const hitable* a, const hitable* b, int axis) {
    aabb a1, a2;
    if (!a->bounding_box(0, 0, a1) || !b->bounding_box(0, 0, a2)) {
        std::cerr << "exists several hitable of no bounding box." << std::endl;
        exit(1);
    }
    return a1.min().e[axis] < a2.min().e[axis];
}

bool bvh_compare_x(const hitable* a, const hitable* b) {
    return bvh_compare(a, b, 0);
}

bool bvh_compare_y(const hitable* a, const hitable* b) {
    return bvh_compare(a, b, 1);
}

bool bvh_compare_z(const hitable* a, const hitable* b) {
    return bvh_compare(a, b, 2);
}

bool bvh::bounding_box(float t0, float t1, aabb &box) const {
    box = this->box;
    return true;
}

bool bvh::hit(float t_min, float t_max, const ray &r, hit_record &hr) const {
    bool fHit = _hit(t_min, t_max, r, hr);
    bool sHit = false;
    if (sNum != -1) {
        if (fHit)
            t_max = hr.t;
        for (int i = 0; i < sNum; i++) {
            if (special[i]->hit(t_min, t_max, r, hr)) {
                sHit = true;
                t_max = hr.t;
            }
        }
    }
    return fHit || sHit;
}

bool bvh::_hit(float t_min, float t_max, const ray &r, hit_record &hr) const {
    if (!box.hit(r, t_min, t_max))
        return false;
    
    bool hit_l = left->hit(t_min, t_max, r, hr);
    if (hit_l)
        t_max = hr.t;
    bool hit_r = right->hit(t_min, t_max, r, hr);
    
    return hit_l || hit_r;
}

bvh::bvh(hitable** list, int start, int end, float t0, float t1) {
    sNum = -1;
    int axis = (int)(drand48() * 3);
    auto comparator = (axis == 0) ? bvh_compare_x : (axis == 1) ? bvh_compare_y : bvh_compare_z;
    
    int length = end - start;
    if (length == 1)
        left = right = list[start];
    else if (length == 2) {
        if (comparator(list[start], list[start+1])) {
            left = list[start];
            right = list[start+1];
        } else {
            left = list[start+1];
            right = list[start];
        }
    } else {
        std::sort(&list[start], &list[end], comparator);
        int mid = start + length/2;
        left = new bvh(list, start, mid, t0, t1);
        right = new bvh(list, mid, end, t0, t1);
    }
    
    aabb a1, a2;
    if (!left->bounding_box(t0, t1, a1) || !right->bounding_box(t0, t1, a2)) {
        std::cerr << "exists several hitable of no bounding box." << std::endl;
        exit(1);
    }
    box = union_aabb(a1, a2);
}

bvh::bvh(hitable** list, hitable** s, int n, int start, int end, float t0, float t1) : bvh::bvh(list, start, end, t0, t1) {
    special = s;
    sNum = n;
}

#endif /* bvh_h */
