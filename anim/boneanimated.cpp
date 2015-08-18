#include "boneanimated.h"
#include "ogl/render.h"

BoneAnimated::BoneAnimated()
{
    _visible = false;
    _length = 100.f;

    angle.AddPoint(0, 0);
    angle.AddPoint(10, 0);
}

void BoneAnimated::Draw()
{
    if (!_visible) return;

    float l = _length;
    float wk = 0.1f;
    Render::PushMatrix();
    Render::MatrixMul(_matrix);
    Render::DrawTriangle(- wk * l / 2.f, 0.f, 0.f, l, wk * l / 2.f, 0.f, 0x7F4F4F4F);
    Render::Line(- wk * l / 2.f, 0.f, 0.f, l, 0x7F000000);
    Render::Line(0.f, l, wk * l / 2.f, 0.f, 0x7F000000);
    Render::Line(- wk * l / 2.f, 0.f, wk * l / 2.f, 0.f, 0x7F000000);
    Render::PopMatrix();
}

void BoneAnimated::AddChild(BoneAnimated *b)
{
    _children.push_back(b);
}

void BoneAnimated::RemoveChild(BoneAnimated *b)
{
    for (BoneList::iterator i = _children.begin(), e = _children.end(); i != e; ++i)
    {
        if (*i == b)
        {
            _children.erase(i);
            return;
        }
    }
}

void BoneAnimated::CalculatePosition(const Matrix &m, int frame, float p)
{
    Matrix _matrix(m);
    _visible = false;
    if (x.Active(frame) || y.Active(frame))
    {
        _matrix.Move(x.GetGlobalValue(frame, p), y.GetGlobalValue(frame, p));
        _visible = true;
    }

    if (angle.Active(frame))
    {
        _matrix.Rotate(angle.GetGlobalValue(frame, p));
        _visible = true;
    }

    if (scaleX.Active(frame) || scaleY.Active(frame))
    {
        _matrix.Scale(scaleX.Active(frame) ? scaleX.GetGlobalValue(frame, p) : 1.f
                      , scaleY.Active(frame) ? scaleY.GetGlobalValue(frame, p) : 1.f);
        _visible = true;
    }

    if (_visible)
    {
        for (BoneList::iterator i = _children.begin(), e = _children.end(); i != e; ++i)
        {
            (*i)->CalculatePosition(_matrix, frame, p);
        }
    }
}
