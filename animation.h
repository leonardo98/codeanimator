#ifndef ANIMATION_H
#define ANIMATION_H

#include "anim/boneanimated.h"

class Animation
{
private:
    BoneList _bones;
    BoneList _root;
    static Animation *_instance;
    bool _boneMoving; // move or rotate
    FPoint _startMovingPos;
    float _startRotateAngle;

public:
    Animation();
    virtual ~Animation();
    void Draw();
    void Update(float dt);

    static Animation *Instance() { return _instance; }

    void CreateBone(FPoint pos);

    int GetBoneAtPoint(const FPoint &pos);

    void StartBoneMoving(uint index, const FPoint &point);
    void BoneMoveTo(uint index, const FPoint &mt, bool forceMoving);

};

#endif // ANIMATION_H
