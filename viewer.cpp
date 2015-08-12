#include "viewer.h"
#include "ogl/render.h"
#include <QWheelEvent>
#include <QTimer>
#include <QStatusBar>

Viewer::Viewer(QWidget *parent)
    : QGLWidget(parent)
    , _viewScale(1.f)
    , _screenOffset((SCREEN_WIDTH = 1024) / 2
                    , (SCREEN_HEIGHT = 768) / 2  )
    , _worldOffset(0.f, 0.f)
{
    Render::InitApplication();

    _clearColor = 0xFF7F7F7F;
    SetFrame(640, 480);

    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(Update()));
    timer->start(20);
    setMouseTracking(true);
}

void Viewer::initializeGL()
{
    glDisable(GL_DEPTH_TEST);
    glEnable ( GL_BLEND );
    glClearColor(0.50f, 0.50f, 0.50f, 1.0f);
    glBlendFunc(GL_SRC_ALPHA ,GL_ONE_MINUS_SRC_ALPHA);
}

void Viewer::resizeGL(int width, int height)
{
    _widgetHeight = height;
    _widgetWidth = width;
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    glOrtho( 0, width, 0, height, -1.0, 1.0);
    glTranslatef(0, height, 0);
    glScalef(1.f, -1.f, 1.f);
    glMatrixMode( GL_MODELVIEW );
    glViewport(0, 0, width, height);
}

void Viewer::wheelEvent(QWheelEvent *event)
{
    _lastMousePos.x = event->x();
    _lastMousePos.y = event->y();
#ifdef WIN32
    OnMouseWheel(event->delta() / 2000.f);
#else
    OnMouseWheel(event->delta() / 200.f);
#endif
}

bool Viewer::OnMouseWheel(float value)
{
    FPoint fp = ScreenToWorld(_lastMousePos);
    _screenOffset = _lastMousePos;
    _worldOffset = fp;
    float MIN = 1 / 32.f;
    float MAX = 4.f;
    _viewScale += value * _viewScale;
//	if (direction > 0 && _viewScale < MAX) {
//		_viewScale *= 1.09f;
//	} else if (direction < 0 && _viewScale > MIN) {
//		_viewScale *= 0.9f;
//	}
    _viewScale = std::min(MAX, std::max(MIN, _viewScale));
    _worldOffset = ScreenToWorld(FPoint(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2));
    _screenOffset = FPoint(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);
    return true;
}

void Viewer::mouseReleaseEvent(QMouseEvent *event)
{
    FPoint mousePos(event->pos().x(), event->pos().y());
    if (event->button() == Qt::LeftButton)
    {
        if ((mousePos - _mouseDownPos).Length() < 10 && _mouseMovingMode < 0.2f)
        {
            _mouseMoveAction = mouse_none;
        }
        _selectionTool.OnMouseUp(mousePos);
        OnMouseUp();
    }
    else if (event->button() == Qt::RightButton)
    {
        _mouseMoveAction = mouse_none;
    }
}

void Viewer::mousePressEvent(QMouseEvent *event)
{
    FPoint mousePos(event->pos().x(), event->pos().y());
    _lastMousePos = mousePos;
    if (event->button() == Qt::LeftButton)
    {
        OnMouseDown(mousePos);
    }
    else if (event->button() == Qt::RightButton)
    {
        _mouseMoveAction = mouse_dragging_world;
    }
}

void Viewer::mouseMoveEvent(QMouseEvent *event)
{
    FPoint mousePos(event->pos().x(), event->pos().y());
//    if ((QApplication::keyboardModifiers() & Qt::ControlModifier) != 0)
//    {
//        if (fabs(_mouseDownPos.x - mousePos.x) > fabs(_mouseDownPos.y - mousePos.y))
//        {
//            mousePos.y = _mouseDownPos.y;
//        }
//        else
//        {
//            mousePos.x = _mouseDownPos.x;
//        }
//    }
    _selectionTool.OnMouseMove(mousePos);
    OnMouseMove(mousePos);
}

