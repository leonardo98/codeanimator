#include "mainwindow.h"
#include "animation.h"

#include <QStatusBar>
#include <QMenu>
#include <QMenuBar>
#include <QFileDialog>

QMainWindow *mainWindow = NULL;

MainWindow::MainWindow()
    : settings("PakholkovLeonid", "Code Animator")
{
    mainWindow = this;

    _codeEditor = new CodeEditor( this );
    _console = new QConsoleWidget( this );
    _viewer = new Viewer( this );
    _splineEditor = new SplineEditor( this );

    _viewerDock = nullptr;
    //_viewerDock = new CustomDock();
    //_viewerDock->setWidget(_viewer);
    //_viewerDock->setWindowTitle("viewer");
    //_viewerDock->setAllowedAreas(Qt::AllDockWidgetAreas);
    //_viewerDock->setFeatures(CustomDock::DockWidgetMovable | CustomDock::DockWidgetFloatable
    //    | CustomDock::DockWidgetClosable);
    //addDockWidget(Qt::RightDockWidgetArea, _viewerDock);

    setCentralWidget(_viewer);

    _codeEditorDock = new CustomDock();
    _codeEditorDock->setWidget(_codeEditor);
    _codeEditorDock->setWindowTitle("code block");
    _codeEditorDock->setAllowedAreas(Qt::AllDockWidgetAreas);
    _codeEditorDock->setFeatures(CustomDock::DockWidgetMovable | CustomDock::DockWidgetFloatable
        | CustomDock::DockWidgetClosable);
    addDockWidget(Qt::LeftDockWidgetArea, _codeEditorDock);

    _consoleDock = new CustomDock();
    _consoleDock->setWidget(_console);
    _consoleDock->setWindowTitle("console");
    _consoleDock->setAllowedAreas(Qt::AllDockWidgetAreas);
    _consoleDock->setFeatures(CustomDock::DockWidgetMovable | CustomDock::DockWidgetFloatable
        | CustomDock::DockWidgetClosable);
    addDockWidget(Qt::LeftDockWidgetArea, _consoleDock);

    _splineEditorDock = new CustomDock();
    _splineEditorDock->setWidget(_splineEditor);
    _splineEditorDock->setWindowTitle("spline editor");
    _splineEditorDock->setAllowedAreas(Qt::AllDockWidgetAreas);
    _splineEditorDock->setFeatures(CustomDock::DockWidgetMovable | CustomDock::DockWidgetFloatable
        | CustomDock::DockWidgetClosable);
    addDockWidget(Qt::BottomDockWidgetArea, _splineEditorDock);

    createMenus();

    restoreGeometry(settings.value("mainwindow_geometry").toByteArray());
    restoreState(settings.value("mainwindow_state").toByteArray());

    // load previos animation
    _storeFileName = settings.value("store_file_name").toString();
    if (_storeFileName.size())
    {
        Animation::Instance()->LoadFromFile(_storeFileName.toStdString());
    }

    statusBar()->showMessage(tr("Ready"));

    connect(_viewer, SIGNAL(uploadTexture()), this, SLOT(uploadLastTexture()));
}

void MainWindow::changeMode()
{
    if (!CreateDotMode())
    {
        Animation::Instance()->DuplicateNewBones();
        Animation::Instance()->ReplaceBones();
    }
}

void MainWindow::PrintToOutput(const char *s)
{
    _console->PrintToOutput(s);
}

void MainWindow::uploadLastTexture()
{
    QString fileName = settings.value("last_texture").toString();
    if (fileName.size())
    {
        if (Animation::Instance()->SetTexture(fileName.toStdString().c_str()))
        {
            char buff[1000];
            sprintf(buff, "Texture: %s uploaded", fileName.toStdString().c_str());
            _console->PrintToOutput(buff);
        }
    }
    disconnect(_viewer, SIGNAL(uploadTexture()), this, SLOT(uploadLastTexture()));
}

MainWindow::~MainWindow()
{
    settings.setValue("mainwindow_geometry", saveGeometry());
    settings.setValue("mainwindow_state", saveState());

    settings.setValue("store_file_name", _storeFileName);
}

