#ifndef BONEPOSITION_H
#define BONEPOSITION_H

#include <vector>
#include "../ogl/fpoint.h"
#include <QPainter>
#include "ogl/matrix.h"

typedef unsigned int uint;

enum SplineType
{
    spline_type_linear,
    spline_type_square,
    spline_type_cubic,
};

enum CornerType
{
    corner_type_sharp,
    corner_type_smooth,
};

struct SplinePoint
{
    int index;
    int indexIn;
    int indexOut;
    CornerType corner;
    SplinePoint()
        : index(-1), indexIn(-1), indexOut(-1), corner(corner_type_sharp)
    {}
};

class Spline
{
private:
    std::vector<float> _pool;
    std::vector<SplinePoint> _points;
    uint _numDivide;
public:
    Spline() : _numDivide(10) {}

    void SetValue(uint i, float value);
    float GetValue(uint i, float p = 0.f);

    void SetDivide(int div);
    uint GetDivide() { return _numDivide; }

    float SetIndex(int &i, float value);
    void UnsetIndex(int &i);

    SplineType GetSegmentType(uint i);
    void SetSegmentType(uint i, SplineType mt);

    CornerType GetCornerType(uint i);
    void SetCornerType(uint i, CornerType mt);

    void DrawSegment(QPainter &painter, uint i, const Matrix &m);
    void Draw(QPainter &painter, const Matrix &m);

    uint AddPoint(float value);
};

typedef float ValueType;

//struct Interval
//{
//private:
//    SplineType _type;
//    std::vector<ValueType> _data;
//public:
//    Interval()
//        : type (spline_type_linear)
//    {}
//    SplineType GetType() { return _type; }
//    void InitLine() {
//        _type = spline_type_linear;
//        _data.clear();
//    }
//    void InitCubic(ValueType a, b) {
//        _type = spline_type_square;
//        _data.resize(1);
//        _data[0]= (a + b) / 2.f;
//    }
//    void InitBicubic(ValueType a, b) {
//        _type = spline_type_cubic;
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
//        if (GetType() == spline_type_linear)
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
