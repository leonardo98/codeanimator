#include "mainwindow.h"

MainWindow::MainWindow()
    : settings("PakholkovLeonid", "CodeAnimator")
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

    restoreGeometry(settings.value("mainwindow_geometry").toByteArray());
    restoreState(settings.value("mainwindow_state").toByteArray());

    _centralSplitter->restoreState(settings.value("central_splitter").toByteArray());
    _centralSplitter->restoreGeometry(settings.value("central_splitter_geometry").toByteArray());
}

MainWindow::~MainWindow()
{
    settings.setValue("central_splitter_geometry", _centralSplitter->saveGeometry());
    settings.setValue("central_splitter", _centralSplitter->saveState());

    settings.setValue("mainwindow_geometry", saveGeometry());
    settings.setValue("mainwindow_state", saveState());
}


