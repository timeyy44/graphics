//
//  material.h
//  opGL_origin
//
//  Created by 颜治文 on 2020/6/20.
//  Copyright © 2020 颜治文. All rights reserved.
//

#ifndef material_h
#define material_h

class material {
public:
    virtual bool scatter(const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered, ray& refra, float& prob, bool& isRefra) const = 0;
};

class lambertian : public material {
public:
    lambertian(const vec3& a) : albedo(a) {}
    virtual bool scatter(const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered, ray& refra, float& prob, bool& isRefra) const;
    
    vec3 albedo;
};

bool lambertian::scatter(const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered, ray& refra, float& prob, bool& isRefra) const {
    vec3 target = rec.p + rec.normal + random_in_unit_sphere();
    scattered = ray(rec.p, target-rec.p, r_in.time());
    isRefra = false;
    attenuation = albedo;
    
    return true;
}

class metal : public material {
public:
    metal(vec3 a, float f) : albedo(a) {
        if (f < 1)
            fuzz = f;
        else
            fuzz = 1.f;
    }
    virtual bool scatter(const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered, ray& refra, float& prob, bool& isRefra) const;
    
    vec3 albedo;
    float fuzz;
};

bool metal::scatter(const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered, ray& refra, float& prob, bool& isRefra) const {
    if (dot(r_in.direction(), rec.normal) > 0)
        return false;
    
    vec3 reflected = reflect(r_in.direction(), rec.normal);
    scattered = ray(rec.p, reflected + fuzz*random_in_unit_sphere());
    isRefra = false;
    attenuation = albedo;
    return true;
}

class dielectric : public material {
public:
    dielectric(float n) : n(n) {}
    virtual bool scatter(const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered, ray& refra, float& prob, bool& isRefra) const;
    
    float n;
};

bool dielectric::scatter(const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered, ray& refra, float& prob, bool& isRefra) const {
    vec3 tNorm;
    vec3 reflected; //= reflect(r_in.direction(), rec.normal);
    vec3 refracted;
    float reflect_prob;
    float cosine;
    float v_to_t;
    attenuation = vec3(1.f, 1.f, 1.f);
    isRefra = true;
    
    if (dot(r_in.direction(), rec.normal) > 0) {
        tNorm = -rec.normal;
        v_to_t = n;
        cosine = n * dot(r_in.direction(), rec.normal) / r_in.direction().length();
    } else {
        tNorm = rec.normal;
        v_to_t = 1.f / n;
        cosine = -dot(r_in.direction(), rec.normal) / r_in.direction().length();
    }
    
    if (refract(r_in.direction(), tNorm, v_to_t, refracted)) {
        reflect_prob = schlick(cosine, n);
    } else {
        reflect_prob = 1.f;
    }
    
    prob = reflect_prob;
    
    reflected = reflect(r_in.direction(), tNorm);
    
    scattered = ray(rec.p, reflected);
    refra = ray(rec.p, refracted);
    
    return true;
}

#endif /* material_h */
