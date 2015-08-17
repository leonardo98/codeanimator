// device specified function

#include "render.h"

Matrix Render::_matrixStack[MAX_MATRIX_AMOUNT];
unsigned int Render::_currentMatrix = 0;
bool Render::addBlendingGlobal = false;

DWORD Render::_currentColor[MAX_COLOR_AMOUNT];
unsigned int Render::_currentColorIndex;

void Render::PushMatrix() {
    //assert((_currentMatrix + 1)< MAX_MATRIX_AMOUNT);
	++_currentMatrix;
	_matrixStack[_currentMatrix] = _matrixStack[_currentMatrix - 1];
}

void Render::PopMatrix() {
    //assert(_currentMatrix > 0);
	--_currentMatrix;
}

void Render::MatrixMove(float x, float y) {
	_matrixStack[_currentMatrix].Move(x, y);
}

void Render::MatrixRotate(float angle) {
	_matrixStack[_currentMatrix].Rotate(angle);
}

void Render::MatrixScale(float sx, float sy) {
	_matrixStack[_currentMatrix].Scale(sx, sy);
}

void Render::MatrixMul(const Matrix &matrix) {
	_matrixStack[_currentMatrix].Mul(matrix);
}

void Render::SetMatrixUnit() {
	_matrixStack[_currentMatrix].Unit();
}

void Render::SetMatrix(const Matrix &matrix) {
	_matrixStack[_currentMatrix] = matrix;
}

const Matrix & Render::GetCurrentMatrix() {
	return _matrixStack[_currentMatrix];
}

void Render::PrintString(float x, float y, const std::string &text, int align) {
	//std::string fontName("data\\font2.fnt");
	//fontName = GetDataDir() + fontName;
	//Font *font;
	//if (_fonts.find(fontName) == _fonts.end()) {
	//	_fonts[fontName] = new Font(fontName.c_str());
	//}
	//font = _fonts[fontName];
	//font->Render(x, y, align, text.c_str());
}

void Render::PrintString(float x, float y, std::string fontName, const std::string &text, DWORD color) {
	//if (fontName == "") {
	//	fontName = "data\\font2.fnt";
	//}
	//if (fontName[1] != ':') {
	//	fontName = GetDataDir() + fontName;
	//}
	//Font *font;
	//if (_fonts.find(fontName) == _fonts.end()) {
	//	_fonts[fontName] = new Font(fontName.c_str());
	//}
	//font = _fonts[fontName];
	//font->SetColor(color);
	//font->Render(x, y, HGETEXT_CENTER, text.c_str());
}

int Render::GetFontHeight(const std::string &fontName) {
	//std::string fn = GetDataDir() + fontName;
	//if (_fonts.find(fn) == _fonts.end()) {
	//	_fonts[fn] = new hgeFont(fn.c_str());
	//}
	//hgeFont *font = _fonts[fn];
	//return font->GetHeight();
	return 1;
}

int Render::GetStringWidth(const std::string &fontName, const char *s) {
	//if (_fonts.find(fontName) == _fonts.end()) {
	//	_fonts[fontName] = new Font(fontName.c_str());
	//}
	//hgeFont *font = _fonts[fontName];
	//return font->GetStringWidth(s);
	return 1;
}

bool Render::InitApplication() {
	_currentColor[_currentColorIndex = 0] = 0xFFFFFFFF;
	_currentMatrix = 0;
	SetMatrixUnit();
	return true;
}

VertexBuffer tmp_quad_for_bar;

void Render::DrawBar(float x, float y, float width, float height, DWORD color) {
    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_TEXTURE_2D);
	Render::PushColorAndMul(color);
	tmp_quad_for_bar.Resize(4, 6);
	tmp_quad_for_bar.VertXY(0).x = x;
	tmp_quad_for_bar.VertXY(0).y = y;
	tmp_quad_for_bar.VertXY(1).x = x + width;
	tmp_quad_for_bar.VertXY(1).y = y;
	tmp_quad_for_bar.VertXY(2).x = x + width;
	tmp_quad_for_bar.VertXY(2).y = y + height;
	tmp_quad_for_bar.VertXY(3).x = x;
	tmp_quad_for_bar.VertXY(3).y = y + height;
	for (int i = 0; i < 4; i++) {
		tmp_quad_for_bar.VertUV(i).x = 0;
		tmp_quad_for_bar.VertUV(i).y = 0;
	}
	tmp_quad_for_bar.Draw();
    glEnable(GL_TEXTURE_2D);
	Render::PopColor();
}

