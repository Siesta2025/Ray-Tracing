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

// -----------
// Dielectrics
// -----------

bool refract(const vec3& v, const vec3& n, float ni_over_nt, vec3& refracted){
    vec3 uv=unit_vector(v);
    float dt=dot(uv,n); // cos(theta)
    float discriminant=1.0-ni_over_nt*ni_over_nt*(1-dt*dt);
    if (discriminant>0){
        refracted=ni_over_nt*(uv-n*dt)-n*sqrt(discriminant); // Vector form of Snell's law
        return true;
    }
    else
        return false; // Total internal reflection
}

// Schlick's approximation for Fresnel effect
float schlick(float cosine, float ref_idx){
    float r0=(1-ref_idx)/(1+ref_idx);
    r0=r0*r0;
    return r0+(1-r0)*pow((1-cosine),5);
}

class dielectric: public material{
    public:
        dielectric(float ri): ref_idx(ri){}
        virtual bool scatter(const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered) const override{
            vec3 outward_normal;

            // Reflection
            vec3 reflected=reflect(r_in.direction(), rec.normal);
            float ni_over_nt;
            attenuation=vec3(1.0,1.0,1.0); // Glasses do not absorb lights
            vec3 refracted;
            float reflect_prob; // Probability
            float cosine; // A correction parameter

            // Direction branches
            if (dot(r_in.direction(),rec.normal)>0){
                outward_normal=-rec.normal;
                ni_over_nt=ref_idx; // Recap: ref_idx of air approximates to 1
                cosine=ref_idx*dot(r_in.direction(),rec.normal)/r_in.direction().length();
            }
            else{
                outward_normal=rec.normal;
                ni_over_nt=1.0/ref_idx;
                cosine=-dot(r_in.direction(),rec.normal)/r_in.direction().length();
            }

            // Refraction or reflection?
            // Logic: use probability to decide between refraction and reflection
            if (refract(r_in.direction(),outward_normal,ni_over_nt,refracted)){
                reflect_prob=schlick(cosine, ref_idx); // Schlick approximation decides the ratio between refraction and reflection
            }
            else{
                // Total internal reflection
                reflect_prob=1.0;
            }

            // Dice roll!
            if (drand48()<reflect_prob){
                scattered=ray(rec.p,reflected);
            }
            else{
                scattered=ray(rec.p,refracted);
            }
            return true;
        }

        float ref_idx;// Refractive index
};

#endif