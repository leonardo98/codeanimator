#include "splineeditor.h"

#include <QMouseEvent>

SplineEditor * SplineEditor::_instance = NULL;

SplineEditor::~SplineEditor()
{
    assert(_instance != NULL);
    _instance = NULL;
}

SplineEditor::SplineEditor(QWidget *parent) : QWidget(parent)
{
    assert(_instance == NULL);
    _instance = this;

    _spline.AddPoint(0, 100.f);
    _spline.AddPoint(40, 0.f);
    _spline.AddPoint(80, 150.f);
    _spline.AddPoint(100, 200.f);

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
    _viewMatrix.Scale(4.f, 1.f);

    //setMouseTracking(true);
}

void SplineEditor::paintEvent(QPaintEvent *)
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
        vEps *= 10;
        hEps *= 10;
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

void SplineEditor::SetSplineSegmentType(uint index, SplineType st)
{
    _spline.SetSegmentType(index, st);
    repaint();
}

void SplineEditor::SetSplinePointFrame(uint index, uint frame)
{
    _spline.SetValue(index, frame, _spline.GetValue(index));
    repaint();
}

void SplineEditor::SetSplinePointValue(uint index, float value)
{
    _spline.SetValue(index, _spline.GetFrame(index), value);
    repaint();
}

void SplineEditor::SetSplinePointFrameAndValue(uint index, uint frame, float value)
{
    _spline.SetValue(index, frame, value);
    repaint();
}

void SplineEditor::AddSplinePoint(uint frame, float value)
{
    _spline.AddPoint(frame, value);
    repaint();
}

void SplineEditor::RemoveSplinePoint(uint index)
{
    _spline.RemovePoint(index);
    repaint();
}

void SplineEditor::ClearSpline()
{
    _spline.Clear();
    repaint();
}

