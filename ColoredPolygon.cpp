#include <QApplication>
#include "ColoredPolygon.h"
//#include "../Core/Core.h"
//#include "../Core/Messager.h"
//#include "../Core/InputSystem.h"
#include "mainwindow.h"
#include "animation.h"
#include <math.h>
#include "ogl/render.h"
#include <set>

GLTexture2D * h_base = NULL;// = Core::getTexture(":main/gfx/red.png");

bool CmpBoneDistance(const std::pair<BoneAnimated *, float> &one, const std::pair<BoneAnimated *, float> &two)
{
    return one.second < two.second;
}

void ColoredPolygon::InitCorners()
{
    if (h_base == NULL)
        h_base = new GLTexture2D(":main/red.png");
    scale = new Sprite(h_base, 4, 13, 12, 12);
    scaleSide = new Sprite(h_base, 4, 1, 10, 10);
}

ColoredPolygon::~ColoredPolygon()
{
    delete scale;
    scale = NULL;
    delete scaleSide;
    scaleSide = NULL;
}

ColoredPolygon::ColoredPolygon(const ColoredPolygon &c) 
{
	_dots = c._dots;
	CalcWidthAndHeight();
	GenerateTriangles();
	_mouseDown = false;
    _dotUnderCursor.clear();
    _selectedDots.clear();
    _debugDraw = false;
    InitCorners();
    _color = 0xFFFFFFFF;
}

ColoredPolygon::ColoredPolygon(rapidxml::xml_node<> *xe)
{
    _boneName = xe->first_attribute("bone")->value();

    rapidxml::xml_attribute<> *size = xe->first_attribute("size");
    if (size)
    {
        _dots.resize(atoi(size->value()));
        std::string array = xe->first_attribute("dots")->value();
        std::string::size_type start = 0;
        std::string::size_type sigma = array.find(";");
        for (unsigned int j = 0; j < _dots.size(); ++j)
        {
            sscanf(array.substr(start, sigma - start).c_str(), "%g %g", &_dots[j].pos.x, &_dots[j].pos.y);
            start = sigma + 1;
            sigma = array.find(";", start);
        }
    }
    else
    {
        rapidxml::xml_node<> *mesh = xe->first_node("mesh");
        if (mesh)
        {
            _dots.resize(atoi(mesh->first_attribute("vert")->value()));
            unsigned int count = 0;
            for (rapidxml::xml_node<> *i = mesh->first_node(); i; i = i->next_sibling())
            {
                sscanf(i->first_attribute("geom")->value(), "%g;%g", &_dots[count].pos.x, &_dots[count].pos.y);
                count++;
            }
        }
    }

    CalcWidthAndHeight();
    GenerateTriangles();
	_mouseDown = false;
    _dotUnderCursor.clear();
    _selectedDots.clear();
    _debugDraw = false;
    InitCorners();
    _color = 0xFFFFFFFF;
}

void ColoredPolygon::UpdatePoints(const PointList &points)
{
    _dots.resize(points.size());
    for (uint i = 0; i < points.size(); ++i)
    {
        _dots[i].pos = points[i];
    }
    CalcWidthAndHeight();
    GenerateTriangles();
    _dotUnderCursor.clear();
    _selectedDots.clear();
    InitCorners();
}

ColoredPolygon::ColoredPolygon(const PointList &points)
{
    _dots.resize(points.size());
    for (uint i = 0; i < points.size(); ++i)
    {
        _dots[i].pos = points[i];
    }
    CalcWidthAndHeight();
    GenerateTriangles();
    _mouseDown = false;
    _dotUnderCursor.clear();
    _selectedDots.clear();
    _debugDraw = false;
    InitCorners();
    _color = 0xFFFFFFFF;
}

void ColoredPolygon::Draw()
{
    _lastDrawMatrix = Render::GetCurrentMatrix();

	DrawTriangles();

    _screenDots.resize(_dots.size());
	for (unsigned int i = 0; i < _dots.size(); ++i) {
        _screenDots[i] = _dots[i].pos;
		Render::GetCurrentMatrix().Mul(_screenDots[i]);	
	}
	if (_triangles.GetVB().Size() == 0) 
	{
		for (unsigned int i = 0; i < _dots.size(); ++i) {
            Render::Line(_dots[i].pos.x, _dots[i].pos.y, _dots[(i + 1) % _dots.size()].pos.x, _dots[(i + 1) % _dots.size()].pos.y, 0xFFFFFFFF);
		}
	}
}

