#ifndef SPLINEEDITOR_H
#define SPLINEEDITOR_H

#include <QWidget>
#include "anim/boneposition.h"
#include "ogl/matrix.h"

class SplineEditor : public QWidget
{
    Q_OBJECT
public:
    explicit SplineEditor(QWidget *parent = 0);

signals:

public slots:

private:
    Spline _spline;

protected:
    void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;
    Matrix _viewMatrix;

};

#endif // SPLINEEDITOR_H
