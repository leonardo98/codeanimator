#include "boneanimated.h"
#include "ogl/render.h"
#include "extmath.h"
#include "animation.h"

const float WideCoef = 0.1f;

BoneAnimated::~BoneAnimated()
{
    for (BoneList::iterator i = _children.begin(), e = _children.end(); i != e; ++i)
    {
        (*i)->SetParent(NULL);
    }
    SetParent(NULL);
    _children.clear();
}

BoneAnimated::BoneAnimated()
{
    _visible = false;
    _length = BONE_LENGTH_MIN;

//    angle.AddPoint(0, 0);
//    angle.AddPoint(10, 0);

    _pos.x = _pos.y = 0.f;
    _angle = 0.f;
    _parent = NULL;
    _moveByParent = false;
}

void BoneAnimated::DrawRed()
{
    Render::PushMatrix();
    Render::MatrixMul(_matrix);
    Render::Line(_length, 0.f, 0.f, 0.f, 0xFF00FF00);
    Render::PopMatrix();
}

FPoint BoneAnimated::GetEndPoint()
{
    FPoint p(_length, 0.f);
    GetMatrix().Mul(p);
    return p;
}

FPoint BoneAnimated::GetGlobalBonePos()
{
    FPoint p(_pos);
    if (GetParent()) GetParent()->GetMatrix().Mul(p);
    return p;
}

void BoneAnimated::Draw()
{
    //if (!_visible) return;

    float l = _length;
    float w = WideCoef * _length / 2.f;
    Render::PushMatrix();
    Render::MatrixMul(_matrix);
    Render::DrawTriangle(0.f, - w, l, 0.f, 0.f, w, 0x7FFFFFFF);
    Render::Line(0.f, - w, l, 0.f, 0x7F000000);
    Render::Line(l, 0.f, 0.f, w, 0x7F000000);
    Render::Line(0.f, - w, 0.f, w, 0x7F000000);
    Render::PopMatrix();




    FPoint s(0, 0);
    FPoint e(GetLength(), 0);
    GetMatrix().Mul(s);
    GetMatrix().Mul(e);

    Render::Circle(s.x, s.y, 10, 0xFFFF0000);
    Render::Circle(e.x, e.y, 10, 0xFFFF0000);


}

