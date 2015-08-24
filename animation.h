#ifndef ANIMATION_H
#define ANIMATION_H

#include "anim/boneanimated.h"

class Animation
{
private:
    BoneList _bones;
    static Animation *_instance;
    bool _boneMoving; // move or rotate
    FPoint _startMovingPos;
    float _startRotateAngle;
    std::vector<uint> _selected;

public:
    Animation();
    virtual ~Animation();
    void Draw();
    void Update(float dt);

    static Animation *Instance() { return _instance; }

    void CreateBone(FPoint pos);

    int GetBoneAtPoint(const FPoint &pos);

    void Picking(int index, bool add);

    void StartBoneMoving(uint index, const FPoint &point);
    void BoneMoveTo(const FPoint &mt, bool changeLength);

    void SelectByArea(const Rect &area);

    void LinkBones(int parent, int child);

    void Remove();
    void Unlink();

    std::string GenerateUnicBoneName();

};

#endif // ANIMATION_H
