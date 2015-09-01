#ifndef ANIMATION_H
#define ANIMATION_H

#include "anim/boneanimated.h"
#include "ogl/textures.h"
#include "ogl/sprite.h"

typedef std::vector<FPoint> PointList;

class Animation
{
private:
    BoneList _bones;
    static Animation *_instance;
    bool _boneMoving; // move or rotate
    int _startMovingBone;
    FPoint _startMovingPos;
    float _startRotateAngle;
    std::vector<uint> _selected;

    void ExcludeChild();

//    PointList testPoints;
    std::vector<float> _deltaAngle;
    std::vector<BoneAnimated *> _boneChain;
    PointList _chainPoints;

    GLTexture2D *_texture;
    Sprite *_baseSprite;

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
    void IKBoneMove(const FPoint &mt);

    void SelectByArea(const Rect &area);

    void LinkBones(int parent, int child);

    void Remove();
    void Unlink();    

    std::string GenerateUnicBoneName();
    bool SetTexture(const char *fileName);
    GLTexture2D *GetTexture() { return _texture; }

//    void Test(FPoint p);

};

void UpdateChain(PointList &points, const FPoint &target);

#endif // ANIMATION_H
