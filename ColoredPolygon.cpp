#include <QApplication>
#include "ColoredPolygon.h"
#include "extmath.h"
//#include "../Core/Core.h"
//#include "../Core/Messager.h"
//#include "../Core/InputSystem.h"
#include "mainwindow.h"
#include "animation.h"
#include <math.h>
#include "ogl/render.h"
#include <set>

GLTexture2D * h_base = NULL;// = Core::getTexture(":main/gfx/red.png");

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

    _dots.resize(atoi(xe->first_attribute("size")->value()));
    std::string array = xe->first_attribute("dots")->value();
    std::string::size_type start = 0;
    std::string::size_type sigma = array.find(";");
    for (unsigned int j = 0; j < _dots.size(); ++j)
    {
        sscanf(array.substr(start, sigma - start).c_str(), "%g %g", &_dots[j].x, &_dots[j].y);
        start = sigma + 1;
        sigma = array.find(";", start);
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
        _dots[i] = points[i];
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
        _dots[i] = points[i];
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
	DrawTriangles();

	_screenDots = _dots;
	for (unsigned int i = 0; i < _dots.size(); ++i) {
		Render::GetCurrentMatrix().Mul(_screenDots[i]);	
	}
	if (_triangles.GetVB().Size() == 0) 
	{
		for (unsigned int i = 0; i < _dots.size(); ++i) {
			Render::Line(_dots[i].x, _dots[i].y, _dots[(i + 1) % _dots.size()].x, _dots[(i + 1) % _dots.size()].y, 0xFFFFFFFF);
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
        Render::Line(_dots[i].x, _dots[i].y, _dots[(i + 1) % _dots.size()].x, _dots[(i + 1) % _dots.size()].y, 0xFFFFFFFF);
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
    Math::Write(xe, "size", _dots.size());
    std::string array;
    for (unsigned int j = 0; j < _dots.size(); ++j)
    {
        char buff[100];
        sprintf(buff, "%g %g;", _dots[j].x, _dots[j].y);
        array += buff;
	}
    Math::Write(xe, "dots", array.c_str());

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
            sprintf(buff, "%g;%g;%g;%g", _triangles.GetVB().VertXY(i).x, _triangles.GetVB().VertXY(i).y
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
		rect.Encapsulate(_dots[i].x, _dots[i].y);
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

int ColoredPolygon::SearchNearest(float x, float y)
{
    static const float SIZEX = 20;
    FPoint p(x, y);
    for (unsigned int i = 0; i < _dots.size(); ++i)
    {
        if ((_dots[i] - p).Length() < SIZEX)
        {
            return i;
        }
    }
    return -1;
}

void ColoredPolygon::EncapsulateAllDots(Rect &rect) {
	for (unsigned int i = 0; i < _screenDots.size(); ++i) {
		//rect.Encapsulate(_screenDots[i].x, _screenDots[i].y);
        rect.Encapsulate(_dots[i].x, _dots[i].y);
	}
}

void ColoredPolygon::GetAllLocalDotsRect(Rect &rect) {
	for (unsigned int i = 0; i < _dots.size(); ++i) {
        rect.Encapsulate(_dots[i].x, _dots[i].y);
	}
}

bool ColoredPolygon::CheckLines(const FPoint &p)
{
    for (unsigned int i = 0; i < _dots.size(); ++i)
    {
        if (Math::DotNearLine(_dots[i], _dots[(i + 1) % _dots.size()], p))
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
	FPoint p(x, y);
	for (unsigned int i = 0; i < _dots.size() && result < 0; ++i) {
		if (Math::DotNearLine(_dots[i], _dots[(i + 1) % _dots.size()], p)) {
			unsigned int index = (i + 1) % _dots.size();
			if (index < _dots.size()) {
				_dots.insert(_dots.begin() + index, p);
				result = index;
			} else {
				_dots.push_back(p);
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
    QVector<FPoint> tmp;
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
    if (!MainWindow::Instance()->CreateDotMode() && _mouseDown) {
//		Matrix reverse;
//		reverse.MakeRevers(parent);

//		FPoint start(_mousePos);
//		FPoint end(mousePos);
//		reverse.Mul(start);
//		reverse.Mul(end);

//		_pos.x += (end.x - start.x);
//		_pos.y += (end.y - start.y);
//		_mousePos = mousePos;
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
            _dots[_dotUnderCursor[i]].x += (end.x - start.x);
            _dots[_dotUnderCursor[i]].y += (end.y - start.y);
        }
		CalcWidthAndHeight();
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
