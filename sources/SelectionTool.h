#ifndef SELECTION_TOOL_H
#define SELECTION_TOOL_H

#include "ogl/render.h"
#include "ogl/Rect.h"

class SelectionTool
{
private:
	bool _mouseDown;
	bool _isSelection;
	FPoint _startPoint;
	Rect _area;
public:
	SelectionTool();
	bool OnMouseDown(const FPoint &mousePos);
    void OnMouseUp();
	void OnMouseMove(const FPoint &mousePos);
	bool IsMouseOver(const FPoint &mousePos);
	void Draw();

    virtual void UpdateSelection(const Rect &area) = 0;
//    void EndSelection();
};

#endif//SELECTION_TOOL_H
