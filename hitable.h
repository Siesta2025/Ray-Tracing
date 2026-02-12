#ifndef HITABLEH
#define HITABLEH

#include "ray.h"

class material; // Forward declaration in order to prevent circularity(material and hitables need to know each other)

// Wrap up all the information we need of a hitting
// Having this, a function can return a bunch of information we need!
struct hit_record{
    float t; // The parameter t
    vec3 p; // Position
    vec3 normal; // Surface normal 
    material *mat_ptr; // Material
};

class hitable{
    public:
        // Virtual function is the key to utilize polymorphism
        // Virtual function establish a dynamic binding, choosing suitable function to call
        // The same interface, different implementations
        virtual bool hit(const ray& r,float t_min,float t_max,hit_record& rec) const =0; // "& rec" means that we want to edit rec
        // "=0" means that it is a virtual function
        // hitable becomes an abstract base class, thus can't be initialized
        // Any subclass inheriting hitable must offer the implementation of the virtual function
};

#endif
