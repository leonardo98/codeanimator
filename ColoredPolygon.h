#ifndef COLRED_POLYGON_H
#define COLRED_POLYGON_H

#include "BeautyBase.h"
#include "../Core/ogl/sprite.h"

//
// 
//  
//

class ColoredPolygon : public BeautyBase
{
private:

	int _width;
	int _height;

	GLTexture2D * _texture;
	std::string _texturePath;
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

public:

	virtual ~ColoredPolygon();
	ColoredPolygon(rapidxml::xml_node<> *xe);
	ColoredPolygon(const ColoredPolygon &c);
    //ColoredPolygon(BinSaver &xe);
    //void SaveToBin(BinSaver &xe);

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
	virtual std::string Type();

	virtual int Width();
	virtual int Height();

	virtual const char *GetIconTexture();
private:
	float _textureScale;
	float _textureAngle;
	Matrix _textureTransform;
};


#endif//COLRED_POLYGON_H
