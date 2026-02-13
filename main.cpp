#include <iostream>
#include <cmath> // For sqrt()
#include <cfloat> // For FLT_MAX
#include "sphere.h"
#include "hitable.h"
#include "hitablelist.h"
#include "vec3.h"
#include "ray.h"
#include "camera.h"
#include "material.h"



vec3 color(const ray& r, hitable *world, int depth){ // depth is recursion depth, in order to prevent infinite recursions
    hit_record rec;

    // Object
    if (world->hit(r,0.001,FLT_MAX,rec)){
    // Using 0.001 is the solution to "Shadow Acne" problem

        ray scattered;
        vec3 attenuation;
        // The flow: use the "side effect" of functions to mutate them

        if (depth<50 && rec.mat_ptr->scatter(r,rec,attenuation,scattered)){
            return attenuation*color(scattered,world,depth+1);
        }
        else{
            return vec3(0,0,0); // Otherwise, set to "black"
        }
    }
    
    // Background 
    else{
        vec3 unit_direction=unit_vector(r.direction());
        float t=0.5*(unit_direction.y()+1.0);
        return (1.0-t)*vec3(1.0,1.0,1.0)+t*vec3(0.5,0.7,1.0);
    }
}
 
int main(){
    int nx=200;
    int ny=100;
    int ns=100; // Doing 100 random trials
    std::cout<<"P3\n"<<nx<<" "<<ny<<"\n255\n"; // The PPM format
    hitable *list[5]; // An array of pointers
    // Why double pointers? Abstract base case can't be instantiated!

    list[0]=new sphere(vec3(0,0,-1),0.5,new lambertian(vec3(0.1,0.2,0.5))); // Heap memory(avoid stack overflow), 'new' returns a pointer/address
    list[1]=new sphere(vec3(0,-100.5,-1),100,new lambertian(vec3(0.8,0.8,0.0))); // A huge sphere representing the ground
    list[2]=new sphere(vec3(1,0,-1),0.5,new metal(vec3(0.8,0.6,0.2)));
    list[3]=new sphere(vec3(-1,0,-1),0.5,new dielectric(1.5));
    list[4]=new sphere(vec3(-1,0,-1),-0.45,new dielectric(1.5)); // Trick: negative radius simulates hollow glass sphere
    hitable *world=new hitable_list(list,5); // C++ allows assigning a derived class pointer to a base class pointer
    // Pay attention that hitable_list is not only a container, but also a hitable object!

    camera cam; // Introduce camera

    for(int j=ny-1;j>=0;j--){
        for(int i=0;i<nx;i++){
            vec3 col(0,0,0); // Color
            for (int s=0;s<ns;s++){
                float u=float(i+drand48())/float(nx); // Sampling inside the pixel
                float v=float(j+drand48())/float(ny);
                ray r=cam.get_ray(u,v);
                col+=color(r,world,0);
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