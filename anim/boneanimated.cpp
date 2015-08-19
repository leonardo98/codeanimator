#include "boneanimated.h"
#include "ogl/render.h"

const float WideCoef = 0.1f;

BoneAnimated::BoneAnimated()
{
    _visible = false;
    _length = 100.f;

//    angle.AddPoint(0, 0);
//    angle.AddPoint(10, 0);

    //_pos.Rotate(M_PI);
    _pos.x = _pos.y = 0.f;
    _angle = M_PI;
}

void BoneAnimated::Draw()
{
    //if (!_visible) return;

    float l = _length;
    float w = WideCoef * _length / 2.f;
    Render::PushMatrix();
    Render::MatrixMove(_pos.x, _pos.y);
    Render::MatrixRotate(_angle);
    //Render::MatrixMul(_matrix);
    Render::DrawTriangle(- w, 0.f, 0.f, l, w, 0.f, 0x7F4F4F4F);
    Render::Line(- w, 0.f, 0.f, l, 0x7F000000);
    Render::Line(0.f, l, w, 0.f, 0x7F000000);
    Render::Line(- w, 0.f, w, 0.f, 0x7F000000);
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

bool BoneAnimated::CheckPoint(FPoint pos)
{
//    if (!_visible)
//        return false;

    Matrix m;
    m.Move(_pos.x, _pos.y);
    m.Rotate(_angle);

    //m.Mul(_matrix);

    Matrix rev;
    rev.MakeRevers(m);

    rev.Mul(pos);

    float l = _length;
    float w = WideCoef * _length;

    return -w < pos.x && pos.x < w && 0.f < pos.y && pos.y < l;
}

bool BoneAnimated::MoveOrRotate(FPoint pos)
{
//    if (!_visible)
//        return false;

    Matrix m;
    m.Move(_pos.x, _pos.y);
    m.Rotate(_angle);

    //m.Mul(_matrix);

    Matrix rev;
    rev.MakeRevers(m);

    rev.Mul(pos);

    float l = _length;
    float w = WideCoef * _length;

    return -w < pos.x && pos.x < w && 0.f < pos.y && pos.y < l / 2;
}

bool BoneAnimated::MoveTo(const FPoint &mt)
{
    _pos += mt;
}