void ColoredPolygon::DebugDraw(bool onlyControl)
{
    if (!onlyControl)
    {
		Draw();
	}

    _debugDraw = true;

    Render::SetFiltering(false);
    Render::PushMatrix();

    for (unsigned int i = 0; i < _triangles.GetVB().SizeIndex(); i += 3)
    {
        Render::Line(_triangles.GetVB().VertXY(_triangles.GetVB().Index(i + 0)).x, _triangles.GetVB().VertXY(_triangles.GetVB().Index(i + 0)).y
                     , _triangles.GetVB().VertXY(_triangles.GetVB().Index(i + 1)).x, _triangles.GetVB().VertXY(_triangles.GetVB().Index(i + 1)).y, 0x4FFFFFFF);
        Render::Line(_triangles.GetVB().VertXY(_triangles.GetVB().Index(i + 2)).x, _triangles.GetVB().VertXY(_triangles.GetVB().Index(i + 2)).y
                     , _triangles.GetVB().VertXY(_triangles.GetVB().Index(i + 1)).x, _triangles.GetVB().VertXY(_triangles.GetVB().Index(i + 1)).y, 0x4FFFFFFF);
        Render::Line(_triangles.GetVB().VertXY(_triangles.GetVB().Index(i + 0)).x, _triangles.GetVB().VertXY(_triangles.GetVB().Index(i + 0)).y
                     , _triangles.GetVB().VertXY(_triangles.GetVB().Index(i + 2)).x, _triangles.GetVB().VertXY(_triangles.GetVB().Index(i + 2)).y, 0x4FFFFFFF);
    }

    //parent = Render::GetCurrentMatrix();
    for (unsigned int i = 0; i < _dots.size(); ++i)
    {
        Render::Line(_dots[i].pos.x, _dots[i].pos.y, _dots[(i + 1) % _dots.size()].pos.x, _dots[(i + 1) % _dots.size()].pos.y, 0xFFFFFFFF);
    }

    Render::SetMatrixUnit();

    float alpha = (Render::GetColor() >> 24) / 255.f;
    Render::SetAlpha(Math::round(0xAF * alpha));
    std::set<int> drawBig;
    for (unsigned int i = 0; i < _dotUnderCursor.size(); ++i)
    {
        drawBig.insert(_dotUnderCursor[i]);
    }
    for (unsigned int i = 0; i < _selectedDots.size(); ++i)
    {
        drawBig.insert(_selectedDots[i]);
    }

    for (unsigned int i = 0; i < _screenDots.size(); ++i) {
        if (drawBig.end() != drawBig.find(i)) {
            scale->Render(_screenDots[i].x - scale->Width() / 2.f, _screenDots[i].y - scale->Height() / 2.f);
        } else {
            scaleSide->Render(_screenDots[i].x - scaleSide->Width() / 2.f, _screenDots[i].y - scaleSide->Height() / 2.f);
        }
    }
    Render::SetAlpha(Math::round(0xFF * alpha));

    Render::PopMatrix();
//		Render::SetFiltering(TileEditorInterface::Instance()->FilteringTexture());


    if (!Animation::Instance()->GetDebugBone())
        return;

    BoneList bones;
    bones.push_back(Animation::Instance()->GetDebugBone());
    Animation::Instance()->GetDebugBone()->GetBoneList(bones);

    float signSq = Math::SignedSquare(_dots);

    for (unsigned int j = 0; j < _selectedDots.size(); ++j)
    {
        uint i = _selectedDots[j];

        FPoint &a = _dots[(i + _dots.size() - 1) % _dots.size()].pos;
        FPoint &b = _dots[i].pos;
        FPoint &c = _dots[(i + 1) % _dots.size()].pos;

        std::vector<std::pair<BoneAnimated *, float> > boneDistance;
        for (uint j = 0; j < bones.size(); ++j)
        {
            BoneAnimated *d = bones[j];

            FPoint s(0, 0);
            FPoint e(d->GetLength(), 0);
            d->GetMatrix().Mul(s);
            d->GetMatrix().Mul(e);
            FPoint center = 0.5f * (s + e);
            if ((s - b).Length() < d->GetLength() / 2
                    || (e - b).Length() < d->GetLength() / 2
                    || Math::Distance(s, e, b) < d->GetLength() / 2
                    )
            {
                if (Math::VMul(a - center, b - center) * signSq >= 0.f || Math::VMul(b - center, c - center) * signSq >= 0.f)
                {
                     boneDistance.push_back(std::make_pair<BoneAnimated *, float>(d, (b - center).Length()));
                }
            }
        }
        std::sort(boneDistance.begin(), boneDistance.end(), CmpBoneDistance);
        for (uint i = 0; i < std::min((size_t)2, boneDistance.size()); ++i)
        {
            boneDistance[i].first->DrawRed();
        }
    }

}

