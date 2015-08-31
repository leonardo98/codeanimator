#include "../types.h"
#include "textures.h"
#include <QtGui>
#include <QtOpenGL>

//============================================================================//
//                                  GLTexture                                 //
//============================================================================//

GLTexture::GLTexture() : m_texture(0), m_failed(false)
{
    glGenTextures(1, &m_texture);
	m_alphaMask = NULL;
}

GLTexture::~GLTexture()
{
    glDeleteTextures(1, &m_texture);
	if (m_alphaMask)
	{
		delete [] m_alphaMask;
	}
}

//============================================================================//
//                                 GLTexture2D                                //
//============================================================================//

bool GLTexture2D::filteringGlobal = false;

GLTexture2D::GLTexture2D(int width, int height)
{
    glBindTexture(GL_TEXTURE_2D, m_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, 4, width, height, 0,
        GL_BGRA, GL_UNSIGNED_BYTE, 0);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    //glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
    glBindTexture(GL_TEXTURE_2D, 0);

    _width = width;
    _height = height;
}


GLTexture2D::GLTexture2D(const char *fileName)
{
    QImage mySurface(fileName);
    _width = mySurface.width();
    _height = mySurface.height();
    if (_width == 0 || _height == 0)
    {
        LOG(std::string("texture with zero size ") + fileName);
        m_failed = true;
        return;
    }

    glBindTexture(GL_TEXTURE_2D, m_texture);
    GLenum format = GL_BGRA;
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, _width, _height, 0, format, GL_UNSIGNED_BYTE, mySurface.bits());

    //This is the magic inside Qt5's implementation
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    glBindTexture(GL_TEXTURE_2D, 0);
}

GLTexture2D::GLTexture2D(const wchar_t */*wFileName*/)
{
    assert(false);
}

//void GLTexture2D::internalLoading(FIBITMAP* bits)
//{
//	if ( !bits )
//	{
//		m_failed = true;
//		return;
//	}

//	// Convert to 32bit
//	FIBITMAP * bits32 = FreeImage_ConvertTo32Bits( bits );
//	if ( !bits32 )
//	{
//		m_failed = true;
//		return;
//	}

//	// Flip
//	::FreeImage_FlipVertical( bits32 );


//	_width = FreeImage_GetWidth( bits32 );
//	_height = FreeImage_GetHeight( bits32 );

//	// Create the opengl texture
//	glGenTextures( 1, &m_texture );
//	glBindTexture( GL_TEXTURE_2D, m_texture );
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

//   //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
//    //glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
//	glBlendFunc(GL_SRC_ALPHA ,GL_ONE_MINUS_SRC_ALPHA);

//	#ifdef FREEIMAGE_BIGENDIAN
//	GLenum format = GL_RGBA;
//	#else
//	GLenum format = GL_BGRA;
//	#endif

//	const GLvoid *ptr = (const GLvoid*)FreeImage_GetBits( bits32 );
//	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA8, _width, _height, 0, format, GL_UNSIGNED_BYTE, ptr );
//    glBindTexture(GL_TEXTURE_2D, 0);

//	createAlphaMask(ptr);
//	FreeImage_Unload( bits32 );

//}

void GLTexture2D::load(int width, int height, const unsigned char *data)
{
    glBindTexture(GL_TEXTURE_2D, m_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, 4, width, height, 0,
        GL_BGRA, GL_UNSIGNED_BYTE, data);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void GLTexture2D::bind() const
{
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, m_texture);

	glEnable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);

	if (filteringGlobal)
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	}
	else
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	}

}

void GLTexture2D::unbind() const
{
    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_TEXTURE_2D);
}

unsigned char *GLTexture2D::CreateBitmapCopy() const
{
	unsigned char *data = new unsigned char[Width() * Height() * sizeof(DWORD)];
	bind();
	glGetTexImage(GL_TEXTURE_2D, 0, GL_BGRA, GL_UNSIGNED_BYTE, data);
	unbind();
	return data;
}

void GLTexture2D::createAlphaMask(const void *bits)
{
	if ( m_alphaMask )
	{
		m_failed = true;
		return;
	}

	unsigned int n = _width * _height / sizeof(unsigned int) + 1;
	m_alphaMask = new unsigned int[n];
	for (unsigned int i = 0; i < n; ++i) m_alphaMask[i] = 0;

	const unsigned int *pixels = (unsigned int *)(bits);

    for (int i = 0; i < _width; ++i)
	{
        for (int j = 0; j < _height; ++j)
		{
			unsigned int v = i + j * _width;
			if ((pixels[v] & 0xFF000000) > 0)
			{
				m_alphaMask[v / sizeof(unsigned int)] |= (1 << (v % sizeof(unsigned int)));
			}
		}
	}
}

bool GLTexture2D::Transparency(int i, int j) const
{
	if (i < 0 || j < 0 || i >= _width || j >= _height)
	{
		return false;
	}
	if (m_alphaMask == 0)
	{
        return false;
	}
	unsigned int v = i + j * _width;
	return (m_alphaMask[v / sizeof(unsigned int)] & (1 << (v % sizeof(unsigned int)))) == 0;
}
