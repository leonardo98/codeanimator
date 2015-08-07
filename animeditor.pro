QT += widgets

HEADERS     = animeditor.h \
    binding.h \
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
    SelectionTool.h
SOURCES     = main.cpp \
              animeditor.cpp \
    binding.cpp \
    qconsolewidget.cpp \
    mainwindow.cpp \
    viewer.cpp \
    ogl/fpoint.cpp \
    ogl/render.cpp \
    ogl/sprite.cpp \
    ogl/textures.cpp \
    ogl/vertexbuffer.cpp \
    extmath.cpp \
    SelectionTool.cpp

QT += opengl widgets

LIBS += -framework Python

