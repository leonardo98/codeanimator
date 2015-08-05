#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSplitter>
#include "animeditor.h"
#include "qconsolewidget.h"
#include <QSettings>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow();
    virtual ~MainWindow();
private:
    QSplitter *_centralSplitter;
    CodeEditor *_codeEditor;
    QConsoleWidget *_console;
    QSettings settings;
};

#endif // MAINWINDOW_H
