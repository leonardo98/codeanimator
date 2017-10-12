#include "rect.h"
#include <math.h>
#include <algorithm>


void Rect::Encapsulate(float x, float y)
{
    if(bClean)
    {
        x1=x2=x;
        y1=y2=y;
        bClean=false;
    }
    else
    {
        if(x<x1) x1=x;
        if(x>x2) x2=x;
        if(y<y1) y1=y;
        if(y>y2) y2=y;
    }
}

bool Rect::TestPoint(float x, float y) const
{
    if(x>=x1 && x<x2 && y>=y1 && y<y2) return true;

    return false;
}

bool Rect::Intersect(const Rect *rect) const
{
    if ((fabs(x1 + x2 - rect->x1 - rect->x2) < (x2 - x1 + rect->x2 - rect->x1))
        && (fabs(y1 + y2 - rect->y1 - rect->y2) < (y2 - y1 + rect->y2 - rect->y1)))
            return true;

    return false;
}

bool Rect::Contain(const Rect *rect) const
{
    if (std::min(x1, x2) < std::min(rect->x1, rect->x2) && std::min(y1, y2) < std::min(rect->y1, rect->y2)
            && std::max(x1, x2) > std::max(rect->x1, rect->x2) && std::max(y1, y2) > std::max(rect->y1, rect->y2))
    {
        return true;
    }

    return false;
}
