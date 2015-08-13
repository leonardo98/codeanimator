#include "splineeditor.h"
#include <QMouseEvent>

SplineEditor::SplineEditor(QWidget *parent) : QWidget(parent)
{
    _spline.AddPoint(100.f);
    _spline.AddPoint(0.f);
    _spline.AddPoint(150.f);
    _spline.AddPoint(200.f);

    _spline.SetSegmentType(0, spline_type_cubic);
    _spline.SetSegmentType(1, spline_type_square);
    _spline.SetSegmentType(2, spline_type_linear);

    // set black background
    QPalette Pal(palette());
    Pal.setColor(QPalette::Background, Qt::white);
    setAutoFillBackground(true);
    setPalette(Pal);
    show();

    _viewMatrix.Move(15.f, 15.f);
    _viewMatrix.Scale(100.f, 1.f);

    setMouseTracking(true);
}

void SplineEditor::paintEvent(QPaintEvent *) Q_DECL_OVERRIDE
{
    QPainter painter(this);
    _spline.Draw(painter, _viewMatrix);

    painter.drawRect(QRectF(0.0, 0.0, width(), height()));
}

void SplineEditor::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        FPoint mousePos(event->pos().x(), event->pos().y());
        Matrix rev;
        rev.MakeRevers(_viewMatrix);
        rev.Mul(mousePos);
        float vEps;
        float hEps;
        rev.GetScale(vEps, hEps);
        vEps *= 5;
        hEps *= 5;
        _spline.OnMouseDown(mousePos, vEps, hEps);
    }
}

void SplineEditor::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        _spline.OnMouseUp();
    }
}

void SplineEditor::mouseMoveEvent(QMouseEvent *event)
{
    if ((event->buttons() & Qt::LeftButton) != 0)
    {
        FPoint mousePos(event->pos().x(), event->pos().y());
        Matrix rev;
        rev.MakeRevers(_viewMatrix);
        rev.Mul(mousePos);
        if (_spline.OnMouseMove(mousePos))
        {
            repaint();
        }
    }

}

void SplineEditor::wheelEvent(QWheelEvent* event)
{}

