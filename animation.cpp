#include "animation.h"
#include <set>
#include "ogl/render.h"
#include "mainwindow.h"

Animation *Animation::_instance = NULL;

Animation::Animation()
{
    assert(_instance == NULL);
    _instance = this;
    _boneMoving = false;
    _startMovingBone = -1;
    _texture = NULL;
    _baseSprite = NULL;
    _meshGenerateBone = -1;

//    testPoints.push_back(FPoint(0, 0));
//    testPoints.push_back(FPoint(0, 100));
//    testPoints.push_back(FPoint(100, 100));
//    testPoints.push_back(FPoint(100, 0));
}

Animation::~Animation()
{
    assert(_instance != NULL);
    _instance = NULL;

    if (_texture)
        delete _texture;

    if (_baseSprite)
        delete _baseSprite;
}

void Animation::Draw()
{
    if (_baseSprite && MainWindow::Instance()->CreateDotMode())
    {
        Render::SetColor(0x7FFFFFFF);
        _baseSprite->Render(- _baseSprite->Width() / 2, - _baseSprite->Height() / 2);
        Render::SetColor(0xFFFFFFFF);
    }

    // hide all bones
    for (BoneList::iterator i = _bones.begin(), e = _bones.end(); i != e; ++i)
    {
        (*i)->SetVisible(false);
    }

    // calculate new positions and visibility
    Matrix pos;
    pos.Unit();
    for (BoneList::iterator i = _bones.begin(), e = _bones.end(); i != e; ++i)
    {
        if ((*i)->GetParent() == NULL)
            (*i)->CalculatePosition(pos, 0.f);
    }

    // draw meshes
    for (uint i = 0; i < _meshes.size(); ++i)
    {
        _meshes[i]->DebugDraw(false);
    }

    // draw bones
    for (BoneList::iterator i = _bones.begin(), e = _bones.end(); i != e; ++i)
    {
        (*i)->Draw();
    }

    // draw selected bones
    for (uint i = 0; i < _selected.size(); ++i)
    {
        _bones[_selected[i]]->DrawSelection();
    }

    // todo: draw mesh here


//    for (int i = 0; i < (int)testPoints.size() - 1; ++i)
//        Render::Line(testPoints[i].x, testPoints[i].y,
//                     testPoints[i + 1].x, testPoints[i + 1].y, 0xFFFFFFFF);
//    for (int i = 0; i < (int)_chainPoints.size() - 1; ++i)
//        Render::Line(_chainPoints[i].x, _chainPoints[i].y,
//                     _chainPoints[i + 1].x, _chainPoints[i + 1].y, 0xFFFFFFFF);
}

std::string Animation::GenerateUnicBoneName()
{
    std::set<std::string> allNames;
    for (BoneList::iterator i = _bones.begin(), e = _bones.end(); i != e; ++i)
    {
        allNames.insert((*i)->GetName());
    }
    int i = 1;
    char name[100];
    do
    {
        sprintf(name, "bone_%03i", i++);
    } while (allNames.find(name) != allNames.end());
    return name;
}

uint Animation::CreateBone(FPoint pos, bool generateMesh)
{
    uint r = _bones.size();
    BoneAnimated *b = new BoneAnimated();
    b->SetBonePos(pos);
    b->SetName(GenerateUnicBoneName());
    _bones.push_back(b);

    if (generateMesh)
    {
        PointList points;

        FPoint p1(pos);
        FPoint p2 = *FPoint(b->GetLength(), 0.f).Rotate(b->GetBoneAngle()) + pos;
        FPoint o = *FPoint(0.f, b->GetLength() * 0.314f).Rotate(b->GetBoneAngle());

        points.push_back(p1 + o);
        points.push_back(p1 - o);
        points.push_back(p2 - o);
        points.push_back(p2 + o);

        ColoredPolygon *c = new ColoredPolygon(points);
        _meshes.push_back(c);
//        b->SetMesh(c);
        _meshGenerateBone = r;
    }

    return r;
}

