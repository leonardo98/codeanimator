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

private slots:
    void about();
    void chooseTexture();
public:
    MainWindow();
    virtual ~MainWindow();
    bool CreateDotMode();
    static MainWindow *Instance() { return (MainWindow *)mainWindow; }

private:
    void createMenus();
    QSplitter *_centralSplitter;
    QSplitter *_codeConsoleSplitter;
    QSplitter *_viewerSplineSplitter;
    CodeEditor *_codeEditor;
    Viewer *_viewer;
    SplineEditor *_splineEditor;

    QAction *_editPoints;

    QConsoleWidget *_console;
    QSettings settings;
};

#endif // MAINWINDOW_H