void Render::DrawTriangle(float x1, float y1, float x2, float y2, float x3, float y3, DWORD color)
{
    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_TEXTURE_2D);
    Render::PushColorAndMul(color);
    tmp_quad_for_bar.Resize(3, 3);
    tmp_quad_for_bar.VertXY(0).x = x1;
    tmp_quad_for_bar.VertXY(0).y = y1;
    tmp_quad_for_bar.VertXY(1).x = x2;
    tmp_quad_for_bar.VertXY(1).y = y2;
    tmp_quad_for_bar.VertXY(2).x = x3;
    tmp_quad_for_bar.VertXY(2).y = y3;
    for (int i = 0; i < 3; i++) {
        tmp_quad_for_bar.VertUV(i).x = 0;
        tmp_quad_for_bar.VertUV(i).y = 0;
        tmp_quad_for_bar.Index(i) = i;
    }
    tmp_quad_for_bar.Draw();
    glEnable(GL_TEXTURE_2D);
    Render::PopColor();
}

void Render::DrawBarTextured(float x, float y, float width, float height, DWORD color) {
    Render::PushColorAndMul(color);
    tmp_quad_for_bar.Resize(4, 6);
    tmp_quad_for_bar.VertXY(0).x = x;
    tmp_quad_for_bar.VertXY(0).y = y;
    tmp_quad_for_bar.VertXY(1).x = x + width;
    tmp_quad_for_bar.VertXY(1).y = y;
    tmp_quad_for_bar.VertXY(2).x = x + width;
    tmp_quad_for_bar.VertXY(2).y = y + height;
    tmp_quad_for_bar.VertXY(3).x = x;
    tmp_quad_for_bar.VertXY(3).y = y + height;
    for (int i = 0; i < 4; i++) {
        tmp_quad_for_bar.VertUV(i).x = (tmp_quad_for_bar.VertXY(i).x - x) / width;
        tmp_quad_for_bar.VertUV(i).y = 1 - (tmp_quad_for_bar.VertXY(i).y - y) / height;
    }
    tmp_quad_for_bar.Draw();
    Render::PopColor();
}


void Render::Line(float x1, float y1, float x2, float y2, DWORD color) {
	GetCurrentMatrix().Mul(x1, y1, x1, y1);
	GetCurrentMatrix().Mul(x2, y2, x2, y2);
	if (_lineCachEnable)
	{
		LineCach cach;
		cach.start.x = x1;
		cach.start.y = y1;
		cach.end.x = x2;
		cach.end.y = y2;
		cach.color = color;
		_linesCach.push_back(cach);
		return;
	}
	Render::PushColorAndMul(color);
	DWORD currentColor = Render::GetColor();
	glColor4ubv( (GLubyte*)&currentColor );
    glBegin(GL_LINES);
    glVertex2d(x1, y1);
    glVertex2d(x2, y2);
    glEnd();
	Render::PopColor();
}

void Render::SetAlpha(DWORD alpha) {
	SetColor((alpha << 24) | (GetColor() & 0xFFFFFF));
}

void Render::Circle(float x, float y, float r, DWORD color) {
	Render::Line(x, y, x + r, y, color);
	float angle = 0.f;
	float xs = sin(angle) * r;
	float ys = cos(angle) * r;
	while (angle < M_PI * 2) {
		angle += static_cast<float>(M_PI)/ 10.f;
		float xe = sin(angle) * r;
		float ye = cos(angle) * r;
		Render::Line(x + xs, y + ys, x + xe, y + ye, color);
		xs = xe;
		ys = ye;
	}
}

void Render::BeginCachingLine()
{
	_linesCach.clear();
	_lineCachEnable = true;
}

void Render::EndCachingLine()
{
	_lineCachEnable = false;
}

