#include "spline.h"
#include "extmath.h"

//BonePosition::BonePosition()
//{
//    Reset();
//}

//void BonePosition::Reset()
//{
//    pos.x = pox.y = angle = scale.x = scale.y = 0.f;
//    frame = 0;
//}

bool CmpPoints(const SplinePoint &one, const SplinePoint &two)
{
    return one.frame < two.frame;
}

void Spline::SetValue(uint i, uint frame, float value)
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

    if (_points[i].frame != frame)
    {
        _points[i].frame = frame;
        std::sort(_points.begin(), _points.end(), CmpPoints);
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

uint Spline::GetFrame(uint i)
{
    return _points[i].frame;
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

void Spline::DrawSegment(QPainter &painter, uint i, const Matrix &m)
{
    const float step = 1.f / _numDivide;
    float ys = GetValue(i, 0.f);
    for (float x = 0; x < 1.f; x += step)
    {
        float ye = GetValue(i, x + step);
        {
            float x1, y1, x2, y2;
            m.Mul(x, ys, x1, y1);
            m.Mul(x + step, ye, x2, y2);
            painter.drawLine(x1, y1, x2, y2);
        }
        ys = ye;
    }

    SplineType sp = GetSegmentType(i);

    if (sp == spline_type_square)
    {
        float x1, y1, x2, y2;

        m.Mul(0.f, GetValue(i, 0.f), x1, y1);
        m.Mul(0.5f, _pool[_points[i].indexOut], x2, y2);
        painter.drawLine(x1, y1, x2, y2);
        painter.drawEllipse(QPoint(x2, y2), 5, 5);

        m.Mul(1.f, GetValue(i, 1.f), x1, y1);
        painter.drawLine(x1, y1, x2, y2);
    }
    else if (sp == spline_type_cubic)
    {
        float x1, y1, x2, y2;

        m.Mul(0.f, GetValue(i, 0.f), x1, y1);
        m.Mul(0.33333f, _pool[_points[i].indexOut], x2, y2);
        painter.drawLine(x1, y1, x2, y2);
        painter.drawEllipse(QPoint(x2, y2), 5, 5);

        m.Mul(1.f - 0.33333f, _pool[_points[i + 1].indexIn], x1, y1);
        m.Mul(1.f, GetValue(i, 1.f), x2, y2);
        painter.drawLine(x1, y1, x2, y2);
        painter.drawEllipse(QPoint(x1, y1), 5, 5);
    }
}

void Spline::Draw(QPainter &painter, const Matrix &m)
{
    Matrix lm;
    for (uint i = 0; (i + 1) < _points.size(); ++i)
    {
        lm = m;
        lm.Move(_points[i].frame, 0.f);
        lm.Scale((_points[i + 1].frame - _points[i].frame), 1.f);

        DrawSegment(painter, i, lm);
    }

    for (uint i = 0; i < _points.size(); ++i)
    {
        lm = m;
        lm.Move(_points[i].frame, 0.f);
        lm.Scale((_points[i + 1].frame - _points[i].frame), 1.f);

        float x, y;
        lm.Mul(0, _pool[_points[i].index], x, y);
        painter.drawRect(QRectF(x - 5, y - 5, 10, 10));
    }
}

uint Spline::AddPoint(uint frame, float value)
{
    uint r = _points.size();
    _points.push_back(SplinePoint());
    SetValue(r, frame, value);
    return r;
}

void Spline::RemovePoint(uint i)
{
    UnsetIndex(_points[i].indexIn);
    UnsetIndex(_points[i].indexOut);
    UnsetIndex(_points[i].index);
    _points.erase(_points.begin() + i);
}

void Spline::Clear()
{
    _points.clear();
    _pool.clear();
}

bool Spline::Active(uint frame)
{
    return (_points.size() > 0 && _points.front().frame <= frame && frame < _points.back().frame);
}

float Spline::GetValueGlobal(uint frame, float timeSinceFrame)
{
    if (!Active(frame))
        return 0.f;

    uint start = 0;
    uint end = _points.size() - 2;
    uint middle = (start + end) / 2;

    while (!(_points[middle].frame <= frame && frame < _points[middle + 1].frame))
    {
        if (_points[middle].frame > frame) {
            end = middle;
        } else if (_points[middle + 1].frame < frame) {
            start = middle + 1;
        }
        middle = (start + end) / 2;
    }

    float segmentProgress = (frame - _points[middle].frame) / (_points[middle + 1].frame - _points[middle].frame) + timeSinceFrame;

    return GetValue(middle, segmentProgress);
}

void Spline::SetValueGlobal(uint frame, float value)
{
    if (_points.size() == 0)
    {
        SplinePoint p;

        _pool.resize(2);
        _pool[0] = _pool[1] = 0.f;

        p.frame = 0;
        p.index = 0;
        p.indexIn = -1;
        p.indexOut = -1;
        p.corner = corner_type_sharp;

        _points.push_back(p);

        p.frame = 9999;
        p.index = 1;
        _points.push_back(p);
    }
    else
        assert(_points.size() >= 2);

    uint start = 0;
    uint end = _points.size() - 2;
    uint middle = (start + end) / 2;

    while (!(_points[middle].frame <= frame && frame < _points[middle + 1].frame))
    {
        if (_points[middle].frame > frame) {
            end = middle;
        } else if (_points[middle + 1].frame < frame) {
            start = middle + 1;
        }
        middle = (start + end) / 2;
    }

    if (_points[middle].frame == frame)
    {
        SetValue(middle, frame, value);
    }
    else if (_points[middle + 1].frame == frame)
    {
        SetValue(middle + 1, frame, value);
    }
}

void SplinePoint::LoadFromXml(rapidxml::xml_node<> *xe)
{
    frame = atoi(xe->first_attribute("frame")->value());
    index = atoi(xe->first_attribute("index")->value());
    indexIn = atoi(xe->first_attribute("indexIn")->value());
    indexOut = atoi(xe->first_attribute("indexOut")->value());
    corner = (CornerType)atoi(xe->first_attribute("corner")->value());
}

void SplinePoint::SaveToXml(rapidxml::xml_node<> *xe)
{
    Math::Write(xe, "frame", frame);
    Math::Write(xe, "index", index);
    Math::Write(xe, "indexIn", indexIn);
    Math::Write(xe, "indexOut", indexOut);
    Math::Write(xe, "corner", (int)corner);
}

Spline::Spline(rapidxml::xml_node<> *xe)
    :_numDivide(10)
{
    if (xe == NULL) return;

    _numDivide = atoi(xe->first_attribute("divide")->value());

    rapidxml::xml_node<> *pool = xe->first_node("pool");
    int n = atoi(pool->first_attribute("size")->value());
    assert(n >= 0);
    _pool.resize(n);
    uint counter = 0;
    std::string array = pool->first_attribute("val")->value();
    std::string::size_type start = 0;
    std::string::size_type sigma = array.find(";");
    while (sigma + 1 < array.size())
    {
        assert(counter < _pool.size());
        _pool[counter] = atof(array.substr(start, sigma - start).c_str());
        ++counter;
        start = sigma + 1;
        sigma = array.find(";", start);
    }
    assert(counter == _pool.size());

    rapidxml::xml_node<> *points = xe->first_node("points");
    n = atoi(points->first_attribute("size")->value());
    assert(n >= 0);
    _points.resize(n);
    counter = 0;
    rapidxml::xml_node<> *p = points->first_node("p");
    while (p != NULL)
    {
        assert(counter < _points.size());
        _points[counter].LoadFromXml(p);
        p = p->next_sibling("p");
        ++counter;
    }
    assert(counter == _points.size());
}

void Spline::SaveToXml(rapidxml::xml_node<> *xe)
{
    if (_points.size() == 0) return;

    Math::Write(xe, "divide", _numDivide);

    // save pool
    rapidxml::xml_node<> *pool = xe->document()->allocate_node(rapidxml::node_element, "pool");
    xe->append_node(pool);

    std::string array;
    char buff[20];
    for (uint i = 0; i < _pool.size(); ++i)
    {
        sprintf(buff, "%g", _pool[i]);
        array += buff;
        array += ";";
    }

    Math::Write(pool, "size", (int)_pool.size());
    Math::Write(pool, "val", array.c_str());

    // save points
    rapidxml::xml_node<> *points = xe->document()->allocate_node(rapidxml::node_element, "points");
    xe->append_node(points);

    Math::Write(points, "size", (int)_points.size());
    for (uint i = 0; i < _points.size(); ++i)
    {
        rapidxml::xml_node<> *p = xe->document()->allocate_node(rapidxml::node_element, "p");
        points->append_node(p);
        _points[i].SaveToXml(p);
    }
}

void SplineMover::OnMouseDown(FPoint mousePos, float wEps, float hEps)
{
    _mousePos = mousePos;
    _activePoint = -1;
    _subPoint = 0;
    for (uint i = 0; i < _points.size(); ++i)
    {
        if (fabs(mousePos.x - _points[i].frame) < wEps
                && fabs(_pool[_points[i].index] - mousePos.y) < hEps)
        {
            _activePoint = i;
            break;
        }
        if (i + 1 < _points.size())
        {
            SplineType sp = GetSegmentType(i);
            if (sp == spline_type_square
                    && fabs(mousePos.x - 0.5f * (_points[i].frame + _points[i + 1].frame)) < wEps
                    && fabs(_pool[_points[i].indexOut] - mousePos.y) < hEps)
            {
                _activePoint = i;
                _subPoint = 1;
                break;
            }
            if (sp == spline_type_cubic)
            {
                if (fabs(mousePos.x - (_points[i].frame + 0.33333f * (_points[i + 1].frame - _points[i].frame))) < wEps
                    && fabs(_pool[_points[i].indexOut] - mousePos.y) < hEps)
                {
                    _activePoint = i;
                    _subPoint = 1;
                    break;
                }
                if (fabs(mousePos.x - (_points[i + 1].frame - 0.33333f * (_points[i + 1].frame - _points[i].frame))) < wEps
                    && fabs(_pool[_points[i + 1].indexIn] - mousePos.y) < hEps)
                {
                    _activePoint = i + 1;
                    _subPoint = -1;
                    break;
                }
            }
        }
    }
}

bool SplineMover::OnMouseMove(FPoint mousePos)
{
    if (_activePoint != -1)
    {
        float delta = mousePos.y - _mousePos.y;
        _mousePos = mousePos;
        if (_subPoint == 0)
        {
            _pool[_points[_activePoint].index] += delta;

            if (_points[_activePoint].indexIn != -1)
                _pool[_points[_activePoint].indexIn] += delta;

            if (_points[_activePoint].indexOut != -1)
                _pool[_points[_activePoint].indexOut] += delta;
        }
        else if (_subPoint == -1)
        {
            _pool[_points[_activePoint].indexIn] += delta;
        }
        else if (_subPoint == 1)
        {
            _pool[_points[_activePoint].indexOut] += delta;
        }
        else
            assert(false);

        return true;
    }
    return false;
}

void SplineMover::OnMouseUp()
{
    _activePoint = -1;
}
