#ifndef CAMERAH
#define CAMERAH

#include "ray.h"

// Encapsulating the camera logic
class camera{
    public:
        camera(vec3 lookfrom, vec3 lookat, vec3 vup, float vfov, float aspect){ 
            // vfov: top to bottom in degrees
            // vup: vertical direction

            vec3 u,v,w; // orthonormal basis
            float theta=vfov*M_PI/180;
            float half_height=tan(theta/2); // We suppose the project plane is z=-1
            float half_width=aspect*half_height; // aspect: aspect ratio, has to match output ratio

            origin=lookfrom;
            w=unit_vector(lookfrom-lookat); // According to right-hand rule, w is the opposite direction of the vision
            u=unit_vector(cross(vup,w));
            v=cross(w,u);

            lower_left_corner=origin-half_width*u-half_height*v-w;
            horizontal=2*half_width*u;
            vertical=2*half_height*v;
        } // Constructor
        ray get_ray(float s,float t){return ray(origin,lower_left_corner+s*horizontal+t*vertical-origin);}
        //  Generate a ray according to the ratios(u,v)

        vec3 origin;
        vec3 vertical;
        vec3 horizontal;
        vec3 lower_left_corner;
};

#endif