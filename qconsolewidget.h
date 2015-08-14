#ifndef QCONSOLEWIDGET_H
#define QCONSOLEWIDGET_H

#include <python2.7/Python.h>

#include <QTextEdit>
#include <QDir>


class QConsoleWidget : public QTextEdit
{
    Q_OBJECT
public:
    QConsoleWidget(QWidget *parent = 0);
    ~QConsoleWidget();
private:
    int fixedPosition;
    PyObject *_pModule;
    PyObject *_catcher;

protected:
    void keyPressEvent (QKeyEvent * event);
public slots:
    void OnChildStarted();
    void OnChildStdOutWrite(QString szOutput);
    void OnChildStdErrWrite(QString szOutput);
    void OnChildTerminate();

    void cursorPositionChanged();
};

#endif // QCONSOLEWIDGET_H
