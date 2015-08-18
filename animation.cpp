#include "animation.h"

Animation *Animation::_instance = NULL;

Animation::Animation()
{
    assert(_instance == NULL);
    _instance = this;
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

    _bones.push_back(b);
    _root.push_back(b);
}
