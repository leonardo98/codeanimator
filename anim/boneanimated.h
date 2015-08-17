#ifndef BONEANIMATED_H
#define BONEANIMATED_H

#include "spline.h"

class BoneAnimated
{
private:
    std::list<BoneAnimated *> children;
    Matrix matrix;
public:
    Spline x;
    Spline y;
    Spline angle;
    Spline scaleX;
    Spline scaleY;
public:
    const Matrix &GetMatrix() { return matrix; }
    BoneAnimated();
    void Draw();
    void AddChild(BoneAnimated *);
    void RemoveChild(BoneAnimated *);
    void CalculatePosition(const Matrix &, int frame);
};

#endif // BONEANIMATED_H
