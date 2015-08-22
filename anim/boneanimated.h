#ifndef BONEANIMATED_H
#define BONEANIMATED_H

#include "spline.h"
#include "ogl/rect.h"

class BoneAnimated;

typedef std::vector<BoneAnimated *> BoneList;

class BoneAnimated
{
private:
    BoneAnimated *_parent;
    BoneList _children;
    bool _visible;
    Matrix _matrix;

    // for editor
    FPoint _pos;
    float _angle;
    float _length;

public:
    Spline x;
    Spline y;
    Spline angle;
    Spline scaleX;
    Spline scaleY;
public:
    const Matrix &GetMatrix() { return _matrix; }
    BoneAnimated();
    void Draw();
    void DrawSelection();
    void AddChild(BoneAnimated *);
    void RemoveChild(BoneAnimated *);
    void SetParent(BoneAnimated *b);
    BoneAnimated *GetParent() { return _parent; }
    void CalculatePosition(const Matrix &, int frame, float p = 0.f);
    void SetLength(float l) { _length = l; }
    void SetVisible(bool v) { _visible = v; }

    bool IfInside(const Rect &area);
    bool CheckPoint(FPoint pos);
    bool MoveOrRotate(FPoint pos);
    bool MoveTo(const FPoint &mt);

    void SetBonePos(const FPoint &pos) { _pos = pos; }
    const FPoint &GetBonePos() { return _pos; }
    void SetBoneAngle(float a) { _angle = a; }
    float GetBoneAngle() { return _angle; }    
};

#endif // BONEANIMATED_H
