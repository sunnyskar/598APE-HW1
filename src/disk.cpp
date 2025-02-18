#include "disk.h"
#include <iostream>

Disk::Disk(const Vector &c, Texture* t, double ya, double pi, double ro, double tx, double ty):Plane(c, t, ya, pi, ro, tx, ty){}


double Disk::getIntersection(Ray ray) {
   double time = Plane::getIntersection(ray); // Check intersection with the infinite plane
   if (time == inf) 
      return time; // If no intersection with the plane, return infinity
   Vector dist = solveScalers(right, up, vect, ray.point + ray.vector * time - center); 
   return (dist.x * dist.x / (textureX * textureX) + dist.y * dist.y / (textureY * textureY) > 1) ? inf : time;
}

bool Disk::getLightIntersection(Ray ray, double* fill){
   const double t = ray.vector.dot(vect);
   const double norm = vect.dot(ray.point)+d;
   const double r = -norm/t;
   if(r<=0. || r>=1.) return false;
   Vector dist = solveScalers(right, up, vect, ray.point+ray.vector*r-center);
   if(  dist.x*dist.x/(textureX*textureX)+dist.y*dist.y/(textureY*textureY)>1  )return false;
   if(texture->opacity>1-1E-6) return true;   
   unsigned char temp[4];
   double amb, op, ref;
   texture->getColor(temp, &amb, &op, &ref,fix(dist.x/textureX-.5), fix(dist.y/textureY-.5));
   if(op>1-1E-6) return true;
   fill[0]*=temp[0]/255.;
   fill[1]*=temp[1]/255.;
   fill[2]*=temp[2]/255.;
   return false;
}

AABB Disk::getBounds() const {
    double halfWidth = textureX / 2.0;
    double halfHeight = textureY / 2.0;
    
    // Create corners in disk's local space
    Vector corners[4] = {
        Vector(halfWidth, 0, halfHeight),
        Vector(halfWidth, 0, -halfHeight),
        Vector(-halfWidth, 0, halfHeight),
        Vector(-halfWidth, 0, -halfHeight)
    };
    
    // Transform corners using disk's orientation vectors
    Vector transformedCorner = center + (right * corners[0].x) + (up * corners[0].z);
    Vector min = transformedCorner;
    Vector max = transformedCorner;
    
    for (int i = 1; i < 4; i++) {
        transformedCorner = center + (right * corners[i].x) + (up * corners[i].z);
        min = Vector::min(min, transformedCorner);
        max = Vector::max(max, transformedCorner);
    }
    
    // Add small thickness in normal direction
    Vector normalOffset = vect * 0.001;
    min = Vector::min(min, min - normalOffset);
    max = Vector::max(max, max + normalOffset);
    
    Vector padding(0.001, 0.001, 0.001);
    return AABB(min - padding, max + padding);
}