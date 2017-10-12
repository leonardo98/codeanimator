#ifdef _DEBUG
#undef _DEBUG
#include <python.h>
#define _DEBUG
#else
#include <python.h>
#endif

#include "qconsolewidget.h"
#include "bindspline.h"

#include <QKeyEvent>

PyObject *_pModule;
PyObject *_catcher;

QConsoleWidget::QConsoleWidget(QWidget *parent) : QTextEdit(parent)
{
    setUndoRedoEnabled(false);

    //setTextColor(QColor("white"));

    /*QPalette p = this->palette();
    p.setColor(QPalette::Base, QColor("#D35800"));
    this->setPalette(p);*/

    fixedPosition = 0;

    {
        std::string stdOutErr =
            "import sys\n\
            class CatchOutErr:\n\
                def __init__(self):\n\
                self.value = ''\n\
                def reset(self):\n\
                self.value = ''\n\
                def write(self, txt):\n\
                self.value += txt\n\
            catchOutErr = CatchOutErr()\n\
            sys.stdout = catchOutErr\n\
            sys.stderr = catchOutErr\n\
            "; //this is python code to redirect stdouts/stderr

        /* Initialize the Python interpreter.  Required. */
        Py_Initialize();

        InitSplineModule();

        _pModule = PyImport_AddModule("__main__"); //create main module
        PyRun_SimpleString(stdOutErr.c_str()); //invoke code to redirect

        /* Execute some Python statements (in module __main__) */
        PyRun_SimpleString("import sys\n");
//        PyRun_SimpleString("print \"Hello!\"");

        _catcher = PyObject_GetAttrString(_pModule,"catchOutErr"); //get our catchOutErr created above

        /* Exit, cleaning up the interpreter */
        //Py_Exit(0);
    }

}

QConsoleWidget::~QConsoleWidget()
{
    Py_Finalize();
}

void QConsoleWidget::OnChildStarted()
{
}

void QConsoleWidget::OnChildStdOutWrite(QString szOutput)
{
#ifdef Q_OS_WIN32
    QString content;

    if (fixedPosition != 0) {
        content = szOutput.right(szOutput.count() - szOutput.indexOf('\n') - 1);
    } else {    
        content = szOutput;
    }

    if (content.startsWith("\f\r\n")) {
        clear();
        insertPlainText(content.right(content.count() - 3));
    } else {
        insertPlainText(content);
    }
#else
    insertPlainText("\n");
    insertPlainText(szOutput);
#endif
    fixedPosition = textCursor().position();
}

void QConsoleWidget::PrintToOutput(const char *s)
{
    insertPlainText(s);
    insertPlainText("\n");
    fixedPosition = textCursor().position();
}

void QConsoleWidget::OnChildStdErrWrite(QString szOutput)
{
    append(szOutput);
    fixedPosition = textCursor().position();
}

void QConsoleWidget::OnChildTerminate()
{
    //exit(1);
}

void QConsoleWidget::keyPressEvent(QKeyEvent *event)
{
    bool accept;
    int key = event->key();
    if (event->matches(QKeySequence::Copy))
    {
        accept = true;
    }
    else if (key == Qt::Key_Backspace)
    {
        accept = textCursor().position() > fixedPosition
                    || (textCursor().hasSelection() && textCursor().position() >= fixedPosition);
    }
    else if (key == Qt::Key_Left)
    {
        accept = textCursor().position() > fixedPosition;
    }
    else if (key == Qt::Key_Return) {
        accept = false;
        int count = toPlainText().count() - fixedPosition;
        QString cmd = toPlainText().right(count);
        //redirect->WriteChildStdIn(cmd + "\n");
        moveCursor(QTextCursor::End);
        insertPlainText("\n");

        if (cmd == "clr")
        {
            clear();
        }
        else
        {
            PyRun_SimpleString(cmd.toStdString().c_str());

            PyErr_Print(); //make python print any errors
            PyObject *output = PyObject_GetAttrString(_catcher,"value"); //get the stdout and stderr from our catchOutErr object
            PyRun_SimpleString("catchOutErr.reset()");
            
            PyObject* pyStr = PyUnicode_AsEncodedString(output, "utf-8", "Error ~");
            insertPlainText(PyBytes_AS_STRING(pyStr));
        }

        fixedPosition = textCursor().position();
    } else if (key == Qt::Key_Up) {
        accept = false;
    } else {
        accept = textCursor().position() >= fixedPosition;
    }

    if (accept) {
        QTextEdit::keyPressEvent(event);
    }
}

void QConsoleWidget::cursorPositionChanged()
{
    if (textCursor().position() < fixedPosition) {
        textCursor().setPosition(fixedPosition);
    }
}