bool ColoredPolygon::PixelCheck(const FPoint &point) { 
    if (Math::Inside(point, _dots) || CheckLines(point)) {
		return true;
	}
    return SearchNearest(point.x, point.y) >= 0;
}

void ColoredPolygon::SaveToXml(rapidxml::xml_node<> *xe)
{
    Math::Write(xe, "bone", _boneName.c_str());

	if (_triangles.GetVB().Size())
	{
        rapidxml::xml_node<> *mesh = xe->document()->allocate_node(rapidxml::node_element, "mesh");
        xe->append_node(mesh);
        Math::Write(mesh, "vert", (int)_triangles.GetVB().Size());
        Math::Write(mesh, "ind", (int)_triangles.GetVB().SizeIndex());
		for (unsigned int i = 0; i < _triangles.GetVB().Size(); ++i)
		{
            rapidxml::xml_node<> *vertex = xe->document()->allocate_node(rapidxml::node_element, "vert");
            mesh->append_node(vertex);
            char buff[1000];
            sprintf(buff, "%0.0f;%0.0f;%g;%g", _triangles.GetVB().VertXY(i).x, _triangles.GetVB().VertXY(i).y
                                        , _triangles.GetVB().VertUV(i).x, _triangles.GetVB().VertUV(i).y);
            Math::Write(vertex, "geom", buff);
		}
        rapidxml::xml_node<> *indexes = xe->document()->allocate_node(rapidxml::node_element, "indexes");        
        xe->append_node(indexes);

        std::string array;
        for (unsigned int i = 0; i < _triangles.GetVB().SizeIndex(); ++i)
		{
            array += Math::IntToStr((int)_triangles.GetVB().Index(i));
            array += ";";
		}
        Math::Write(indexes, "array", array.c_str());
	}
    else
    {
        Math::Write(xe, "size", (int)_dots.size());
        std::string array;
        for (unsigned int j = 0; j < _dots.size(); ++j)
        {
            char buff[100];
            sprintf(buff, "%g %g;", _dots[j].pos.x, _dots[j].pos.y);
            array += buff;
        }
        Math::Write(xe, "dots", array.c_str());
     }
}

int ColoredPolygon::Width() {
	return _width;
}

int ColoredPolygon::Height() {
	return _height;
}

void ColoredPolygon::CalcWidthAndHeight() {
	Rect rect;
	rect.Clear();
	for (unsigned int i = 0; i < _dots.size(); ++i) {
        rect.Encapsulate(_dots[i].pos.x, _dots[i].pos.y);
	}
	_width = fabs(rect.x2 - rect.x1);
	_height = fabs(rect.y2 - rect.y1);
}

void ColoredPolygon::GenerateTriangles()
{
    Matrix m;
    m.Unit();
    if (Animation::Instance()->GetTexture())
    {
        m.Move(-0.5f, -0.5f);
        m.Scale(1.f / Animation::Instance()->GetTexture()->Width(), 1.f / Animation::Instance()->GetTexture()->Height());
    }
    Math::GenerateTriangles(_dots, _triangles, _color, Animation::Instance()->GetTexture(), &m);
}

void ColoredPolygon::DrawTriangles() {
    Render::PushColorAndMul(_color);
	_triangles.Render();
	Render::PopColor();
}

void ColoredPolygon::DrawBinded()
{
    _trianglesBinded = _triangles;
    Matrix tmp;
    for (uint i = 0; i < _trianglesBinded.GetVB().Size(); ++i)
    {
        bool needMul = false;
        tmp.Zero();
        if (_dots[i].p[0].bone)
        {
            tmp.Add(_dots[i].p[0].bone->GetAnimVertMatrix(), _dots[i].p[0].mass);
            needMul = true;
        }
        if (_dots[i].p[1].bone)
        {
            tmp.Add(_dots[i].p[1].bone->GetAnimVertMatrix(), _dots[i].p[1].mass);
        }
        if (needMul)
            tmp.Mul(_trianglesBinded.GetVB().VertXY(i));
    }

    Render::PushColorAndMul(_color);
    _trianglesBinded.Render();
    Render::PopColor();
}

