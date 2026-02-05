#include <iostream>
#include "vec3.h"
#include "ray.h"

// Check if the ray hits the sphere
bool hit_sphere(const vec3& center, float radius, const ray& r){ // For float, we directly copy it because only 4 bytes
    vec3 oc=r.origin()-center; // The vector from origin to the center
    float a=dot(r.direction(),r.direction());
    float b=2.0*dot(oc,r.direction()); // A good habit to use 2.0
    float c=dot(oc,oc)-radius*radius;
    float discriminant=b*b-4*a*c; // Doing some algebra...
    return (discriminant>0);
}

// Pay attention that vec3 can represent both colors and vectors
vec3 color(const ray& r){ // Only read the ray, cannot modify it
    // Suppose the center lies on (0,0,-1), if the ray hits the sphere, then coloring red
    if (hit_sphere(vec3(0,0,-1),0.5,r))
        return vec3(1,0,0);
    
    // Fills the background with gradient color
    vec3 unit_direction=unit_vector(r.direction()); // Normalize
    float t=0.5*(unit_direction.y()+1.0); // Mapping y(-1~1) to (0~1) in order to control colors
    return (1.0-t)*vec3(1.0,1.0,1.0)+t*vec3(0.5,0.7,1.0); // Blending white(0.0,0.0,0.0) and blue(0.5,0.7,1.0)
}

int main(){
    int nx=200;
    int ny=100;
    std::cout<<"P3\n"<<nx<<" "<<ny<<"\n255\n"; // The PPM format
    vec3 lower_left_corner(-2.0,-1.0,-1.0);
    vec3 horizontal(4.0,0.0,0.0);
    vec3 vertical(0.0,2.0,0.0);
    vec3 origin(0.0,0.0,0.0);
    for(int j=ny-1;j>=0;j--){
        for(int i=0;i<nx;i++){
            // The ratio
            float u=float(i)/float(nx);
            float v=float(j)/float(ny);
            // Constructing the ray pointing to the destination
            ray r(origin,lower_left_corner+u*horizontal+v*vertical);
            // Creating corresponding color
            vec3 col=color(r);
            // Scale to int between 0 and 255, 255.99 is a tricky multiplier, as int cast would simply truncate the float
            int ir=int(255.99*col[0]);
            int ig=int(255.99*col[1]);
            int ib=int(255.99*col[2]);
            
            std::cout<<ir<<" "<<ig<<" "<<ib<<"\n";
        }
    }
}