int Animation::GetBoneAtPoint(const FPoint &pos)
{
    for (uint i = 0; i < _bones.size(); ++i)
    {
        if (_bones[i]->CheckPoint(pos))
        {
            BoneAnimated *b = _bones[i]->GetBoneAtPoint(pos);
            if (b)
            {
                for (uint j = 0; j < _bones.size(); ++j)
                {
                    if (_bones[j] == b)
                    {
                        return (int)j;
                    }
                }
            }
            return (int)i;
        }
    }
    return -1;
}

void Animation::StartBoneMoving(uint index, const FPoint &point)
{
    _boneMoving = _bones[index]->MoveOrRotate(point);
    _startMovingPos = point;
    _startRotateAngle = _bones[index]->GetBoneAngle();
    _startMovingBone = index;

    // -=IK=-

    //bone under cursor
    BoneAnimated *mb = _bones[_startMovingBone];

    //find all selected parents
    _boneChain.clear();
    _boneChain.push_back(mb);

    std::set<BoneAnimated*> sel;
    for (uint i = 0; i < _selected.size(); ++i)
    {
        sel.insert(_bones[_selected[i]]);
    }

    BoneAnimated *parent = mb->GetParent();
    while (parent && sel.find(parent) != sel.end())
    {
        _boneChain.push_back(parent);
        parent = parent->GetParent();
    }

    for (uint i = 0; i < _boneChain.size() / 2; ++i)
    {
        BoneAnimated *tmp = _boneChain[i];
        _boneChain[i] = _boneChain[_boneChain.size() - 1 - i];
        _boneChain[_boneChain.size() - 1 - i] = tmp;
    }

    //form FPoint list
    _chainPoints.resize(_boneChain.size());
    for (uint i = 0; i < _boneChain.size(); ++i)
    {
        FPoint p(_boneChain[i]->GetBonePos());
        if (_boneChain[i]->GetParent())
        {
            _boneChain[i]->GetParent()->GetMatrix().Mul(p);
        }
        _chainPoints[i]= p;
    }
    _chainPoints.push_back(_startMovingPos);
    _deltaAngle.resize(_boneChain.size());
    for (uint i = 0; i < _deltaAngle.size(); ++i)
    {
        FPoint a(_chainPoints[i]);
        FPoint b(_chainPoints[i + 1]);

        Matrix rev;
        rev.MakeRevers(_boneChain[i]->GetMatrix());

        rev.Mul(a);
        rev.Mul(b);

        _deltaAngle[i] = - atan2f(b.y - a.y, b.x - a.x);
    }

}

void Animation::BoneMoveTo(const FPoint &point, bool changeLength)
{
    if (_boneMoving || _selected.size() > 1)
    {
        for (uint i = 0; i < _selected.size(); ++i)
        {
            if (_bones[_selected[i]]->GetMoveByParent()) continue;

            if (_bones[_selected[i]]->GetParent() == NULL)
                _bones[_selected[i]]->MoveTo(point - _startMovingPos);
            else
            {
                FPoint p(point);
                FPoint s(_startMovingPos);

                Matrix rev;
                rev.MakeRevers(_bones[_selected[i]]->GetParent()->GetMatrix());

                rev.Mul(p);
                rev.Mul(s);

                _bones[_selected[i]]->MoveTo(p - s);
            }
        }
        _startMovingPos = point;

        if (_selected.size() == 1)
        {
            for (uint i = 0; i < _bones.size(); ++i)
            {
                FPoint p(_bones[_selected[0]]->GetBonePos());
                if (_bones[_selected[0]]->GetParent())
                {
                    _bones[_selected[0]]->GetParent()->GetMatrix().Mul(p);
                }

                if (i != _selected[0] && _bones[i]->CheckPoint(p))
                {
                    LinkBones(i, _selected[0]);
                    return;
                }
            }
        }
    }
    else if (_selected.size() == 1)
    {
        FPoint s(_bones[_selected[0]]->GetBonePos());
        FPoint e(point);

        if (_bones[_selected[0]]->GetParent() == NULL)
            _bones[_selected[0]]->SetBoneAngle(_startRotateAngle
                                        + (atan2(point.y - _bones[_selected[0]]->GetBonePos().y, point.x - _bones[_selected[0]]->GetBonePos().x)
                                            - atan2(_startMovingPos.y - _bones[_selected[0]]->GetBonePos().y, _startMovingPos.x - _bones[_selected[0]]->GetBonePos().x)));
        else
        {
            FPoint o(_bones[_selected[0]]->GetBonePos());
            FPoint sm(_startMovingPos);
            Matrix rev;
            rev.MakeRevers(_bones[_selected[0]]->GetParent()->GetMatrix());
            rev.Mul(e);
            rev.Mul(sm);

            _bones[_selected[0]]->SetBoneAngle(_startRotateAngle + (atan2(e.y - o.y, e.x - o.x)
                                                                    - atan2(sm.y - o.y, sm.x - o.x)));

        }
        if (changeLength)
        {
            _bones[_selected[0]]->SetLength((s - e).Length());
        }
        if (_meshGenerateBone >= 0)
        {
            PointList points;
            BoneAnimated *b = _bones[_selected[0]];

            FPoint p1(b->GetBonePos());
            FPoint p2 = *FPoint(b->GetLength(), 0.f).Rotate(b->GetBoneAngle()) + b->GetBonePos();
            FPoint o = *FPoint(0.f, b->GetLength() * 0.314f).Rotate(b->GetBoneAngle());

            points.push_back(p1 + o);
            points.push_back(p1 - o);
            points.push_back(p2 - o);
            points.push_back(p2 + o);

            _meshes[_meshGenerateBone]->UpdatePoints(points);
        }
    }
}

