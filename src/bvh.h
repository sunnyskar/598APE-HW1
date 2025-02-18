#ifndef BVH_H
#define BVH_H

#include "vector.h"
#include <vector>

class AABB {
public:
    Vector min, max;

    AABB() : min(0, 0, 0), max(0, 0, 0) {}
    AABB(const Vector& min, const Vector& max);
    bool intersect(const Ray& ray) const;
    static AABB Union(const AABB& a, const AABB& b);
    float surfaceArea() const;
};

class Shape;

class BVHNode {
public:
    AABB bounds;
    BVHNode* left;
    BVHNode* right;
    std::vector<Shape*> shapes;

    BVHNode();
    ~BVHNode();
};

class ShapeNode;
class BVH {
public:
    BVHNode* root;

    BVH();
    ~BVH();
    BVH(ShapeNode* listStart);
    bool intersect(const Ray& ray, Shape*& hitShape, double& tHit) const;
    bool getDebugBoxes(std::vector<AABB>& boxes, int mode) const;

private:
    BVHNode* buildRecursive(std::vector<Shape*>& shapes, int depth);
    bool intersectRecursive(BVHNode* node, const Ray& ray, Shape*& hitShape, double& tHit) const;
    AABB computeBounds(const std::vector<Shape*>& shapes);
    int getLongestAxis(const AABB& bounds);
    void collectDebugBoxes(BVHNode* node, std::vector<AABB>& boxes, int mode, int depth) const;
};

#endif // BVH_H