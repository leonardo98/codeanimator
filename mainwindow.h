#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSplitter>
#include "animeditor.h"
#include "viewer.h"
#include "qconsolewidget.h"
#include "splineeditor.h"
#include <QSettings>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow();
    virtual ~MainWindow();
private:
    QSplitter *_centralSplitter;
    QSplitter *_codeConsoleSplitter;
    QSplitter *_viewerSplineSplitter;
    CodeEditor *_codeEditor;
    Viewer *_viewer;
    SplineEditor *_splineEditor;

    QConsoleWidget *_console;
    QSettings settings;
};

#endif // MAINWINDOW_H
