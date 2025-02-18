#include<string.h>
#include<stdio.h>
#include<limits>
#include<math.h>
#include<stdlib.h>
//#include <printf.h>
#include <stddef.h>
#include "vector.h"
#include <stdexcept>

Vector::Vector(double a, double b, double c) : x(a), y(b), z(c) {
}
void Vector::operator -= (const Vector& rhs) {
   x-=rhs.x; y-=rhs.y; z-=rhs.z;
}
void Vector::operator += (const Vector& rhs) {
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


Vector Vector::operator - (const Vector& rhs) const {
   return Vector(x-rhs.x, y-rhs.y, z-rhs.z);
}
Vector Vector::operator + (const Vector& rhs) const {
   return Vector(x+rhs.x, y+rhs.y, z+rhs.z);
}

Vector Vector::operator * (const Vector a) const {
   return Vector(x * a.x, y * a.y, z * a.z);
}

Vector Vector::operator * (const double rhs) const {
   return Vector(x*rhs, y*rhs, z*rhs);
}
Vector Vector::operator * (const float rhs) const {
   return Vector(x*rhs, y*rhs, z*rhs);
}
Vector Vector::operator * (const int rhs) const {
   return Vector(x*rhs, y*rhs, z*rhs);
}
Vector Vector::operator / (const double rhs) const {
   return Vector(x/rhs, y/rhs, z/rhs);
}
Vector Vector::operator / (const float rhs) const {
   return Vector(x/rhs, y/rhs, z/rhs);
}
Vector Vector::operator / (const int rhs) const {
   return Vector(x/rhs, y/rhs, z/rhs);
}

double Vector::operator[](int index) const {
   switch(index) {
      case 0: return x;
      case 1: return y;
      case 2: return z;
      default: throw std::out_of_range("Vector index out of range");
   }
}

Vector Vector::cross(const Vector& a) const {
   return Vector(y*a.z-z*a.y, z*a.x-x*a.z, x*a.y-y*a.x);
}
double Vector::mag2(){
   return x*x+y*y+z*z; 
}
double Vector::mag(){
   return sqrt(x*x+y*y+z*z); 
}
double Vector::dot(const Vector& a){
   return x*a.x+y*a.y+z*a.z;
}
Vector Vector::normalize(){
   double m = mag();
   return Vector(x/m, y/m, z/m); 
}

Vector Vector::min(const Vector& a, const Vector& b) {
    return Vector(std::min(a.x, b.x), std::min(a.y, b.y), std::min(a.z, b.z));
}

Vector Vector::max(const Vector& a, const Vector& b) {
    return Vector(std::max(a.x, b.x), std::max(a.y, b.y), std::max(a.z, b.z));
}

 

Vector solveScalers(const Vector& v1, const Vector& v2, const Vector& v3, const Vector& C) {
    double inv_denom = 1.0 / (
        v1.x * (v2.y * v3.z - v2.z * v3.y) -
        v1.y * (v2.x * v3.z - v2.z * v3.x) +
        v1.z * (v2.x * v3.y - v2.y * v3.x)
    );

    double a = C.x * (v2.y * v3.z - v2.z * v3.y) -
               C.y * (v2.x * v3.z - v2.z * v3.x) +
               C.z * (v2.x * v3.y - v2.y * v3.x);

    double b = v1.x * (C.y * v3.z - C.z * v3.y) -
               v1.y * (C.x * v3.z - C.z * v3.x) +
               v1.z * (C.x * v3.y - C.y * v3.x);

    double c = v1.x * (v2.y * C.z - v2.z * C.y) -
               v1.y * (v2.x * C.z - v2.z * C.x) +
               v1.z * (v2.x * C.y - v2.y * C.x);

    return Vector(a * inv_denom, b * inv_denom, c * inv_denom);
}




Ray::Ray(const Vector& po, const Vector& ve): point(po), vector(ve){}
