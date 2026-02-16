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
#include <random>
#include <vector>
#include <omp.h>

inline double random_double(){
    static thread_local std::mt19937 generator(std::random_device{}());
    std::uniform_real_distribution<double> distribution(0.0,1.0);
    return distribution(generator);
}

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
        return (1.0-t)*vec3(1.0,0.95,0.9)+t*vec3(0.85,0.7,0.75);
    }
}

hitable *random_scene(){
    // Memory allocation
    int n=1000;
    hitable **seikai=new hitable*[n+1]; // Have to use heap memory('cause we want the array to exist even after this function ends)

    // Floor
    seikai[0]=new sphere(vec3(0,-1000,0), 1000, new metal(vec3(0.7, 0.7, 0.65)));

    int i=1; // Counter

    // Sakura rain
    for (int k=0;k<500;k++){
        float x=random_double()*20.0-10.0;
        float z=random_double()*20.0-10.0;
        float y=random_double()*8.0+0.2;
        vec3 center(x,y,z);

        float tone=random_double();
        vec3 albedo;

        if (tone<0.6){
            albedo=vec3(1.0,0.75,0.85);
        }
        else if (tone<0.85){
            albedo=vec3(1.0,1.0,1.0);
        }
        else{
            albedo=vec3(0.6,0.8,0.4);
        }

        float size=0.05+random_double()*0.08;

        seikai[i++]=new sphere(center,size,new lambertian(albedo));
    }

    // The Big Three
    seikai[i++]=new sphere(vec3(0,1,0),1.0,new dielectric(1.5));
    seikai[i++]=new sphere(vec3(-4,1,0),1.0,new lambertian(vec3(0.95,0.95,0.95)));
    seikai[i++]=new sphere(vec3(4,1,0),1.0,new metal(vec3(0.9,0.7,0.6)));

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
    float aperture=0.25;
    camera cam(lookfrom,lookat,vec3(0,1,0),20,float(nx)/float(ny),aperture,dist_to_focus);

    // Storage(framebuffer): to store the colors first, avoiding parallel bug
    std::vector<vec3> framebuffer(nx*ny);

    #pragma omp parallel for schedule(dynamic,1)
    for(int j=ny-1;j>=0;j--){
        for(int i=0;i<nx;i++){
            vec3 col(0,0,0); // Color
            for (int s=0;s<ns;s++){
                float u=float(i+random_double())/float(nx); // Sampling inside the pixel
                float v=float(j+random_double())/float(ny);
                ray r=cam.get_ray(u,v);
                col+=color(r,world,0);
            }
            col/=float(ns); // Average
            // Bloom Hack
            col*=1.1;
            // Improve saturation
            float gray=0.21*col[0]+0.72*col[1]+0.07*col[2];
            col=vec3(gray,gray,gray)+1.2*(col-vec3(gray,gray,gray));  

            // Gamma correction
            col=vec3(sqrt(col[0]),sqrt(col[1]),sqrt(col[2]));

            // Fill in the framebuffer
            int index=(ny-1-j)*nx+i;
            framebuffer[index]=col;
        }
    }
    
    // Print at the end
    for (int k=0;k<nx*ny;k++){
        vec3 col=framebuffer[k];

        // Avoid leak
        if (col[0]>1.0) col[0]=1.0;
        if (col[1]>1.0) col[1]=1.0;
        if (col[2]>1.0) col[2]=1.0;
        
        // Scale to int between 0 and 255, 255.99 is a tricky multiplier, as int cast would simply truncate the float
        int ir=int(255.99*col[0]);
        int ig=int(255.99*col[1]);
        int ib=int(255.99*col[2]);
            
        std::cout<<ir<<" "<<ig<<" "<<ib<<"\n";
    }

    // A signal
    std::cerr<<"Render complete!";
}