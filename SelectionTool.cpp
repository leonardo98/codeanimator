#include <QApplication>
//#include "../Core/InputSystem.h"
#include "SelectionTool.h"
//#include "TileEditor.h"

SelectionTool::SelectionTool()
: _mouseDown(false)
, _isSelection(false)
{
}

bool SelectionTool::OnMouseDown(const FPoint &mousePos)
{
	//if (IsMouseOver(mousePos))
	{
		_mouseDown = true;
		_startPoint = mousePos;
		OnMouseMove(mousePos);
		return true;
	}
	return false;
}

void SelectionTool::OnMouseUp(const FPoint &mousePos)
{
	if (!_mouseDown)
	{
		return;
	}
	_mouseDown = false;
	// do some action (call)
    //TileEditor::Instance()->ProcessSelectionArea(_area);
}

void SelectionTool::OnMouseMove(const FPoint &mousePos)
{
	if (!_mouseDown)
	{
		return;
	}
    _area.x1 = std::min(_startPoint.x, mousePos.x);
    _area.y1 = std::min(_startPoint.y, mousePos.y);
    _area.x2 = std::max(_startPoint.x, mousePos.x);
    _area.y2 = std::max(_startPoint.y, mousePos.y);
}

bool SelectionTool::IsMouseOver(const FPoint &mousePos)
{
    return (QApplication::keyboardModifiers() & Qt::ShiftModifier) != 0;
}

void SelectionTool::Draw()
{
	if (!_mouseDown)
	{
		return;
	}
	Render::PushMatrix();
	Render::SetMatrixUnit();
	Render::DrawBar(_area.x1, _area.y1, _area.x2 - _area.x1, _area.y2 - _area.y1, 0x1F7F7FFF);
	Render::Line(_area.x1, _area.y1, _area.x2, _area.y1, 0x4F7F7FFF);
	Render::Line(_area.x2, _area.y1, _area.x2, _area.y2, 0x4F7F7FFF);
	Render::Line(_area.x2, _area.y2, _area.x1, _area.y2, 0x4F7F7FFF);
	Render::Line(_area.x1, _area.y2, _area.x1, _area.y1, 0x4F7F7FFF);
	Render::PopMatrix();
}
