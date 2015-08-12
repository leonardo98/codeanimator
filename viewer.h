#ifndef VIEWER_H
#define VIEWER_H

#include <QGLWidget>
#include "ogl/fpoint.h"
#include "SelectionTool.h"

enum MouseActionState
{
    mouse_dragging_world,
    mouse_moving_beauty,
    mouse_moving_beauty_control,
    mouse_select_region,
    mouse_drop_or_select_region,
    mouse_none
};

class Viewer : public QGLWidget
{
    Q_OBJECT
public slots:
    void Update();
protected:
    float _viewScale; //

    FPoint _worldOffset; //
    FPoint _screenOffset; //

    int SCREEN_WIDTH;
    int SCREEN_HEIGHT;

    int _widgetWidth;
    int _widgetHeight;
    int Width() { return _widgetWidth; }
    int Height() { return _widgetHeight; }
    unsigned int _clearColor;
    int _viewportWidth, _viewportHeight;
    void SetFrame(int w, int h) { _viewportWidth = w; _viewportHeight = h; }
    FPoint _lastMousePos;
    bool OnMouseWheel(float value);
    FPoint ScreenToWorld(const FPoint &screenPos);
    FPoint WorldToScreen(const FPoint &worldPos);
    MouseActionState _mouseMoveAction;
    FPoint _mouseDownPos;
    float _mouseMovingMode;
    SelectionTool _selectionTool;

    void OnMouseMove(const FPoint &mousePos);
    void OnMouseUp();
    void OnMouseDown(const FPoint &mousePos);
    bool _mouseDown;
    FPoint _mouseWorld;

public:
    Viewer(QWidget *parent);

    void initializeGL();
    void paintGL();
    void resizeGL(int width, int height);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent* event) ;


};

#endif // VIEWER_H