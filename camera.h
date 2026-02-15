#ifndef CAMERAH
#define CAMERAH

#include "ray.h"

// Find a random point in the unit disk, to simulate thin lens
vec3 random_in_unit_disk(){
    vec3 p;
    do{
        p=2.0*vec3(drand48(),drand48(),0)-vec3(1,1,0);
    } while(dot(p,p)>=1.0);
    return p;
}

// Encapsulating the camera logic
class camera{
    public:
        camera(vec3 lookfrom, vec3 lookat, vec3 vup, float vfov, float aspect, float aperture, float focus_dist){
            // vup: the "absolute upward" direction
            // vfov: vertival field of view
            // aspect: aspect ratio
            // focus_dist: distance of the film plane(as a scaliing coefficient)
            
            lens_radius=aperture/2; // As a scaling coefficient
            float theta=vfov*M_PI/180;
            float half_height=tan(theta/2); // Here we suppose that the film plane is z=-1(by default)
            float half_width=aspect*half_height;

            // Building the orthonormal basis
            origin=lookfrom;
            w=unit_vector(lookfrom-lookat);
            u=unit_vector(cross(vup,w));
            v=cross(w,u);

            lower_left_corner=origin-half_width*focus_dist*u-half_height*focus_dist*v-focus_dist*w; // focus_dist is a scaling coefficient
            horizontal=2*half_width*focus_dist*u;
            vertical=2*half_height*focus_dist*v;
        }

        ray get_ray(float s, float t){
            vec3 rd=lens_radius*random_in_unit_disk();
            vec3 offset=u*rd.x()+v*rd.y(); // Getter

            return ray(origin+offset, lower_left_corner+s*horizontal+t*vertical-origin-offset); // Offset is a "bias"
        }

        vec3 origin;
        vec3 lower_left_corner;
        vec3 horizontal;
        vec3 vertical;
        vec3 u,v,w;
        float lens_radius;
};

#endif