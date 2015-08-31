#ifndef COLRED_POLYGON_H
#define COLRED_POLYGON_H

#include "ogl/sprite.h"
#include "ogl/rect.h"

class ColoredPolygon
{
private:

    FPoint _mousePos;
    DWORD _color;

    Sprite *scaleSide;
    Sprite *scale;

    bool _debugDraw;
    int _width;
	int _height;

    void CalcWidthAndHeight();

	QVector<FPoint> _dots; //   
	QVector<FPoint> _screenDots;
	Sprite _triangles;
	bool _mouseDown;
    QVector<int> _dotUnderCursor;
    QVector<int> _selectedDots;

	void GenerateTriangles();
	VertexBuffer tri;
	void FillTriangle(const FPoint &a, const FPoint &b, const FPoint &c, VertexBuffer &tri);
	void DrawTriangles();

    int SearchNearest(float x, float y);

	int CreateDot(float x, float y);
    void RemoveDot(QVector<int> index);

    void InitCorners();
    bool CanCut(const std::string &message, const std::string &substr, std::string &result);

public:

	virtual ~ColoredPolygon();
	ColoredPolygon(rapidxml::xml_node<> *xe);
	ColoredPolygon(const ColoredPolygon &c);

	virtual void Draw();
	virtual void DebugDraw(bool onlyControl);
	virtual void Update(float dt) {}
	virtual void MouseDown(const FPoint &mouse);
    virtual bool MouseMove(const FPoint &mouse);
	virtual void MouseUp(const FPoint &mouse);
	virtual bool PixelCheck(const FPoint &point);
	virtual bool GeometryCheck(const FPoint &point);
	virtual bool Command(const std::string &cmd);
	virtual void EncapsulateAllDots(Rect &rect);
	virtual void GetAllLocalDotsRect(Rect &rect);
    virtual bool Selection(const Rect& rect, bool full);
	virtual void SaveToXml(rapidxml::xml_node<> *xe);

	virtual int Width();
	virtual int Height();

	virtual const char *GetIconTexture();
};


#endif//COLRED_POLYGON_H
