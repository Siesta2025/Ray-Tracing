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

hitable *random_scene(){
    // Memory allocation
    int n=500;
    hitable **seikai=new hitable*[n+1]; // Have to use heap memory('cause we want the array to exist even after this function ends)

    // Floor
    seikai[0]=new sphere(vec3(0,-1000,0),1000,new lambertian(vec3(0.5,0.5,0.5)));

    int i=1; // Counter

    // Grid loop for random small spheres
    for (int a=-11;a<11;a++){
        for (int b=-11;b<11;b++){
            float choose_mat=drand48();
            vec3 center(a+0.9*drand48(),0.2,b+0.9*drand48()); // Good habit to break the problem down!
            // 0.9 is a subtle number, preventing overflow

            if ((center-vec3(4,0.2,0)).length()>0.9){
                if (choose_mat<0.8){
                    // Matte
                    seikai[i++]=new sphere(center,0.2,new lambertian(vec3(drand48()*drand48(),drand48()*drand48(),drand48()*drand48())));
                    // Square to make color softer(remember Gamma correction?)
                }
                else if (choose_mat<0.95){
                    // Metal
                    seikai[i++]=new sphere(center,0.2,new metal(vec3(0.5*(1+drand48()),0.5*(1+drand48()),0.5*(1+drand48()))));
                    // Graphics optimization
                }
                else{
                    // Glass
                    seikai[i++]=new sphere(center,0.2,new dielectric(1.5));
                }
            }
        }
    }

    // The Big Three
    seikai[i++]=new sphere(vec3(0,1,0),1.0,new dielectric(1.5));
    seikai[i++]=new sphere(vec3(-4,1,0),1.0,new lambertian(vec3(0.4,0.2,0.1)));
    seikai[i++]=new sphere(vec3(4,1,0),1.0,new metal(vec3(0.7,0.6,0.5)));

    return new hitable_list(seikai,i);
}
 
int main(){
    // Parameters
    int nx=1200;
    int ny=800;
    int ns=500; // Doing 500 random trials
    std::cout<<"P3\n"<<nx<<" "<<ny<<"\n255\n"; // The PPM format

    // Random world
    hitable *world=random_scene();

    // Camera setting
    vec3 lookfrom(13,2,3);
    vec3 lookat(0,0,0);
    float dist_to_focus=10.0;
    float aperture=0.1;
    camera cam(lookfrom,lookat,vec3(0,1,0),20,float(nx)/float(ny),aperture,dist_to_focus);

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