void Animation::BoneCreateTo(const FPoint &point)
{
    FPoint s(_bones[_selected[0]]->GetBonePos());
    FPoint e(point);

    if (_bones[_selected[0]]->GetParent() != NULL)
    {
        Matrix rev;
        rev.MakeRevers(_bones[_selected[0]]->GetParent()->GetMatrix());
        rev.Mul(e);
    }
    _bones[_selected[0]]->SetBoneAngle(atan2(e.y - s.y, e.x - s.x));
    _bones[_selected[0]]->SetLength((s - e).Length());

    if (_meshGenerateBone >= 0)
    {
        PointList points;
        BoneAnimated *b = _bones[_selected[0]];

        FPoint p1(b->GetBonePos());
        FPoint p2 = *FPoint(b->GetLength(), 0.f).Rotate(b->GetBoneAngle()) + b->GetBonePos();
        FPoint o = *FPoint(0.f, b->GetLength() * 0.314f).Rotate(b->GetBoneAngle());

        points.push_back(p1 + o);
        points.push_back(p1 - o);
        points.push_back(p2 - o);
        points.push_back(p2 + o);

        _meshes[_meshGenerateBone]->UpdatePoints(points);
    }
}

void Animation::IKBoneMove(const FPoint &point)
{
    if (_boneChain.size() <= 1)
    {
        BoneMoveTo(point, false);
        return;
    }

    //ik - search positions
    UpdateChain(_chainPoints, point);
//    testPoints = points;

    //update bone positiobns
    for (int i = 0; i < _boneChain.size(); ++i)
    {
        FPoint a = _boneChain[i]->GetBonePos();//points[i];
        FPoint b = _chainPoints[i + 1];

        if (_boneChain[i]->GetParent())
        {
            Matrix rev;
            rev.MakeRevers(_boneChain[i]->GetParent()->GetMatrix());

            //rev.Mul(a);
            rev.Mul(b);
        }

        _boneChain[i]->SetBoneAngle(atan2(b.y - a.y, b.x - a.x) + _deltaAngle[i]);
    }
}

void Animation::ExcludeChild()
{
    std::set<BoneAnimated*> sel;
    for (uint i = 0; i < _bones.size(); ++i)
    {
        _bones[i]->SetMoveByParent(false);
    }
    for (uint i = 0; i < _selected.size(); ++i)
    {
        sel.insert(_bones[_selected[i]]);
    }
    for (uint i = 0; i < _bones.size(); ++i)
    {
        BoneAnimated *parent = _bones[i]->GetParent();
        if (parent)
        {
            while (parent->GetParent())
            {
                parent = parent->GetParent();
            }
            _bones[i]->SetMoveByParent(sel.find(parent) != sel.end());
        }
    }
}

