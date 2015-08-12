#include "mainwindow.h"
#include <QStatusBar>

QMainWindow *mainWindow = NULL;

MainWindow::MainWindow()
    : settings("PakholkovLeonid", "CodeAnimator")
{
    mainWindow = this;
    //createMenus();

    _centralSplitter = new QSplitter(Qt::Horizontal, this);
    _codeConsoleSplitter = new QSplitter(Qt::Vertical, this);
    _viewerSplineSplitter = new QSplitter(Qt::Vertical, this);

    _codeEditor = new CodeEditor( this );
    _console = new QConsoleWidget( this );
    _viewer = new Viewer( this );
    _splineEditor = new SplineEditor( this );

    _codeConsoleSplitter->addWidget(_codeEditor);
    _codeConsoleSplitter->addWidget(_console);

    _centralSplitter->addWidget(_codeConsoleSplitter);
    _centralSplitter->addWidget(_viewerSplineSplitter);

    _viewerSplineSplitter->addWidget(_viewer);
    _viewerSplineSplitter->addWidget(_splineEditor);

    _centralSplitter->setStretchFactor(0, 0);
    _centralSplitter->setStretchFactor(1, 1);

    _codeConsoleSplitter->setStretchFactor(0, 0);
    _codeConsoleSplitter->setStretchFactor(1, 1);

    _viewerSplineSplitter->setStretchFactor(0, 0);
    _viewerSplineSplitter->setStretchFactor(1, 1);

    setCentralWidget(_centralSplitter);

    restoreGeometry(settings.value("mainwindow_geometry").toByteArray());
    restoreState(settings.value("mainwindow_state").toByteArray());

    _centralSplitter->restoreState(settings.value("central_splitter").toByteArray());
    _centralSplitter->restoreGeometry(settings.value("central_splitter_geometry").toByteArray());

    _codeConsoleSplitter->restoreState(settings.value("code_console_splitter").toByteArray());
    _codeConsoleSplitter->restoreGeometry(settings.value("code_console_splitter_geometry").toByteArray());

    _viewerSplineSplitter->restoreState(settings.value("view_spline_splitter").toByteArray());
    _viewerSplineSplitter->restoreGeometry(settings.value("view_spline_splitter_geometry").toByteArray());

    statusBar()->showMessage(tr("Ready"));
}

MainWindow::~MainWindow()
{
    settings.setValue("view_spline_splitter_geometry", _viewerSplineSplitter->saveGeometry());
    settings.setValue("view_spline_splitter", _viewerSplineSplitter->saveState());

    settings.setValue("code_console_splitter_geometry", _codeConsoleSplitter->saveGeometry());
    settings.setValue("code_console_splitter", _codeConsoleSplitter->saveState());

    settings.setValue("central_splitter_geometry", _centralSplitter->saveGeometry());
    settings.setValue("central_splitter", _centralSplitter->saveState());

    settings.setValue("mainwindow_geometry", saveGeometry());
    settings.setValue("mainwindow_state", saveState());
}


