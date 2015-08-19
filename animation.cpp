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

void Animation::BoneMoveTo(uint index, const FPoint &point, bool forceMoving)
{
    if (_boneMoving || forceMoving)
    {
        _bones[index]->MoveTo(point - _startMovingPos);
        _startMovingPos = point;
    }
    else
        _bones[index]->SetBoneAngle(_startRotateAngle
                                    + (atan2(point.y - _bones[index]->GetBonePos().y, point.x - _bones[index]->GetBonePos().x)
                                        - atan2(_startMovingPos.y - _bones[index]->GetBonePos().y, _startMovingPos.x - _bones[index]->GetBonePos().x)));
}
