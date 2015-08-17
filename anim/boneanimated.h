#ifndef BONEANIMATED_H
#define BONEANIMATED_H

#include "spline.h"

class BoneAnimated;

typedef std::list<BoneAnimated *> BoneList;

class BoneAnimated
{
private:
    BoneList _children;
    Matrix _matrix;
    bool _visible;
    float _length;
public:
    Spline x;
    Spline y;
    Spline angle;
    Spline scaleX;
    Spline scaleY;
public:
    const Matrix &GetMatrix() { return _matrix; }
    BoneAnimated();
    void Draw();
    void AddChild(BoneAnimated *);
    void RemoveChild(BoneAnimated *);
    void CalculatePosition(const Matrix &, int frame, float p = 0.f);
    void SetLength(float l) { _length = l; }
};

#endif // BONEANIMATED_H
