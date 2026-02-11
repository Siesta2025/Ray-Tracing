#include <iostream>
#include <cmath> // For sqrt()
#include <cfloat> // For FLT_MAX
#include "sphere.h"
#include "hitablelist.h"
#include "vec3.h"
#include "ray.h"
#include "camera.h"

// Using rejection method to pick a random point in unit sphere
vec3 random_in_unit_sphere(){
    vec3 p;
    do{
        p=2.0*vec3(drand48(),drand48(),drand48())-vec3(1,1,1); // Shift the origin to (1,1,1)
    }while (dot(p,p)>=1.0);
    return p;
}

vec3 color(const ray& r,hitable *world){
    hit_record rec;
    if (world->hit(r,0.0,FLT_MAX,rec)){
        vec3 target=rec.p+rec.normal+random_in_unit_sphere(); // Hitting point+ normal+ random direction=target
        return 0.5*color(ray(rec.p,target-rec.p),world);
        // Recursion method (like sending a spy to inspect), assuming the reflectivity is 50%
    }
    else{
        vec3 unit_direction=unit_vector(r.direction());
        float t=0.5*(unit_direction.y()+1.0);
        return (1.0-t)*vec3(1.0,1.0,1.0)+t*vec3(0.5,0.7,1.0); // Not hit, blending background color
    }
}
 
int main(){
    int nx=200;
    int ny=100;
    int ns=100; // Doing 100 random trials
    std::cout<<"P3\n"<<nx<<" "<<ny<<"\n255\n"; // The PPM format
    hitable *list[2]; // An array of pointers
    // Why double pointers? Abstract base case can't be instantiated!

    list[0]=new sphere(vec3(0,0,-1),0.5); // Heap memory(avoid stack overflow), 'new' returns a pointer/address
    list[1]=new sphere(vec3(0,-100.5,-1),100); // A huge sphere representing the ground
    hitable *world=new hitable_list(list,2); // C++ allows assigning a derived class pointer to a base class pointer
    // Pay attention that hitable_list is not only a container, but also a hitable object!

    camera cam; // Introduce camera

    for(int j=ny-1;j>=0;j--){
        for(int i=0;i<nx;i++){
            vec3 col(0,0,0); // Color
            for (int s=0;s<ns;s++){
                float u=float(i+drand48())/float(nx); // Sampling inside the pixel
                float v=float(j+drand48())/float(ny);
                ray r=cam.get_ray(u,v);
                col+=color(r,world);
            }
            col/=float(ns); // Average
            // Gamma correction
            col=vec3(sqrt(col[0]),sqrt(col[1]),sqrt(col[2]));
            // Scale to int between 0 and 255, 255.99 is a tricky multiplier, as int cast would simply truncate the float
            int ir=int(255.99*col[0]);
            int ig=int(255.99*col[1]);
            int ib=int(255.99*col[2]);
            
            std::cout<<ir<<" "<<ig<<" "<<ib<<"\n";
        }
    }
}