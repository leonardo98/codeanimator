QT += widgets

HEADERS     = animeditor.h \
    binding.h \
    qconsolewidget.h \
    redirect.h \
    mainwindow.h
SOURCES     = main.cpp \
              animeditor.cpp \
    binding.cpp \
    qconsolewidget.cpp \
    redirect.cpp \
    mainwindow.cpp

LIBS += -framework Python

