#ifndef SPRITE_H
#define SPRITE_H

#include "../types.h"

#include "matrix.h"
#include "textures.h"
#include "vertexbuffer.h"

class Sprite {
public:
    Sprite();
    Sprite(const GLTexture2D *texture, float x = 0.f, float y = 0.f, float width = 0.f, float height = 0.f);
    void Set(const GLTexture2D *texture, float x = 0.f, float y = 0.f, float width = 0.f, float height = 0.f);
    void Render();
    void Render(float x, float y);
    inline float Width() {
        return _originWidth;
    }
    inline float Height() {
        return _originHeight;
    }
	VertexBuffer &GetVB() { return _quad; }
	bool PixelCheck(float x, float y);
	bool GeometryCheck(float x, float y);
	void GetTextureRect(float &x, float &y, float &w, float &h) const;
	const GLTexture2D *GetTexture() const { return _origin; }
private:
    VertexBuffer _quad;
    Matrix _matrix;
    const GLTexture2D *_origin;
	float _originX;
	float _originY;
    float _originWidth;
    float _originHeight;
};

#endif // SPRITE_H