int ColoredPolygon::SearchNearest(float x, float y)
{
    FPoint dist;
    _lastDrawMatrix.GetScale(dist.x, dist.y);
    float SIZEX = 20 / dist.x;

    FPoint p(x, y);
    for (unsigned int i = 0; i < _dots.size(); ++i)
    {
        if ((_dots[i].pos - p).Length() < SIZEX)
        {
            return i;
        }
    }
    return -1;
}

bool ColoredPolygon::CheckLines(const FPoint &p)
{
    for (unsigned int i = 0; i < _dots.size(); ++i)
    {
        if (Math::DotNearLine(_dots[i].pos, _dots[(i + 1) % _dots.size()].pos, p))
        {
            return true;
        }
    }
    return false;
}

int ColoredPolygon::CreateDot(float x, float y) {
	if (_dots.size() >= 50) {
		return false;
	}
	int result = -1;
    OnePoint point;
	FPoint p(x, y);
	for (unsigned int i = 0; i < _dots.size() && result < 0; ++i) {
        if (Math::DotNearLine(_dots[i].pos, _dots[(i + 1) % _dots.size()].pos, p)) {
			unsigned int index = (i + 1) % _dots.size();
            point.pos = p;
			if (index < _dots.size()) {
                _dots.insert(_dots.begin() + index, point);
				result = index;
			} else {
                _dots.push_back(point);
				result = _dots.size() - 1;
			}
		}
	}
	if (result >= 0) {
		GenerateTriangles();
	}
	return result;
}

void ColoredPolygon::RemoveDot(QVector<int> index) {
	if (_dots.size() <= 3) {
		return;
	}
    std::vector<OnePoint> tmp;
    for (int i = 0; i < _dots.size(); ++i)
    {
        int j = 0;
        for (; j < index.size() && i != index[j]; ++j);
        if (j >= index.size())
        {
            tmp.push_back(_dots[i]);
        }
    }
    _dots = tmp;
}

void ColoredPolygon::MouseDown(const FPoint &mouse) {
    _selectedDots.clear();

	if (!_debugDraw) {
		return;
	}
    if (!MainWindow::Instance()->CreateDotMode()) {
        _selectedDots = _dotUnderCursor;
	}

    int index = SearchNearest(mouse.x, mouse.y);
    if (index == -1)
    {
        int result = CreateDot(mouse.x, mouse.y);
        if (result >= 0)
        {
            _dotUnderCursor = _selectedDots = QVector<int>(1, result);
        }
        else
        {
            _dotUnderCursor.clear();
            _selectedDots.clear();
        }
    }
    else
    {
        _selectedDots.push_back(index);
        _dotUnderCursor = _selectedDots;
    }

	_mouseDown = true;
	_mousePos = mouse;
}

bool ColoredPolygon::MouseMove(const FPoint &mousePos) {
    if (!MainWindow::Instance()->CreateDotMode() && _mouseDown)
    {
        return true;
	}

	if (!_debugDraw || !_mouseDown) {
        int r = SearchNearest(mousePos.x, mousePos.y);
        if (r == -1)
        {
            _dotUnderCursor.clear();
        }
        else
        {
            _dotUnderCursor = QVector<int>(1, r);
        }
		_mousePos = mousePos;
        return false;
	}

    FPoint start(_mousePos);
	FPoint end(mousePos);

    if (_dotUnderCursor.size() == 0) {
//		_pos.x += (end.x - start.x);
//		_pos.y += (end.y - start.y);
	} else {
        for (unsigned int i = 0; i < _dotUnderCursor.size(); ++i)
        {
            _dots[_dotUnderCursor[i]].pos.x += (end.x - start.x);
            _dots[_dotUnderCursor[i]].pos.y += (end.y - start.y);
        }
		CalcWidthAndHeight();
        GenerateTriangles();
    }
	_mousePos = mousePos;
    return true;
}

void ColoredPolygon::MouseUp(const FPoint &mouse) {
	_mouseDown = false;
    _dotUnderCursor.clear();
	GenerateTriangles();
}

const char *ColoredPolygon::GetIconTexture() {
	return NULL;
}

