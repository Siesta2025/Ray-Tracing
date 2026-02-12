#ifndef SPHEREH
#define SPHEREH

#include "hitable.h"

class sphere: public hitable{ // Inheritance, tellling us sphere is a hitable object
    public:
        // Constructors
        sphere(){}
        sphere(vec3 cen, float r, material *m): center(cen),radius(r), mat_ptr(m) {}; 

        virtual bool hit(const ray& r,float tmin, float tmax, hit_record& rec) const override; 
        // Keyword 'override' is a safety feature, if the function name or parameters can't match with the ones in the parent class, system would raise an error
        
        vec3 center;
        float radius;  
        material *mat_ptr;
};

// Implementation
// const function, ready for constant sphere to call 
bool sphere::hit(const ray& r,float t_min,float t_max,hit_record& rec) const {
    vec3 oc=r.origin()-center;
    float a=dot(r.direction(),r.direction());
    float b=dot(oc,r.direction());
    float c=dot(oc,oc)-radius*radius;
    float discriminant=b*b-a*c; // Doing some algebra...
    if (discriminant>0){
        float temp=(-b-sqrt(b*b-a*c))/a; // The nearest one first
        if (temp<t_max&&temp>t_min){ // t_max and t_min are constraints on the scale of t
            rec.t=temp; // Change the record
            rec.p=r.point_at_parameter(rec.t);
            rec.normal=(rec.p-center)/radius; //With normalization
            rec.mat_ptr=mat_ptr; // Material
            return true;
        }
        temp=(-b+sqrt(b*b-a*c))/a; // If the upper one is invalid, bigger one is also acceptable
        if (temp<t_max&&temp>t_min){
            rec.t=temp;
            rec.p=r.point_at_parameter(rec.t);
            rec.normal=(rec.p-center)/radius;
            rec.mat_ptr=mat_ptr;
            return true;
        }
    }
    return false;
}

#endif