#ifndef FPOINT_H
#define FPOINT_H

#include <math.h>
#include <stdio.h>


/*
** Fast 1.0/sqrtf(float) routine
*/
float InvSqrt(float x);

class FPoint
{
public:
    float	x,y;

    FPoint(float _x, float _y)	{ x=_x; y=_y; }
    void Set(float _x, float _y);
    FPoint() {}

    FPoint	operator-  ()					const { return FPoint(-x, -y);		}
    FPoint	operator-  (const FPoint &v) const { return FPoint(x-v.x, y-v.y); }
    FPoint	operator+  (const FPoint &v) const { return FPoint(x+v.x, y+v.y); }
    FPoint&	operator-= (const FPoint &v)		  { x-=v.x; y-=v.y; return *this;	}
    FPoint&	operator+= (const FPoint &v)		  { x+=v.x; y+=v.y; return *this;	}
    bool		operator== (const FPoint &v)	const { return (x==v.x && y==v.y);		}
    bool		operator!= (const FPoint &v)	const { return (x!=v.x || y!=v.y);		}

    FPoint	operator/  (const float scalar)	const { return FPoint(x/scalar, y/scalar); }
    FPoint	operator*  (const float scalar) const { return FPoint(x*scalar, y*scalar); }
    FPoint&	operator*= (const float scalar)		  { x*=scalar; y*=scalar; return *this;   }

    float		Dot(const FPoint *v) const { return x*v->x + y*v->y; }
    float		Length() const { return sqrtf(Dot(this)); }
    float		Angle(const FPoint *v = 0) const;

    void		Clamp(const float max) { if(Length() > max)	{ Normalize(); x *= max; y *= max; } }
    FPoint*	Normalize() { float rc=InvSqrt(Dot(this)); x*=rc; y*=rc; return this; }
    FPoint*	Rotate(float a);
};

inline FPoint operator* (const float s, const FPoint &v)		{ return v*s; }
inline float	 operator^ (const FPoint &v, const FPoint &u) { return v.Angle(&u); }
inline float	 operator% (const FPoint &v, const FPoint &u) { return v.Dot(&u); }


#endif // FPOINT_H
