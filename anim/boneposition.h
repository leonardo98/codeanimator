#ifndef BONEPOSITION_H
#define BONEPOSITION_H

#include <vector>
#include "../ogl/fpoint.h"

typedef unsigned int uint;

enum MoveType
{
    move_type_linear,
    move_type_square,
    move_type_cubic,
};

struct SplinePoint
{
    int index;
    int indexIn;
    int indexOut;
    SplinePoint()
        : index(-1), indexIn(-1), indexOut(-1)
    {}
};

class Spline
{
private:
    std::vector<float> _pool;
    std::vector<SplinePoint> _points;
public:
    void SetValue(uint i, float value);
    float GetValue(uint i, float p = 0.f);

    float SetIndex(int &i, float value);
    void UnsetIndex(int &i);

    MoveType GetSegmentType(uint i);
    void SetSegmentType(uint i, MoveType mt);

    void DrawSegment(uint i);

    uint AddPoint(float value);
};

typedef float ValueType;

//struct Interval
//{
//private:
//    MoveType _type;
//    std::vector<ValueType> _data;
//public:
//    Interval()
//        : type (move_type_linear)
//    {}
//    MoveType GetType() { return _type; }
//    void InitLine() {
//        _type = move_type_linear;
//        _data.clear();
//    }
//    void InitCubic(ValueType a, b) {
//        _type = move_type_square;
//        _data.resize(1);
//        _data[0]= (a + b) / 2.f;
//    }
//    void InitBicubic(ValueType a, b) {
//        _type = move_type_cubic;
//        _data.resize(2);
//        _data[0]= a;
//        _data[1] = b;
//    }

//    // editor
//    void Draw();
//    void MouseMove(FPoint mousePos);
//    void MouseDown(FPoint mousePos);
//    void MouseUp();

//    ValueType GetValue(float p)
//    {
//        if (GetType() == move_type_linear)
//        {
//            return
//        }
//    }
//};

//typedef std::vector<std::pair<ValueType, Interval> > KeyArray;

//struct BonePosition
//{
//private:
//public:
//    void Reset();
//    BonePosition();
//};

#endif // BONEPOSITION_H
