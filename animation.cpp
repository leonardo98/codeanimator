#include "animation.h"

Animation *Animation::_instance = NULL;

Animation::Animation()
{
    assert(_instance == NULL);
    _instance = this;
    _boneMoving = false;
}

Animation::~Animation()
{
    assert(_instance != NULL);
    _instance = NULL;
}

void Animation::Draw()
{
    // hide all bones
    for (BoneList::iterator i = _bones.begin(), e = _bones.end(); i != e; ++i)
    {
        (*i)->SetVisible(false);
    }

    // calculate new positions and visibility
    Matrix pos;
    pos.Unit();
    for (BoneList::iterator i = _root.begin(), e = _root.end(); i != e; ++i)
    {
        (*i)->CalculatePosition(pos, 0.f);
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
}

void Animation::CreateBone(FPoint pos)
{
    BoneAnimated *b = new BoneAnimated();
    b->MoveTo(pos);
    _bones.push_back(b);
    _root.push_back(b);
}

int Animation::GetBoneAtPoint(const FPoint &pos)
{
    for (uint i = 0; i < _bones.size(); ++i)
    {
        if (_bones[i]->CheckPoint(pos))
            return (int)i;
    }
    return -1;
}

void Animation::StartBoneMoving(uint index, const FPoint &point)
{
    _boneMoving = _bones[index]->MoveOrRotate(point);
    _startMovingPos = point;
    _startRotateAngle = _bones[index]->GetBoneAngle();
}

void Animation::BoneMoveTo(const FPoint &point)
{
    if (_boneMoving || _selected.size() > 1)
    {
        for (uint i = 0; i < _selected.size(); ++i)
        {
            _bones[_selected[i]]->MoveTo(point - _startMovingPos);
        }
        _startMovingPos = point;
    }
    else if (_selected.size() == 1)
        _bones[_selected[0]]->SetBoneAngle(_startRotateAngle
                                    + (atan2(point.y - _bones[_selected[0]]->GetBonePos().y, point.x - _bones[_selected[0]]->GetBonePos().x)
                                        - atan2(_startMovingPos.y - _bones[_selected[0]]->GetBonePos().y, _startMovingPos.x - _bones[_selected[0]]->GetBonePos().x)));
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
}

void Animation::SelectByArea(const Rect &area)
{
    _selected.clear();
    for (uint i = 0; i < _bones.size(); ++i)
    {
        if (_bones[i]->IfInside(area))
            _selected.push_back(i);
    }
}
