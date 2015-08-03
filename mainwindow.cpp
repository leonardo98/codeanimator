#include "mainwindow.h"

MainWindow::MainWindow()
{
    //createMenus();

    _centralSplitter = new QSplitter(Qt::Horizontal, this);

    _codeEditor = new CodeEditor( this );
    _console = new QConsoleWidget( this );

    _centralSplitter->addWidget(_codeEditor);
    _centralSplitter->addWidget(_console);
    _centralSplitter->setStretchFactor(0, 0);
    _centralSplitter->setStretchFactor(1, 1);

    setCentralWidget(_centralSplitter);
}

