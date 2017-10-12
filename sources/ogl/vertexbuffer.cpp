#include "vertexbuffer.h"
#include "render.h"

FPoint VertexBuffer::_verticesWorkingCopy[1024];

VertexBuffer::VertexBuffer()
: _vertices(NULL)
, _texCoords(NULL)
, _indexes(NULL)
, _sizeVert(0)
, _sizeIndex(0)
{
}

void VertexBuffer::Clear()
{
    _sizeVert = 0;
    _sizeIndex = 0;
}

VertexBuffer::~VertexBuffer()
{
    Clear();
}

void VertexBuffer::Resize(unsigned int vertexSize, unsigned int indexSize)
{
    Clear();
    if (vertexSize)
    {
        _sizeVert = vertexSize;
        _vertices.resize(vertexSize);
        _texCoords.resize(vertexSize);
    }
    if (indexSize)
    {
        _sizeIndex = indexSize;
        _indexes.resize(indexSize);
    }
    if (vertexSize == 4 && indexSize == 6)
    {
        Index(0) = 0;
        Index(1) = 1;
        Index(2) = 2;
        Index(3) = 0;
        Index(4) = 2;
        Index(5) = 3;
    }
}

FPoint &VertexBuffer::VertXY(unsigned int index)
{
    assert(index < _sizeVert);
    return (_vertices[index]);
}

FPoint &VertexBuffer::VertUV(unsigned int index)
{
    assert(index < _sizeVert);
    return (_texCoords[index]);
}

unsigned int &VertexBuffer::Index(unsigned int index)
{
    assert(index < _sizeIndex);
    return (_indexes[index]);
}

void VertexBuffer::Draw()
{
    if (_sizeVert == 0)
    {
        return;
    }
    //assert(0 < _vertices.size());
    for (unsigned int i = 0; i < _sizeVert; ++i)
    {
        Render::GetCurrentMatrix().Mul(_vertices[i].x, _vertices[i].y
                                       , _verticesWorkingCopy[i].x, _verticesWorkingCopy[i].y);
    }
    DWORD color = Render::GetColor();
    glColor4ubv((GLubyte *)&color);

    glVertexPointer(2, GL_FLOAT, 0, _verticesWorkingCopy);
    glTexCoordPointer(2, GL_FLOAT, 0, &(_texCoords[0]));
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisableClientState( GL_COLOR_ARRAY );
    glDrawElements(GL_TRIANGLES, _sizeIndex,  GL_UNSIGNED_INT, &(_indexes[0]));
}

void VertexBuffer::Add(const VertexBuffer &vb)
{
    if (vb.Size() == 0)
    {
        return;
    }
    _vertices.resize(_sizeVert + vb._sizeVert);
    _texCoords.resize(_sizeVert + vb._sizeVert);
    _indexes.resize(_sizeIndex + vb._sizeIndex);

    for (unsigned int i = 0; i < vb._sizeVert; ++i)
    {
        _vertices[_sizeVert + i] = vb._vertices[i];
        _texCoords[_sizeVert + i] = vb._texCoords[i];
    }
    for (unsigned int i = 0; i < vb._sizeIndex; ++i)
    {
        _indexes[_sizeIndex + i] = _sizeVert + vb._indexes[i];
    }

    _sizeVert += vb._sizeVert;
    _sizeIndex += vb._sizeIndex;
}

void VertexBuffer::Union(const VertexBuffer &vb)
{
    if (vb.Size() == 0)
    {
        return;
    }
    _vertices.resize(_sizeVert + vb._sizeVert);
    _texCoords.resize(_sizeVert + vb._sizeVert);
    _indexes.resize(_sizeIndex + vb._sizeIndex);

    // новые вершины нужно проверить - вдруг такая уже есть
    int counter = _sizeVert;

    for (unsigned int i = 0; i < vb._sizeIndex; ++i)
    {
        FPoint p(vb._vertices[vb._indexes[i]]);
        FPoint uv(vb._texCoords[vb._indexes[i]]);

        int index = -1;
        for (unsigned int j = 0; j < counter; ++j)
        {
            if ((_vertices[j] - p).Length() < 1e-5
                && (_texCoords[j] - uv).Length() < 1e-5)
            {
                index = (int)j;
                break;
            }
        }

        if (index == -1)
        {
            _vertices[counter] = p;
            _texCoords[counter] = uv;
            _indexes[_sizeIndex + i] = counter;
            ++counter;
        }
        else
        {
            _indexes[_sizeIndex + i] = index;
        }
    }

    _sizeVert = counter;
    _sizeIndex += vb._sizeIndex;
}

VertexBuffer::VertexBuffer(const VertexBuffer &vb)
: _vertices(NULL)
, _texCoords(NULL)
, _indexes(NULL)
, _sizeVert(0)
, _sizeIndex(0)
{
    Resize(vb.Size(), vb.SizeIndex());
    for (unsigned int i = 0; i < _sizeVert; ++i)
    {
        _vertices[i] = vb._vertices[i];
        _texCoords[i] = vb._texCoords[i];
    }
    for (unsigned int i = 0; i < _sizeIndex; ++i)
    {
        _indexes[i] = vb._indexes[i];
    }
}

const VertexBuffer VertexBuffer::operator=(const VertexBuffer &vb)
{
    Resize(vb.Size(), vb.SizeIndex());
    for (unsigned int i = 0; i < _sizeVert; ++i)
    {
        _vertices[i] = vb._vertices[i];
        _texCoords[i] = vb._texCoords[i];
    }
    for (unsigned int i = 0; i < _sizeIndex; ++i)
    {
        _indexes[i] = vb._indexes[i];
    }
    return *this;
}
