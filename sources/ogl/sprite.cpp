#include "sprite.h"
#include "render.h"
#include "extmath.h"

Sprite::Sprite()
: _origin(NULL)
{
}

Sprite::Sprite(const GLTexture2D *texture, float x, float y, float width, float height)
{
    Set(texture, x, y, width, height);
}

void Sprite::Set(const GLTexture2D *texture, float x, float y, float width, float height) {
    _origin = texture;
    if (_origin == NULL) {
        return;
    }
    _originX = x;
    _originY = y;
    if (width <= 0.f)
    {
        _originWidth = width = texture->Width();
        _originHeight = height = texture->Height();
    }
    else
    {
        _originWidth = width;
        _originHeight = height;
    }

    _quad.Resize(4, 6);
    _quad.VertXY(0).Set(0, 0);
    _quad.VertXY(1).Set(width, 0);
    _quad.VertXY(2).Set(width, height);
    _quad.VertXY(3).Set(0, height);
    _quad.VertUV(0).Set(x / texture->Width(), y / texture->Height());
    _quad.VertUV(1).Set((x + width)/ texture->Width(), y / texture->Height());
    _quad.VertUV(2).Set((x + width)/ texture->Width(), (y + height) / texture->Height());
    _quad.VertUV(3).Set(x / texture->Width(), (y + height) / texture->Height());
    _quad.Index(0) = 0;
    _quad.Index(1) = 1;
    _quad.Index(2) = 2;
    _quad.Index(3) = 0;
    _quad.Index(4) = 2;
    _quad.Index(5) = 3;

    _matrix.Unit();
}
    
void Sprite::Render() {
    //assert(_origin != NULL);
    //_quad = _origin->_forCopyQuad;
    if (_origin)
    {
        _origin->bind();
        _quad.Draw();
        _origin->unbind();
    }
    else
    {
        _quad.Draw();
    }
}

void Sprite::Render(float x, float y) {
    Render::PushMatrix();
    Render::MatrixMove(x, y);
    Render();
    Render::PopMatrix();
}

bool Sprite::PixelCheck(float x, float y)
{
    FPoint p(x, y);
    for (unsigned int i = 0; i < _quad.SizeIndex(); i += 3)
    {
        if (Math::Inside(p, _quad.VertXY(_quad.Index(i)), _quad.VertXY(_quad.Index(i + 1)), _quad.VertXY(_quad.Index(i + 2))))
        {
            if (_origin)
            {
                FPoint m(x - _quad.VertXY(_quad.Index(i)).x, y - _quad.VertXY(_quad.Index(i)).y);
                FPoint a(_quad.VertXY(_quad.Index(i + 1)) - _quad.VertXY(_quad.Index(i)));
                FPoint b(_quad.VertXY(_quad.Index(i + 2)) - _quad.VertXY(_quad.Index(i)));

                float k1 = (m.x * b.y - m.y * b.x) / (a.x * b.y - a.y * b.x);
                float k2 = (fabs(b.y) > 1e-5) ? (m.y - k1 * a.y) / b.y : (m.x - k1 * a.x) / b.x;

                if (k1 < 0 || k1 > 1 || k2 < 0 || k2 > 1) {
                    return false;
                }

                float originWidth = _origin->Width();
                float originHeight = _origin->Height();

                m = _quad.VertUV(_quad.Index(i));
                a = _quad.VertUV(_quad.Index(i + 1)) - m;
                b = _quad.VertUV(_quad.Index(i + 2)) - m;

                m += k1 * a + k2 * b;

                int i = m.x * originWidth;
                int j = m.y * originHeight;

                return !(_origin->Transparency(i, j));
            }
            return true;
        }
    }
    return false;
}

bool Sprite::GeometryCheck(float x, float y)
{
    FPoint p(x, y);
    for (unsigned int i = 0; i < _quad.SizeIndex(); i += 3)
    {
        if (Math::Inside(p, _quad.VertXY(_quad.Index(i)), _quad.VertXY(_quad.Index(i + 1)), _quad.VertXY(_quad.Index(i + 2))))
        {
            return true;
        }
    }
    return false;
}

void Sprite::GetTextureRect(float &x, float &y, float &w, float &h) const
{
    x = _originX;
    y = _originY;
    w = _originWidth;
    h = _originHeight;
}
