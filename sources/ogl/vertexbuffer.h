#ifndef VERTEXBUFFER_H
#define VERTEXBUFFER_H

#include "fpoint.h"
#include <QVector>

class VertexBuffer
{
private:
	QVector<FPoint> _vertices;
    static FPoint _verticesWorkingCopy[1024];
    QVector<FPoint> _texCoords;
    QVector<unsigned int> _indexes;
    unsigned int _sizeVert;
    unsigned int _sizeIndex;
public:
    VertexBuffer();
    VertexBuffer(const VertexBuffer &vb);
	const VertexBuffer operator=(const VertexBuffer &vb);
    ~VertexBuffer();
    unsigned int Size() const { return _sizeVert; } 
	unsigned int SizeIndex() const { return _sizeIndex; } 
	void Clear();
    void Resize(unsigned int vertexSize, unsigned int indexSize);
    FPoint &VertXY(unsigned int index);
    FPoint &VertUV(unsigned int index);
    unsigned int &Index(unsigned int index);
	void Add(const VertexBuffer &vb);
	void Union(const VertexBuffer &vb);// slow add
    void Draw();
};

#endif // VERTEXBUFFER_H
