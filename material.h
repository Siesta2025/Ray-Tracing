#ifndef MATERIALH
#define MATERIALH

#include "ray.h"
#include "hitable.h"

// The abstract base class
class material{
    public:
        virtual bool scatter(const ray& r_in, const hit_record& rec, vec3& attenutation, ray& scattered) const=0;
};

// -----------------
// Lambertian(matte)
// -----------------

// Using rejection method to pick a random point in unit sphere
vec3 random_in_unit_sphere(){
    vec3 p;
    do{
        p=2.0*vec3(drand48(),drand48(),drand48())-vec3(1,1,1); // Shift the origin to (1,1,1)
    }while (dot(p,p)>=1.0);
    return p;
}
class lambertian: public material{
    public:
        lambertian(const vec3& a): albedo(a){}
        virtual bool scatter(const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered) const override{
            vec3 target=rec.p+rec.normal+random_in_unit_sphere();
            scattered=ray(rec.p,target-rec.p);
            attenuation=albedo;
            return true;
        }
        // rec, attenuation, and scattered are all references--expect to manipulate them using material_function

        vec3 albedo; // The rate of reflectivity
        // albedo, attenuation are vec3s, in order to control RGB separately
        // albedo is essentially the color(color is presented by reflection, according to physics)
};

// -----
// Metal
// -----

vec3 reflect(const vec3& v, const vec3& n){ // n stands for normal vector
    return v-2*dot(v,n)*n; // Reflection
}

class metal: public material{
    public:
        metal(const vec3& a): albedo(a){}
        virtual bool scatter(const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered) const override{
            vec3 reflected=reflect(unit_vector(r_in.direction()),rec.normal);
            scattered=ray(rec.p,reflected);
            attenuation=albedo;
            
            // We have to ensure that the normal and the reflected ray are on the same side(normals point to the outside of sphere, by default)
            return (dot(scattered.direction(),rec.normal)>0);
        }
        vec3 albedo;
};

#endif