#ifndef BONEANIMATED_H
#define BONEANIMATED_H

#include "boneposition.h"

class BoneAnimated
{
    Spline x;
    Spline y;
    Spline angle;
    Spline scaleX;
    Spline scaleY;
public:
    BoneAnimated();
    void Draw();
};

#endif // BONEANIMATED_H