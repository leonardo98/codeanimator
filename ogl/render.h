// device specified function
#ifndef MYENGINE_RENDER_H
#define MYENGINE_RENDER_H

#include "matrix.h"
#include "textures.h"
#include "vertexbuffer.h"

#define MAX_MATRIX_AMOUNT 32
#define MAX_COLOR_AMOUNT 32

struct LineCach
{
	FPoint start;
	FPoint end;
	DWORD color;
};

class Render
{

private:
	static Matrix _matrixStack[MAX_MATRIX_AMOUNT];
	static unsigned int _currentMatrix;
	
	static QVector<LineCach> _linesCach;
	static bool _lineCachEnable;
	static bool _slowUnion;
	static DWORD _currentColor[MAX_COLOR_AMOUNT];
	static unsigned int _currentColorIndex;

public:

	static void EnableSlowUnion(bool value) { _slowUnion = value; }
	static void PushMatrix();
	static void PopMatrix();
	static void MatrixMove(float x, float y);
	static void MatrixRotate(float angle);
	static void MatrixScale(float sx, float sy);
	static void MatrixMul(const Matrix &matrix);
	static void SetMatrixUnit();
	static void SetMatrix(const Matrix &matrix);
	static const Matrix & GetCurrentMatrix();

	static void MulColor(DWORD color);
	static inline void PushColor() {
		assert((_currentColorIndex + 1)< MAX_COLOR_AMOUNT);
		_currentColor[_currentColorIndex + 1] = _currentColor[_currentColorIndex];
		++_currentColorIndex;
	}
	static inline void PopColor() {
		assert(_currentColorIndex > 0);
		--_currentColorIndex;
	}
	static void PushColorAndMul(DWORD color)
	{
		PushColor();
		MulColor(color);
	}

	static void SetColor(DWORD color);
	static DWORD GetColor();

	static void SetAlpha(DWORD alpha);
	static inline void SetBlendMode(DWORD blendMode) {
		addBlendingGlobal = (blendMode != 0);
		if (addBlendingGlobal)
		{
			glBlendFunc(GL_SRC_ALPHA, GL_ONE);
		}
		else
		{
			glBlendFunc(GL_SRC_ALPHA ,GL_ONE_MINUS_SRC_ALPHA); 
		}
	}
	static inline DWORD GetBlendMode() {
		return addBlendingGlobal ? 1 : 0;
	}

	static void PrintString(float x, float y, std::string fontName, const std::string &text, DWORD color = 0xFFFFFFFF);
	static void PrintString(float x, float y, const std::string &text, int align);
	static bool InitApplication();
	static void SetDataDir(const std::string &dataDir);
	static void RunApplication();
	static void ExitApplication();
	static std::string Error();
	static int GetFontHeight(const std::string &fontName);
	static int GetStringWidth(const std::string &fontName, const char *s);
	static void ShowMessage(const char *str, const char *caption);
	static bool ShowAskMessage(const char *str, const char *caption);
	static bool IsRightMouseButton();
	static bool IsLeftMouseButton();
    static void DrawBar(float x, float y, float width, float height, DWORD color);
    static void DrawBarTextured(float x, float y, float width, float height, DWORD color);

    static void Line(float x1, float y1, float x2, float y2, DWORD color);
	static void Circle(float x, float y, float r, DWORD color);

	static void DrawStripe(const QVector<FPoint> &dots, GLTexture2D *texture, float y1, float y2, float x1, float stepX, int startI, int endI, VertexBuffer *vb = NULL);
	static void DrawSmartStripe(const QVector<FPoint> &dots, GLTexture2D *texture, VertexBuffer *vb = NULL);
    static void DrawTexturePart(GLTexture2D *texture, float x1, float y1, float x2, float y2, float sx1, float sy1, float sx2, float sy2, VertexBuffer *vb = NULL);

	static void BeginCachingLine();
	static void EndCachingLine();
	static void DrawCachingLine();
	static void SetFiltering(bool);
	static bool GetFiltering();

	static void SetScreenWidth(int w) { _screenWidth = w; }
	static void SetScreenHeight(int h) { _screenHeight = h; }

	static int ScreenWidth() { return _screenWidth; }
	static int ScreenHeight() { return _screenHeight; }

private:
	static bool addBlendingGlobal;
	static int _screenWidth;
	static int _screenHeight;
	friend class VertexBuffer;
};


#endif//MYENGINE_RENDER_H
