#include "splineeditor.h"

SplineEditor::SplineEditor(QWidget *parent) : QWidget(parent)
{
    _spline.AddPoint(100.f);
    _spline.AddPoint(0.f);
    _spline.AddPoint(150.f);
    _spline.AddPoint(200.f);

    _spline.SetSegmentType(0, spline_type_square);
    _spline.SetSegmentType(1, spline_type_square);
    _spline.SetSegmentType(2, spline_type_square);
}

void SplineEditor::paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE
{
    QPainter painter(this);
    _spline.Draw(painter);
}