bool ColoredPolygon::CanCut(const std::string &message, const std::string &substr, std::string &result)
{
    unsigned int rr = message.find(substr);
    if (rr != 0) {
        return false;
    } else {
        result = message.substr(substr.size());
        return true;
    }
}

bool ColoredPolygon::RemoveDots()
{
    if (_selectedDots.size() > 0)
    {
        RemoveDot(_selectedDots);
        _selectedDots.clear();
        _dotUnderCursor.clear();
        GenerateTriangles();
        return true;
    }
    return false;
}

bool ColoredPolygon::GeometryCheck(const FPoint &point) {
    FPoint p(point);
	return Math::Inside(p, _dots); 
}

bool ColoredPolygon::Selection(const Rect& rect, bool full)
{
    _selectedDots.clear();
    for (unsigned int i = 0; i < _screenDots.size(); ++i) {
        if (rect.TestPoint(_screenDots[i].x, _screenDots[i].y)) {
            _selectedDots.push_back(i);
        }
    }
    return !full || _selectedDots.size() == _screenDots.size();
}

void ColoredPolygon::BindToBone(BoneAnimated *bone)
{    
    BoneList bones;
    bones.push_back(bone);
    bone->GetBoneList(bones);
    for (uint j = 0; j < bones.size(); ++j)
    {
        bones[j]->FixMatrix();
    }

    float signSq = Math::SignedSquare(_dots);

    std::vector<uint> skipped;

    for (uint i = 0; i < _dots.size(); ++i)
    {
        FPoint &a = _dots[(i + _dots.size() - 1) % _dots.size()].pos;
        FPoint &b = _dots[i].pos;
        FPoint &c = _dots[(i + 1) % _dots.size()].pos;

        std::vector<std::pair<BoneAnimated *, float> > boneDistance;
        for (uint j = 0; j < bones.size(); ++j)
        {
            BoneAnimated *d = bones[j];

            FPoint s(0, 0);
            FPoint e(d->GetLength(), 0);
            d->GetMatrix().Mul(s);
            d->GetMatrix().Mul(e);
            FPoint center = 0.5f * (s + e);

            if (Math::VMul(a - center, b - center) * signSq >= 0.f || Math::VMul(b - center, c - center) * signSq >= 0.f)
            {
                if ((d->GetParent() == NULL && (s - b).Length() < d->GetLength() / 2)
                        || (!d->HasChild() && (e - b).Length() < d->GetLength() / 2))
                {
                    boneDistance.push_back(
                                std::make_pair<BoneAnimated *, float>
                                (d, 0.f)
                                );
                }
                else
                {
                    boneDistance.push_back(std::make_pair<BoneAnimated *, float>(d, Math::Distance(s, e, b) ) );
                }
            }
        }

        if (boneDistance.size() == 1)
        {
            _dots[i].p[0].boneName = boneDistance.back().first->GetName();
            _dots[i].p[0].bone = boneDistance.back().first;
            _dots[i].p[0].mass = 1.f;

            _dots[i].p[1].boneName = "";
            _dots[i].p[1].bone = NULL;
            _dots[i].p[1].mass = 0.f;
        }
        else if (boneDistance.size() >= 2)
        {
            std::sort(boneDistance.begin(), boneDistance.end(), CmpBoneDistance);
            float m = boneDistance[0].second + boneDistance[1].second;

            _dots[i].p[0].boneName = boneDistance[0].first->GetName();
            _dots[i].p[0].bone = boneDistance[0].first;
            _dots[i].p[0].mass = boneDistance[1].second / m;

            _dots[i].p[1].boneName = boneDistance[1].first->GetName();
            _dots[i].p[1].bone = boneDistance[1].first;
            _dots[i].p[1].mass = boneDistance[0].second / m;
        }
        else
        {
            skipped.push_back(i);
        }
    }

    if (skipped.size())
    {
        assert(false);// fail
    }
}

void ColoredPolygon::ReplaceBonesWith(BoneList &bones)
{
    std::map<std::string, BoneAnimated *> byNames;
    for (const auto i : bones)
    {
        byNames[i->GetName()] = i;
    }

    for (uint i = 0; i < _dots.size(); ++i)
    {
        for (int j = 0; j < 2; ++j)
        {
            if (!_dots[i].p[0].boneName.empty())
            {
                assert(byNames.find(_dots[i].p[0].bone->GetName()) != byNames.end());
                _dots[i].p[0].bone = byNames[_dots[i].p[0].bone->GetName()];
            }
        }
    }
}
