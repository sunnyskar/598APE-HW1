#include "bvh.h"
#include <algorithm>
#include <limits>
#include "shape.h"
#include <iostream>
#include "disk.h"

AABB::AABB(const Vector& min, const Vector& max) : min(min), max(max) {}

bool AABB::intersect(const Ray& ray) const {
    // Handle division by zero
    double tx1 = (min.x - ray.point.x) / (ray.vector.x != 0 ? ray.vector.x : 1e-6);
    double tx2 = (max.x - ray.point.x) / (ray.vector.x != 0 ? ray.vector.x : 1e-6);
    double ty1 = (min.y - ray.point.y) / (ray.vector.y != 0 ? ray.vector.y : 1e-6);
    double ty2 = (max.y - ray.point.y) / (ray.vector.y != 0 ? ray.vector.y : 1e-6);
    double tz1 = (min.z - ray.point.z) / (ray.vector.z != 0 ? ray.vector.z : 1e-6);
    double tz2 = (max.z - ray.point.z) / (ray.vector.z != 0 ? ray.vector.z : 1e-6);

    double tmin = std::min(tx1, tx2);
    double tmax = std::max(tx1, tx2);
    tmin = std::max(tmin, std::min(ty1, ty2));
    tmax = std::min(tmax, std::max(ty1, ty2));
    tmin = std::max(tmin, std::min(tz1, tz2));
    tmax = std::min(tmax, std::max(tz1, tz2));

    return tmax >= tmin && tmax >= -1e-6;
}

AABB AABB::Union(const AABB& a, const AABB& b) {
    Vector min = Vector::min(a.min, b.min);
    Vector max = Vector::max(a.max, b.max);
    return AABB(min, max);
}

float AABB::surfaceArea() const {
    Vector size = max - min;
    return 2.0f * (size.x * size.y + size.x * size.z + size.y * size.z);
}


// BVHNode implementation
BVHNode::BVHNode() : left(nullptr), right(nullptr) {}

BVHNode::~BVHNode() {
    delete left;
    delete right;
}

// BVH implementation
BVH::BVH() : root(nullptr) {}

BVH::~BVH() {
    delete root;
}

BVH::BVH(ShapeNode* listStart) {
    std::vector<Shape*> shapes;
    ShapeNode* current = listStart;
    while (current != nullptr) {
        if (dynamic_cast<Disk*>(current->data)) {
            std::cout << "Initial Disk at position (" 
                      << current->data->center.x << "," 
                      << current->data->center.y << "," 
                      << current->data->center.z << ")" << std::endl;
        }
        shapes.push_back(current->data);
        current = current->next;
    }
    root = buildRecursive(shapes, 0);
}


BVHNode* BVH::buildRecursive(std::vector<Shape*>& shapes, int depth) {
    BVHNode* node = new BVHNode();
    
    // Base case: if we have 4 or fewer shapes, or we're too deep
    if (shapes.size() <= 4 || depth > 20) {
        node->bounds = computeBounds(shapes);
        node->shapes = shapes;
        return node;
    }
    
    // Find the axis with the largest spread
    AABB centroidBounds;
    Vector centroid = shapes[0]->center;
    centroidBounds.min = centroid;
    centroidBounds.max = centroid;
    
    for (const auto& shape : shapes) {
        centroid = shape->center;
        centroidBounds.min = Vector::min(centroidBounds.min, centroid);
        centroidBounds.max = Vector::max(centroidBounds.max, centroid);
    }
    
    Vector extent = centroidBounds.max - centroidBounds.min;
    int axis = 0;
    if (extent.y > extent.x) axis = 1;
    if (extent.z > extent[axis]) axis = 2;
    
    // Sort shapes based on their centroids along the chosen axis
    std::sort(shapes.begin(), shapes.end(),
        [axis](const Shape* a, const Shape* b) {
            return a->center[axis] < b->center[axis];
        });
    
    // Split point is the median
    size_t mid = shapes.size() / 2;
    std::vector<Shape*> leftShapes(shapes.begin(), shapes.begin() + mid);
    std::vector<Shape*> rightShapes(shapes.begin() + mid, shapes.end());
    
    // Recursively build children
    node->left = buildRecursive(leftShapes, depth + 1);
    node->right = buildRecursive(rightShapes, depth + 1);
    
    // Compute bounds for this node
    node->bounds = AABB::Union(node->left->bounds, node->right->bounds);
    
    return node;
}


