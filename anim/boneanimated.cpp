#include "boneanimated.h"
#include "ogl/render.h"

const float WideCoef = 0.1f;

BoneAnimated::BoneAnimated()
{
    _visible = false;
    _length = 100.f;

//    angle.AddPoint(0, 0);
//    angle.AddPoint(10, 0);

    _pos.x = _pos.y = 0.f;
    _angle = M_PI;
    _parent = NULL;
}

void BoneAnimated::Draw()
{
    //if (!_visible) return;

    float l = _length;
    float w = WideCoef * _length / 2.f;
    Render::PushMatrix();
//    Render::MatrixMove(_pos.x, _pos.y);
//    Render::MatrixRotate(_angle);
    Render::MatrixMul(_matrix);
    Render::DrawTriangle(- w, 0.f, 0.f, l, w, 0.f, 0x7F4F4F4F);
    Render::Line(- w, 0.f, 0.f, l, 0x7F000000);
    Render::Line(0.f, l, w, 0.f, 0x7F000000);
    Render::Line(- w, 0.f, w, 0.f, 0x7F000000);
    Render::PopMatrix();
}

void BoneAnimated::DrawSelection()
{
    float l = _length;
    float w = WideCoef * _length;
    Render::PushMatrix();
//    Render::MatrixMove(_pos.x, _pos.y);
//    Render::MatrixRotate(_angle);
    Render::MatrixMul(_matrix);
    Render::Line(- w, - w, - w, l + w, 0x7F000000);
    Render::Line( w, - w, w, l + w, 0x7F000000);
    Render::Line(- w, - w, w, - w, 0x7F000000);
    Render::Line(- w, l + w, w, l + w, 0x7F000000);

    if (_parent)
        Render::Circle(0, 0, w, 0x7F000000);

    if (_children.size())
        Render::Circle(0, l, w, 0x7F000000);

    Render::PopMatrix();
}

bool BoneAnimated::AddChild(BoneAnimated *b)
{
    BoneAnimated *parent = b->GetParent();
    while (parent)
    {
        if (parent == this) return false;
        parent = parent->GetParent();
    }
    parent = GetParent();
    while (parent)
    {
        if (parent == b) return false;
        parent = parent->GetParent();
    }

    for (BoneList::iterator i = _children.begin(), e = _children.end(); i != e; ++i)
    {
        if ((*i) == b)
        {
            return true;
        }
    }

    _children.push_back(b);
    return true;
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
    _matrix = m;
    _matrix.Move(_pos.x, _pos.y);
    _matrix.Rotate(_angle);

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

    //if (_visible)
    {
        for (BoneList::iterator i = _children.begin(), e = _children.end(); i != e; ++i)
        {
            (*i)->CalculatePosition(_matrix, frame, p);
        }
    }
}

bool BoneAnimated::CheckPoint(FPoint pos)
{
//    if (!_visible)
//        return false;

    Matrix m;
//    m.Move(_pos.x, _pos.y);
//    m.Rotate(_angle);

    m.Mul(_matrix);

    Matrix rev;
    rev.MakeRevers(m);

    rev.Mul(pos);

    float l = _length;
    float w = WideCoef * _length;

    return -w < pos.x && pos.x < w && - w < pos.y && pos.y < l + w;
}

bool BoneAnimated::MoveOrRotate(FPoint pos)
{
//    if (!_visible)
//        return false;

    Matrix m;
//    m.Move(_pos.x, _pos.y);
//    m.Rotate(_angle);

    m.Mul(_matrix);

    Matrix rev;
    rev.MakeRevers(m);

    rev.Mul(pos);

    float l = _length;
    float w = WideCoef * _length;

    return -w < pos.x && pos.x < w && - w < pos.y && pos.y < l / 2;
}

bool BoneAnimated::MoveTo(const FPoint &mt)
{
    _pos += mt;
}

bool BoneAnimated::IfInside(const Rect &area)
{
    if (!(area.x1 <= _pos.x && _pos.x <= area.x2
            &&  area.y1 <= _pos.y && _pos.y <= area.y2))
    {
        return false;
    }

    FPoint p(0, _length);
    p.Rotate(_angle);
    p += _pos;

    return (area.x1 <= p.x && p.x <= area.x2 && area.y1 <= p.y && p.y <= area.y2);
}

void BoneAnimated::SetParent(BoneAnimated *b)
{
    if (_parent)
        _parent->RemoveChild(this);

    if (b && b->AddChild(this))
    {
        _parent = b;
    }
    else
    {
        _parent = NULL;
    }
}

