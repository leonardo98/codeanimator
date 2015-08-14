#include <python2.7/Python.h>

#include "bindspline.h"
#include "splineeditor.h"

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

static PyMethodDef SplineMethods[] = {
//    ...
    {"setSegmentType",  spline_setSegmentType, METH_VARARGS,
     "Set segment type 1 - linear, 2 - square, 3 - cubic"},
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