void Render::DrawCachingLine()
{
	Render::PushMatrix();
	Render::SetMatrixUnit();
	for (unsigned int i = 0; i < _linesCach.size(); ++i)
	{
		Render::Line(_linesCach[i].start.x, _linesCach[i].start.y
					, _linesCach[i].end.x, _linesCach[i].end.y
					, _linesCach[i].color);
	}
	Render::PopMatrix();
}

QVector<LineCach> Render::_linesCach;
bool Render::_lineCachEnable = false;
bool Render::_slowUnion = false;

void Render::DrawStripe(const QVector<FPoint> &dots, GLTexture2D *texture, float y1, float y2, float x1, float stepX, int startI, int endI, VertexBuffer *vb)
{
	assert(dots.size() >= 2);

	VertexBuffer quad;
	quad.Resize(4, 6);
	quad.VertUV(2).y = quad.VertUV(3).y = y1;
	quad.VertUV(1).y = quad.VertUV(0).y = y2;

	FPoint f;
	FPoint n;
	float lenX = x1;

	float h2 = fabs(y2 - y1) / 2 * texture->Height();

	for (int i = startI; i < endI - 1; ++i)
	{
		f.x = dots[i].y - dots[i + 1].y;
		f.y = dots[i + 1].x - dots[i].x;
		f *= h2 / f.Length();

		if ((i + 2) < static_cast<int>(dots.size()))
		{
			n.x = dots[i + 1].y - dots[i + 2].y;
			n.y = dots[i + 2].x - dots[i + 1].x;
			n *= h2 / n.Length();
		}
		else
		{
			n = f;
		}

		quad.VertUV(3).x = quad.VertUV(0).x = lenX;
		lenX += stepX;
		quad.VertUV(2).x = quad.VertUV(1).x = lenX;

		quad.VertXY(0).x = dots[i].x + f.x;
		quad.VertXY(0).y = dots[i].y + f.y;

		quad.VertXY(1).x = dots[i + 1].x + n.x;
		quad.VertXY(1).y = dots[i + 1].y + n.y;

		quad.VertXY(2).x = dots[i + 1].x - n.x;
		quad.VertXY(2).y = dots[i + 1].y - n.y;

		quad.VertXY(3).x = dots[i].x - f.x;
		quad.VertXY(3).y = dots[i].y - f.y;

		if (vb == NULL)
		{
			quad.Draw();
		}
		else
		{
			if (_slowUnion)
			{
				vb->Union(quad);
			}
			else
			{
				vb->Add(quad);
			}
		}
	}
}

void Render::DrawTexturePart(GLTexture2D *texture, float x1, float y1, float x2, float y2, float sx1, float sy1, float sx2, float sy2, VertexBuffer *vb)
{
    VertexBuffer quad;
    quad.Resize(4, 6);

    quad.VertXY(3).x = quad.VertXY(0).x = sx1;
    quad.VertXY(1).y = quad.VertXY(0).y = sy1;
    quad.VertXY(2).x = quad.VertXY(1).x = sx2;
    quad.VertXY(3).y = quad.VertXY(2).y = sy2;

    quad.VertUV(3).x = quad.VertUV(0).x = x1 / texture->Width();
    quad.VertUV(1).y = quad.VertUV(0).y = y1 / texture->Height();
    quad.VertUV(2).x = quad.VertUV(1).x = x2 / texture->Width();
    quad.VertUV(3).y = quad.VertUV(2).y = y2 / texture->Height();

    if (vb == NULL)
    {
        quad.Draw();
    }
    else
    {
        if (_slowUnion)
        {
            vb->Union(quad);
        }
        else
        {
            vb->Add(quad);
        }
    }
}


