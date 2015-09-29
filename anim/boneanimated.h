#ifndef BONEANIMATED_H
#define BONEANIMATED_H

#include "spline.h"
#include "ogl/rect.h"

class BoneAnimated;

#define BONE_LENGTH_MIN 10.f

typedef std::vector<BoneAnimated *> BoneList;

class BoneAnimated
{
private:
    BoneAnimated *_parent;
    BoneList _children;
    bool _visible;
    Matrix _parentMatrix;
    Matrix _animMatrix;
    Matrix _matrix;
    bool _moveByParent;

    Matrix _bindPointMatrix;// mul it before point mul
    FPoint _bindPos;
    float _bindAngle;

    // for editor
    FPoint _pos;
    float _angle;
    float _length;

    std::string _name;

public:
    Spline x;
    Spline y;
    Spline angle;
    Spline scaleX;
    Spline scaleY;
public:
    void GetBoneList(BoneList &bones);
    const Matrix &GetMatrix() { return _matrix; }
    BoneAnimated();
    virtual ~BoneAnimated();
    void Draw();
    void DrawRed();
    void DrawSelection();
    bool AddChild(BoneAnimated *);
    void RemoveChild(BoneAnimated *);
    void SetParent(BoneAnimated *b);
    BoneAnimated *GetParent() { return _parent; }
    BoneAnimated *GetBoneAtPoint(const FPoint &pos);
    void CalculatePosition(const Matrix &, int frame, float p = 0.f);
    void SetLength(float l) { _length = std::max(BONE_LENGTH_MIN, l); }
    float GetLength() { return _length; }
    void SetVisible(bool v) { _visible = v; }

    bool IfInside(const Rect &area);
    bool CheckPoint(FPoint pos);
    bool MoveOrRotate(FPoint pos);
    bool MoveTo(const FPoint &mt);

    void SetBonePos(const FPoint &pos);
    const FPoint &GetBonePos() { return _pos; }
    void SetBoneAngle(float a);
    float GetBoneAngle() { return _angle; }    

    void SetMoveByParent(bool b) { _moveByParent = b;}
    bool GetMoveByParent() { return _moveByParent; }

    void SetName(const std::string &name) { _name = name; }
    const std::string &GetName() { return _name; }

    void FixMatrix();

    BoneAnimated(rapidxml::xml_node<> *xe);
    void SaveToXml(rapidxml::xml_node<> *xe);

    const Matrix &GetAnimVertMatrix();

    void ResetPos();

};

#endif // BONEANIMATED_H