void Animation::Picking(int index, bool add)
{
    if (index == -1 && !add)
    {
        _selected.clear();
        return;
    }
    if (index == -1)
    {
        return;
    }
    if (!add)
    {
        for (uint i = 0; i < _selected.size(); ++i)
        {
            if (_selected[i] == index)
            {
                return;
            }
        }
        _selected.clear();
        _selected.push_back(index);
        ExcludeChild();
        return;
    }
    for (uint i = 0; i < _selected.size(); ++i)
    {
        if (_selected[i] == index)
        {
            _selected[i] = _selected.back();
            _selected.pop_back();
            return;
        }
    }
    _selected.push_back(index);
    ExcludeChild();
}

void Animation::SelectByArea(const Rect &area)
{
    _selected.clear();
    for (uint i = 0; i < _bones.size(); ++i)
    {
        if (_bones[i]->IfInside(area))
        {
            _selected.push_back(i);
        }
    }
    ExcludeChild();
}

void Animation::LinkBones(int parent, int child)
{
    if (parent >= 0 && child >= 0)
    {
        if (_bones[child]->GetParent() != _bones[parent])
        {
            _bones[child]->SetParent(_bones[parent]);
        }
        return;
    }
    if (child >= 0)
    {
        _bones[child]->SetParent(NULL);
        return;
    }
    if (parent >= 0)
    {
        assert(false);
        // todo: i do not know what to do)
        //_bones[parent]->
        return;
    }
}

void Animation::Remove()
{
    for (uint i = 0; i < _selected.size(); ++i)
    {
        delete _bones[_selected[i]];
        _bones[_selected[i]] = NULL;
    }
    for (uint i = 0; i < _bones.size(); )
    {
        if (_bones[i] == NULL)
        {
            _bones[i] = _bones.back();
            _bones.pop_back();
        }
        else
            ++i;
    }
    _selected.clear();
}

void Animation::Unlink()
{
    for (uint i = 0; i < _selected.size(); ++i)
    {
        _bones[_selected[i]]->SetParent(NULL);
    }
}

void UpdateChain(PointList &points, const FPoint &target)
{
    FPoint &b = points.back();
    int counter = 0;
    while ((b - target).Length() > 1e-1f && counter < 10)
    {
        for (int i = points.size() - 2; i >= 0; --i)
        {
            FPoint &a = points[i];

            FPoint d(b - a);
            FPoint t(target - a);

//            float sinAngle = (d.x * t.y - d.y * t.x) / (d.Length() * t.Length());
//            int n = sinAngle < 0 ? -1 : 1;
//            float angle = asin(sinAngle);
//            if (n == -1)
//            {
//                angle = M_PI - angle;
//            }
            float angle = atan2(t.y, t.x) - atan2(d.y, d.x);

            for (int j = i + 1; j < points.size(); ++j)
            {
                FPoint p(points[j] - a);
                p.Rotate(angle);
                points[j] = p + a;
            }

            if ((b - target).Length() < 1e-1f)
            {
                return;
            }
        }
        counter++;
    }
}

bool Animation::SetTexture(const char *fileName)
{
    if (_texture)
        delete _texture;
    if (_baseSprite)
        delete _baseSprite;
    _texture = new GLTexture2D(fileName);
    if (!_texture->failed())
    {
        _baseSprite = new Sprite(_texture, 0, 0, _texture->Width(), _texture->Height());
    }
    return !_texture->failed();
}

void Animation::StartBoneCreating(uint index, const FPoint &point)
{
    _boneMoving = false;
    _startMovingPos = point;
    _startRotateAngle = 0.f;
    _startMovingBone = index;
}

FPoint Animation::GetBoneEnd(uint index)
{
    FPoint r(_bones[index]->GetLength(), 0.f);
    _bones[index]->GetMatrix().Mul(r);
    return r;
}

void Animation::Finish()
{
    _meshGenerateBone = -1;
}


//void Animation::Test(FPoint p)
//{
//    UpdateChain(testPoints, p);
//}
