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

        vec3 background=(1.0-t)*vec3(0.8,0.9,1.0)+t*vec3(0.1,0.3,0.7);

        // Sunlight
        vec3 sun_dir=unit_vector(vec3(1.0,0.8,0.5));
        float sun_intensity=dot(unit_direction,sun_dir);
        if (sun_intensity>0.95){
            background+=vec3(0.8,0.6,0.2)*(sun_intensity-0.95)*10.0;
        }
        return background;
    }
}

hitable *random_scene(){
    // Memory allocation
    int n=500;
    hitable **seikai=new hitable*[n+1]; // Have to use heap memory('cause we want the array to exist even after this function ends)

    // Floor
    seikai[0]=new sphere(vec3(0,-1000,0), 1000, new lambertian(vec3(0.3, 0.5, 0.35)));

    int i=1; // Counter


    // Grid loop for random small spheres
    for (int a=-11;a<11;a++){
        for (int b=-11;b<11;b++){
            float choose_mat=random_double();
            vec3 center(a+0.9*random_double(),0.2,b+0.9*random_double()); // Good habit to break the problem down!
            // 0.9 is a subtle number, preventing overflow

            if ((center-vec3(4,0.2,0)).length()>0.9){
                if (choose_mat<0.4){
                    seikai[i++]=new sphere(center,0.2,new lambertian(vec3(1.0,0.7,0.8)));
                }
                else if (choose_mat<0.7){
                    seikai[i++]=new sphere(center,0.2,new lambertian(vec3(0.95,0.95,0.9)));
                }
                else if (choose_mat<0.9){
                    seikai[i++]=new sphere(center,0.2,new metal(vec3(0.4,0.6,0.9)));
                }
                else{
                    seikai[i++]=new sphere(center,0.2,new dielectric(1.5));
                }
            }
        }
    }

    // The Big Three
    seikai[i++]=new sphere(vec3(0,1,0),1.0,new dielectric(1.5));
    seikai[i++]=new sphere(vec3(-4,1,0),1.0,new lambertian(vec3(1.0,0.6,0.6)));
    seikai[i++]=new sphere(vec3(4,1,0),1.0,new metal(vec3(0.8,0.8,0.9)));

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
        // Scale to int between 0 and 255, 255.99 is a tricky multiplier, as int cast would simply truncate the float
        int ir=int(255.99*col[0]);
        int ig=int(255.99*col[1]);
        int ib=int(255.99*col[2]);
            
        std::cout<<ir<<" "<<ig<<" "<<ib<<"\n";
    }

    // A signal
    std::cerr<<"Render complete!";
}