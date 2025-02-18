#include "box.h"

Box::Box(const Vector &c, Texture* t, double ya, double pi, double ro, double tx, double ty):Plane(c, t, ya, pi, ro, tx, ty){}
Box::Box(const Vector &c, Texture* t, double ya, double pi, double ro, double tx):Plane(c, t, ya, pi, ro, tx,tx){}

double Box::getIntersection(Ray ray){
   double time = Plane::getIntersection(ray);
   Vector dist = solveScalers(right, up, vect, ray.point+ray.vector*time-center);
   if(time==inf) 
      return time;
   return ( ((dist.x>=0)?dist.x:-dist.x)>textureX/2 || ((dist.y>=0)?dist.y:-dist.y)>textureY/2 )?inf:time;
}

bool Box::getLightIntersection(Ray ray, double* fill){
   const double t = ray.vector.dot(vect);
   const double norm = vect.dot(ray.point)+d;
   const double r = -norm/t;
   if(r<=0. || r>=1.) return false;
   Vector dist = solveScalers(right, up, vect, ray.point+ray.vector*r-center);
   if( ((dist.x>=0)?dist.x:-dist.x)>textureX/2 || ((dist.y>=0)?dist.y:-dist.y)>textureY/2 ) return false;

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

AABB Box::getBounds() const {
    // Calculate the eight corners of the box
    std::vector<Vector> corners(8, Vector(0,0,0));
    Vector halfWidth = right * (textureX / 2);
    Vector halfHeight = up * (textureY / 2);
    Vector halfDepth = vect * 0.001; // Small thickness for the box

    // Front face corners
    corners[0] = center - halfWidth - halfHeight + halfDepth;
    corners[1] = center + halfWidth - halfHeight + halfDepth;
    corners[2] = center - halfWidth + halfHeight + halfDepth;
    corners[3] = center + halfWidth + halfHeight + halfDepth;

    // Back face corners
    corners[4] = center - halfWidth - halfHeight - halfDepth;
    corners[5] = center + halfWidth - halfHeight - halfDepth;
    corners[6] = center - halfWidth + halfHeight - halfDepth;
    corners[7] = center + halfWidth + halfHeight - halfDepth;

    // Find the minimum and maximum points
    Vector min(std::numeric_limits<double>::max(), std::numeric_limits<double>::max(), std::numeric_limits<double>::max());
    Vector max(std::numeric_limits<double>::lowest(), std::numeric_limits<double>::lowest(), std::numeric_limits<double>::lowest());

    for (int i = 0; i < 8; ++i) {
        min = Vector::min(min, corners[i]);
        max = Vector::max(max, corners[i]);
    }

    // Add padding for numerical precision
    Vector padding(1e-4, 1e-4, 1e-4);
    return AABB(min - padding, max + padding);
}