void Viewer::OnMouseDown(const FPoint &mousePos)
{
    _mouseDown = true;
    _lastMousePos = _mouseDownPos = mousePos;
    _mouseMovingMode = 0.f;
    FPoint worldClickPos = ScreenToWorld(mousePos);

    FPoint fp(mousePos.x, mousePos.y);

    _selectionTool.OnMouseDown(FPoint(fp.x, fp.y));

//    if ((QApplication::keyboardModifiers() & Qt::Key_Space) != 0)
//    {
//        _mouseMoveAction = mouse_dragging_world;
//        return;
//    }

}

void Viewer::OnMouseUp()
{
    _mouseMovingMode = 0.f;
    _mouseDown = false;
//	if (_currents.beauty.size() == 1) {
//		_currents.beauty[0]->MouseUp(_lastMousePos);
//	}
//	if (_pushCopyOnMouseUp) {
//        PushCopyToRedoUndoManager();
//		_pushCopyOnMouseUp = false;
//	}
    _mouseMoveAction = mouse_none;
}

void Viewer::OnMouseMove(const FPoint &mousePos)
{
    FPoint newMmouseWorld = ScreenToWorld(mousePos);

    if (_mouseMoveAction == mouse_dragging_world) {
        _worldOffset -= (mousePos - _lastMousePos) / _viewScale;
//	} else if (_mouseMoveAction == mouse_moving_beauty/* || _mouseMoveAction == mouse_moving_group*/) {
//		if ((mousePos - _mouseDownPos).Length() > 10 || _mouseMovingMode >= 0.2f) {
//			_mouseMovingMode = 0.2f;

//			if (!TileEditorInterface::Instance()->CreateDotMode()) {
//				for (unsigned int i = 0; i < _currents.beauty.size(); ++i) {
//					_currents.beauty[i]->ShiftTo(newMmouseWorld.x - _mouseWorld.x, newMmouseWorld.y - _mouseWorld.y);
//				}
//                TileEditorInterface::Instance()->UpdateProperties();
//			}
//			else
//			{
//				for (unsigned int i = 0; i < _currents.beauty.size(); ++i) {
//					_currents.beauty[i]->MouseMove(mousePos);
//				}
//			}
//			if (_currents.beauty.size()) {
//				_pushCopyOnMouseUp = true;
//			}
//		}
//	}
//	else if (_mouseMoveAction == mouse_moving_beauty_control)
//	{
//		for (unsigned int i = 0; i < _currents.beauty.size(); ++i) {
//            _pushCopyOnMouseUp |= _currents.beauty[i]->MouseMove(mousePos);
//		}
//        TileEditorInterface::Instance()->UpdateProperties();
//	}
//	if (!_mouseDown && !TileEditorInterface::Instance()->CreateDotMode()) {//
//		_currents.beautyUnderCursor = BeautyAtPoint(newMmouseWorld);
    }

    _lastMousePos = mousePos;
    _mouseWorld = newMmouseWorld;

    char buff[100];
    sprintf(buff, "x: %0.2f, y: %0.2f", _mouseWorld.x, _mouseWorld.y);

    mainWindow->statusBar()->showMessage(tr(buff));
}

FPoint Viewer::ScreenToWorld(const FPoint &screenPos) {
    return (screenPos - _screenOffset) / _viewScale + _worldOffset;
}

FPoint Viewer::WorldToScreen(const FPoint &worldPos) {
    return (worldPos - _worldOffset) * _viewScale + _screenOffset;
}


void Viewer::paintGL()
{

    glClearColor(0.50f, 0.50f, 0.50f, 1.0f);
    glBlendFunc(GL_SRC_ALPHA ,GL_ONE_MINUS_SRC_ALPHA);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);
    glEnable ( GL_BLEND );

