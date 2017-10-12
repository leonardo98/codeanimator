#ifndef QCONSOLEWIDGET_H
#define QCONSOLEWIDGET_H

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

protected:
    void keyPressEvent (QKeyEvent * event);
public slots:
    void OnChildStarted();
    void OnChildStdOutWrite(QString szOutput);
    void OnChildStdErrWrite(QString szOutput);
    void OnChildTerminate();

    void PrintToOutput(const char *s);

    void cursorPositionChanged();
};

#endif // QCONSOLEWIDGET_H
