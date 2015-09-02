#ifndef VIEWER_H
#define VIEWER_H

#include <QGLWidget>
#include "ogl/fpoint.h"
#include "SelectionTool.h"

enum MouseActionState
{
    mouse_dragging_world,
    mouse_moving_bone,
    mouse_moving_mesh,
    mouse_moving_mesh_point,
    mouse_select_region,
    mouse_drop_or_select_region,
    mouse_none
};

class Viewer
        : public QGLWidget
        , public SelectionTool
{
    Q_OBJECT
public slots:
    void Update();
signals:
    void uploadTexture();
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

    void OnMouseMove(const FPoint &mousePos);
    void OnMouseUp();
    void OnMouseDown(const FPoint &mousePos);
    bool _mouseDown;
    enum ChainState
    {
        chain_none,
        chain_wait_first,
        chain_work
    };
    ChainState _chainState;
    FPoint _mouseWorld;

    enum HotKeysMode
    {
        none_key,
        create_bone_key,
        create_bone_chain_key,
        length_bone_key,
        ik_bone_key,
    };
    HotKeysMode _hotKeysMode;

    std::string _cursorText;

public:
    Viewer(QWidget *parent);

    void initializeGL();
    void paintGL();
    void resizeGL(int width, int height);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent* event) ;

    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);
    void UpdateSelection(const Rect &area);

//    virtual bool eventFilter(QObject *obj, QEvent *event);
};

#endif // VIEWER_H
