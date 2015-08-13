#include "boneposition.h"

//BonePosition::BonePosition()
//{
//    Reset();
//}

//void BonePosition::Reset()
//{
//    pos.x = pox.y = angle = scale.x = scale.y = 0.f;
//    frame = 0;
//}

void Spline::SetValue(uint i, float value)
{
    float delta = SetIndex(_points[i].index, value);

    if (_points[i].indexIn != -1)
    {
        _pool[_points[i].indexIn] += delta;
    }
    if (_points[i].indexOut != -1)
    {
        _pool[_points[i].indexOut] += delta;
    }
}

float Spline::GetValue(uint i, float p)
{
    SplineType mt = GetSegmentType(i);

    if (mt == spline_type_linear)
        return _pool[_points[i].index] * (1 - p)
                + _pool[_points[i + 1].index] * p;

    if (mt == spline_type_square)
        return _pool[_points[i].index] * (1 - p) * (1 - p)
                + _pool[_points[i].indexOut] * (1 - p) * p * 2
                + _pool[_points[i + 1].index] * p * p;

    if (mt == spline_type_cubic)
        return _pool[_points[i].index] * (1 - p) * (1 - p) * (1 - p)
                + _pool[_points[i].indexOut] * (1 - p) * (1 - p) * p * 3
                + _pool[_points[i + 1].indexIn] * (1 - p) * p * p * 3
                + _pool[_points[i + 1].index] * p * p * p;

    return 0.f;
}

void Spline::UnsetIndex(int &i)
{
    if (i == -1) return;

    int cp = i;

    if (_pool.size() == 1)
    {
        _pool.clear();
    }
    else
    {
        _pool[i] = _pool.back();
        _pool.pop_back();
    }

    int ps = _pool.size();

    for (uint p = 0; p < _points.size(); p++)
    {
        if (_points[p].index == cp)
            _points[p].index = -1;
        if (_points[p].indexIn == cp)
            _points[p].indexIn = -1;
        if (_points[p].indexOut == cp)
            _points[p].indexOut = -1;

        if (_points[p].index == ps)
            _points[p].index = cp;
        if (_points[p].indexIn == ps)
            _points[p].indexIn = cp;
        if (_points[p].indexOut == ps)
            _points[p].indexOut = cp;
    }
}

float Spline::SetIndex(int &i, float value)
{
    float delta = 0.f;
    if (i == -1)
    {
        i = _pool.size();
        _pool.push_back(value);
    }
    else
    {
        delta = value - _pool[i];
        _pool[i] = value;
    }
    return delta;
}

SplineType Spline::GetSegmentType(uint i)
{
    if (_points[i].indexOut == -1 && _points[i + 1].indexIn == -1)
    {
        return spline_type_linear;
    }
    if (_points[i].indexOut == _points[i + 1].indexIn)
    {
        return spline_type_square;
    }
    return spline_type_cubic;
}

void Spline::SetSegmentType(uint i, SplineType mt)
{
    if (mt == spline_type_linear)
    {
        UnsetIndex(_points[i].indexOut);
        UnsetIndex(_points[i + 1].indexIn);
    }
    else if (mt == spline_type_square)
    {
        UnsetIndex(_points[i].indexOut);
        UnsetIndex(_points[i + 1].indexIn);

        //SetIndex(_points[i].indexOut, 0.5f * (_pool[_points[i].index] + _pool[_points[i + 1].index]));
        SetIndex(_points[i].indexOut, _pool[_points[i].index]);
        _points[i + 1].indexIn = _points[i].indexOut;
    }
    else if (mt == spline_type_cubic)
    {
        UnsetIndex(_points[i].indexOut);
        UnsetIndex(_points[i + 1].indexIn);

        SetIndex(_points[i].indexOut, _pool[_points[i].index]);
        SetIndex(_points[i + 1].indexIn, _pool[_points[i + 1].index]);
    }
}

CornerType Spline::GetCornerType(uint i)
{
    return _points[i].corner;
}

void Spline::SetCornerType(uint i, CornerType mt)
{
    if (_points[i].corner == mt) return;

    _points[i].corner = mt;
    if (mt == corner_type_smooth)
    {
        if (i > 0 && GetSegmentType(i - 1) != spline_type_cubic)
        {
            SetSegmentType(i - 1, spline_type_cubic);
        }
        if ((i + 1) < _points.size() && GetSegmentType(i) != spline_type_cubic)
        {
            SetSegmentType(i, spline_type_cubic);
        }

        if (_points[i].indexIn != -1)
            _pool[_points[i].indexIn] = _pool[_points[i].index];

        if (_points[i].indexOut != -1)
            _pool[_points[i].indexOut] = _pool[_points[i].index];
    }
}

void Spline::DrawSegment(QPainter &painter, uint i, float start)
{
    const float step = 5.f;
    float y1 = GetValue(i, 0.f);
    for (float x = start; x < start + 99; x += step)
    {
        float y2 = GetValue(i, (x + step - start) / 100.f);
        painter.drawLine(x, y1, x + step, y2);
        y1 = y2;
    }
}

void Spline::Draw(QPainter &painter)
{
    float x = 16;
    for (uint i = 0; (i + 1) < _points.size(); ++i)
    {
        DrawSegment(painter, i, x);
        x += 100;
    }
}

uint Spline::AddPoint(float value)
{
    uint r = _points.size();
    _points.push_back(SplinePoint());
    SetValue(r, value);
    return r;
}
