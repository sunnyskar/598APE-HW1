#include<string.h>
#include<stdio.h>
#include<limits>
#include<math.h>
#include<stdlib.h>
//#include <printf.h>
#include <stddef.h>
#include "vector.h"

Vector::Vector(double a, double b, double c) : x(a), y(b), z(c) {
}
void Vector::operator -= (const Vector rhs) {
   x-=rhs.x; y-=rhs.y; z-=rhs.z;
}
void Vector::operator += (const Vector rhs) {
   x+=rhs.x; y+=rhs.y; z+=rhs.z;
}
void Vector::operator *= (const double rhs) {
   x*=rhs; y*=rhs; z*=rhs;
}
void Vector::operator *= (const float rhs) {
   x*=rhs; y*=rhs; z*=rhs;
}
void Vector::operator *= (const int rhs) {
   x*=rhs; y*=rhs; z*=rhs;
}
void Vector::operator /= (const double rhs) {
   x/=rhs; y/=rhs; z/=rhs;
}
void Vector::operator /= (const float rhs) {
   x/=rhs; y/=rhs; z/=rhs;
}
void Vector::operator /= (const int rhs) {
   x/=rhs; y/=rhs; z/=rhs;
}


Vector Vector::operator - (const Vector rhs) {
   return Vector(x-rhs.x, y-rhs.y, z-rhs.z);
}
Vector Vector::operator + (const Vector rhs) {
   return Vector(x+rhs.x, y+rhs.y, z+rhs.z);
}
/*
Vector Vector::operator * (const Vector a) {
   return Vector(y*a.z-z*a.y, z*a.x-x*a.z, x*a.y-y*a.x);
}*/
Vector Vector::operator * (const double rhs) {
   return Vector(x*rhs, y*rhs, z*rhs);
}
Vector Vector::operator * (const float rhs) {
   return Vector(x*rhs, y*rhs, z*rhs);
}
Vector Vector::operator * (const int rhs) {
   return Vector(x*rhs, y*rhs, z*rhs);
}
Vector Vector::operator / (const double rhs) {
   return Vector(x/rhs, y/rhs, z/rhs);
}
Vector Vector::operator / (const float rhs) {
   return Vector(x/rhs, y/rhs, z/rhs);
}
Vector Vector::operator / (const int rhs) {
   return Vector(x/rhs, y/rhs, z/rhs);
}
Vector Vector::cross(const Vector a) {
   return Vector(y*a.z-z*a.y, z*a.x-x*a.z, x*a.y-y*a.x);
}
double Vector::mag2(){
   return x*x+y*y+z*z; 
}
double Vector::mag(){
   return sqrt(x*x+y*y+z*z); 
}
double Vector::dot(const Vector a){
   return x*a.x+y*a.y+z*a.z;
}
Vector Vector::normalize(){
   double m = mag();
   return Vector(x/m, y/m, z/m); 
}

  
Vector solveScalers(const Vector v1, const Vector v2, Vector v3, Vector C) {
    // Precompute shared terms
    const double v1x_v2y = v1.x * v2.y;
    const double v1y_v2x = v1.y * v2.x;
    const double v1y_v2z = v1.y * v2.z;
    const double v1z_v2y = v1.z * v2.y;
    const double v1x_v2z = v1.x * v2.z;
    const double v1z_v2x = v1.z * v2.x;

    const double denom = fma(v1z_v2y, v3.x, 
                    fma(-v1y_v2z, v3.x,
                    fma(v1x_v2z, v3.y,
                    fma(-v1z_v2x, v3.y,
                    fma(v1y_v2x, v3.z,
                        -v1x_v2y * v3.z)))));

    // Numerator calculations using precomputed terms
    const double a = C.z * v2.y * v3.x - C.y * v2.z * v3.x - C.z * v2.x * v3.y + C.x * v2.z * v3.y + C.y * v2.x * v3.z - C.x * v2.y * v3.z;
    const double b = -C.z * v1.y * v3.x + C.y * v1.z * v3.x + C.z * v1.x * v3.y - C.x * v1.z * v3.y - C.y * v1.x * v3.z + C.x * v1.y * v3.z;
    const double c = C.z * v1y_v2x - C.y * v1z_v2x - C.z * v1x_v2y + C.x * v1z_v2y + C.y * v1x_v2z - C.x * v1y_v2z;
    const double inv_denom = 1.0 / denom;
    return Vector(a * inv_denom, b * inv_denom, c * inv_denom);
}


Ray::Ray(const Vector& po, const Vector& ve): point(po), vector(ve){}
