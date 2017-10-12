#ifndef COLRED_POLYGON_H
#define COLRED_POLYGON_H

#include "ogl/sprite.h"
#include "ogl/rect.h"
#include "anim/boneanimated.h"
#include "extmath.h"

typedef std::vector<FPoint> PointList;

struct BoneDistance
{
    BoneAnimated *bone;
    float distance;
};

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

    std::vector<OnePoint> _dots; //
    QVector<FPoint> _screenDots;
    Sprite _triangles;
    Sprite _trianglesBinded;
    bool _mouseDown;
    QVector<int> _dotUnderCursor;
    QVector<int> _selectedDots;

    VertexBuffer tri;
    void FillTriangle(const FPoint &a, const FPoint &b, const FPoint &c, VertexBuffer &tri);
    void DrawTriangles();

    int SearchNearest(float x, float y);

    int CreateDot(float x, float y);
    bool CheckLines(const FPoint &p);
    void RemoveDot(QVector<int> index);

    void InitCorners();
    bool CanCut(const std::string &message, const std::string &substr, std::string &result);

    std::string _boneName;

    Matrix _lastDrawMatrix;

public:

    void ReplaceBonesWith(BoneList &bones);
    void GenerateTriangles();
    const std::string &GetBone() { return _boneName; }
    void SetBone(const std::string &name) { _boneName = name; }

    virtual ~ColoredPolygon();
    ColoredPolygon(rapidxml::xml_node<> *xe);
    ColoredPolygon(const ColoredPolygon &c);
    ColoredPolygon(const PointList &points);

    void UpdatePoints(const PointList &points);

    virtual void Draw();
    void DrawBinded();
    virtual void DebugDraw(bool onlyControl);
    virtual void Update(float dt) {}
    virtual void MouseDown(const FPoint &mouse);
    virtual bool MouseMove(const FPoint &mouse);
    virtual void MouseUp(const FPoint &mouse);
    virtual bool PixelCheck(const FPoint &point);
    virtual bool GeometryCheck(const FPoint &point);
    bool RemoveDots();
    virtual bool Selection(const Rect& rect, bool full);
    virtual void SaveToXml(rapidxml::xml_node<> *xe);

    void BindToBone(BoneAnimated *bone);

    virtual int Width();
    virtual int Height();

    virtual const char *GetIconTexture();

    void ResetMass();
};


#endif//COLRED_POLYGON_H
