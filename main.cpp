#include <iostream>
#include "vec3.h"// For operator overloading

int main(){
    int nx=200;
    int ny=100;
    std::cout<<"P3\n"<<nx<<" "<<ny<<"\n255\n"; // The PPM format
    for(int j=ny-1;j>=0;j--){
        for(int i=0;i<nx;i++){
            //Cast to float, ensuring the result is a float between 0 and 1
            vec3 col(float(i)/float(nx),float(j)/float(ny),0.2);
            // Scale to int between 0 and 255, 255.99 is a tricky multiplier, as int cast would simply truncate the float
            int ir=int(255.99*col[0]);
            int ig=int(255.99*col[1]);
            int ib=int(255.99*col[2]);
            
            std::cout<<ir<<" "<<ig<<" "<<ib<<"\n";
        }
    }
}