void Render::DrawSmartStripe(const QVector<FPoint> &dots, GLTexture2D *texture, VertexBuffer *vb)
{
	assert(dots.size() >= 3);
	if (dots.size() == 3)
	{
		// ничего внахлест
		DrawStripe(dots, texture, 0.5f, 1.f, 0.25f, 0.25f, 0, 3, vb);
		return;
	}
	if (dots.size() == 4)
	{
		DrawStripe(dots, texture, 0.5f, 1.f, 0.25f, 0.25f, 0, 3, vb);
		// серединка внахлест
		DrawStripe(dots, texture, 0.5f, 1.f, 0.25f, 0.25f, dots.size() - 3, dots.size(), vb);
		return;
	}
	if (dots.size() % 4 == 1)
	{
		// ничего внахлест
		DrawStripe(dots, texture, 0.5f, 1.f, -0.25f, 0.25f, 0, 2, vb);
		DrawStripe(dots, texture, 0.f, 0.5f, 0.0f, 0.25f, 1, dots.size() - 1, vb);
		DrawStripe(dots, texture, 0.5f, 1.f, 0.5f, 0.25f, dots.size() - 2, dots.size(), vb);
		return;
	}
	if (dots.size() % 4 == 2)
	{
		DrawStripe(dots, texture, 0.5f, 1.f, -0.25f, 0.25f, 0, 2, vb);
		DrawStripe(dots, texture, 0.f, 0.5f, 0.0f, 0.25f, 1, dots.size() - 2, vb);
		DrawStripe(dots, texture, 0.5f, 1.f, 0.5f, 0.25f, dots.size() - 3, dots.size() - 1, vb);
		// предпоследний сектор внахлест
		DrawStripe(dots, texture, 0.5f, 1.f, 0.25f, 0.25f, dots.size() - 3, dots.size(), vb);
		return;
	}
	if (dots.size() % 4 == 3)
	{
		// ничего внахлест
		DrawStripe(dots, texture, 0.5f, 1.f, 0.75f, 0.25f, 0, 2, vb);
		DrawStripe(dots, texture, 0.f, 0.5f, 0.0f, 0.25f, 1, dots.size() - 1, vb);
		DrawStripe(dots, texture, 0.5f, 1.f, 0.0f, 0.25f, dots.size() - 2, dots.size(), vb);
		return;
	}
	DrawStripe(dots, texture, 0.5f, 1.f, -0.25f, 0.25f, 0, 2, vb);
	DrawStripe(dots, texture, 0.f, 0.5f, 0.0f, 0.25f, 1, dots.size() - 2, vb);
	DrawStripe(dots, texture, 0.5f, 1.f, 0.0f, 0.25f, dots.size() - 3, dots.size() - 1, vb);
	// предпоследний сектор внахлест
	DrawStripe(dots, texture, 0.5f, 1.f, 0.25f, 0.25f, dots.size() - 3, dots.size(), vb);
}

void Render::SetFiltering(bool value)
{
	GLTexture2D::filteringGlobal = value;
}

bool Render::GetFiltering()
{
	return GLTexture2D::filteringGlobal;
}

int Render::_screenWidth = 0;
int Render::_screenHeight = 0;

void Render::MulColor(DWORD color)
{
//#ifdef WIN32
    color = (0xFF00FF00 & color)
                | ((0x00FF0000 & color) >> 16)
                | ((0x000000FF & color) << 16);
//#endif
    if (color == 0xFFFFFFFF)
	{
		return;
	}
	if (GetColor() == 0xFFFFFFFF)
	{
		SetColor(color);
		return;
	}
	DWORD currentColor = GetColor();
	SetColor(
				(((((currentColor & 0xFF000000)) >> 24 * ((color & 0xFF000000) >> 24)) / 0xFF) << 24)
                    | (((((currentColor & 0xFF0000)) >> 16 * ((color & 0xFF0000) >> 16)) / 0xFF) << 16)
					| (((((currentColor & 0xFF00)) >> 8 * ((color & 0xFF00) >> 8)) / 0xFF) << 8)
					| ((currentColor & 0xFF) * (color & 0xFF) / 0xFF)
			);
}

void Render::SetColor(DWORD color) {
//#ifdef WIN32
    _currentColor[_currentColorIndex] = (0xFF00FF00 & color)
                                        | ((0x00FF0000 & color) >> 16)
                                        | ((0x000000FF & color) << 16);
//#else
//	_currentColor[_currentColorIndex] = color;
//#endif
}

DWORD Render::GetColor() {
//#ifdef WIN32
    return (0xFF00FF00 & _currentColor[_currentColorIndex])
            | ((0x00FF0000 & _currentColor[_currentColorIndex]) >> 16)
            | ((0x000000FF & _currentColor[_currentColorIndex]) << 16);
//#else
//	return _currentColor[_currentColorIndex];
//#endif
}