bool BVH::intersect(const Ray& ray, Shape*& hitShape, double& tHit) const {
    return intersectRecursive(root, ray, hitShape, tHit);
}

bool BVH::intersectRecursive(BVHNode* node, const Ray& ray, Shape*& hitShape, double& tHit) const {
    if (!node || !node->bounds.intersect(ray)) {
        return false;
    }

    if (node->shapes.size() > 0) {
        bool hit = false;
        for (const auto& shape : node->shapes) {
            double t = shape->getIntersection(ray);
            if (t != inf && t > 1e-6 && t < tHit) {
                tHit = t;
                hitShape = shape;
                hit = true;
            }
        }
        return hit;
    }

    double leftT = tHit;
    double rightT = tHit;
    Shape* leftShape = nullptr;
    Shape* rightShape = nullptr;

    bool hitLeft = node->left && intersectRecursive(node->left, ray, leftShape, leftT);
    bool hitRight = node->right && intersectRecursive(node->right, ray, rightShape, rightT);

    if (hitLeft && (!hitRight || leftT < rightT)) {
        tHit = leftT;
        hitShape = leftShape;
        return true;
    }
    if (hitRight) {
        tHit = rightT;
        hitShape = rightShape;
        return true;
    }

    return false;
}


// Helper functions
AABB BVH::computeBounds(const std::vector<Shape*>& shapes) {
    if (shapes.empty()) {
        return AABB(Vector(0,0,0), Vector(0,0,0));
    }

    Vector min(std::numeric_limits<double>::max(), std::numeric_limits<double>::max(), std::numeric_limits<double>::max());
    Vector max(std::numeric_limits<double>::lowest(), std::numeric_limits<double>::lowest(), std::numeric_limits<double>::lowest());
    
    for (const auto& shape : shapes) {
        AABB shapeBounds = shape->getBounds();
        min = Vector::min(min, shapeBounds.min);
        max = Vector::max(max, shapeBounds.max);
    }

    Vector padding(1e-4, 1e-4, 1e-4);
    AABB bounds(min - padding, max + padding);
              
    return bounds;
}

int BVH::getLongestAxis(const AABB& bounds) {
    Vector extent = bounds.max - bounds.min;
    if (extent.x > extent.y && extent.x > extent.z) return 0;
    if (extent.y > extent.z) return 1;
    return 2;
}

bool BVH::getDebugBoxes(std::vector<AABB>& boxes, int mode) const {
    if (!root) return false;
    boxes.clear();
    collectDebugBoxes(root, boxes, mode, 0);
    return true;
}

void BVH::collectDebugBoxes(BVHNode* node, std::vector<AABB>& boxes, int mode, int depth) const {
    if (!node) return;

    // Mode 1: Show all boxes
    // Mode 2: Show only leaf nodes
    // Mode 3: Show only internal nodes
    
    switch (mode) {
        case 1:
            boxes.push_back(node->bounds);
            break;
        case 2:
            if (!node->left && !node->right) {
                boxes.push_back(node->bounds);
            }
            break;
        case 3:
            if (node->left || node->right) {
                boxes.push_back(node->bounds);
            }
            break;
    }

    // Recursively collect child boxes
    if (node->left) collectDebugBoxes(node->left, boxes, mode, depth + 1);
    if (node->right) collectDebugBoxes(node->right, boxes, mode, depth + 1);
}

