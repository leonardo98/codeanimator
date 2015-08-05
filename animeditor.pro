QT += widgets

HEADERS     = animeditor.h \
    binding.h \
    qconsolewidget.h \
    mainwindow.h
SOURCES     = main.cpp \
              animeditor.cpp \
    binding.cpp \
    qconsolewidget.cpp \
    mainwindow.cpp

LIBS += -framework Python

