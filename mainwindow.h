#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSplitter>
#include "animeditor.h"
#include "qconsolewidget.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow();
private:
    QSplitter *_centralSplitter;
    CodeEditor *_codeEditor;
    QConsoleWidget *_console;
};

#endif // MAINWINDOW_H
