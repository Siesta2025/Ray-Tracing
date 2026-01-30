#ifndef VEC3H
#define VEC3H

#include <cmath>// For sqrt()
#include <cstdlib>
#include <iostream>

using std::sqrt;// Tells the compiler that sqrt comes from std

class vec3{

public:
    float e[3];
    vec3():e{0,0,0} {}; //Default initialization
    vec3(float e0,float e1,float e2){
        e[0]=e0;
        e[1]=e1;
        e[2]=e2;
    } //Initialization with parameters
    inline float x() const {return e[0];}//Getter, usually defined as const function
    inline float y() const {return e[1];}
    inline float z() const {return e[2];}
    inline float r() const {return e[0];}
    inline float g() const {return e[1];}
    inline float b() const {return e[2];}

    inline const vec3& operator+() const {return *this;} // Operator overloading(definition)
    inline vec3 operator-() const {return vec3(-e[0],-e[1],-e[2]);}// Negative number
    // Two versions, difference lies in permission
    inline float operator[](int i) const {return e[i];}// Read only
    inline float& operator[](int i) {return e[i];}// Read-write access allowed

    // Operator overloading
    inline vec3& operator+=(const vec3& v2){
        e[0]+=v2.e[0];
        e[1]+=v2.e[1];
        e[2]+=v2.e[2];
        return *this;
    } 
    inline vec3& operator-=(const vec3& v2){
        e[0]-=v2.e[0];
        e[1]-=v2.e[1];
        e[2]-=v2.e[2];
        return *this;
    }
    inline vec3& operator*=(const vec3& v2){
        e[0]*=v2.e[0];
        e[1]*=v2.e[1];
        e[2]*=v2.e[2];
        return *this;
    }
    inline vec3& operator/=(const vec3& v2){
        e[0]/=v2.e[0];
        e[1]/=v2.e[1];
        e[2]/=v2.e[2];
        return *this;
    }
    // Multiplication with number
    inline vec3& operator*=(const float t){
        e[0]*=t;
        e[1]*=t;
        e[2]*=t;
        return *this;
    }
    inline vec3& operator/=(const float t){
        e[0]/=t;
        e[1]/=t;
        e[2]/=t;
        return *this;
    }

    inline float length() const {return sqrt(e[0]*e[0]+e[1]*e[1]+e[2]*e[2]);}
    inline float squared_length() const {return e[0]*e[0]+e[1]*e[1]+e[2]*e[2];}
    inline void make_unit_vector(){
        float k=1.0/sqrt(e[0]*e[0]+e[1]*e[1]+e[2]*e[2]);
        e[0]*=k;
        e[1]*=k;
        e[2]*=k;
    }// Just changes the original vector

    // In the original version in the textbook, float e[3]; is at this line, which indicates that the order doesn't matter
};

// Definitions of global functions
inline vec3 operator+(const vec3& v1,const vec3& v2){
    return vec3(v1.e[0]+v2.e[0],v1.e[1]+v2.e[1],v1.e[2]+v2.e[2]);
}
inline vec3 operator-(const vec3& v1,const vec3& v2){
    return vec3(v1.e[0]-v2.e[0],v1.e[1]-v2.e[1],v1.e[2]-v2.e[2]);
}
inline vec3 operator*(const vec3& v1, const vec3& v2){
    return vec3(v1.e[0]*v2.e[0],v1.e[1]*v2.e[1],v1.e[2]*v2.e[2]);
}
inline vec3 operator/(const vec3& v1,const vec3& v2){
    return vec3(v1.e[0]/v2.e[0],v1.e[1]/v2.e[1],v1.e[2]/v2.e[2]);
}
// Scalar Operations
inline vec3 operator*(float t,const vec3& v){
    return vec3(t*v.e[0],t*v.e[1],t*v.e[2]);
}
inline vec3 operator*(const vec3& v,float t){
    return t*v;// Consider another way of using
}
inline vec3 operator/(const vec3& v,float t){
    return vec3(v.e[0]/t,v.e[1]/t,v.e[2]/t);
}
// Dot product
inline float dot(const vec3& v1,const vec3 &v2){
    return v1.e[0]*v2.e[0]+v1.e[1]*v2.e[1]+v1.e[2]*v2.e[2];
}
// Cross product
inline vec3 cross(const vec3& v1,const vec3& v2){
    return vec3((v1.e[1]*v2.e[2]-v1.e[2]*v2.e[1]),(-(v1.e[0]*v2.e[2]-v1.e[2]*v2.e[0])),(v1.e[0]*v2.e[1]-v1.e[1]*v2.e[0]));
}
// Getting unit vector
inline vec3 unit_vector(vec3 v){
    return v/v.length();
}
// Overload std::cout
inline std::ostream& operator<<(std::ostream& os,const vec3& t){
    os<<t.e[0]<<" "<<t.e[1]<<" "<<t.e[2];
    return os;
}

#endif