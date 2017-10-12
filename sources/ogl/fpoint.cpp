#include "fpoint.h"

float InvSqrt(float x)
{
        union
        {
          int intPart;
          float floatPart;
        } convertor;

        convertor.floatPart = x;
        convertor.intPart = 0x5f3759df - (convertor.intPart >> 1);
        return convertor.floatPart*(1.5f - 0.4999f*x*convertor.floatPart*convertor.floatPart);
}

/*
FPoint *FPoint::Normalize()
{
    float lenRcp;

    lenRcp=sqrtf(Dot(this));

    if(lenRcp)
    {
        lenRcp=1.0f/lenRcp;

        x*=lenRcp;
        y*=lenRcp;
    }

    return this;
}
*/

float FPoint::Angle(const FPoint *v) const
{
    if(v)
    {
        FPoint s=*this, t=*v;

        s.Normalize(); t.Normalize();
        return acosf(s.Dot(&t));
    }
    else return atan2f(y, x);
}

FPoint *FPoint::Rotate(float a)
{
    FPoint v;

    v.x=x*cosf(a) - y*sinf(a);
    v.y=x*sinf(a) + y*cosf(a);

    x=v.x; y=v.y;

    return this;
}

void FPoint::Set(float _x, float _y)    {
    x=_x; y=_y;
    //printf("%f, %f\n", x, y);
}