void BoneAnimated::DrawSelection()
{
    float l = _length;
    float w = WideCoef * _length;
    Render::PushMatrix();
    Render::MatrixMul(_matrix);
    Render::Line(- w, - w, l + w, - w, 0x7F000000);
    Render::Line(- w, w, l + w, w, 0x7F000000);
    Render::Line(- w, - w, - w, w, 0x7F000000);
    Render::Line(l + w, - w, l + w, w, 0x7F000000);

    if (_parent)
        Render::Circle(0, 0, w, 0x7F000000);

    if (_children.size())
        Render::Circle(l, 0, w, 0x7F000000);

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
    _parentMatrix = m;

    _matrix = m;
    _matrix.Move(_pos.x, _pos.y);
    _matrix.Rotate(_angle);

    _visible = false;
    if (x.Active(frame) || y.Active(frame))
    {
        _matrix.Move(x.GetValueGlobal(frame, p), y.GetValueGlobal(frame, p));
        _visible = true;
    }

    if (angle.Active(frame))
    {
        _matrix.Rotate(angle.GetValueGlobal(frame, p));
        _visible = true;
    }

    if (scaleX.Active(frame) || scaleY.Active(frame))
    {
        _matrix.Scale(scaleX.Active(frame) ? scaleX.GetValueGlobal(frame, p) : 1.f
                      , scaleY.Active(frame) ? scaleY.GetValueGlobal(frame, p) : 1.f);
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

    Matrix rev;
    rev.MakeRevers(_matrix);

    rev.Mul(pos);

    float l = _length;
    float w = WideCoef * _length;

    return -w < pos.y && pos.y < w && - w < pos.x && pos.x < l + w;
}

bool BoneAnimated::MoveOrRotate(FPoint pos)
{
//    if (!_visible)
//        return false;

    Matrix rev;
    rev.MakeRevers(_matrix);

    rev.Mul(pos);

    float l = _length;
    float w = WideCoef * _length;

    return -w < pos.y && pos.y < w && - w < pos.x && pos.x < l / 2;
}

bool BoneAnimated::MoveTo(const FPoint &mt)
{
    _pos += mt;
    CalculatePosition(_parentMatrix, 0.f);
}

void BoneAnimated::MoveEnd(const FPoint &dst, const FPoint &src)
{

}

bool BoneAnimated::IfInside(const Rect &area)
{
    FPoint s(0, 0);
    GetMatrix().Mul(s);

    if (!(area.x1 <= s.x && s.x <= area.x2 && area.y1 <= s.y && s.y <= area.y2))
    {
        return false;
    }

    FPoint p(_length, 0);
    GetMatrix().Mul(p);

    return (area.x1 <= p.x && p.x <= area.x2 && area.y1 <= p.y && p.y <= area.y2);
}

void BoneAnimated::SetParent(BoneAnimated *b)
{
    FPoint s(0, 0);
    GetMatrix().Mul(s);

    FPoint e(_length, 0);
    //e.Rotate(_angle);
    GetMatrix().Mul(e);

    if (_parent)
        _parent->RemoveChild(this);

    if (b && b->AddChild(this))
    {
        _parent = b;

        Matrix rev;
        rev.MakeRevers(_parent->GetMatrix());

        rev.Mul(s);
        rev.Mul(e);

        SetBonePos(s);
        SetBoneAngle(atan2(e.y - s.y, e.x - s.x));
    }
    else
    {
        _parent = NULL;
        SetBonePos(s);
        SetBoneAngle(atan2(e.y - s.y, e.x - s.x));
    }
}

BoneAnimated *BoneAnimated::GetBoneAtPoint(const FPoint &pos)
{
    for (uint i = 0; i < _children.size(); ++i)
    {
        if (_children[i]->CheckPoint(pos))
        {
            BoneAnimated *b = _children[i]->GetBoneAtPoint(pos);
            return b ? b : _children[i];
        }
    }
    return NULL;
}

void BoneAnimated::SetBoneAngle(float a)
{
    _angle = a;
    CalculatePosition(_parentMatrix, 0.f);
}

void BoneAnimated::SetBonePos(const FPoint &pos)
{
    _pos = pos;
    CalculatePosition(_parentMatrix, 0.f);
}

BoneAnimated::BoneAnimated(rapidxml::xml_node<> *xe)
    : x(xe->first_node("x"))
    , y(xe->first_node("y"))
    , angle(xe->first_node("angle"))
    , scaleX(xe->first_node("scaleX"))
    , scaleY(xe->first_node("scaleY"))
    , _parent(NULL)
{
    _name = xe->first_attribute("name")->value();
    _pos.x = atof(xe->first_attribute("x")->value());
    _pos.y = atof(xe->first_attribute("y")->value());
    _angle = atof(xe->first_attribute("angle")->value());
    _length = atof(xe->first_attribute("length")->value());
    _bindPointMatrix.ReadFromString(xe->first_attribute("bpm")->value());

    rapidxml::xml_node<> *children = xe->first_node("children");
    rapidxml::xml_node<> *bone = children->first_node("bone");
    _children.resize(atoi(children->first_attribute("size")->value()));
    int counter = 0;
    while (bone)
    {
        assert(counter < _children.size());
        _children[counter] = new BoneAnimated(bone);
        _children[counter]->_parent = this;
        Animation::Instance()->RegisterBone(_children[counter]);
        counter++;
        bone = bone->next_sibling("bone");
    }
    assert(counter == _children.size());
}

BoneAnimated::BoneAnimated(const BoneAnimated &b)
    : x(b.x)
    , y(b.y)
    , angle(b.angle)
    , scaleX(b.scaleX)
    , scaleY(b.scaleY)
    , _parent(NULL)
    , _name(b._name)
    , _length(b._length)
    , _bindPointMatrix(b._bindPointMatrix)
    , _pos(b._pos)
    , _angle(b._angle)
{
    _children.resize(b._children.size());
    for (uint i = 0; i < b._children.size();++i)
    {
        _children[i] = new BoneAnimated(*b._children[i]);
        _children[i]->_parent = this;
        Animation::Instance()->RegisterBone(_children[i]);
    }
}

void BoneAnimated::SaveToXml(rapidxml::xml_node<> *xe)
{
    Math::Write(xe, "name", _name.c_str());
    Math::Write(xe, "x", _pos.x);
    Math::Write(xe, "y", _pos.y);
    Math::Write(xe, "angle", _angle);
    Math::Write(xe, "length", _length);
    Math::Write(xe, "bpm", _bindPointMatrix.PrintToString().c_str());

    if (!x.Empty())
    {
        rapidxml::xml_node<> *splineX = xe->document()->allocate_node(rapidxml::node_element, "x");
        xe->append_node(splineX);
        x.SaveToXml(splineX);
    }

    if (!y.Empty())
    {
        rapidxml::xml_node<> *splineY = xe->document()->allocate_node(rapidxml::node_element, "y");
        xe->append_node(splineY);
        y.SaveToXml(splineY);
    }

    if (!angle.Empty())
    {
        rapidxml::xml_node<> *splineAngle = xe->document()->allocate_node(rapidxml::node_element, "angle");
        xe->append_node(splineAngle);
        angle.SaveToXml(splineAngle);
    }

    if (!scaleX.Empty())
    {
        rapidxml::xml_node<> *splineScaleX = xe->document()->allocate_node(rapidxml::node_element, "scaleX");
        xe->append_node(splineScaleX);
        scaleX.SaveToXml(splineScaleX);
    }

    if (!scaleY.Empty())
    {
        rapidxml::xml_node<> *splineScaleY = xe->document()->allocate_node(rapidxml::node_element, "scaleY");
        xe->append_node(splineScaleY);
        scaleY.SaveToXml(splineScaleY);
    }

    rapidxml::xml_node<> *children = xe->document()->allocate_node(rapidxml::node_element, "children");
    xe->append_node(children);

    Math::Write(children, "size", (int)_children.size());
    for (unsigned int j = 0; j < _children.size(); ++j)
    {
        rapidxml::xml_node<> *bone = xe->document()->allocate_node(rapidxml::node_element, "bone");
        children->append_node(bone);
        _children[j]->SaveToXml(bone);
    }
}

void BoneAnimated::FixMatrix()
{
    _bindPointMatrix.MakeRevers(GetMatrix());
}

const Matrix &BoneAnimated::GetAnimVertMatrix()
{
    _animMatrix = GetMatrix();
    _animMatrix.Mul(_bindPointMatrix);
    return _animMatrix;
}

void BoneAnimated::GetBoneList(BoneList &bones)
{
    for (auto o : _children)
    {
        bones.push_back(o);
        o->GetBoneList(bones);
    }
}

