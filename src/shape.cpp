#include "shape.h"
#include <iostream>

Shape::Shape(const Vector &c, Texture* t, double ya, double pi, double ro): center(c), texture(t), yaw(ya), pitch(pi), roll(ro){
};

void Shape::setAngles(double a, double b, double c){
   yaw =a; pitch = b; roll = c;
   xcos = cos(yaw);
   xsin = sin(yaw);
   ycos = cos(pitch);
   ysin = sin(pitch);
   zcos = cos(roll);
   zsin = sin(roll);
}

void Shape::setYaw(double a){
   yaw =a;
   xcos = cos(yaw);
   xsin = sin(yaw);
}

void Shape::setPitch(double b){
   pitch = b;
   ycos = cos(pitch);
   ysin = sin(pitch);
}

void Shape::setRoll(double c){
   roll = c;
   zcos = cos(roll);
   zsin = sin(roll);
}

typedef struct {
   double time;
   Shape* shape;
} TimeAndShape;

void insertionSort(TimeAndShape *arr, int n) {
    for (int i = 1; i < n; ++i) {
        TimeAndShape key = arr[i];
        int j = i - 1;
        while (j >= 0 && arr[j].time > key.time) {
            arr[j + 1] = arr[j];
            j = j - 1;
        }
        arr[j + 1] = key;
    }
}

void calcColor(unsigned char* toFill, Autonoma* c, BVH* bvh, Ray ray, unsigned int depth) {
    double tHit = std::numeric_limits<double>::max();
    Shape* hitShape = nullptr;
    
    // Get the actual shape intersection through BVH
    bool hit = bvh->intersect(ray, hitShape, tHit);
    
    // Only proceed with coloring if we hit an actual shape
    if (!hit || hitShape == nullptr || tHit == inf) {
        double opacity, reflection, ambient;
        Vector temp = ray.vector.normalize();
        const double x = temp.x;
        const double z = temp.z;
        const double me = (temp.y < 0) ? -temp.y : temp.y;
        const double angle = atan2(z, x);
        c->skybox->getColor(toFill, &ambient, &opacity, &reflection, fix(angle/M_TWO_PI), fix(me));
        return;
    }
    // Calculate intersection point with the actual shape
    Vector intersect = ray.point + ray.vector * tHit;
    
    // Get color from the actual shape
    double opacity, reflection, ambient;
    hitShape->getColor(toFill, &ambient, &opacity, &reflection, c, Ray(intersect, ray.vector), depth);
    
    // Calculate lighting only if we hit a valid point on the disk
    if (hit && hitShape) {
        // Calculate lighting
        double lightData[3];
        Vector normal = hitShape->getNormal(intersect);
        getLight(lightData, c, intersect, normal, hitShape->reversible());
        
        // Apply lighting to color
        toFill[0] = (unsigned char)(toFill[0] * (ambient + lightData[0] * (1-ambient)));
        toFill[1] = (unsigned char)(toFill[1] * (ambient + lightData[1] * (1-ambient)));
        toFill[2] = (unsigned char)(toFill[2] * (ambient + lightData[2] * (1-ambient)));
    }

    // Handle transparency and reflection
    if (depth < c->depth && (opacity < 1-1e-6 || reflection > 1e-6)) {
        unsigned char col[4];
        
        // Handle transparency
        if (opacity < 1-1e-6) {
            Ray nextRay = Ray(intersect + ray.vector * 1E-4, ray.vector);
            calcColor(col, c, bvh, nextRay, depth+1);
            toFill[0] = (unsigned char)(toFill[0] * opacity + col[0] * (1-opacity));
            toFill[1] = (unsigned char)(toFill[1] * opacity + col[1] * (1-opacity));
            toFill[2] = (unsigned char)(toFill[2] * opacity + col[2] * (1-opacity));
        }
        
        // Handle reflection
        if (reflection > 1e-6) {
            Vector norm = hitShape->getNormal(intersect).normalize();
            Vector vec = ray.vector - norm * (2 * norm.dot(ray.vector));
            Ray nextRay = Ray(intersect + vec * 1E-4, vec);
            calcColor(col, c, bvh, nextRay, depth+1);
            
            toFill[0] = (unsigned char)(toFill[0] * (1-reflection) + col[0] * reflection);
            toFill[1] = (unsigned char)(toFill[1] * (1-reflection) + col[1] * reflection);
            toFill[2] = (unsigned char)(toFill[2] * (1-reflection) + col[2] * reflection);
        }
    }
}
