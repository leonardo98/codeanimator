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
    void uploadLastTexture();
    void saveAs();
    void save();
    void open();
    void changeMode();
public:
    MainWindow();
    virtual ~MainWindow();
    bool CreateDotMode();
    bool AnimationMode();
    bool BoneEditMode();
    static MainWindow *Instance() { return (MainWindow *)mainWindow; }
    void PrintToOutput(const char *s);

private:
    QString _storeFileName;
    void createMenus();
    QSplitter *_centralSplitter;
    QSplitter *_codeConsoleSplitter;
    QSplitter *_viewerSplineSplitter;
    CodeEditor *_codeEditor;
    Viewer *_viewer;
    SplineEditor *_splineEditor;

    QAction *_editPoints;
    QAction *_editAnimation;
    QAction *_editBone;

    QConsoleWidget *_console;
    QSettings settings;
};

#endif // MAINWINDOW_H