//    Render::SetFiltering(TileEditorInterface::Instance()->FilteringTexture());
    { //
        //Render::BeginCachingLine();
        Render::PushMatrix();
        Render::DrawBar(2, 2, Width()- 4, Height() - 4, _clearColor);
        Render::MatrixMove(_screenOffset.x, _screenOffset.y);
        Render::MatrixScale(_viewScale, _viewScale);
        Render::MatrixMove(-_worldOffset.x, -_worldOffset.y);

//		if (_currents.beautyUnderCursor) {
//			Render::SetAlpha(0x5F);
//			_currents.beautyUnderCursor->DebugDraw(true);
//			Render::SetAlpha(0xFF);
//		}
//		if (_currents.beauty.size()) {
//			Render::SetAlpha(0xAF);
//			for (unsigned int i = 0; i < _currents.beauty.size(); ++i) {
//				_currents.beauty[i]->DebugDraw(true);
//			}
//			Render::SetAlpha(0xFF);
//		}

        //		if (TileEditorInterface::Instance()->NetVisible())
        {
            Matrix m;
            m.MakeRevers(Render::GetCurrentMatrix());
            float startX = 0.f;
            float startY = 0.f;
            m.Mul(startX, startY);
            float endX = Width();
            float endY = Height();
            m.Mul(endX, endY);
            float STEP = 64.f;
            float x = static_cast<int>(startX / STEP) * STEP;
            while (x < endX) {
                Render::Line(x, startY, x, endY, 0x0FFFFFFF);
                x += STEP;
            }
            float y = static_cast<int>(startY / STEP) * STEP;
            while (y < endY) {
                Render::Line(startX, y, endX, y, 0x0FFFFFFF);
                y += STEP;
            }
        }
       // Render::EndCachingLine();
        //if (TileEditorInterface::Instance()->ViewportVisible())
        {
            Render::Line( - 99999, 0 ,99999, 0, 0x4F000000);
//            Render::Line(_viewportWidth, 0, _viewportWidth, _viewportHeight, 0x7F000000);
//            Render::Line(_viewportWidth, _viewportHeight, 0, _viewportHeight, 0x7F000000);
            Render::Line(0, - 99999, 0, 99999, 0x4F000000);
        }
        Render::PopMatrix();

        {
            Render::PushMatrix();
            Render::MatrixMove(Width() / 2.f, Height() / 2.f);
            Render::MatrixScale(_viewScale, _viewScale);
            Render::MatrixMove(- _viewportWidth / 2.f, - _viewportHeight / 2.f);
            Render::Line(0, 0, _viewportWidth, 0, 0x7FFF0000);
            Render::Line(_viewportWidth, 0, _viewportWidth, _viewportHeight, 0x7FFF0000);
            Render::Line(_viewportWidth, _viewportHeight, 0, _viewportHeight, 0x7FFF0000);
            Render::Line(0, _viewportHeight, 0, 0, 0x7FFF0000);
            Render::PopMatrix();
        }
        //Render::DrawCachingLine();
    }

    //	char buff[10];
//	Math::FloatToChar(_viewScale, buff);
//	Render::PrintString(940, 0, "", buff);
    _selectionTool.Draw();
//	Render::SetFiltering(TileEditorInterface::Instance()->FilteringTexture());
//    {
//        float x = _widgetWidth / 2 - (64 * _commandButtons.size()) / 2;
//        for (unsigned int i = 0; i < _commandButtons.size(); ++i)
//        {
//            if (i == 0 && GetButtonsState() != bs_standart)
//            {
//                _commandButtons[i]->SetColor(0xFF7F7F7F);
//            }
//            else
//            {
//                _commandButtons[i]->SetColor(0xFFFFFFFF);
//            }
//            _commandButtons[i]->SetPosition(x, 30);
//            _commandButtons[i]->Draw();
//            x += 64;
//        }
//    }
}

void Viewer::Update()
{
//    float dt = 0.02f;
//    Core::Update(dt);
    //Update(dt);
    updateGL();
}