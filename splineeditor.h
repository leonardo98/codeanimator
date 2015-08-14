#ifndef SPLINEEDITOR_H
#define SPLINEEDITOR_H

#include <QWidget>
#include "anim/spine.h"
#include "ogl/matrix.h"

class SplineEditor : public QWidget
{
    Q_OBJECT
public:
    explicit SplineEditor(QWidget *parent = 0);

signals:

public slots:

private:
    SplineMover _spline;

protected:
    void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;
    Matrix _viewMatrix;

public:
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent* event) ;

};

#endif // SPLINEEDITOR_H
