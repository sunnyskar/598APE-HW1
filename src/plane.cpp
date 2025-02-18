#include "plane.h"

Plane::Plane(const Vector &c, Texture* t, double ya, double pi, double ro, double tx, double ty) : Shape(c, t, ya, pi, ro), vect(c), right(c), up(c){
   textureX = tx; textureY = ty;
   setAngles(yaw, pitch, roll);
   normalMap = NULL;
   mapX = textureX; mapY = textureY;
}

void Plane::setAngles(double a, double b, double c){
   yaw =a; pitch = b; roll = c;
   xcos = cos(yaw);
   xsin = sin(yaw);
   ycos = cos(pitch);
   ysin = sin(pitch);
   zcos = cos(roll);
   zsin = sin(roll);
   vect.x = xsin*ycos*zcos+ysin*zsin;
   vect.y = ysin*zcos-xsin*ycos*zsin;
   vect.z = xcos*ycos;
   up.x = -xsin*ysin*zcos+ycos*zsin;
   up.y = ycos*zcos+xsin*ysin*zsin;
   up.z = -xcos*ysin;
   right.x = xcos*zcos;
   right.y = -xcos*zsin;
   right.z = -xsin;
   d = -vect.dot(center);
}

void Plane::setYaw(double a){
   yaw =a;
   xcos = cos(yaw);
   xsin = sin(yaw);
   
   vect.x = xsin*ycos*zcos+ysin*zsin;
   vect.y = ysin*zcos-xsin*ycos*zsin;
   vect.z = xcos*ycos;
   up.x = -xsin*ysin*zcos+ycos*zsin;
   up.y = ycos*zcos+xsin*ysin*zsin;
   up.z = -xcos*ysin;   
   right.x = xcos*zcos;
   right.y = -xcos*zsin;
   right.z = -xsin;
   d = -vect.dot(center);
}

void Plane::setPitch(double b){
   pitch = b;
   ycos = cos(pitch);
   ysin = sin(pitch);
   vect.x = xsin*ycos*zcos+ysin*zsin;
   vect.y = ysin*zcos-xsin*ycos*zsin;
   vect.z = xcos*ycos;
   up.x = -xsin*ysin*zcos+ycos*zsin;
   up.y = ycos*zcos+xsin*ysin*zsin;
   up.z = -xcos*ysin;
   d = -vect.dot(center);
}

void Plane::setRoll(double c){
   roll = c;
   zcos = cos(roll);
   zsin = sin(roll);
   vect.x = xsin*ycos*zcos+ysin*zsin;
   vect.y = ysin*zcos-xsin*ycos*zsin;
//   vect.z = xcos*ycos;
   up.x = -xsin*ysin*zcos+ycos*zsin;
   up.y = ycos*zcos+xsin*ysin*zsin;
  // up.z = -xcos*ysin;
   right.x = xcos*zcos;
   right.y = -xcos*zsin;
   //right.z = -xsin;
   d = -vect.dot(center);
}

double Plane::getIntersection(Ray ray){
   const double t = ray.vector.dot(vect);
   const double norm = vect.dot(ray.point)+d;
   const double r = -norm/t;
   return (r>0)?r:inf;
}

bool Plane::getLightIntersection(Ray ray, double* fill){
   const double t = ray.vector.dot(vect);
   const double norm = vect.dot(ray.point)+d;
   const double r = -norm/t;
   if(r<=0. || r>=1.) return false;

   if(texture->opacity>1-1E-6) return true;   
   Vector dist = solveScalers(right, up, vect, ray.point-center);
   unsigned char temp[4];
   double amb, op, ref;
   texture->getColor(temp, &amb, &op, &ref,fix(dist.x/textureX-.5), fix(dist.y/textureY-.5));
   if(op>1-1E-6) return true;
   fill[0]*=temp[0]/255.;
   fill[1]*=temp[1]/255.;
   fill[2]*=temp[2]/255.;
   return false;
}

AABB Plane::getBounds() const {
    const double LARGE_SIZE = 10000.0;
    
    // Calculate the plane's normal vector (vect) and its perpendicular vectors (right, up)
    // These are already calculated in setAngles() and stored in the class
    
    // For an infinite plane, we need to extend in both directions perpendicular to the normal
    Vector u = right * LARGE_SIZE;
    Vector v = up * LARGE_SIZE;
    
    // Calculate eight corners of the extended plane space
    std::vector<Vector> corners(8, Vector(0,0,0));
    
    // Front face (in normal direction)
    corners[0] = center - u - v;
    corners[1] = center + u - v;
    corners[2] = center - u + v;
    corners[3] = center + u + v;
    
    // Back face (opposite to normal direction)
    Vector normalOffset = vect * 0.001; // Small thickness
    for(int i = 0; i < 4; i++) {
        corners[i + 4] = corners[i] - normalOffset;
        corners[i] = corners[i] + normalOffset;
    }
    
    // Find the minimum and maximum points
    Vector min(std::numeric_limits<double>::max(), std::numeric_limits<double>::max(), std::numeric_limits<double>::max());
    Vector max(std::numeric_limits<double>::lowest(), std::numeric_limits<double>::lowest(), std::numeric_limits<double>::lowest());
    
    for (int i = 0; i < 8; i++) {
        min = Vector::min(min, corners[i]);
        max = Vector::max(max, corners[i]);
    }
    
    // Add padding for numerical precision
    Vector padding(1e-4, 1e-4, 1e-4);
    return AABB(min - padding, max + padding);
}


void Plane::move(){
   d = -vect.dot(center);
}
void Plane::getColor(unsigned char* toFill,double* am, double* op, double* ref, Autonoma* r, Ray ray, unsigned int depth){
   Vector dist = solveScalers(right, up, vect, ray.point-center);
   texture->getColor(toFill, am, op, ref, fix(dist.x/textureX-.5), fix(dist.y/textureY-.5));
}
unsigned char Plane::reversible(){ 
   return 1; }

Vector Plane::getNormal(Vector point){
   if(normalMap==NULL)
      return vect;
   else{
      Vector dist = solveScalers(right, up, vect, point-center);
      double am, ref, op;
      unsigned char norm[3];
      normalMap->getColor(norm, &am, &op, &ref, fix(dist.x/mapX-.5+mapOffX), fix(dist.y/mapY-.5+mapOffY));
      Vector ret = ((norm[0]-128)*right+(norm[1]-128)*up+norm[2]*vect).normalize();
      return ret;
   }
}