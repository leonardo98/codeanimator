#include <python2.7/Python.h>

#include "bindspline.h"
#include "splineeditor.h"

// import spline

// 
// spline.setSegmentType(segment index, segment type)
//
static PyObject *spline_setSegmentType(PyObject *self, PyObject *args)
{
    int index;
    int type;
   // int sts;

    if (!PyArg_ParseTuple(args, "ii", &index, &type))
        return NULL;

    if (SplineEditor::Instance())
    {
        SplineEditor::Instance()->SetSplineSegmentType(index, (SplineType)type);
    }

    return Py_BuildValue("s", "ok");
}

//
// spline.setPointFrame(point index, point frame)
//
static PyObject *spline_setPointFrame(PyObject *self, PyObject *args)
{
    int index;
    int frame;
   // int sts;

    if (!PyArg_ParseTuple(args, "ii", &index, &frame))
        return NULL;

    if (SplineEditor::Instance())
    {
        SplineEditor::Instance()->SetSplinePointFrame(index, frame);
    }

    return Py_BuildValue("s", "ok");
}

// 
// spline.setPointValue(point index, point value)
//
static PyObject *spline_setPointValue(PyObject *self, PyObject *args)
{
    int index;
    float value;
   // int sts;

    if (!PyArg_ParseTuple(args, "if", &index, &value))
        return NULL;

    if (SplineEditor::Instance())
    {
        SplineEditor::Instance()->SetSplinePointValue(index, value);
    }

    return Py_BuildValue("s", "ok");
}

//
// spline.setPointFrameAndValue(point index, point frame, point value)
//
static PyObject *spline_setPointFrameAndValue(PyObject *self, PyObject *args)
{
    int index;
    int frame;
    float value;

    if (!PyArg_ParseTuple(args, "iif", &index, &frame, &value))
        return NULL;

    if (SplineEditor::Instance())
    {
        SplineEditor::Instance()->SetSplinePointFrameAndValue(index, frame, value);
    }

    return Py_BuildValue("s", "ok");
}

//
// spline.addPoint(point frame, point value)
//
static PyObject *spline_addPoint(PyObject *self, PyObject *args)
{
    int frame;
    float value;

    if (!PyArg_ParseTuple(args, "if", &frame, &value))
        return NULL;

    if (SplineEditor::Instance())
    {
        SplineEditor::Instance()->AddSplinePoint(frame, value);
    }

    return Py_BuildValue("s", "ok");
}

//
// spline.removePoint(point index)
//
static PyObject *spline_removePoint(PyObject *self, PyObject *args)
{
    int index;

    if (!PyArg_ParseTuple(args, "i", &index))
        return NULL;

    if (SplineEditor::Instance())
    {
        SplineEditor::Instance()->RemoveSplinePoint(index);
    }

    return Py_BuildValue("s", "ok");
}

//
// spline.clear()
//
static PyObject *spline_clear(PyObject *self, PyObject *args)
{

    if (SplineEditor::Instance())
    {
        SplineEditor::Instance()->ClearSpline();
    }

    return Py_BuildValue("s", "ok");
}

static PyMethodDef SplineMethods[] = {
//    ...
    {"setSegmentType",  spline_setSegmentType, METH_VARARGS, "Set segment type 1 - linear, 2 - square, 3 - cubic"},
    {"setPointFrame",  spline_setPointFrame, METH_VARARGS, "Set spline point frame"},
    {"setPointValue",  spline_setPointValue, METH_VARARGS, "Set spline point value"},
    {"setPointFrameAndValue",  spline_setPointFrameAndValue, METH_VARARGS, "Set spline point frame and value"},
    {"addPoint",  spline_addPoint, METH_VARARGS, "Add new point at spline by frame and value"},
    {"removePoint",  spline_removePoint, METH_VARARGS, "Remove point from spline by index"},
    {"clear",  spline_clear, METH_VARARGS, "Remove point from spline by index"},
//    ...
    {NULL, NULL, 0, NULL}        /* Sentinel */
};

PyMODINIT_FUNC InitSpline(void)
{
    (void) Py_InitModule("spline", SplineMethods);
}

void InitSplineModule()
{
    InitSpline();
}
