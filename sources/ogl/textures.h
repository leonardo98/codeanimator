#ifndef TEXTURES_H
#define TEXTURES_H

#include <QGLWidget>

//#define assert(a) Q_ASSERT(a);

class GLTexture
{
public:
    GLTexture();
    virtual ~GLTexture();
    virtual void bind() const = 0;
    virtual void unbind() const = 0;
    virtual bool failed() const {return m_failed;}
protected:
    GLuint m_texture;
    bool m_failed;
	unsigned int *m_alphaMask;
};

class GLTexture2D : public GLTexture
{
private:
    int _width;
    int _height;
    //void internalLoading(FIBITMAP* bits);
	void createAlphaMask(const void *);
public:
	static bool filteringGlobal;
    GLTexture2D(int width, int height);
    GLTexture2D(const wchar_t *wFileName);
    GLTexture2D(const char *fileName);
    void load(int width, int height, const unsigned char *data);
	unsigned char *CreateBitmapCopy() const;
    virtual void bind() const;
    virtual void unbind() const;
    int Width() const { return _width; }
    int Height() const { return _height; }
	GLuint glHandle() { return m_texture; }
	bool Transparency(int i, int j) const;
};

#endif // TEXTURES_H
