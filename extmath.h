#ifndef MYENGINE_MATH_H
#define MYENGINE_MATH_H

#define fatof(a) static_cast<float>(atof(a))

#include "types.h"
#include "ogl/Matrix.h"
#include "ogl/vertexbuffer.h"
#include "ogl/sprite.h"

class Math {
public:
	static int round(float a);
	static int sign(float a);
	static float random(float a, float b);
	static DWORD ReadColor(std::string color);
	static std::string ColorToString(DWORD color);
	static float Read(rapidxml::xml_node<> *xe, const char *name, const float defaultValue);
	static bool Read(rapidxml::xml_node<> *xe, const char *name, const bool defaultValue);
    static void Write(rapidxml::xml_node<> *xe, const char *name, const float value);
    static void Write(rapidxml::xml_node<> *xe, const char *name, const int value);
    static void Write(rapidxml::xml_node<> *xe, const char *name, const unsigned int value);
    static void Write(rapidxml::xml_node<> *xe, const char *name, const char *value);
    static const char * FloatToChar(float f, char *s = NULL);
	static bool GetCirclesIntersect(float X1, float Y1, float R1, float X2, float Y2, float R2, FPoint &P1, FPoint &P2);
	static bool GetCirclesIntersect2(float x1, float y1, float r1, float x2, float y2, float r2, FPoint &q1, FPoint &q2);
	static float VMul(const FPoint &one, const FPoint &two);
	static float SMul(const FPoint &one, const FPoint &two);
	static FPoint lerp(const FPoint &one, const FPoint &two, float p);
	static float lerp(float one, float two, float p);
	//  
	static bool STrinagle(float a, float b, float c, float &s);
	static float Distance(const FPoint &one, const FPoint &two, const FPoint &point);
	static std::string IntToStr(int i);
	
	//    
	static bool Intersection(const FPoint &line1Start, const FPoint &line1End, 
								const FPoint &line2Start, const FPoint &line2End, FPoint *result);

	//     
	static bool Inside(const FPoint &m, const QVector<FPoint> &dots);
	static bool Inside(const FPoint &m, const FPoint &a, const FPoint &b, const FPoint &c);
	static bool DotNearLine(const FPoint &one, const FPoint &two, const FPoint &p);
	static std::string GetOnlyFileName(const std::string &filePath);
	static std::string ProcessTexture(const char *filePath, const std::string &currentWorkDir);
    static std::string CutFileName(const std::string &filePath);
    static bool GenerateTriangles(const QVector<FPoint> &, Sprite &vb, DWORD color = 0xFFFFFFFF, GLTexture2D * texture = 0, const Matrix *transform = NULL);
	//static void FillTriangle(const FPoint &a, const FPoint &b, const FPoint &c, VertexBuffer &vb, DWORD color = 0xFFFFFFFF, GLTexture2D * texture = 0, const Matrix *transform = NULL);
	static void FillTriangle(int index, int a, int b, int c, VertexBuffer &vb);

    static float SignedSquare(const QVector<FPoint> &dots);
//    static bool Convex(const FPoint &a, const FPoint &b, const FPoint &c, const FPoint &d);
};

#endif//MYENGINE_MATH_H
