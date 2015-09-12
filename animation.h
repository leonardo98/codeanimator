#ifndef ANIMATION_H
#define ANIMATION_H

#include <QObject>

#include "anim/boneanimated.h"
#include "ogl/textures.h"
#include "ogl/sprite.h"
#include "ColoredPolygon.h"

class Animation : public QObject
{
    Q_OBJECT
public slots:
    void Remove();
    void Unlink();

private:
    std::vector<ColoredPolygon *> _meshes;
    int _meshGenerateBone;
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
    std::string _fileName;

public:
    Animation();
    virtual ~Animation();
    void Draw();
    void Update(float dt);

    static Animation *Instance() { return _instance; }

    uint CreateBone(FPoint pos, bool generateMesh);
    void StartBoneCreating(uint index, const FPoint &point);

    int GetBoneAtPoint(const FPoint &pos);

    void Picking(int index, bool add);

    void StartBoneMoving(uint index, const FPoint &point);
    void BoneCreateTo(const FPoint &point);
    void BoneMoveTo(const FPoint &mt, bool changeLength);
    void IKBoneMove(const FPoint &mt);

    void SelectByArea(const Rect &area);

    void LinkBones(int parent, int child);

    std::string GenerateUnicBoneName();
    bool SetTexture(const char *fileName);
    GLTexture2D *GetTexture() { return _texture; }
    FPoint GetBoneEnd(uint index);

    void Finish();

    void PolygonMouseDown(const FPoint &pos);
    void PolygonMouseMove(const FPoint &pos);
    void PolygonMouseUp(const FPoint &pos);
    bool PolygonActive(const FPoint &pos);

    void RemoveBones();

    void LoadFromFile(const std::string &fileName);
    void SaveToFile(const std::string &fileName);

};

void UpdateChain(PointList &points, const FPoint &target);

#endif // ANIMATION_H