void MainWindow::createMenus()
{
    QAction *action;
    QMenu *menu;
    {
        menu = menuBar()->addMenu(tr("&About"));

        action = new QAction(tr("&About"), this);
        connect(action, SIGNAL(triggered()), this, SLOT(about()));
        menu->addAction(action);

    }
    {
        menu = menuBar()->addMenu(tr("&File"));

        action = new QAction(tr("&New..."), this);
        connect(action, SIGNAL(triggered()), this, SLOT(about()));
        menu->addAction(action);

        action = new QAction(tr("&Open..."), this);
        connect(action, SIGNAL(triggered()), this, SLOT(open()));
        menu->addAction(action);

        action = new QAction(tr("&Save"), this);
        connect(action, SIGNAL(triggered()), this, SLOT(save()));
        action->setShortcuts(QKeySequence::Save);
        menu->addAction(action);

        action = new QAction(tr("Save &as..."), this);
        connect(action, SIGNAL(triggered()), this, SLOT(saveAs()));
        menu->addAction(action);

        menu->addSeparator();

        action = new QAction(tr("E&xit"), this);
        action->setShortcuts(QKeySequence::Quit);
        connect(action, SIGNAL(triggered()), this, SLOT(close()));
        menu->addAction(action);
    }
    {
        menu = menuBar()->addMenu(tr("&Edit"));

        action = new QAction(tr("&Delete"), this);
        connect(action, SIGNAL(triggered()), Animation::Instance(), SLOT(Remove()));
//        QList<QKeySequence> keys;
//        keys.push_back(QKeySequence::Backspace);
//        keys.push_back(QKeySequence::Delete);
        action->setShortcuts(QKeySequence::Delete);
        menu->addAction(action);
    }
    {
        QActionGroup* group = new QActionGroup( this );

        menu = menuBar()->addMenu(tr("&Mode"));

        action = new QAction(tr("&Animation"), this);
        action->setActionGroup(group);
        action->setCheckable(true);
        action->setChecked(false);
        connect(action, SIGNAL(changed()), this, SLOT(changeMode()));
        menu->addAction(action);

        action = new QAction(tr("&Bone"), this);
        action->setActionGroup(group);
        action->setCheckable(true);
        action->setChecked(true);
        connect(action, SIGNAL(changed()), this, SLOT(changeMode()));
        menu->addAction(action);

        _editPoints = action = new QAction(tr("&Texture"), this);
        action->setActionGroup(group);
        action->setCheckable(true);
        action->setChecked(false);
        connect(action, SIGNAL(changed()), this, SLOT(changeMode()));
        menu->addAction(action);

    }
    {
        menu = menuBar()->addMenu(tr("&Animation"));

        action = new QAction(tr("&Choose texture..."), this);
        connect(action, SIGNAL(triggered()), this, SLOT(chooseTexture()));
        menu->addAction(action);

        menu->addSeparator();

        action = new QAction(tr("&Unlink bone from parent"), this);
        connect(action, SIGNAL(triggered()), Animation::Instance(), SLOT(Unlink()));
        action->setShortcut(Qt::Key_P);
        menu->addAction(action);

        menu->addSeparator();

        action = new QAction(tr("&Bind points to bones"), this);
        connect(action, SIGNAL(triggered()), Animation::Instance(), SLOT(CreatePointMass()));
        menu->addAction(action);
    }
}

void MainWindow::about()
{
    QMessageBox::about(this, tr("About 2D Game Animation Editor"),
            tr("The <b>2D Game Animation Editor</b> version 2.0a<br>by Pakholkov Leonid<br>Feel free to contact: am98pln@gmail.com"));
}

void MainWindow::chooseTexture()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"), "", tr("PNG (*.png)"));

    if (fileName.size())
    {
        Animation::Instance()->SetTexture(fileName.toStdString().c_str());
        settings.setValue("last_texture", fileName.toStdString().c_str());
    }
}

bool MainWindow::CreateDotMode()
{
    return _editPoints->isChecked();
}

void MainWindow::open()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"), "", tr("Animation (*.aml)"));

    if (fileName.size())
    {
        _storeFileName = fileName;

        Animation::Instance()->LoadFromFile(_storeFileName.toStdString());
    }
}

void MainWindow::save()
{
    if (!_storeFileName.size())
    {
        saveAs();
        return;
    }

    Animation::Instance()->SaveToFile(_storeFileName.toStdString());
}

void MainWindow::saveAs()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save File"), "", tr("Animation (*.aml)"));

    if (fileName.size())
    {
        _storeFileName = fileName;
        save();
    }
}

