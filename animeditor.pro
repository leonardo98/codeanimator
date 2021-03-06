QT += widgets

HEADERS     = animeditor.h \
    qconsolewidget.h \
    mainwindow.h \
    viewer.h \
    ogl/fpoint.h \
    ogl/matrix.h \
    ogl/render.h \
    ogl/sprite.h \
    ogl/textures.h \
    ogl/vertexbuffer.h \
    rapidxml/rapidxml_iterators.hpp \
    rapidxml/rapidxml_print.hpp \
    rapidxml/rapidxml_utils.hpp \
    rapidxml/rapidxml.hpp \
    types.h \
    extmath.h \
    SelectionTool.h \
    anim/boneanimated.h \
    splineeditor.h \
    bindspline.h \
    anim/spline.h \
    animation.h \
    ColoredPolygon.h \
    ogl/rect.h
SOURCES     = main.cpp \
              animeditor.cpp \
    qconsolewidget.cpp \
    mainwindow.cpp \
    viewer.cpp \
    ogl/fpoint.cpp \
    ogl/render.cpp \
    ogl/sprite.cpp \
    ogl/textures.cpp \
    ogl/vertexbuffer.cpp \
    extmath.cpp \
    SelectionTool.cpp \
    anim/boneanimated.cpp \
    splineeditor.cpp \
    anim/spline.cpp \
    bindspline.cpp \
    animation.cpp \
    ColoredPolygon.cpp \
    ogl/rect.cpp

QT += opengl widgets

LIBS += -framework Python

DISTFILES +=

RESOURCES += \
    resources.qrc

QMAKE_MAC_SDK = macosx10.11
