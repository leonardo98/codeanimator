#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "animeditor.h"
#include "viewer.h"
#include "qconsolewidget.h"
#include "splineeditor.h"

#include <QSettings>
#include <QMainWindow>
#include <QDockWidget>

class CustomDock : public QDockWidget
{
    Q_OBJECT
        signals :
    void onClose();
public:
    virtual void closeEvent(QCloseEvent *)
    {
        emit onClose();
    }
};

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
    static MainWindow *Instance() { return (MainWindow *)mainWindow; }
    void PrintToOutput(const char *s);

private:
    QString _storeFileName;
    void createMenus();
    CodeEditor *_codeEditor;
    Viewer *_viewer;
    SplineEditor *_splineEditor;

    CustomDock *_viewerDock;
    CustomDock *_codeEditorDock;
    CustomDock *_consoleDock;
    CustomDock *_splineEditorDock;

    QAction *_editPoints;

    QConsoleWidget *_console;
    QSettings settings;
};

#endif // MAINWINDOW_H
