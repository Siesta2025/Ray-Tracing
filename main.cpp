#include <iostream>
#include <cmath> // For sqrt()
#include <cfloat> // For FLT_MAX
#include "sphere.h"
#include "hitablelist.h"
#include "vec3.h"
#include "ray.h"

// Don't forget the core logic of ray is A+tB
// Cauculating the t of the hitting point
float hit_sphere(const vec3& center, float radius, const ray& r){ // For float, we directly copy it because only 4 bytes
    vec3 oc=r.origin()-center; // The vector from origin to the center
    float a=dot(r.direction(),r.direction());
    float b=2.0*dot(oc,r.direction()); // A good habit to use 2.0
    float c=dot(oc,oc)-radius*radius;
    float discriminant=b*b-4*a*c; // Doing some algebra...
    if (discriminant<0){
        return -1.0; // Never hits
    }
    else{
        return (-b-sqrt(discriminant))/(2.0*a); // Return the smaller(nearest intersection because the camera is at the origin by default)
    }
}

// Pay attention that vec3 can represent both colors and vectors
// Blending sphere color and background color 
vec3 color(const ray& r){ // Only read the ray, cannot modify it
    float t=hit_sphere(vec3(0,0,-1),0.5,r);
    if (t>0.0){
        vec3 N=unit_vector(r.point_at_parameter(t)-vec3(0,0,-1)); // The vector from sphere center to the hitting point
        return 0.5*vec3(N.x()+1,N.y()+1,N.z()+1); // Blending sphere color 
    }
    vec3 unit_direction=unit_vector(r.direction());
    t=0.5*(unit_direction.y()+1.0);
    return (1.0-t)*vec3(1.0,1.0,1.0)+t*vec3(0.5,0.7,1.0); // Doesn't hit,thus blending background color
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