#ifndef RAYH
#define RAYH
#include "vec3.h" // For vec3

class ray
{
    public:
        // ray(){}; is dangerous
        ray():A(vec3(0,0,0)),B(vec3(0,0,0)) {};
        ray(const vec3& a, const vec3& b){A=a;B=b;} 
        // What really gets locked is a,b, not A,B
        // A is actually mutable, because it's a replica of a
        // & only means that in the parameter passing process, a and b aren't copied
        // But in the function body, they are copied
        vec3 origin() const {return A;}
        vec3 direction() const {return B;}
        vec3 point_at_parameter(float t) const {return A+t*B;}

        vec3 A; // Origin point
        vec3 B; // Direction
};

#endif 