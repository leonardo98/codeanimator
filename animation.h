#ifndef ANIMATION_H
#define ANIMATION_H

#include "anim/boneanimated.h"

class Animation
{
private:
    BoneList _bones;
    BoneList _root;
    static Animation *_instance;

public:
    Animation();
    virtual ~Animation();
    void Draw();
    void Update(float dt);

    static Animation *Instance() { return _instance; }

    void CreateBone(FPoint pos);

};

#endif // ANIMATION_H
